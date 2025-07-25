/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "LuaHandleSynced.h"

#include "LuaInclude.h"

#include "LuaUtils.h"
#include "LuaArchive.h"
#include "LuaCallInCheck.h"
#include "LuaConfig.h"
#include "LuaConstGL.h"
#include "LuaConstCMD.h"
#include "LuaConstCMDTYPE.h"
#include "LuaConstCOB.h"
#include "LuaConstEngine.h"
#include "LuaConstGame.h"
#include "LuaConstPlatform.h"
#include "LuaInterCall.h"
#include "LuaSyncedCtrl.h"
#include "LuaSyncedRead.h"
#include "LuaSyncedTable.h"
#include "LuaUICommand.h"
#include "LuaUnsyncedCtrl.h"
#include "LuaUnsyncedRead.h"
#include "LuaFeatureDefs.h"
#include "LuaUnitDefs.h"
#include "LuaWeaponDefs.h"
#include "LuaScream.h"
#include "LuaMaterial.h"
#include "LuaOpenGL.h"
#include "LuaVFS.h"
#include "LuaZip.h"

#include "Game/Game.h"
#include "Game/WordCompletion.h"
#include "Sim/Misc/GlobalSynced.h"
#include "Sim/Misc/TeamHandler.h"
#include "Sim/Features/FeatureDef.h"
#include "Sim/Features/FeatureDefHandler.h"
#include "Sim/Units/BuildInfo.h"
#include "Sim/Units/UnitDef.h"
#include "Sim/Units/UnitDefHandler.h"
#include "Sim/Units/Scripts/CobInstance.h"
#include "Sim/Units/Scripts/LuaUnitScript.h"
#include "Sim/Weapons/Weapon.h"
#include "Sim/Weapons/WeaponDefHandler.h"
#include "System/EventHandler.h"
#include "System/creg/SerializeLuaState.h"
#include "System/FileSystem/FileHandler.h"
#include "System/Log/ILog.h"
#include "System/SpringMath.h"
#include "System/LoadLock.h"

#include "System/Misc/TracyDefs.h"


LuaRulesParams::Params  CSplitLuaHandle::gameParams;



/******************************************************************************/
/******************************************************************************/
// ##     ## ##    ##  ######  ##    ## ##    ##  ######  ######## ########
// ##     ## ###   ## ##    ##  ##  ##  ###   ## ##    ## ##       ##     ##
// ##     ## ####  ## ##         ####   ####  ## ##       ##       ##     ##
// ##     ## ## ## ##  ######     ##    ## ## ## ##       ######   ##     ##
// ##     ## ##  ####       ##    ##    ##  #### ##       ##       ##     ##
// ##     ## ##   ### ##    ##    ##    ##   ### ##    ## ##       ##     ##
//  #######  ##    ##  ######     ##    ##    ##  ######  ######## ########

CUnsyncedLuaHandle::CUnsyncedLuaHandle(CSplitLuaHandle* _base, const std::string& _name, int _order)
	: CLuaHandle(_name, _order, false, false)
	, base(*_base)
{
	D.allowChanges = false;
}


CUnsyncedLuaHandle::~CUnsyncedLuaHandle() = default;


bool CUnsyncedLuaHandle::Init(std::string code, const std::string& file)
{
	if (!IsValid())
		return false;

	// other watch defs not initialized here since unsupported on unsynced
	watchExplosionDefs.resize(weaponDefHandler->NumWeaponDefs(), false);

	// load the standard libraries
	LUA_OPEN_LIB(L, luaopen_base);
	LUA_OPEN_LIB(L, luaopen_math);
	LUA_OPEN_LIB(L, luaopen_table);
	LUA_OPEN_LIB(L, luaopen_string);
	//LUA_OPEN_LIB(L, luaopen_io);
	//LUA_OPEN_LIB(L, luaopen_os);
	//LUA_OPEN_LIB(L, luaopen_package);
	//LUA_OPEN_LIB(L, luaopen_debug);
	EnactDevMode();

	// delete some dangerous functions
	lua_pushnil(L); lua_setglobal(L, "dofile");
	lua_pushnil(L); lua_setglobal(L, "loadfile");
	lua_pushnil(L); lua_setglobal(L, "loadlib");
	lua_pushnil(L); lua_setglobal(L, "loadstring"); // replaced
	lua_pushnil(L); lua_setglobal(L, "require");

	lua_pushvalue(L, LUA_GLOBALSINDEX);

	AddBasicCalls(L);

	// remove Script.Kill()
	lua_getglobal(L, "Script");
		LuaPushNamedNil(L, "Kill");
	lua_pop(L, 1);

	LuaPushNamedCFunc(L, "loadstring", CLuaHandle::LoadStringData);
	LuaPushNamedCFunc(L, "CallAsTeam", CSplitLuaHandle::CallAsTeam);
	/*** @global COBSCALE integer */ 
	LuaPushNamedNumber(L, "COBSCALE",  COBSCALE);

	// load our libraries
	{
		#define KILL { KillLua(); return false; }
		if (!LuaSyncedTable::PushEntries(L)) KILL

		if (!AddCommonModules(L)						       ) KILL
		if (!AddEntriesToTable(L, "VFS",                   LuaVFS::PushUnsynced       )) KILL
		if (!AddEntriesToTable(L, "VFS",         LuaZipFileReader::PushUnsynced       )) KILL
		if (!AddEntriesToTable(L, "VFS",         LuaZipFileWriter::PushUnsynced       )) KILL
		if (!AddEntriesToTable(L, "VFS",               LuaArchive::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "UnitDefs",         LuaUnitDefs::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "WeaponDefs",     LuaWeaponDefs::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "FeatureDefs",   LuaFeatureDefs::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "Script",          LuaInterCall::PushEntriesUnsynced)) KILL
		if (!AddEntriesToTable(L, "Script",             LuaScream::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "Spring",         LuaSyncedRead::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "Spring",       LuaUnsyncedCtrl::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "Spring",       LuaUnsyncedRead::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "Spring",          LuaUICommand::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "gl",                 LuaOpenGL::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "GL",                LuaConstGL::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "Engine",        LuaConstEngine::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "Platform",      LuaConstPlatform::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "Game",            LuaConstGame::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "CMD",              LuaConstCMD::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "CMDTYPE",      LuaConstCMDTYPE::PushEntries        )) KILL
		if (!AddEntriesToTable(L, "LOG",                 LuaUtils::PushLogEntries     )) KILL
		#undef KILL
	}

	lua_settop(L, 0);

	// add code from the sub-class
	if (!base.AddUnsyncedCode(L)) {
		KillLua();
		return false;
	}

	lua_settop(L, 0);
	if (!LoadCode(L, std::move(code), file)) {
		KillLua();
		return false;
	}

	lua_settop(L, 0);
	eventHandler.AddClient(this);
	return true;
}


void CUnsyncedLuaHandle::EnactDevMode() const
{
	SwapEnableModule(L, devMode, LUA_DBLIBNAME, luaopen_debug);
}


/***
 * @class UnsyncedCallins
 * Functions called by the Engine (Unsynced).
 */

/**********************
 * Misc
 *
 * @section misc
 *********************/

/*** Receives data sent via `SendToUnsynced` callout.
 *
 * @function UnsyncedCallins:RecvFromSynced
 * @param ... any
 */
void CUnsyncedLuaHandle::RecvFromSynced(lua_State* srcState, int args)
{
	if (!IsValid())
		return;


	LUA_CALL_IN_CHECK(L);
	luaL_checkstack(L, 2 + args, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return; // the call is not defined

	LuaUtils::CopyData(L, srcState, args);

	// call the routine
	RunCallIn(L, cmdStr, args, 0);
}

/*** Custom Object Rendering
 *
 * For the following calls drawMode can be one of the following, notDrawing = 0, normalDraw = 1, shadowDraw = 2, reflectionDraw = 3, refractionDraw = 4, and finally gameDeferredDraw = 5 which was added in 102.0.
 *
 * @section custom_object
 */

/*** For custom rendering of units
 *
 * @function UnsyncedCallins:DrawUnit
 * @param unitID integer
 * @param drawMode number
 * @return boolean suppressEngineDraw
 * @deprecated
 */
bool CUnsyncedLuaHandle::DrawUnit(const CUnit* unit)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, false);
	luaL_checkstack(L, 4, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return false;

	const bool oldDrawState = LuaOpenGL::IsDrawingEnabled(L);
	LuaOpenGL::SetDrawingEnabled(L, true);

	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, game->GetDrawMode());

	const bool success = RunCallIn(L, cmdStr, 2, 1);
	LuaOpenGL::SetDrawingEnabled(L, oldDrawState);

	if (!success)
		return false;

	const bool draw = luaL_optboolean(L, -1, false);
	lua_pop(L, 1);
	return draw;
}



/*** For custom rendering of features
 *
 * @function UnsyncedCallins:DrawFeature
 * @param featureID integer
 * @param drawMode number
 * @return boolean suppressEngineDraw
 * @deprecated
 */
bool CUnsyncedLuaHandle::DrawFeature(const CFeature* feature)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, false);
	luaL_checkstack(L, 4, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return false;

	const bool oldDrawState = LuaOpenGL::IsDrawingEnabled(L);
	LuaOpenGL::SetDrawingEnabled(L, true);

	lua_pushnumber(L, feature->id);
	lua_pushnumber(L, game->GetDrawMode());

	const bool success = RunCallIn(L, cmdStr, 2, 1);
	LuaOpenGL::SetDrawingEnabled(L, oldDrawState);

	if (!success)
		return false;

	const bool draw = luaL_optboolean(L, -1, false);
	lua_pop(L, 1);
	return draw;
}


/*** For custom rendering of shields.
 *
 * @function UnsyncedCallins:DrawShield
 * @param featureID integer
 * @param weaponID integer
 * @param drawMode number
 * @return boolean suppressEngineDraw
 * @deprecated
 */
