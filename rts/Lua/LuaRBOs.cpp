/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */


#include "LuaRBOs.h"

#include "LuaInclude.h"

#include "LuaHandle.h"
#include "LuaHashString.h"
#include "LuaUtils.h"
#include "Rendering/GlobalRendering.h"


/******************************************************************************
 * RBO
 * @see rts/Lua/LuaRBOs.cpp
******************************************************************************/

LuaRBOs::~LuaRBOs()
{
	for (const RBO* rbo: rbos) {
		glDeleteRenderbuffersEXT(1, &rbo->id);
	}
}


/******************************************************************************/
/******************************************************************************/

bool LuaRBOs::PushEntries(lua_State* L)
{
	CreateMetatable(L);

	REGISTER_LUA_CFUNC(CreateRBO);
	REGISTER_LUA_CFUNC(DeleteRBO);

	return true;
}


bool LuaRBOs::CreateMetatable(lua_State* L)
{
	luaL_newmetatable(L, "RBO");
	HSTR_PUSH_CFUNC(L, "__gc",        meta_gc);
	HSTR_PUSH_CFUNC(L, "__index",     meta_index);
	HSTR_PUSH_CFUNC(L, "__newindex",  meta_newindex);
	lua_pop(L, 1);
	return true;
}


/******************************************************************************/
/******************************************************************************/

const LuaRBOs::RBO* LuaRBOs::GetLuaRBO(lua_State* L, int index)
{
	return static_cast<RBO*>(LuaUtils::GetUserData(L, index, "RBO"));
}


/******************************************************************************/
/******************************************************************************/

void LuaRBOs::RBO::Init()
{
	index   = -1u;
	id      = 0;

	target  = GL_RENDERBUFFER_EXT;
	format  = GL_RGBA;

	xsize   = 0;
	ysize   = 0;
	samples = 0;
}


void LuaRBOs::RBO::Free(lua_State* L)
{
	if (id == 0)
		return;

	glDeleteRenderbuffersEXT(1, &id);
	id = 0;

	{
		// get rid of the userdatum
		LuaRBOs& activeRBOs = CLuaHandle::GetActiveRBOs(L);
		auto& rbos = activeRBOs.rbos;

		assert(index < rbos.size());
		assert(rbos[index] == this);

		rbos[index] = rbos.back();
		rbos[index]->index = index;
		rbos.pop_back();
	}
}


/******************************************************************************/
/******************************************************************************/

int LuaRBOs::meta_gc(lua_State* L)
{
	RBO* rbo = static_cast<RBO*>(luaL_checkudata(L, 1, "RBO"));
	rbo->Free(L);
	return 0;
}


int LuaRBOs::meta_index(lua_State* L)
{
	const RBO* rbo = static_cast<RBO*>(luaL_checkudata(L, 1, "RBO"));

	switch (hashString(luaL_checkstring(L, 2))) {
		case hashString(  "valid"): { lua_pushboolean(L, glIsRenderbufferEXT(rbo->id)); return 1; } break;
		case hashString( "target"): { lua_pushnumber(L, rbo->target );                  return 1; } break;
		case hashString( "format"): { lua_pushnumber(L, rbo->format );                  return 1; } break;
		case hashString(  "xsize"): { lua_pushnumber(L, rbo->xsize  );                  return 1; } break;
		case hashString(  "ysize"): { lua_pushnumber(L, rbo->ysize  );                  return 1; } break;
		case hashString("samples"): { lua_pushnumber(L, rbo->samples);                  return 1; } break;
		default                   : {                                                             } break;
	}

	return 0;
}


int LuaRBOs::meta_newindex(lua_State* L)
{
	return 0;
}


/******************************************************************************/
/******************************************************************************/

/***
 * User Data RBO
 * @class RBO
 * @field target GL
 * @field format GL
 * @field xsize integer
 * @field ysize integer
 * @field valid boolean
 * @field samples integer will return globalRendering->msaaLevel for multisampled RBO or 0 otherwise
 */

/***
 * @class CreateRBOData
 * @x_helper
 * @field target GL
 * @field format GL
 * @field samples number? any number here will result in creation of multisampled RBO
 */

/***
 * @function gl.CreateRBO
 * @param xsize integer
 * @param ysize integer
 * @param data CreateRBOData
 * @return RBO
 */
int LuaRBOs::CreateRBO(lua_State* L)
{
	RBO rbo;
	rbo.Init();

	rbo.xsize = (GLsizei)luaL_checknumber(L, 1);
	rbo.ysize = (GLsizei)luaL_checknumber(L, 2);

	constexpr int tableIdx = 3;
	if (lua_istable(L, tableIdx)) {
		{
			lua_getfield(L, tableIdx, "target");

			if (lua_isnumber(L, -1))
				rbo.target = (GLenum)lua_tonumber(L, -1);

			lua_pop(L, 1);
		}
		{
			lua_getfield(L, tableIdx, "format");

			if (lua_isnumber(L, -1))
				rbo.format = (GLenum)lua_tonumber(L, -1);

			lua_pop(L, 1);
		}
		{
			lua_getfield(L, tableIdx, "samples");

			// not std::clamp(lua_tonumber(L, -1), 2, globalRendering->msaaLevel);
			// AA sample count has to equal the default FB or blitting breaks
			if (lua_isnumber(L, -1))
				rbo.samples = globalRendering->msaaLevel;

			lua_pop(L, 1);
		}
	}

	glGenRenderbuffersEXT(1, &rbo.id);
	glBindRenderbufferEXT(rbo.target, rbo.id);

	// allocate the memory
	// in theory glRenderbufferStorageMultisample(...,samples = 0,...) is equivalent
	// to glRenderbufferStorage, so these two calls could be replaced with one later
	if (rbo.samples > 1)
		glRenderbufferStorageMultisampleEXT(rbo.target, rbo.samples, rbo.format, rbo.xsize, rbo.ysize);
	else
		glRenderbufferStorageEXT(rbo.target, rbo.format, rbo.xsize, rbo.ysize);

	glBindRenderbufferEXT(rbo.target, 0);

	RBO* rboPtr = static_cast<RBO*>(lua_newuserdata(L, sizeof(RBO)));
	*rboPtr = rbo;

	luaL_getmetatable(L, "RBO");
	lua_setmetatable(L, -2);

	if (rboPtr->id != 0) {
		LuaRBOs& activeRBOs = CLuaHandle::GetActiveRBOs(L);
		auto& rbos = activeRBOs.rbos;

		rbos.push_back(rboPtr);
		rboPtr->index = rbos.size() - 1;
	}

	return 1;
}


/***
 * @function gl.DeleteRBO
 * @param rbo RBO
 */
int LuaRBOs::DeleteRBO(lua_State* L)
{
	if (lua_isnil(L, 1)) {
		return 0;
	}
	RBO* rbo = static_cast<RBO*>(luaL_checkudata(L, 1, "RBO"));
	rbo->Free(L);
	return 0;
}


/******************************************************************************/
/******************************************************************************/