bool CUnsyncedLuaHandle::DrawShield(const CUnit* unit, const CWeapon* weapon)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, false);
	luaL_checkstack(L, 5, __func__);

	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return false;

	const bool oldDrawState = LuaOpenGL::IsDrawingEnabled(L);
	LuaOpenGL::SetDrawingEnabled(L, true);

	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, weapon->weaponNum + LUA_WEAPON_BASE_INDEX);
	lua_pushnumber(L, game->GetDrawMode());

	const bool success = RunCallIn(L, cmdStr, 3, 1);
	LuaOpenGL::SetDrawingEnabled(L, oldDrawState);

	if (!success)
		return false;

	const bool draw = luaL_optboolean(L, -1, false);
	lua_pop(L, 1);
	return draw;
}


/*** For custom rendering of weapon (& other) projectiles
 *
 * @function UnsyncedCallins:DrawProjectile
 * @param projectileID integer
 * @param drawMode number
 * @return boolean suppressEngineDraw
 * @deprecated
 */
bool CUnsyncedLuaHandle::DrawProjectile(const CProjectile* projectile)
{
	RECOIL_DETAILED_TRACY_ZONE;
	assert(projectile->weapon || projectile->piece);

	LUA_CALL_IN_CHECK(L, false);
	luaL_checkstack(L, 5, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return false;

	const bool oldDrawState = LuaOpenGL::IsDrawingEnabled(L);
	LuaOpenGL::SetDrawingEnabled(L, true);

	lua_pushnumber(L, projectile->id);
	lua_pushnumber(L, game->GetDrawMode());

	const bool success = RunCallIn(L, cmdStr, 2, 1);
	LuaOpenGL::SetDrawingEnabled(L, oldDrawState);

	if (!success)
		return false;

	const bool draw = luaL_optboolean(L, -1, false);
	lua_pop(L, 1);
	return draw;
}


/***
 *
 * @function UnsyncedCallins:DrawMaterial
 * @param uuid integer
 * @param drawMode number
 * @return boolean suppressEngineDraw
 * @deprecated
 */
bool CUnsyncedLuaHandle::DrawMaterial(const LuaMaterial* material)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, false);
	luaL_checkstack(L, 4, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return false;

	const bool oldDrawState = LuaOpenGL::IsDrawingEnabled(L);
	LuaOpenGL::SetDrawingEnabled(L, true);

	lua_pushnumber(L, material->uuid);
	// lua_pushnumber(L, material->type);
	lua_pushnumber(L, game->GetDrawMode());

	const bool success = RunCallIn(L, cmdStr, 2, 1);
	LuaOpenGL::SetDrawingEnabled(L, oldDrawState);

	if (!success)
		return false;

	const bool draw = luaL_optboolean(L, -1, false);
	lua_pop(L, 1);
	return draw;
}


//
// Call-Outs
//


/******************************************************************************/
/******************************************************************************/
//  ######  ##    ## ##    ##  ######  ######## ########
// ##    ##  ##  ##  ###   ## ##    ## ##       ##     ##
// ##         ####   ####  ## ##       ##       ##     ##
//  ######     ##    ## ## ## ##       ######   ##     ##
//       ##    ##    ##  #### ##       ##       ##     ##
// ##    ##    ##    ##   ### ##    ## ##       ##     ##
//  ######     ##    ##    ##  ######  ######## ########

CSyncedLuaHandle::CSyncedLuaHandle(CSplitLuaHandle* _base, const std::string& _name, int _order)
	: CLuaHandle(_name, _order, false, true)
	, base(*_base)
	, origNextRef(-1)
{
	D.allowChanges = true;
}


CSyncedLuaHandle::~CSyncedLuaHandle()
{
	// kill all unitscripts running in this handle
	CLuaUnitScript::HandleFreed(this);
}


bool CSyncedLuaHandle::Init(std::string code, const std::string& file)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!IsValid())
		return false;

	watchUnitDefs.resize(unitDefHandler->NumUnitDefs() + 1, false);
	watchFeatureDefs.resize(featureDefHandler->NumFeatureDefs() + 1, false);
	watchExplosionDefs.resize(weaponDefHandler->NumWeaponDefs(), false);
	watchProjectileDefs.resize(weaponDefHandler->NumWeaponDefs() + 1, false); // last bit controls piece-projectiles
	watchAllowTargetDefs.resize(weaponDefHandler->NumWeaponDefs(), false);

	// load the standard libraries
	SPRING_LUA_OPEN_LIB(L, luaopen_base);
	SPRING_LUA_OPEN_LIB(L, luaopen_math);
	SPRING_LUA_OPEN_LIB(L, luaopen_table);
	SPRING_LUA_OPEN_LIB(L, luaopen_string);
	//SPRING_LUA_OPEN_LIB(L, luaopen_io);
	//SPRING_LUA_OPEN_LIB(L, luaopen_os);
	//SPRING_LUA_OPEN_LIB(L, luaopen_package);
	//SPRING_LUA_OPEN_LIB(L, luaopen_debug);
	EnactDevMode();

	lua_getglobal(L, "next");
	origNextRef = luaL_ref(L, LUA_REGISTRYINDEX);

	// delete/replace some dangerous functions
	lua_pushnil(L); lua_setglobal(L, "dofile");
	lua_pushnil(L); lua_setglobal(L, "loadfile");
	lua_pushnil(L); lua_setglobal(L, "loadlib");
	lua_pushnil(L); lua_setglobal(L, "require");
	lua_pushnil(L); lua_setglobal(L, "rawequal"); //FIXME not unsafe anymore since split?
	lua_pushnil(L); lua_setglobal(L, "rawget"); //FIXME not unsafe anymore since split?
	lua_pushnil(L); lua_setglobal(L, "rawset"); //FIXME not unsafe anymore since split?
//	lua_pushnil(L); lua_setglobal(L, "getfenv");
//	lua_pushnil(L); lua_setglobal(L, "setfenv");
	lua_pushnil(L); lua_setglobal(L, "newproxy"); // sync unsafe cause of __gc
	lua_pushnil(L); lua_setglobal(L, "gcinfo");
	lua_pushnil(L); lua_setglobal(L, "collectgarbage");

	lua_pushvalue(L, LUA_GLOBALSINDEX);
	LuaPushNamedCFunc(L, "loadstring", CLuaHandle::LoadStringData);
	LuaPushNamedCFunc(L, "pairs", SyncedPairs);
	LuaPushNamedCFunc(L, "next",  SyncedNext);
	lua_pop(L, 1);

	lua_pushvalue(L, LUA_GLOBALSINDEX);

	AddBasicCalls(L); // into Global

	// adjust the math.random() and math.randomseed() calls
	lua_getglobal(L, "math");
		LuaPushNamedCFunc(L, "random", SyncedRandom);
		LuaPushNamedCFunc(L, "randomseed", SyncedRandomSeed);
	lua_pop(L, 1); // pop the global math table

	lua_getglobal(L, "Script");
		LuaPushNamedCFunc(L, "AddActionFallback",    AddSyncedActionFallback);
		LuaPushNamedCFunc(L, "RemoveActionFallback", RemoveSyncedActionFallback);
		LuaPushNamedCFunc(L, "GetWatchUnit",         GetWatchUnitDef);
		LuaPushNamedCFunc(L, "SetWatchUnit",         SetWatchUnitDef);
		LuaPushNamedCFunc(L, "GetWatchFeature",      GetWatchFeatureDef);
		LuaPushNamedCFunc(L, "SetWatchFeature",      SetWatchFeatureDef);
		LuaPushNamedCFunc(L, "GetWatchExplosion",    GetWatchExplosionDef);
		LuaPushNamedCFunc(L, "SetWatchExplosion",    SetWatchExplosionDef);
		LuaPushNamedCFunc(L, "GetWatchProjectile",   GetWatchProjectileDef);
		LuaPushNamedCFunc(L, "SetWatchProjectile",   SetWatchProjectileDef);
		LuaPushNamedCFunc(L, "GetWatchAllowTarget",  GetWatchAllowTargetDef);
		LuaPushNamedCFunc(L, "SetWatchAllowTarget",  SetWatchAllowTargetDef);
		LuaPushNamedCFunc(L, "GetWatchWeapon",       GetWatchWeaponDef);
		LuaPushNamedCFunc(L, "SetWatchWeapon",       SetWatchWeaponDef);
	lua_pop(L, 1);

	// add the custom file loader
	LuaPushNamedCFunc(L, "SendToUnsynced", SendToUnsynced);
	LuaPushNamedCFunc(L, "CallAsTeam",     CSplitLuaHandle::CallAsTeam);
	LuaPushNamedNumber(L, "COBSCALE",      COBSCALE);

	// load our libraries  (LuaSyncedCtrl overrides some LuaUnsyncedCtrl entries)
	{
		#define KILL { KillLua(); return false; }
		if (!AddCommonModules(L)						     ) KILL
		if (!AddEntriesToTable(L, "VFS",                   LuaVFS::PushSynced       )) KILL
		if (!AddEntriesToTable(L, "VFS",         LuaZipFileReader::PushSynced       )) KILL
		if (!AddEntriesToTable(L, "VFS",         LuaZipFileWriter::PushSynced       )) KILL
		if (!AddEntriesToTable(L, "UnitDefs",         LuaUnitDefs::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "WeaponDefs",     LuaWeaponDefs::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "FeatureDefs",   LuaFeatureDefs::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "Script",          LuaInterCall::PushEntriesSynced)) KILL
		if (!AddEntriesToTable(L, "Spring",       LuaUnsyncedCtrl::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "Spring",         LuaSyncedCtrl::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "Spring",         LuaSyncedRead::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "Spring",          LuaUICommand::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "Engine",        LuaConstEngine::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "Game",            LuaConstGame::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "CMD",              LuaConstCMD::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "CMDTYPE",      LuaConstCMDTYPE::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "COB",              LuaConstCOB::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "SFX",              LuaConstSFX::PushEntries      )) KILL
		if (!AddEntriesToTable(L, "LOG",                 LuaUtils::PushLogEntries   )) KILL
		#undef KILL
	}

	// add code from the sub-class
	if (!base.AddSyncedCode(L)) {
		KillLua();
		return false;
	}

	lua_settop(L, 0);
	creg::AutoRegisterCFunctions(GetName(), L);

	if (!LoadCode(L, std::move(code), file)) {
		KillLua();
		return false;
	}

	lua_settop(L, 0);
	eventHandler.AddClient(this);
	return true;
}


void CSyncedLuaHandle::EnactDevMode() const
{
	SwapEnableModule(L, devMode, LUA_DBLIBNAME, luaopen_debug);
}


//
// Call-Ins
//

/***
 * @class SyncedCallins
 * Functions called by the Engine (Synced).
 */

bool CSyncedLuaHandle::SyncedActionFallback(const std::string& msg, int playerID)
{
	RECOIL_DETAILED_TRACY_ZONE;
	string cmd = msg;
	const std::string::size_type pos = cmd.find_first_of(" \t");
	if (pos != string::npos)
		cmd.resize(pos);

	if (textCommands.find(cmd) == textCommands.end())
		return false;

	// strip the leading '/'
	return GotChatMsg(msg.substr(1), playerID);
}


/*** Called when the unit reaches an unknown command in its queue (i.e. one not handled by the engine).
 *
 * @function SyncedCallins:CommandFallback
 * @param unitID integer
 * @param unitDefID integer
 * @param unitTeam integer
 * @param cmdID integer
 * @param cmdParams number[]
 * @param cmdOptions CommandOptions
 * @param cmdTag number
 * @return boolean whether to remove the command from the queue
 */
bool CSyncedLuaHandle::CommandFallback(const CUnit* unit, const Command& cmd)
{
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 9, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	LuaUtils::PushUnitAndCommand(L, unit, cmd);

	// call the function
	if (!RunCallIn(L, cmdStr, 7, 1))
		return true;

	const bool remove = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return remove; // return 'true' to remove the command
}


/*** Called when the command is given, before the unit's queue is altered.
 *
 * @function SyncedCallins:AllowCommand
 *
 * The queue remains untouched when a command is blocked, whether it would be queued or replace the queue.
 *
 * @param unitID integer
 * @param unitDefID integer
 * @param unitTeam integer
 * @param cmdID integer
 * @param cmdParams number[]
 * @param cmdOptions CommandOptions
 * @param cmdTag number
 * @param synced boolean
 * @param fromLua boolean
 * @return boolean whether it should be let into the queue.
 */
bool CSyncedLuaHandle::AllowCommand(const CUnit* unit, const Command& cmd, int playerNum, bool fromSynced, bool fromLua)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 7 + 3, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	const int argc = LuaUtils::PushUnitAndCommand(L, unit, cmd);

	lua_pushnumber(L, playerNum);
	lua_pushboolean(L, fromSynced);
	lua_pushboolean(L, fromLua);

	// call the function
	if (!RunCallIn(L, cmdStr, argc + 3, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/*** Called just before unit is created.
 *
 * @function SyncedCallins:AllowUnitCreation
 * @param unitDefID integer
 * @param builderID integer
 * @param builderTeam integer
 * @param x number
 * @param y number
 * @param z number
 * @param facing FacingInteger
 * @return boolean allow, boolean dropOrder
 */
std::pair <bool, bool> CSyncedLuaHandle::AllowUnitCreation(
	const UnitDef* unitDef,
	const CUnit* builder,
	const BuildInfo* buildInfo
) {
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 10, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return {true, true}; // the call is not defined

	lua_pushnumber(L, unitDef->id);
	lua_pushnumber(L, builder->id);
	lua_pushnumber(L, builder->team);

	if (buildInfo != nullptr) {
		lua_pushnumber(L, buildInfo->pos.x);
		lua_pushnumber(L, buildInfo->pos.y);
		lua_pushnumber(L, buildInfo->pos.z);
		lua_pushnumber(L, buildInfo->buildFacing);
	}

	// call the function
	if (!RunCallIn(L, cmdStr, (buildInfo != nullptr)? 7 : 3, 2))
		return {true, true};

	// get the results
	const bool allow = luaL_optboolean(L, -2, true);
	const bool  drop = luaL_optboolean(L, -1, true);
	lua_pop(L, 2);
	return {allow, drop};
}


/*** Called just before a unit is transferred to a different team.
 *
 * @function SyncedCallins:AllowUnitTransfer
 * @param unitID integer
 * @param unitDefID integer
 * @param oldTeam integer
 * @param newTeam integer
 * @param capture boolean
 * @return boolean whether or not the transfer is permitted.
 */
bool CSyncedLuaHandle::AllowUnitTransfer(const CUnit* unit, int newTeam, bool capture)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 7, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, unit->unitDef->id);
	lua_pushnumber(L, unit->team);
	lua_pushnumber(L, newTeam);
	lua_pushboolean(L, capture);

	// call the function
	if (!RunCallIn(L, cmdStr, 5, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/*** Called just before a unit progresses its build percentage.
 *
 * @function SyncedCallins:AllowUnitBuildStep
 * @param builderID integer
 * @param builderTeam integer
 * @param unitID integer
 * @param unitDefID integer
 * @param part number
 * @return boolean whether or not the build makes progress.
 */
bool CSyncedLuaHandle::AllowUnitBuildStep(const CUnit* builder, const CUnit* unit, float part)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 7, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	lua_pushnumber(L, builder->id);
	lua_pushnumber(L, builder->team);
	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, unit->unitDef->id);
	lua_pushnumber(L, part);

	// call the function
	if (!RunCallIn(L, cmdStr, 5, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/***
 *
 * @function SyncedCallins:AllowUnitCaptureStep
 * @param builderID integer
 * @param builderTeam integer
 * @param unitID integer
 * @param unitDefID integer
 * @param part number
 * @return boolean whether or not the capture makes progress.
 */
bool CSyncedLuaHandle::AllowUnitCaptureStep(const CUnit* builder, const CUnit* unit, float part)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 7, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	lua_pushnumber(L, builder->id);
	lua_pushnumber(L, builder->team);
	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, unit->unitDef->id);
	lua_pushnumber(L, part);

	// call the function
	if (!RunCallIn(L, cmdStr, 5, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/***
 *
 * @function SyncedCallins:AllowUnitTransport
 * @param transporterID integer
 * @param transporterUnitDefID integer
 * @param transporterTeam integer
 * @param transporteeID integer
 * @param transporteeUnitDefID integer
 * @param transporteeTeam integer
 * @return boolean whether or not the transport is allowed
 */
bool CSyncedLuaHandle::AllowUnitTransport(const CUnit* transporter, const CUnit* transportee)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 2 + 6, __func__);

	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return true;

	lua_pushnumber(L, transporter->id);
	lua_pushnumber(L, transporter->unitDef->id);
	lua_pushnumber(L, transporter->team);
	lua_pushnumber(L, transportee->id);
	lua_pushnumber(L, transportee->unitDef->id);
	lua_pushnumber(L, transportee->team);

	if (!RunCallIn(L, cmdStr, 6, 1))
		return true;

	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/***
 *
 * @function SyncedCallins:AllowUnitTransportLoad
 * @param transporterID integer
 * @param transporterUnitDefID integer
 * @param transporterTeam integer
 * @param transporteeID integer
 * @param transporteeUnitDefID integer
 * @param transporteeTeam integer
 * @param x number
 * @param y number
 * @param z number
 * @return boolean whether or not the transport load is allowed
 */
bool CSyncedLuaHandle::AllowUnitTransportLoad(
	const CUnit* transporter,
	const CUnit* transportee,
	const float3& loadPos,
	bool allowed
) {
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 2 + 9, __func__);

	static const LuaHashString cmdStr(__func__);

	// use engine default if callin does not exist
	if (!cmdStr.GetGlobalFunc(L))
		return allowed;

	lua_pushnumber(L, transporter->id);
	lua_pushnumber(L, transporter->unitDef->id);
	lua_pushnumber(L, transporter->team);
	lua_pushnumber(L, transportee->id);
	lua_pushnumber(L, transportee->unitDef->id);
	lua_pushnumber(L, transportee->team);
	lua_pushnumber(L, loadPos.x);
	lua_pushnumber(L, loadPos.y);
	lua_pushnumber(L, loadPos.z);

	if (!RunCallIn(L, cmdStr, 9, 1))
		return true;

	// ditto if it does but returns nothing
	const bool allow = luaL_optboolean(L, -1, allowed);
	lua_pop(L, 1);
	return allow;
}


/***
 *
 * @function SyncedCallins:AllowUnitTransportUnload
 * @param transporterID integer
 * @param transporterUnitDefID integer
 * @param transporterTeam integer
 * @param transporteeID integer
 * @param transporteeUnitDefID integer
 * @param transporteeTeam integer
 * @param x number
 * @param y number
 * @param z number
 * @return boolean whether or not the transport unload is allowed
 */
bool CSyncedLuaHandle::AllowUnitTransportUnload(
	const CUnit* transporter,
	const CUnit* transportee,
	const float3& unloadPos,
	bool allowed
) {
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 2 + 9, __func__);

	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return allowed;

	lua_pushnumber(L, transporter->id);
	lua_pushnumber(L, transporter->unitDef->id);
	lua_pushnumber(L, transporter->team);
	lua_pushnumber(L, transportee->id);
	lua_pushnumber(L, transportee->unitDef->id);
	lua_pushnumber(L, transportee->team);
	lua_pushnumber(L, unloadPos.x);
	lua_pushnumber(L, unloadPos.y);
	lua_pushnumber(L, unloadPos.z);

	if (!RunCallIn(L, cmdStr, 9, 1))
		return true;

	const bool allow = luaL_optboolean(L, -1, allowed);
	lua_pop(L, 1);
	return allow;
}


/***
 *
 * @function SyncedCallins:AllowUnitCloak
 * @param unitID integer
 * @param enemyID integer?
 * @return boolean whether unit is allowed to cloak
 */
bool CSyncedLuaHandle::AllowUnitCloak(const CUnit* unit, const CUnit* enemy)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 2 + 2, __func__);

	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return true;


	lua_pushnumber(L, unit->id);

	if (enemy != nullptr)
		lua_pushnumber(L, enemy->id);
	else
		lua_pushnil(L);


	if (!RunCallIn(L, cmdStr, 2, 1))
		return true;

	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/***
 *
 * @function SyncedCallins:AllowUnitCloak
 * @param unitID integer
 * @param objectID integer?
 * @param weaponNum number?
 * @return boolean whether unit is allowed to decloak
 */
bool CSyncedLuaHandle::AllowUnitDecloak(const CUnit* unit, const CSolidObject* object, const CWeapon* weapon)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 2 + 3, __func__);

	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return true;


	lua_pushnumber(L, unit->id);

	if (object != nullptr)
		lua_pushnumber(L, object->id);
	else
		lua_pushnil(L);

	if (weapon != nullptr)
		lua_pushnumber(L, weapon->weaponNum);
	else
		lua_pushnil(L);


	if (!RunCallIn(L, cmdStr, 3, 1))
		return true;

	assert(lua_isboolean(L, -1));

	const bool allow = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return allow;
}


/***
 *
 * @function SyncedCallins:AllowUnitKamikaze
 * @param unitID integer
 * @param targetID integer
 * @return boolean whether unit is allowed to selfd
 */
bool CSyncedLuaHandle::AllowUnitKamikaze(const CUnit* unit, const CUnit* target, bool allowed)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 2 + 2, __func__);

	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return allowed;

	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, target->id);

	if (!RunCallIn(L, cmdStr, 2, 1))
		return true;

	const bool allow = luaL_optboolean(L, -1, allowed);
	lua_pop(L, 1);
	return allow;
}


/*** Called just before feature is created.
 *
 * @function SyncedCallins:AllowFeatureCreation
 * @param featureDefID integer
 * @param teamID integer
 * @param x number
 * @param y number
 * @param z number
 * @return boolean whether or not the creation is permitted
 */
bool CSyncedLuaHandle::AllowFeatureCreation(const FeatureDef* featureDef, int teamID, const float3& pos)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 7, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	lua_pushnumber(L, featureDef->id);
	lua_pushnumber(L, teamID);
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	lua_pushnumber(L, pos.z);

	// call the function
	if (!RunCallIn(L, cmdStr, 5, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/*** Called just before a feature changes its build percentage.
 *
 * @function SyncedCallins:AllowFeatureBuildStep
 *
 * Note that this is also called for resurrecting features, and for refilling features with resources before resurrection.
 * On reclaim the part values are negative, and on refill and resurrect they are positive.
 * Part is the percentage the feature be built or reclaimed per frame.
 * Eg. for a 30 workertime builder, that's a build power of 1 per frame.
 * For a 50 buildtime feature reclaimed by this builder, part will be 100/-50(/1) = -2%, or -0.02 numerically.
 *
 * @param builderID integer
 * @param builderTeam integer
 * @param featureID integer
 * @param featureDefID integer
 * @param part number
 *
 * @return boolean whether or not the change is permitted
 */
bool CSyncedLuaHandle::AllowFeatureBuildStep(const CUnit* builder, const CFeature* feature, float part)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 7, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	lua_pushnumber(L, builder->id);
	lua_pushnumber(L, builder->team);
	lua_pushnumber(L, feature->id);
	lua_pushnumber(L, feature->def->id);
	lua_pushnumber(L, part);

	// call the function
	if (!RunCallIn(L, cmdStr, 5, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/*** Called when a team sets the sharing level of a resource.
 *
 * @function SyncedCallins:AllowResourceLevel
 * @param teamID integer
 * @param res string
 * @param level number
 * @return boolean whether or not the sharing level is permitted
 */
bool CSyncedLuaHandle::AllowResourceLevel(int teamID, const std::string& type, float level)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 5, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	lua_pushnumber(L, teamID);
	lua_pushsstring(L, type);
	lua_pushnumber(L, level);

	// call the function
	if (!RunCallIn(L, cmdStr, 3, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/*** Called just before resources are transferred between players.
 *
 * @function SyncedCallins:AllowResourceTransfer
 * @param oldTeamID integer
 * @param newTeamID integer
 * @param res string
 * @param amount number
 * @return boolean whether or not the transfer is permitted.
 */
bool CSyncedLuaHandle::AllowResourceTransfer(int oldTeam, int newTeam, const char* type, float amount)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 6, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	lua_pushnumber(L, oldTeam);
	lua_pushnumber(L, newTeam);
	lua_pushstring(L, type);
	lua_pushnumber(L, amount);

	// call the function
	if (!RunCallIn(L, cmdStr, 4, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/*** Determines if this unit can be controlled directly in FPS view.
 *
 * @function SyncedCallins:AllowDirectUnitControl
 * @param unitID integer
 * @param unitDefID integer
 * @param unitTeam integer
 * @param playerID integer
 * @return boolean allow
 */
bool CSyncedLuaHandle::AllowDirectUnitControl(int playerID, const CUnit* unit)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 6, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, unit->unitDef->id);
	lua_pushnumber(L, unit->team);
	lua_pushnumber(L, playerID);

	// call the function
	if (!RunCallIn(L, cmdStr, 4, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/*** Called when a construction unit wants to "use his nano beams".
 *
 * @function SyncedCallins:AllowBuilderHoldFire
 *
 * @param unitID integer
 * @param unitDefID integer
 * @param action -1|CMD
 * 
 * One of the following:
 * 	- `-1` build,
 * 	- `CMD.REPAIR` repair,
 * 	- `CMD.RECLAIM` reclaim,
 * 	- `CMD.RESTORE` restore,
 * 	- `CMD.RESURRECT` resurrect, or
 * 	- `CMD.CAPTURE` capture.
 * 
 * @return boolean actionAllowed
 */
bool CSyncedLuaHandle::AllowBuilderHoldFire(const CUnit* unit, int action)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 2 + 3 + 1, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, unit->unitDef->id);
	lua_pushnumber(L, action);

	// call the function
	if (!RunCallIn(L, cmdStr, 3, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/*** Whether a start position should be allowed
 *
 * @function SyncedCallins:AllowStartPosition
 *
 * clamped{X,Y,Z} are the coordinates clamped into start-boxes, raw is where player tried to place their marker.
 *
 * The readyState can be any one of:
 *
 *     0 - player picked a position,
 *     1 - player clicked ready,
 *     2 - player pressed ready OR the game was force-started (player did not click ready, but is now forcibly readied) or
 *     3 - the player failed to load.
 *     The default 'failed to choose' start-position is the north-west point of their startbox, or (0,0,0) if they do not have a startbox.
 *
 * @param playerID integer
 * @param teamID integer
 * @param readyState number
 * @param clampedX number
 * @param clampedY number
 * @param clampedZ number
 * @param rawX number
 * @param rawY number
 * @param rawZ number
 *
 * @return boolean allow
 */
bool CSyncedLuaHandle::AllowStartPosition(int playerID, int teamID, unsigned char readyState, const float3& clampedPos, const float3& rawPickPos)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 13, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return true; // the call is not defined

	// push the start position and playerID
	lua_pushnumber(L, playerID);
	lua_pushnumber(L, teamID);
	lua_pushnumber(L, readyState);
	lua_pushnumber(L, clampedPos.x);
	lua_pushnumber(L, clampedPos.y);
	lua_pushnumber(L, clampedPos.z);
	lua_pushnumber(L, rawPickPos.x);
	lua_pushnumber(L, rawPickPos.y);
	lua_pushnumber(L, rawPickPos.z);

	// call the function
	if (!RunCallIn(L, cmdStr, 9, 1))
		return true;

	// get the results
	const bool allow = luaL_optboolean(L, -1, true);
	lua_pop(L, 1);
	return allow;
}


/*** Enable both Spring.MoveCtrl.SetCollideStop and Spring.MoveCtrl.SetTrackGround to enable this call-in.
 *
 * @function SyncedCallins:MoveCtrlNotify
 *
 * @param unitID integer
 * @param unitDefID integer
 * @param unitTeam integer
 * @param data number was supposed to indicate the type of notification but currently never has a value other than 1 ("unit hit the ground").
 *
 * @return boolean whether or not the unit should remain script-controlled (false) or return to engine controlled movement (true).
 */
bool CSyncedLuaHandle::MoveCtrlNotify(const CUnit* unit, int data)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, false);
	luaL_checkstack(L, 6, __func__);

	static const LuaHashString cmdStr(__func__);
	if (!cmdStr.GetGlobalFunc(L))
		return false; // the call is not defined

	// push the unit info
	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, unit->unitDef->id);
	lua_pushnumber(L, unit->team);
	lua_pushnumber(L, data);

	// call the function
	if (!RunCallIn(L, cmdStr, 4, 1))
		return false;

	// get the results
	const bool disable = luaL_optboolean(L, -1, false);
	lua_pop(L, 1);
	return disable;
}


/*** Called when pre-building terrain levelling terraforms are completed (c.f. levelGround)
 *
 * @function SyncedCallins:TerraformComplete
 * @param unitID integer
 * @param unitDefID integer
 * @param unitTeam integer
 * @param buildUnitID integer
 * @param buildUnitDefID integer
 * @param buildUnitTeam integer
 * @return boolean if true the current build order is terminated
 */
bool CSyncedLuaHandle::TerraformComplete(const CUnit* unit, const CUnit* build)
{
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, false);
	luaL_checkstack(L, 8, __func__);

	const LuaUtils::ScopedDebugTraceBack dbgTrace(L);
	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return false; // the call is not defined

	// push the unit info
	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, unit->unitDef->id);
	lua_pushnumber(L, unit->team);

	// push the construction info
	lua_pushnumber(L, build->id);
	lua_pushnumber(L, build->unitDef->id);
	lua_pushnumber(L, build->team);

	// call the function
	if (!RunCallInTraceback(L, cmdStr, 6, 1, dbgTrace.GetErrFuncIdx(), false))
		return false;

	// get the results
	const bool stop = luaL_optboolean(L, -1, false);
	lua_pop(L, 1);
	return stop;
}


/***
 * Damage Controllers
 *
 * For the following callins, in addition to being a regular weapon, weaponDefID may be one of the following:
 *
 *     -1 - debris collision, also default of Spring.AddUnitDamage
 *     -2 - ground collision
 *     -3 - object collision
 *     -4 - fire damage
 *     -5 - water damage
 *     -6 - kill damage
 *     -7 - crush damage
 */

/*** Called before damage is applied to the unit, allows fine control over how much damage and impulse is applied.
 *
 * @function SyncedCallins:UnitPreDamaged
 *
 * Called after every damage modification (even `HitByWeaponId`) but before the damage is applied
 *
 * expects two numbers returned by lua code:
 * 1st is stored under *newDamage if newDamage != NULL
 * 2nd is stored under *impulseMult if impulseMult != NULL
 *
 * @param unitID integer
 * @param unitDefID integer
 * @param unitTeam integer
 * @param damage number
 * @param paralyzer boolean
 * @param weaponDefID integer? Synced Only
 * @param projectileID integer? Synced Only
 * @param attackerID integer? Synced Only
 * @param attackerDefID integer? Synced Only
 * @param attackerTeam integer? Synced Only
 *
 * @return number newDamage, number impulseMult
 */
bool CSyncedLuaHandle::UnitPreDamaged(
	const CUnit* unit,
	const CUnit* attacker,
	float damage,
	int weaponDefID,
	int projectileID,
	bool paralyzer,
	float* newDamage,
	float* impulseMult
) {
	RECOIL_DETAILED_TRACY_ZONE;
	LUA_CALL_IN_CHECK(L, false);
	luaL_checkstack(L, 2 + 2 + 10, __func__);

	const LuaUtils::ScopedDebugTraceBack dbgTrace(L);
	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return false;

	int inArgCount = 5;
	int outArgCount = 2;

	lua_pushnumber(L, unit->id);
	lua_pushnumber(L, unit->unitDef->id);
	lua_pushnumber(L, unit->team);
	lua_pushnumber(L, damage);
	lua_pushboolean(L, paralyzer);
	//FIXME pass impulse too?

	if (GetHandleFullRead(L)) {
		lua_pushnumber(L, weaponDefID); inArgCount += 1;
		lua_pushnumber(L, projectileID); inArgCount += 1;

		if (attacker != nullptr) {
			lua_pushnumber(L, attacker->id);
			lua_pushnumber(L, attacker->unitDef->id);
			lua_pushnumber(L, attacker->team);
			inArgCount += 3;
		}
	}

	// call the routine
	// NOTE:
	//   RunCallInTraceback removes the error-handler by default
	//   this has to be disabled when using ScopedDebugTraceBack
	//   or it would mess up the stack
	if (!RunCallInTraceback(L, cmdStr, inArgCount, outArgCount, dbgTrace.GetErrFuncIdx(), false))
		return false;

	assert(newDamage != nullptr);
	assert(impulseMult != nullptr);

	if (lua_isnumber(L, -2)) {
		*newDamage = lua_tonumber(L, -2);
	} else if (!lua_isnumber(L, -2) || lua_isnil(L, -2)) {
		// first value is obligatory, so may not be nil
		LOG_L(L_WARNING, "%s(): 1st return-value should be a number (newDamage)", cmdStr.GetString());
	}

	if (lua_isnumber(L, -1)) {
		*impulseMult = lua_tonumber(L, -1);
	} else if (!lua_isnumber(L, -1) && !lua_isnil(L, -1)) {
		// second value is optional, so nils are OK
		LOG_L(L_WARNING, "%s(): 2nd return-value should be a number (impulseMult)", cmdStr.GetString());
	}

	lua_pop(L, outArgCount);
	// returns true to disable engine dmg handling
	return (*newDamage == 0.0f && *impulseMult == 0.0f);
}


/*** Called before damage is applied to the feature.
 *
 * @function SyncedCallins:FeaturePreDamaged
 *
 * Allows fine control over how much damage and impulse is applied.
 *
 * @param featureID integer
 * @param featureDefID integer
 * @param featureTeam integer
 * @param damage number
 * @param weaponDefID integer
 * @param projectileID integer
 * @param attackerID integer
 * @param attackerDefID integer
 * @param attackerTeam integer
 * @return number newDamage
 * @return number impulseMult
 */
bool CSyncedLuaHandle::FeaturePreDamaged(
	const CFeature* feature,
	const CUnit* attacker,
	float damage,
	int weaponDefID,
	int projectileID,
	float* newDamage,
	float* impulseMult
) {
	RECOIL_DETAILED_TRACY_ZONE;
	assert(newDamage != nullptr);
	assert(impulseMult != nullptr);

	LUA_CALL_IN_CHECK(L, false);
	luaL_checkstack(L, 2 + 9 + 2, __func__);

	const LuaUtils::ScopedDebugTraceBack dbgTrace(L);
	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return false;

	int inArgCount = 4;
	int outArgCount = 2;

	lua_pushnumber(L, feature->id);
	lua_pushnumber(L, feature->def->id);
	lua_pushnumber(L, feature->team);
	lua_pushnumber(L, damage);

	if (GetHandleFullRead(L)) {
		lua_pushnumber(L, weaponDefID); inArgCount += 1;
		lua_pushnumber(L, projectileID); inArgCount += 1;

		if (attacker != nullptr) {
			lua_pushnumber(L, attacker->id);
			lua_pushnumber(L, attacker->unitDef->id);
			lua_pushnumber(L, attacker->team);
			inArgCount += 3;
		}
	}

	// call the routine
	if (!RunCallInTraceback(L, cmdStr, inArgCount, outArgCount, dbgTrace.GetErrFuncIdx(), false))
		return false;

	if (lua_isnumber(L, -2)) {
		*newDamage = lua_tonumber(L, -2);
	} else if (!lua_isnumber(L, -2) || lua_isnil(L, -2)) {
		// first value is obligatory, so may not be nil
		LOG_L(L_WARNING, "%s(): 1st value returned should be a number (newDamage)", cmdStr.GetString());
	}

	if (lua_isnumber(L, -1)) {
		*impulseMult = lua_tonumber(L, -1);
	} else if (!lua_isnumber(L, -1) && !lua_isnil(L, -1)) {
		// second value is optional, so nils are OK
		LOG_L(L_WARNING, "%s(): 2nd value returned should be a number (impulseMult)", cmdStr.GetString());
	}

	lua_pop(L, outArgCount);
	// returns true to disable engine dmg handling
	return (*newDamage == 0.0f && *impulseMult == 0.0f);
}

/*** Called before any engine shield-vs-projectile logic executes.
 *
 * @function SyncedCallins:ShieldPreDamaged
 *
 * If the weapon is a hitscan type (BeamLaser or LightningCanon) then proID is nil and beamEmitterWeaponNum and beamEmitterUnitID are populated instead.
 *
 * @param projectileID integer
 * @param projectileOwnerID integer
 * @param shieldWeaponNum integer
 * @param shieldCarrierID integer
 * @param bounceProjectile boolean
 * @param beamEmitterWeaponNum integer
 * @param beamEmitterUnitID integer
 * @param startX number
 * @param startY number
 * @param startZ number
 * @param hitX number
 * @param hitY number
 * @param hitZ number
 *
 * @return boolean if true the gadget handles the collision event and the engine does not remove the projectile
 */
bool CSyncedLuaHandle::ShieldPreDamaged(
	const CProjectile* projectile,
	const CWeapon* shieldEmitter,
	const CUnit* shieldCarrier,
	bool bounceProjectile,
	const CWeapon* beamEmitter,
	const CUnit* beamCarrier,
	const float3& startPos,
	const float3& hitPos
) {
	RECOIL_DETAILED_TRACY_ZONE;
	assert((projectile != nullptr) || ((beamEmitter != nullptr) && (beamCarrier != nullptr)));
	LUA_CALL_IN_CHECK(L, false);
	luaL_checkstack(L, 2 + 7 + 1, __func__);

	const LuaUtils::ScopedDebugTraceBack dbgTrace(L);
	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return false;

	// push the call-in arguments
	if (projectile != nullptr) {
		// Regular projectiles
		lua_pushnumber(L, projectile->id);
		lua_pushnumber(L, projectile->GetOwnerID());
		lua_pushnumber(L, shieldEmitter->weaponNum + LUA_WEAPON_BASE_INDEX);
		lua_pushnumber(L, shieldCarrier->id);
		lua_pushboolean(L, bounceProjectile);
		lua_pushnil(L);
		lua_pushnil(L);
	} else {
		// Beam projectiles
		lua_pushnumber(L, -1);
		lua_pushnumber(L, -1);
		lua_pushnumber(L, shieldEmitter->weaponNum + LUA_WEAPON_BASE_INDEX);
		lua_pushnumber(L, shieldCarrier->id);
		lua_pushboolean(L, bounceProjectile);
		lua_pushnumber(L, beamEmitter->weaponNum + LUA_WEAPON_BASE_INDEX);
		lua_pushnumber(L, beamCarrier->id);
	}

	lua_pushnumber(L, startPos.x);
	lua_pushnumber(L, startPos.y);
	lua_pushnumber(L, startPos.z);
	lua_pushnumber(L, hitPos.x);
	lua_pushnumber(L, hitPos.y);
	lua_pushnumber(L, hitPos.z);

	// call the routine
	if (!RunCallInTraceback(L, cmdStr, 13, 1, dbgTrace.GetErrFuncIdx(), false))
		return false;

	// pop the return-value; must be true or false
	const bool ret = luaL_optboolean(L, -1, false);
	lua_pop(L, 1);
	return ret;
}


/*** Determines if this weapon can automatically generate targets itself. See also commandFire weaponDef tag.
 *
 * @function SyncedCallins:AllowWeaponTargetCheck
 *
 * Only called for weaponDefIDs registered via `Script.SetWatchAllowTarget` or `Script.SetWatchWeapon`.
 *
 * @param attackerID integer
 * @param attackerWeaponNum integer
 * @param attackerWeaponDefID integer
 *
 * @return boolean allowCheck
 * @return boolean ignoreCheck
 *
 * @see Script.SetWatchAllowTarget
 * @see Script.SetWatchWeapon
 */
int CSyncedLuaHandle::AllowWeaponTargetCheck(unsigned int attackerID, unsigned int attackerWeaponNum, unsigned int attackerWeaponDefID)
{
	RECOIL_DETAILED_TRACY_ZONE;
	int ret = -1;

	if (!watchAllowTargetDefs[attackerWeaponDefID])
		return ret;

	LUA_CALL_IN_CHECK(L, -1);
	luaL_checkstack(L, 2 + 3 + 1, __func__);

	const LuaUtils::ScopedDebugTraceBack dbgTrace(L);
	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return ret;

	lua_pushnumber(L, attackerID);
	lua_pushnumber(L, attackerWeaponNum + LUA_WEAPON_BASE_INDEX);
	lua_pushnumber(L, attackerWeaponDefID);

	if (!RunCallInTraceback(L, cmdStr, 3, 1, dbgTrace.GetErrFuncIdx(), false))
		return ret;

	ret = lua_toint(L, -1);

	lua_pop(L, 1);
	return ret;
}


/*** Controls blocking of a specific target from being considered during a weapon's periodic auto-targeting sweep.
 *
 * @function SyncedCallins:AllowWeaponTarget
 *
 * Only called for weaponDefIDs registered via `Script.SetWatchAllowTarget` or `Script.SetWatchWeapon`.
 *
 * @param attackerID integer
 * @param targetID integer
 * @param attackerWeaponNum integer
 * @param attackerWeaponDefID integer
 * @param defPriority number
 *
 * @return boolean allowed
 * @return number the new priority for this target (if you don't want to change it, return defPriority). Lower priority targets are targeted first.
 *
 * @see Script.SetWatchAllowTarget
 * @see Script.SetWatchWeapon
 */
bool CSyncedLuaHandle::AllowWeaponTarget(
	unsigned int attackerID,
	unsigned int targetID,
	unsigned int attackerWeaponNum,
	unsigned int attackerWeaponDefID,
	float* targetPriority
) {
	RECOIL_DETAILED_TRACY_ZONE;
	bool ret = true;

	if (!watchAllowTargetDefs[attackerWeaponDefID])
		return ret;

	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 2 + 5 + 2, __func__);

	const LuaUtils::ScopedDebugTraceBack dbgTrace(L);
	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return ret;

	// casts are only here to preserve -1's passed from *CAI as floats
	const int siTargetID = static_cast<int>(targetID);
	const int siAttackerWN = static_cast<int>(attackerWeaponNum);

	lua_pushnumber(L, attackerID);
	lua_pushnumber(L, siTargetID);
	lua_pushnumber(L, siAttackerWN + LUA_WEAPON_BASE_INDEX * (siAttackerWN >= 0));
	lua_pushnumber(L, attackerWeaponDefID);

	if (targetPriority != nullptr) {
		// Weapon::AutoTarget
		lua_pushnumber(L, *targetPriority);
	} else {
		// {Air,Mobile}CAI::AutoGenerateTarget
		lua_pushnil(L);
	}

	if (!RunCallInTraceback(L, cmdStr, 5, 2, dbgTrace.GetErrFuncIdx(), false))
		return ret;

	if (targetPriority != nullptr)
		*targetPriority = luaL_optnumber(L, -1, *targetPriority);

	ret = luaL_optboolean(L, -2, false);

	lua_pop(L, 2);
	return ret;
}


/*** Controls blocking of a specific intercept target from being considered during an interceptor weapon's periodic auto-targeting sweep.
 *
 * @function SyncedCallins:AllowWeaponInterceptTarget
 *
 * Only called for weaponDefIDs registered via `Script.SetWatchAllowTarget` or `Script.SetWatchWeapon`.
 *
 * @param interceptorUnitID integer
 * @param interceptorWeaponID integer
 * @param targetProjectileID integer
 *
 * @return boolean allowed
 *
 * @see Script.SetWatchAllowTarget
 * @see Script.SetWatchWeapon
 */
bool CSyncedLuaHandle::AllowWeaponInterceptTarget(
	const CUnit* interceptorUnit,
	const CWeapon* interceptorWeapon,
	const CProjectile* interceptorTarget
) {
	RECOIL_DETAILED_TRACY_ZONE;
	bool ret = true;

	if (!watchAllowTargetDefs[interceptorWeapon->weaponDef->id])
		return ret;

	LUA_CALL_IN_CHECK(L, true);
	luaL_checkstack(L, 2 + 3 + 1, __func__);

	const LuaUtils::ScopedDebugTraceBack dbgTrace(L);
	static const LuaHashString cmdStr(__func__);

	if (!cmdStr.GetGlobalFunc(L))
		return ret;

	lua_pushnumber(L, interceptorUnit->id);
	lua_pushnumber(L, interceptorWeapon->weaponNum + LUA_WEAPON_BASE_INDEX);
	lua_pushnumber(L, interceptorTarget->id);

	if (!RunCallInTraceback(L, cmdStr, 3, 1, dbgTrace.GetErrFuncIdx(), false))
		return ret;

	ret = luaL_optboolean(L, -1, false);

	lua_pop(L, 1);
	return ret;
}


//
// Call-Outs
//

int CSyncedLuaHandle::SyncedRandom(lua_State* L)
{
	RECOIL_DETAILED_TRACY_ZONE;
	#if 0
	spring_lua_synced_rand(L);
	return 1;
	#endif

	switch (lua_gettop(L)) {
		case 0: {
			lua_pushnumber(L, gsRNG.NextFloat());
			return 1;
		} break;

		case 1: {
			if (lua_isnumber(L, 1)) {
				const int u = lua_toint(L, 1);

				if (u < 1)
					luaL_error(L, "error: too small upper limit (%d) given to math.random(), should be >= 1 {synced}", u);

				lua_pushnumber(L, 1 + gsRNG.NextInt(u));
				return 1;
			}
		} break;

		case 2: {
			if (lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
				const int lower = lua_toint(L, 1);
				const int upper = lua_toint(L, 2);

				if (lower > upper)
					luaL_error(L, "Empty interval in math.random() {synced}");

				const float diff  = (upper - lower);
				const float r = gsRNG.NextFloat(); // [0,1], not [0,1) ?

				lua_pushnumber(L, std::clamp(lower + int(r * (diff + 1)), lower, upper));
				return 1;
			}
		} break;

		default: {
		} break;
	}

	luaL_error(L, "Incorrect arguments to math.random() {synced}");
	return 0;
}

int CSyncedLuaHandle::SyncedRandomSeed(lua_State* L)
{
	RECOIL_DETAILED_TRACY_ZONE;
	gsRNG.SetSeed(luaL_checkint(L, -1), false);
	return 0;
}


int CSyncedLuaHandle::SyncedNext(lua_State* L)
{
	RECOIL_DETAILED_TRACY_ZONE;
	constexpr int whiteList[] = {
		LUA_TSTRING,
		LUA_TNUMBER,
		LUA_TBOOLEAN,
		LUA_TNIL,
		LUA_TTHREAD //FIXME LUA_TTHREAD is normally _not_ synced safe but LUS handler needs it atm (and uses it in a safe way)
	};

	const CSyncedLuaHandle* slh = GetSyncedHandle(L);
	assert(slh->origNextRef > 0);
	const int oldTop = lua_gettop(L);

	lua_rawgeti(L, LUA_REGISTRYINDEX, slh->origNextRef);
	lua_pushvalue(L, 1);

	if (oldTop >= 2) {
		lua_pushvalue(L, 2);
	} else {
		lua_pushnil(L);
	}

	lua_call(L, 2, LUA_MULTRET);
	const int retCount = lua_gettop(L) - oldTop;
	assert(retCount == 1 || retCount == 2);

	if (retCount >= 2) {
		const int keyType = lua_type(L, -2);
		const auto it = std::find(std::begin(whiteList), std::end(whiteList), keyType);

		if (it == std::end(whiteList)) {
			if (LuaUtils::PushDebugTraceback(L) > 0) {
				lua_pushfstring(L, "Iterating a table with keys of type \"%s\" in synced context!", lua_typename(L, keyType));
				lua_call(L, 1, 1);

				const auto* errMsg = lua_tostring(L, -1);
				LOG_L(L_WARNING, "%s", errMsg);
			}
			lua_pop(L, 1); // either nil or the errMsg
		}
	}

	return retCount;
}

int CSyncedLuaHandle::SyncedPairs(lua_State* L)
{
	RECOIL_DETAILED_TRACY_ZONE;
	/* copied from lbaselib.cpp */
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_pushcfunction(L, SyncedNext);  /* return generator, */
	lua_pushvalue(L, 1);  /* state, */
	lua_pushnil(L);  /* and initial value */
	return 3;
}


/***
 * Invoke `UnsyncedCallins:RecvFromSynced` callin with the given arguments.
 * 
 * @function SyncedCallins.SendToUnsynced
 *
 * @param ... nil|boolean|number|string|table Arguments. Typically the first argument is the name of a function to call.
 *
 * Argument tables will be recursively copied and stripped of unsupported types and metatables.
 *
 * @see UnsyncedCallins:RecvFromSynced
 */
int CSyncedLuaHandle::SendToUnsynced(lua_State* L)
{
	RECOIL_DETAILED_TRACY_ZONE;
	const int args = lua_gettop(L);
	if (args <= 0) {
		luaL_error(L, "Incorrect arguments to SendToUnsynced()");
	}

	static const int supportedTypes =
		  (1 << LUA_TNIL)
		| (1 << LUA_TBOOLEAN)
		| (1 << LUA_TNUMBER)
		| (1 << LUA_TSTRING)
		| (1 << LUA_TTABLE)
	;

	for (int i = 1; i <= args; i++) {
		const int t = (1 << lua_type(L, i));
		if (!(t & supportedTypes)) {
			luaL_error(L, "Incorrect data type for SendToUnsynced(), arg %d", i);
		}
	}

	CUnsyncedLuaHandle* ulh = CSplitLuaHandle::GetUnsyncedHandle(L);
	ulh->RecvFromSynced(L, args);
	return 0;
}


int CSyncedLuaHandle::AddSyncedActionFallback(lua_State* L)
{
	RECOIL_DETAILED_TRACY_ZONE;
	std::string cmdRaw = "/" + std::string(luaL_checkstring(L, 1));
	std::string cmd = cmdRaw;

	const std::string::size_type pos = cmdRaw.find_first_of(" \t");

	if (pos != string::npos)
		cmd.resize(pos);

	if (cmd.empty()) {
		lua_pushboolean(L, false);
		return 1;
	}

	auto lhs = GetSyncedHandle(L);
	lhs->textCommands[cmd] = luaL_checkstring(L, 2);
	wordCompletion.AddWord(cmdRaw, true, false, false);
	lua_pushboolean(L, true);
	return 1;
}


int CSyncedLuaHandle::RemoveSyncedActionFallback(lua_State* L)
{
	RECOIL_DETAILED_TRACY_ZONE;
	//TODO move to LuaHandle
	std::string cmdRaw = "/" + std::string(luaL_checkstring(L, 1));
	std::string cmd = cmdRaw;

	const std::string::size_type pos = cmdRaw.find_first_of(" \t");

	if (pos != string::npos)
		cmd.resize(pos);

	if (cmd.empty()) {
		lua_pushboolean(L, false);
		return 1;
	}

	auto lhs = GetSyncedHandle(L);
	auto& cmds = lhs->textCommands;

	const auto it = cmds.find(cmd);

	if (it != cmds.end()) {
		cmds.erase(it);
		wordCompletion.RemoveWord(cmdRaw);
		lua_pushboolean(L, true);
	} else {
		lua_pushboolean(L, false);
	}

	return 1;
}



#define GetWatchDef(HandleType, DefType)                                                \
	int C ## HandleType ## LuaHandle::GetWatch ## DefType ## Def(lua_State* L) {        \
		const C ## HandleType ## LuaHandle* lhs = Get ## HandleType ## Handle(L);                   \
		const auto& vec = lhs->watch ## DefType ## Defs;                    \
                                                                            \
		const uint32_t defIdx = luaL_checkint(L, 1);                        \
                                                                            \
		if ((defIdx == -1u) && (&vec == &lhs->watchProjectileDefs)) {       \
			lua_pushboolean(L, vec[vec.size() - 1]);                        \
			return 1;                                                       \
		}                                                                   \
                                                                            \
		if (defIdx >= vec.size())                                           \
			return 0;                                                       \
                                                                            \
		lua_pushboolean(L, vec[defIdx]);                                    \
		return 1;                                                           \
	}

#define SetWatchDef(HandleType, DefType)                                                \
	int C ## HandleType ## LuaHandle::SetWatch ## DefType ## Def(lua_State* L) {        \
		C ## HandleType ## LuaHandle* lhs = Get ## HandleType ## Handle(L);                         \
		auto& vec = lhs->watch ## DefType ## Defs;                          \
                                                                            \
		const uint32_t defIdx = luaL_checkint(L, 1);                        \
                                                                            \
		if ((defIdx == -1u) && (&vec == &lhs->watchProjectileDefs)) {       \
			vec[vec.size() - 1] = luaL_checkboolean(L, 2);                  \
			return 0;                                                       \
		}                                                                   \
                                                                            \
		if (defIdx >= vec.size())                                           \
			return 0;                                                       \
                                                                            \
		vec[defIdx] = luaL_checkboolean(L, 2);                              \
		return 0;                                                           \
	}

int CSyncedLuaHandle::GetWatchWeaponDef(lua_State* L) {
	RECOIL_DETAILED_TRACY_ZONE;
	bool watched = false;

	// trickery to keep Script.GetWatchWeapon backward-compatible
	{
		GetWatchExplosionDef(L);
		watched |= luaL_checkboolean(L, -1);
		lua_pop(L, 1);
	}
	{
		GetWatchProjectileDef(L);
		watched |= luaL_checkboolean(L, -1);
		lua_pop(L, 1);
	}
	{
		GetWatchAllowTargetDef(L);
		watched |= luaL_checkboolean(L, -1);
		lua_pop(L, 1);
	}

	lua_pushboolean(L, watched);
	return 1;
}

/*** Register/deregister callins working per defID.
 *
 * Some of the engine callins can result in so many callins the engine doesn't forward them until registered
 * through the following SetWatch* methods.
 *
 * The GetWatch* methods can be used to query the currently registered defIDs.
 *
 * @section watch_methods
 */

/*** Query whether any callins are registered for a unitDefID.
 *
 * @function Script.GetWatchUnit
 *
 * @param unitDefID integer
 * @return boolean watched Watch status.
 *
 * @see Script.SetWatchUnit
 */

GetWatchDef(Synced, Unit)


/*** Query whether any callins are registered for a featureDefID.
 *
 * @function Script.GetWatchFeature
 *
 * @param featureDefID integer
 * @return boolean watched `true` if callins are registered, otherwise `false`.
 *
 * @see Script.SetWatchFeature
 */

GetWatchDef(Synced, Feature)


/*** Query whether any callins are registered for a weaponDefID.
 *
 * @function Script.GetWatchWeapon
 *
 * Same as calling:
 * ```lua
 * Script.GetWatchExplosion(weaponDefID) or Script.GetWatchProjectile(weaponDefID) or Script.GetWatchAllowTarget(weaponDefID)
 * ```
 *
 * @param weaponDefID integer
 * @return boolean watched True if watch is enabled for any weaponDefID callins.
 *
 * @see Script.SetWatchWeapon
 */

/*** Query whether explosion callins are registered for a weaponDefID.
 *
 * @function Script.GetWatchExplosion
 *
 * @param weaponDefID integer
 * @return boolean watched `true` if callins are registered, otherwise `false`.
 *
 * @see Script.SetWatchExplosion
 */

GetWatchDef(Synced, Explosion)
GetWatchDef(Unsynced, Explosion)


/*** Query whether projectile callins are registered for a weaponDefID.
 *
 * @function Script.GetWatchProjectile
 *
 * @param weaponDefID integer
 * @return boolean watched `true` if callins are registered, otherwise `false`.
 *
 * @see Script.SetWatchProjectile
 */

GetWatchDef(Synced, Projectile)


/*** Query whether weapon targeting callins are registered for a weaponDefID.
 *
 * @function Script.GetWatchAllowTarget
 *
 * @param weaponDefID integer
 * @return boolean watched `true` if callins are registered, otherwise `false`.
 *
 * @see Script.SetWatchAllowTarget
 */

GetWatchDef(Synced, AllowTarget)


/*** Register or deregister unitDefID for expensive callins.
 *
 * @function Script.SetWatchUnit
 *
 * @param unitDefID integer
 * @param watch boolean Whether to register or deregister.
 *
 * @see Script.GetWatchUnit
 * @see Callins:UnitFeatureCollision
 * @see Callins:UnitUnitCollision
 * @see Callins:UnitMoveFailed
 */

SetWatchDef(Synced, Unit)


/*** Register or deregister featureDefID for expensive callins.
 *
 * @function Script.SetWatchFeature
 *
 * @param featureDefID integer
 * @param watch boolean Whether to register or deregister.
 *
 * @see Script.GetWatchFeature
 * @see Callins:UnitFeatureCollision
 */

SetWatchDef(Synced, Feature)


/*** Register or deregister weaponDefID for all expensive callins.
 *
 * @function Script.SetWatchWeapon
 *
 * Equivalent to calling:
 *
 * ```lua
 * Script.SetWatchExplosion(weaponDefID)
 * Script.SetWatchProjectile(weaponDefID)
 * Script.SetWatchAllowTarget(weaponDefID)
 * ```
 *
 * Generally it's better to use those methods to avoid registering uneeded callins.
 *
 * @param weaponDefID integer
 * @param watch boolean Whether to register or deregister.
 *
 * @see Script.GetWatchWeapon
 * @see Script.SetWatchExplosion
 * @see Script.SetWatchProjectile
 * @see Script.SetWatchAllowTarget
 */

/*** Register or deregister weaponDefID for explosion callins.
 *
 * @function Script.SetWatchExplosion
 *
 * @param weaponDefID integer
 * @param watch boolean Whether to register or deregister.
 *
 * @see Script.GetWatchExplosion
 * @see Callins:Explosion
 */

SetWatchDef(Synced, Explosion)
SetWatchDef(Unsynced, Explosion)


/*** Register or deregister weaponDefID for expensive projectile callins.
 *
 * @function Script.SetWatchProjectile
 *
 * @param weaponDefID integer weaponDefID for weapons or -1 to watch for debris.
 * @param watch boolean Whether to register or deregister.
 *
 * @see Script.GetWatchProjectile
 * @see Callins:ProjectileCreated
 * @see Callins:ProjectileDestroyed
 */

SetWatchDef(Synced, Projectile)


/*** Register or deregister weaponDefID for weapon targeting callins.
 *
 * @function Script.SetWatchAllowTarget
 *
 * @param weaponDefID integer
 * @param watch boolean Whether to register or deregister.
 *
 * @see Script.GetWatchAllowTarget
 * @see SyncedCallins:AllowWeaponTargetCheck
 * @see SyncedCallins:AllowWeaponTarget
 * @see SyncedCallins:AllowWeaponInterceptTarget
 */

SetWatchDef(Synced, AllowTarget)

#undef GetWatchDef
#undef SetWatchDef


/******************************************************************************/
/******************************************************************************/
//  ######  ##     ##    ###    ########  ######## ########
// ##    ## ##     ##   ## ##   ##     ## ##       ##     ##
// ##       ##     ##  ##   ##  ##     ## ##       ##     ##
//  ######  ######### ##     ## ########  ######   ##     ##
//       ## ##     ## ######### ##   ##   ##       ##     ##
// ##    ## ##     ## ##     ## ##    ##  ##       ##     ##
//  ######  ##     ## ##     ## ##     ## ######## ########

CSplitLuaHandle::CSplitLuaHandle(const std::string& _name, int _order)
	: syncedLuaHandle(this, _name, _order)
	, unsyncedLuaHandle(this, _name, _order + 1)
{
}


CSplitLuaHandle::~CSplitLuaHandle()
{
	RECOIL_DETAILED_TRACY_ZONE;
	// must be called before their dtors!!!
	syncedLuaHandle.KillLua();
	unsyncedLuaHandle.KillLua();
}


bool CSplitLuaHandle::InitSynced(bool dryRun)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!IsValid()) {
		KillLua();
		return false;
	}

	std::string syncedCode;
	if (!dryRun) {
		syncedCode = LoadFile(GetSyncedFileName(), GetInitFileModes());
		if (syncedCode.empty()) {
			KillLua();
			return false;
		}
	}

	auto lock = CLoadLock::GetUniqueLock(); // for loading of models
	const bool haveSynced = syncedLuaHandle.Init(std::move(syncedCode), GetSyncedFileName());

	if (!IsValid() || !haveSynced) {
		KillLua();
		return false;
	}

	syncedLuaHandle.CheckStack();
	return true;
}


bool CSplitLuaHandle::InitUnsynced()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!IsValid()) {
		KillLua();
		return false;
	}

	std::string unsyncedCode = LoadFile(GetUnsyncedFileName(), GetInitFileModes());
	if (unsyncedCode.empty()) {
		KillLua();
		return false;
	}

	auto lock = CLoadLock::GetUniqueLock();
	const bool haveUnsynced = unsyncedLuaHandle.Init(std::move(unsyncedCode), GetUnsyncedFileName());

	if (!IsValid() || !haveUnsynced) {
		KillLua();
		return false;
	}

	unsyncedLuaHandle.CheckStack();
	return true;
}


bool CSplitLuaHandle::Init(bool dryRun)
{
	RECOIL_DETAILED_TRACY_ZONE;
	SetFullCtrl(true);
	SetFullRead(true);
	SetCtrlTeam(CEventClient::AllAccessTeam);
	SetReadTeam(CEventClient::AllAccessTeam);
	SetReadAllyTeam(CEventClient::AllAccessTeam);
	SetSelectTeam(GetInitSelectTeam());

	return InitSynced(dryRun) && (dryRun || InitUnsynced());
}


bool CSplitLuaHandle::FreeUnsynced()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!unsyncedLuaHandle.IsValid())
		return false;

	unsyncedLuaHandle.KillLua();
	unsyncedLuaHandle.~CUnsyncedLuaHandle();

	return true;
}


bool CSplitLuaHandle::LoadUnsynced()
{
	RECOIL_DETAILED_TRACY_ZONE;
	::new (&unsyncedLuaHandle) CUnsyncedLuaHandle(this, syncedLuaHandle.GetName(), syncedLuaHandle.GetOrder() + 1);

	if (!unsyncedLuaHandle.IsValid()) {
		unsyncedLuaHandle.KillLua();
		return false;
	}

	return InitUnsynced();
}


bool CSplitLuaHandle::SwapSyncedHandle(lua_State* L, lua_State* L_GC)
{
	RECOIL_DETAILED_TRACY_ZONE;
	eventHandler.RemoveClient(&syncedLuaHandle);

	LUA_CLOSE(&syncedLuaHandle.L);
	syncedLuaHandle.SetLuaStates(L, L_GC);

	if (!IsValid()) {
		return false;
	}

	// update which callins are present in the new state
	eventHandler.AddClient(&syncedLuaHandle);
	return true;
}


string CSplitLuaHandle::LoadFile(const std::string& filename, const std::string& modes) const
{
	RECOIL_DETAILED_TRACY_ZONE;
	string vfsModes(modes);
	if (CSyncedLuaHandle::devMode)
		vfsModes = SPRING_VFS_RAW + vfsModes;

	CFileHandler f(filename, vfsModes);
	string code;
	if (!f.LoadStringData(code))
		code.clear();

	return code;
}

//
// Call-Outs
//

/***
 * @class CallAsTeamOptions
 * @x_helper
 * @field ctrl integer Ctrl team ID.
 * @field read integer Read team ID.
 * @field select integer Select team ID.
 */

/*** Calls a function from given team's PoV. In particular this makes callouts obey that team's visibility rules.
 *
 * @function Spring.CallAsTeam
 * @param teamID integer Team ID.
 * @param func fun(...) The function to call.
 * @param ... any Arguments to pass to the function.
 * @return any ... The return values of the function.
 */
/***
 * @function Spring.CallAsTeam
 * @param options CallAsTeamOptions Options.
 * @param func fun(...) The function to call.
 * @param ... any Arguments to pass to the function.
 * @return any ... The return values of the function.
 */
int CSplitLuaHandle::CallAsTeam(lua_State* L)
{
	RECOIL_DETAILED_TRACY_ZONE;
	const int args = lua_gettop(L);
	if ((args < 2) || !lua_isfunction(L, 2))
		luaL_error(L, "[%s] incorrect arguments", __func__);

	// save the current access
	const bool prevFullCtrl    = CLuaHandle::GetHandleFullCtrl(L);
	const bool prevFullRead    = CLuaHandle::GetHandleFullRead(L);
	const int prevCtrlTeam     = CLuaHandle::GetHandleCtrlTeam(L);
	const int prevReadTeam     = CLuaHandle::GetHandleReadTeam(L);
	const int prevReadAllyTeam = CLuaHandle::GetHandleReadAllyTeam(L);
	const int prevSelectTeam   = CLuaHandle::GetHandleSelectTeam(L);

	// parse the new access
	if (lua_isnumber(L, 1)) {
		const int teamID = lua_toint(L, 1);

		if ((teamID < CEventClient::MinSpecialTeam) || (teamID >= teamHandler.ActiveTeams()))
			luaL_error(L, "[%s] bad teamID %d", __func__, teamID);

		// ctrl
		CLuaHandle::SetHandleCtrlTeam(L, teamID);
		CLuaHandle::SetHandleFullCtrl(L, teamID == CEventClient::AllAccessTeam);
		// read
		CLuaHandle::SetHandleReadTeam(L, teamID);
		CLuaHandle::SetHandleReadAllyTeam(L, (teamID < 0) ? teamID : teamHandler.AllyTeam(teamID));
		CLuaHandle::SetHandleFullRead(L, teamID == CEventClient::AllAccessTeam);
		// select
		CLuaHandle::SetHandleSelectTeam(L, teamID);
	}
	else if (lua_istable(L, 1)) {
		const int table = 1;
		for (lua_pushnil(L); lua_next(L, table) != 0; lua_pop(L, 1)) {
			if (!lua_israwstring(L, -2) || !lua_isnumber(L, -1))
				continue;

			const int teamID = lua_toint(L, -1);

			if ((teamID < CEventClient::MinSpecialTeam) || (teamID >= teamHandler.ActiveTeams()))
				luaL_error(L, "[%s] bad teamID %d", __func__, teamID);

			switch (hashString(lua_tostring(L, -2))) {
				case hashString("ctrl"): {
					CLuaHandle::SetHandleCtrlTeam(L, teamID);
					CLuaHandle::SetHandleFullCtrl(L, teamID == CEventClient::AllAccessTeam);
				} break;
				case hashString("read"): {
					CLuaHandle::SetHandleReadTeam(L, teamID);
					CLuaHandle::SetHandleReadAllyTeam(L, (teamID < 0) ? teamID : teamHandler.AllyTeam(teamID));
					CLuaHandle::SetHandleFullRead(L, teamID == CEventClient::AllAccessTeam);
				} break;
				case hashString("select"): {
					CLuaHandle::SetHandleSelectTeam(L, teamID);
				} break;
			}
		}
	}
	else {
		luaL_error(L, "Incorrect arguments to CallAsTeam()");
	}

	// call the function
	const int funcArgs = lua_gettop(L) - 2;

	// protected call so that the permissions are always reverted
	const int error = lua_pcall(L, funcArgs, LUA_MULTRET, 0);

	// revert the permissions
	CLuaHandle::SetHandleFullCtrl(L, prevFullCtrl);
	CLuaHandle::SetHandleFullRead(L, prevFullRead);
	CLuaHandle::SetHandleCtrlTeam(L, prevCtrlTeam);
	CLuaHandle::SetHandleReadTeam(L, prevReadTeam);
	CLuaHandle::SetHandleReadAllyTeam(L, prevReadAllyTeam);
	CLuaHandle::SetHandleSelectTeam(L, prevSelectTeam);

	if (error != 0) {
		LOG_L(L_ERROR, "[%s] error %i (%s)", __func__, error, lua_tostring(L, -1));
		lua_error(L);
	}

	return lua_gettop(L) - 1;	// the teamID/table is still on the stack
}



/******************************************************************************/
/******************************************************************************/
