/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "LuaUnsyncedCtrl.h"

#include "Game/Camera/DollyController.h"
#include "LuaConfig.h"
#include "LuaInclude.h"
#include "LuaHandle.h"
#include "LuaHashString.h"
#include "LuaMenu.h"
#include "LuaOpenGLUtils.h"
#include "LuaParser.h"
#include "LuaTextures.h"
#include "LuaUtils.h"

#include "ExternalAI/EngineOutHandler.h"
#include "ExternalAI/SkirmishAIHandler.h"
#include "Game/Game.h"
#include "Game/Camera.h"
#include "Game/CameraHandler.h"
#include "Game/Camera/CameraController.h"
#include "Game/ChatMessage.h"
#include "Game/GameSetup.h"
#include "Game/GlobalUnsynced.h"
#include "Game/IVideoCapturing.h"
#include "Game/SelectedUnitsHandler.h"
#include "Game/Players/Player.h"
#include "Game/Players/PlayerHandler.h"
#include "Game/InMapDraw.h"
#include "Game/InMapDrawModel.h"
#include "Game/UI/CommandColors.h"
#include "Game/UI/CursorIcons.h"
#include "Game/UI/GuiHandler.h"
#include "Game/UI/InfoConsole.h"
#include "Game/UI/KeyCodes.h"
#include "Game/UI/KeySet.h"
#include "Game/UI/KeyBindings.h"
#include "Game/UI/MiniMap.h"
#include "Game/UI/MouseHandler.h"
#include "Game/UI/Groups/Group.h"
#include "Game/UI/Groups/GroupHandler.h"
#include "Map/MapInfo.h"
#include "Map/ReadMap.h"
#include "Map/BaseGroundDrawer.h"
#include "Map/BaseGroundTextures.h"
#include "Map/SMF/SMFGroundDrawer.h"
#include "Map/SMF/ROAM/RoamMeshDrawer.h"
#include "Net/Protocol/NetProtocol.h"
#include "Net/GameServer.h"
#include "Rendering/Env/ISky.h"
#include "Rendering/Env/SunLighting.h"
#include "Rendering/Env/WaterRendering.h"
#include "Rendering/Env/MapRendering.h"
#include "Rendering/Env/IGroundDecalDrawer.h"
#include "Rendering/Env/Particles/Classes/NanoProjectile.h"
#include "Rendering/GL/myGL.h"
#include "Rendering/CommandDrawer.h"
#include "Rendering/IconHandler.h"
#include "Rendering/Models/3DModel.h"
#include "Rendering/Models/IModelParser.h"
#include "Rendering/Features/FeatureDrawer.h"
#include "Rendering/Units/UnitDrawer.h"
#include "Rendering/Map/InfoTexture/IInfoTextureHandler.h"
#include "Rendering/Textures/Bitmap.h"
#include "Rendering/Textures/NamedTextures.h"
#include "Sim/Features/FeatureDef.h"
#include "Sim/Features/FeatureDefHandler.h"
#include "Sim/Features/FeatureHandler.h"
#include "Sim/Misc/TeamHandler.h"
#include "Sim/Projectiles/Projectile.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Units/Unit.h"
#include "Sim/Units/UnitDefHandler.h"
#include "Sim/Units/UnitHandler.h"
#include "System/Config/ConfigHandler.h"
#include "System/EventHandler.h"
#include "System/GlobalConfig.h"
#include "System/Log/DefaultFilter.h"
#include "System/Log/ILog.h"
#include "System/Net/PackPacket.h"
#include "System/Platform/Misc.h"
#include "System/SafeUtil.h"
#include "System/UnorderedMap.hpp"
#include "System/StringUtil.h"
#include "System/Sound/ISound.h"
#include "System/Sound/ISoundChannels.h"
#include "System/FileSystem/FileHandler.h"
#include "System/FileSystem/DataDirLocater.h"
#include "System/FileSystem/FileSystem.h"
#include "System/Platform/Watchdog.h"
#include "System/Platform/WindowManagerHelper.h"
#include "System/SpringHash.h"
#include "System/LoadLock.h"


#if !defined(HEADLESS) && !defined(NO_SOUND)
#include "System/Sound/OpenAL/EFX.h"
#include "System/Sound/OpenAL/EFXPresets.h"
#endif

#include <cctype>
#include <cfloat>
#include <cinttypes>

#include <fstream>

#include <SDL_keyboard.h>
#include <SDL_clipboard.h>
#include <SDL_mouse.h>

// MinGW defines this for a WINAPI function
#undef SendMessage
#undef CreateDirectory
#undef Yield


/******************************************************************************
 * Callouts to set state
 *
 * @see rts/Lua/LuaUnsyncedCtrl.cpp
******************************************************************************/


bool LuaUnsyncedCtrl::PushEntries(lua_State* L)
{
	REGISTER_LUA_CFUNC(Ping);
	REGISTER_LUA_CFUNC(Echo);
	REGISTER_LUA_CFUNC(Log);

	REGISTER_LUA_CFUNC(SendMessage);
	REGISTER_LUA_CFUNC(SendMessageToPlayer);
	REGISTER_LUA_CFUNC(SendMessageToTeam);
	REGISTER_LUA_CFUNC(SendMessageToAllyTeam);
	REGISTER_LUA_CFUNC(SendMessageToSpectators);

	REGISTER_LUA_CFUNC(SendPublicChat);
	REGISTER_LUA_CFUNC(SendAllyChat);
	REGISTER_LUA_CFUNC(SendSpectatorChat);
	REGISTER_LUA_CFUNC(SendPrivateChat);

	REGISTER_LUA_CFUNC(LoadSoundDef);
	REGISTER_LUA_CFUNC(PlaySoundFile);
	REGISTER_LUA_CFUNC(PlaySoundStream);
	REGISTER_LUA_CFUNC(StopSoundStream);
	REGISTER_LUA_CFUNC(PauseSoundStream);
	REGISTER_LUA_CFUNC(SetSoundStreamVolume);
	REGISTER_LUA_CFUNC(SetSoundEffectParams);

	REGISTER_LUA_CFUNC(SetCameraState);
	REGISTER_LUA_CFUNC(SetCameraTarget);

	REGISTER_LUA_CFUNC(RunDollyCamera);
	REGISTER_LUA_CFUNC(PauseDollyCamera);
	REGISTER_LUA_CFUNC(ResumeDollyCamera);
	REGISTER_LUA_CFUNC(SetDollyCameraMode);
	REGISTER_LUA_CFUNC(SetDollyCameraPosition);
	REGISTER_LUA_CFUNC(SetDollyCameraCurve);
	REGISTER_LUA_CFUNC(SetDollyCameraLookCurve);
	REGISTER_LUA_CFUNC(SetDollyCameraLookPosition);
	REGISTER_LUA_CFUNC(SetDollyCameraLookUnit);
	REGISTER_LUA_CFUNC(SetDollyCameraRelativeMode);

	REGISTER_LUA_CFUNC(DeselectUnit);
	REGISTER_LUA_CFUNC(DeselectUnitMap);
	REGISTER_LUA_CFUNC(DeselectUnitArray);
	REGISTER_LUA_CFUNC(SelectUnit);
	REGISTER_LUA_CFUNC(SelectUnitMap);
	REGISTER_LUA_CFUNC(SelectUnitArray);
	REGISTER_LUA_CFUNC(SetBoxSelectionByEngine);

	REGISTER_LUA_CFUNC(AddWorldIcon);
	REGISTER_LUA_CFUNC(AddWorldText);
	REGISTER_LUA_CFUNC(AddWorldUnit);

	REGISTER_LUA_CFUNC(DrawUnitCommands);

	REGISTER_LUA_CFUNC(SetTeamColor);

	REGISTER_LUA_CFUNC(AssignMouseCursor);
	REGISTER_LUA_CFUNC(ReplaceMouseCursor);

	REGISTER_LUA_CFUNC(SetCustomCommandDrawData);

	REGISTER_LUA_CFUNC(SetAutoShowMetal);
	REGISTER_LUA_CFUNC(SetDrawSky);
	REGISTER_LUA_CFUNC(SetDrawWater);
	REGISTER_LUA_CFUNC(SetDrawGround);
	REGISTER_LUA_CFUNC(SetDrawGroundDeferred);
	REGISTER_LUA_CFUNC(SetDrawModelsDeferred);
	REGISTER_LUA_CFUNC(SetVideoCapturingMode);
	REGISTER_LUA_CFUNC(SetVideoCapturingTimeOffset);

	REGISTER_LUA_CFUNC(SetWaterParams);

	REGISTER_LUA_CFUNC(AddMapLight);
	REGISTER_LUA_CFUNC(AddModelLight);
	REGISTER_LUA_CFUNC(UpdateMapLight);
	REGISTER_LUA_CFUNC(UpdateModelLight);
	REGISTER_LUA_CFUNC(SetMapLightTrackingState);
	REGISTER_LUA_CFUNC(SetModelLightTrackingState);
	REGISTER_LUA_CFUNC(SetMapShader);
	REGISTER_LUA_CFUNC(SetMapSquareTexture);
	REGISTER_LUA_CFUNC(SetMapShadingTexture);
	REGISTER_LUA_CFUNC(SetSkyBoxTexture);

	REGISTER_LUA_CFUNC(SetUnitNoDraw);
	REGISTER_LUA_CFUNC(SetUnitEngineDrawMask);
	REGISTER_LUA_CFUNC(SetUnitAlwaysUpdateMatrix);
	REGISTER_LUA_CFUNC(SetUnitNoMinimap);
	REGISTER_LUA_CFUNC(SetUnitNoGroup);
	REGISTER_LUA_CFUNC(SetUnitNoSelect);
	REGISTER_LUA_CFUNC(SetUnitLeaveTracks);
	REGISTER_LUA_CFUNC(SetUnitSelectionVolumeData);
	REGISTER_LUA_CFUNC(SetFeatureNoDraw);
	REGISTER_LUA_CFUNC(SetFeatureEngineDrawMask);
	REGISTER_LUA_CFUNC(SetFeatureAlwaysUpdateMatrix);
	REGISTER_LUA_CFUNC(SetFeatureFade);
	REGISTER_LUA_CFUNC(SetFeatureSelectionVolumeData);

	REGISTER_LUA_CFUNC(AddUnitIcon);
	REGISTER_LUA_CFUNC(FreeUnitIcon);
	REGISTER_LUA_CFUNC(UnitIconSetDraw); // deprecated
	REGISTER_LUA_CFUNC(SetUnitIconDraw);


	REGISTER_LUA_CFUNC(ExtractModArchiveFile);

	// moved from LuaUI

//FIXME	REGISTER_LUA_CFUNC(SetShockFrontFactors);

	REGISTER_LUA_CFUNC(SetConfigInt);
	REGISTER_LUA_CFUNC(SetConfigFloat);
	REGISTER_LUA_CFUNC(SetConfigString);

	REGISTER_LUA_CFUNC(CreateDir);

	REGISTER_LUA_CFUNC(SendCommands);
	REGISTER_LUA_CFUNC(GiveOrder);
	REGISTER_LUA_CFUNC(GiveOrderToUnit);
	REGISTER_LUA_CFUNC(GiveOrderToUnitMap);
	REGISTER_LUA_CFUNC(GiveOrderToUnitArray);
	REGISTER_LUA_CFUNC(GiveOrderArrayToUnit);
	REGISTER_LUA_CFUNC(GiveOrderArrayToUnitMap);
	REGISTER_LUA_CFUNC(GiveOrderArrayToUnitArray);

	REGISTER_LUA_CFUNC(SendLuaUIMsg);
	REGISTER_LUA_CFUNC(SendLuaGaiaMsg);
	REGISTER_LUA_CFUNC(SendLuaRulesMsg);
	REGISTER_LUA_CFUNC(SendLuaMenuMsg);

	REGISTER_LUA_CFUNC(LoadCmdColorsConfig);
	REGISTER_LUA_CFUNC(LoadCtrlPanelConfig);

	REGISTER_LUA_CFUNC(SetActiveCommand);
	REGISTER_LUA_CFUNC(ForceLayoutUpdate);

	REGISTER_LUA_CFUNC(SetMouseCursor);
	REGISTER_LUA_CFUNC(WarpMouse);

	REGISTER_LUA_CFUNC(SetClipboard);

	REGISTER_LUA_CFUNC(SetCameraOffset);

	REGISTER_LUA_CFUNC(SetLosViewColors);

	REGISTER_LUA_CFUNC(SetNanoProjectileParams);

	REGISTER_LUA_CFUNC(Reload);
	REGISTER_LUA_CFUNC(Restart);
	REGISTER_LUA_CFUNC(Start);
	REGISTER_LUA_CFUNC(Quit);

	REGISTER_LUA_CFUNC(SetWMIcon);
	REGISTER_LUA_CFUNC(SetWMCaption);

	REGISTER_LUA_CFUNC(SetUnitDefIcon);
	REGISTER_LUA_CFUNC(SetUnitDefImage);

	REGISTER_LUA_CFUNC(SetUnitGroup);

	REGISTER_LUA_CFUNC(SetShareLevel);
	REGISTER_LUA_CFUNC(ShareResources);

	REGISTER_LUA_CFUNC(SetLastMessagePosition);

	REGISTER_LUA_CFUNC(MarkerAddPoint);
	REGISTER_LUA_CFUNC(MarkerAddLine);
	REGISTER_LUA_CFUNC(MarkerErasePosition);

	REGISTER_LUA_CFUNC(SetDrawSelectionInfo);

	REGISTER_LUA_CFUNC(SetBuildSpacing);
	REGISTER_LUA_CFUNC(SetBuildFacing);

	REGISTER_LUA_CFUNC(SetAtmosphere);
	REGISTER_LUA_CFUNC(SetSunLighting);
	REGISTER_LUA_CFUNC(SetSunDirection);
	REGISTER_LUA_CFUNC(SetMapRenderingParams);

	REGISTER_LUA_CFUNC(ForceTesselationUpdate);

	REGISTER_LUA_CFUNC(SendSkirmishAIMessage);

	REGISTER_LUA_CFUNC(SetLogSectionFilterLevel);

	REGISTER_LUA_CFUNC(ClearWatchDogTimer);
	REGISTER_LUA_CFUNC(GarbageCollectCtrl);

	REGISTER_LUA_CFUNC(PreloadUnitDefModel);
	REGISTER_LUA_CFUNC(PreloadFeatureDefModel);
	REGISTER_LUA_CFUNC(PreloadSoundItem);
	REGISTER_LUA_CFUNC(LoadModelTextures);

	REGISTER_LUA_CFUNC(CreateGroundDecal);
	REGISTER_LUA_CFUNC(DestroyGroundDecal);
	REGISTER_LUA_CFUNC(SetGroundDecalPosAndDims);
	REGISTER_LUA_CFUNC(SetGroundDecalQuadPosAndHeight);
	REGISTER_LUA_CFUNC(SetGroundDecalRotation);
	REGISTER_LUA_CFUNC(SetGroundDecalTexture);
	REGISTER_LUA_CFUNC(SetGroundDecalTextureParams);
	REGISTER_LUA_CFUNC(SetGroundDecalAlpha);
	REGISTER_LUA_CFUNC(SetGroundDecalNormal);
	REGISTER_LUA_CFUNC(SetGroundDecalTint);
	REGISTER_LUA_CFUNC(SetGroundDecalMisc);
	REGISTER_LUA_CFUNC(SetGroundDecalCreationFrame);

	REGISTER_LUA_CFUNC(SDLSetTextInputRect);
	REGISTER_LUA_CFUNC(SDLStartTextInput);
	REGISTER_LUA_CFUNC(SDLStopTextInput);

	REGISTER_LUA_CFUNC(SetWindowGeometry);
	REGISTER_LUA_CFUNC(SetWindowMinimized);
	REGISTER_LUA_CFUNC(SetWindowMaximized);
	REGISTER_LUA_CFUNC(SetMiniMapRotation);
	
	REGISTER_LUA_CFUNC(Yield);

	return true;
}


/******************************************************************************/
/******************************************************************************/
//
//  Parsing helpers
//

static inline CProjectile* ParseRawProjectile(lua_State* L, const char* caller, int index, bool synced)
{
	if (!lua_isnumber(L, index)) {
		luaL_error(L, "[%s] projectile ID parameter in %s() not a number\n", __func__, caller);
		return nullptr;
	}

	CProjectile* p = nullptr;

	if (synced) {
		p = projectileHandler.GetProjectileBySyncedID(lua_toint(L, index));
	} else {
		p = projectileHandler.GetProjectileByUnsyncedID(lua_toint(L, index));
	}

	return p;
}


static inline CUnit* ParseRawUnit(lua_State* L, const char* caller, int index)
{
	if (!lua_isnumber(L, index)) {
		luaL_error(L, "[%s] ID parameter in %s() not a number\n", __func__, caller);
		return nullptr;
	}

	return (unitHandler.GetUnit(lua_toint(L, index)));
}

static inline CFeature* ParseRawFeature(lua_State* L, const char* caller, int index)
{
	if (!lua_isnumber(L, index)) {
		luaL_error(L, "[%s] ID parameter in %s() not a number\n", __func__, caller);
		return nullptr;
	}

	return (featureHandler.GetFeature(luaL_checkint(L, index)));
}


static inline CUnit* ParseAllyUnit(lua_State* L, const char* caller, int index)
{
	CUnit* unit = ParseRawUnit(L, caller, index);

	if (unit == nullptr)
		return nullptr;

	if (CLuaHandle::GetHandleReadAllyTeam(L) < 0)
		return (CLuaHandle::GetHandleFullRead(L)? unit: nullptr);

	if (unit->allyteam == CLuaHandle::GetHandleReadAllyTeam(L))
		return unit;

	return nullptr;
}


static inline CUnit* ParseCtrlUnit(lua_State* L, const char* caller, int index)
{
	CUnit* unit = ParseRawUnit(L, caller, index);

	if (unit == nullptr)
		return nullptr;

	if (CanControlTeam(L, unit->team))
		return unit;

	return nullptr;
}

static inline CFeature* ParseCtrlFeature(lua_State* L, const char* caller, int index)
{
	CFeature* feature = ParseRawFeature(L, caller, index);

	if (feature == nullptr)
		return nullptr;

	if (CanControlTeam(L, feature->team))
		return feature;

	return nullptr;
}


static inline CUnit* ParseSelectUnit(lua_State* L, const char* caller, int index)
{
	CUnit* unit = ParseRawUnit(L, caller, index);

	if (unit == nullptr || unit->noSelect)
		return nullptr;

	// NB:
	//   partial god-mode does not set selectTeam to AllAccess (which
	//   is registered in controlledTeams); spectatingFullSelect does
	if (gu->GetMyPlayer()->CanControlTeam(unit->team))
		return unit;

	if (CanSelectTeam(L, unit->team))
		return unit;

	return nullptr;
}



/******************************************************************************
 * Ingame Console
 * @section console
******************************************************************************/


/*** Send a ping request to the server
 *
 * @function Spring.Ping
 *
 * @param pingTag number
 *
 * @return nil
 */
int LuaUnsyncedCtrl::Ping(lua_State* L)
{
	// pre-game ping would not be handled properly, send via GUI
	if (guihandler == nullptr)
		return 0;

	guihandler->RunCustomCommands({"@@netping " + IntToString(luaL_optint(L, 1, 0), "%u")}, false);
	return 0;
}


/* Documented at LuaUtils::Echo */
int LuaUnsyncedCtrl::Echo(lua_State* L)
{
	return LuaUtils::Echo(L);
}

/* Documented at LuaUtils::Log */
int LuaUnsyncedCtrl::Log(lua_State* L)
{
	return LuaUtils::Log(L);
}


/***
 * @function Spring.SendCommands
 * @param commands string[]
 */

/***
 * @function Spring.SendCommands
 * @param command string
 * @param ... string additional commands
 * @return nil
 */
int LuaUnsyncedCtrl::SendCommands(lua_State* L)
{
	if ((guihandler == nullptr) || gs->noHelperAIs)
		return 0;

	vector<string> cmds;

	if (lua_istable(L, 1)) { // old style -- table
		constexpr int tableIdx = 1;
		for (lua_pushnil(L); lua_next(L, tableIdx) != 0; lua_pop(L, 1)) {
			if (lua_israwstring(L, -1)) {
				string action = lua_tostring(L, -1);
				if (action[0] != '@')
					action = "@@" + action;

				cmds.push_back(action);
			}
		}
	}
	else if (lua_israwstring(L, 1)) { // new style -- function parameters
		for (int i = 1; lua_israwstring(L, i); i++) {
			string action = lua_tostring(L, i);
			if (action[0] != '@')
				action = "@@" + action;

			cmds.push_back(action);
		}
	}
	else {
		luaL_error(L, "Incorrect arguments to SendCommands()");
	}

	lua_settop(L, 0); // pop the input arguments

	configHandler->EnableWriting(globalConfig.luaWritableConfigFile);
	guihandler->RunCustomCommands(cmds, false);
	configHandler->EnableWriting(true);
	return 0;
}


static string ParseMessage(lua_State* L, const string& msg)
{
	string::size_type start = msg.find("<PLAYER");
	if (start == string::npos)
		return msg;

	const char* number = msg.c_str() + start + strlen("<PLAYER");
	char* endPtr;
	const int playerID = (int)strtol(number, &endPtr, 10);

	if ((endPtr == number) || (*endPtr != '>'))
		luaL_error(L, "Bad message format: %s", msg.c_str());

	if (!playerHandler.IsValidPlayer(playerID))
		luaL_error(L, "Invalid message playerID: %c", playerID); //FIXME

	const CPlayer* player = playerHandler.Player(playerID);
	if ((player == nullptr) || !player->active || player->name.empty())
		luaL_error(L, "Invalid message playerID: %c", playerID);

	const string head = msg.substr(0, start);
	const string tail = msg.substr(endPtr - msg.c_str() + 1);

	return head + player->name + ParseMessage(L, tail);
}


/******************************************************************************
 * Chat Messages
 * @section chatmessages
******************************************************************************/

/*** Sends a chat message to everyone (players and spectators).
 *
 * @function Spring.SendPublicChat
 * @param message string
 * @return nil
 */
int LuaUnsyncedCtrl::SendPublicChat(lua_State* L) {
	// Check arguments: Expects 1 string argument
	if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
		return luaL_error(L, "Incorrect arguments to Spring.SendPublicChat(message string)");
	}

	game->SendNetChat(luaL_checksstring(L, 1), ChatMessage::TO_EVERYONE);
	return 0;
}

/*** Sends a chat message to the sender's ally team (if a spectator, to other spectators).
 *
 * @function Spring.SendAllyChat
 * @param message string
 * @return nil
 */
int LuaUnsyncedCtrl::SendAllyChat(lua_State* L) {
	if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
		return luaL_error(L, "Incorrect arguments to Spring.SendAllyChat(message string)");
	}

	game->SendNetChat(luaL_checksstring(L, 1), ChatMessage::TO_ALLIES);
	return 0;
}

/*** Sends a chat message to spectators. Works even if you're a player.
 *
 * @function Spring.SendSpectatorChat
 * @param message string
 * @return nil
 */
int LuaUnsyncedCtrl::SendSpectatorChat(lua_State* L) {
	if (lua_gettop(L) != 1 || !lua_isstring(L, 1)) {
		return luaL_error(L, "Incorrect arguments to Spring.SendSpectatorChat(message string)");
	}

	game->SendNetChat(luaL_checksstring(L, 1), ChatMessage::TO_SPECTATORS);
	return 0;
}

/*** Sends a private chat message to a specific player ID.
 *
 * @function Spring.SendPrivateChat
 * @param message string
 * @param playerID integer
 * @return nil
 */
int LuaUnsyncedCtrl::SendPrivateChat(lua_State* L) {
	if (lua_gettop(L) != 2 || !lua_isstring(L, 1))
		return luaL_error(L, "Incorrect arguments to Spring.SendPrivateChat(message string, playerID integer)");

	const int playerID = luaL_checkint(L, 2);
	if (!playerHandler.IsValidPlayer(playerID))
		return luaL_error(L, "Error in function '%s': Invalid Player ID %d", __func__, playerID);

	game->SendNetChat(luaL_checksstring(L, 1), playerID);
	return 0;
}

static void PrintMessage(lua_State* L, const string& msg)
{
	LOG("%s", ParseMessage(L, msg).c_str());
}

/******************************************************************************
 * Messages
 * @section messages
******************************************************************************/


/*** @function Spring.SendMessage
 * @param message string
 * @return nil
 */
int LuaUnsyncedCtrl::SendMessage(lua_State* L)
{
	PrintMessage(L, luaL_checksstring(L, 1));
	return 0;
}


/*** @function Spring.SendMessageToSpectators
 * @param message string ``"`<PLAYER#>`"`` where `#` is a player ID.
 * 
 * This will be replaced with the player's name. e.g.
 * ```lua
 * Spring.SendMessage("`<PLAYER1>` did something") -- "ProRusher did something"
 * ```
 * 
 * @return nil
 */
int LuaUnsyncedCtrl::SendMessageToSpectators(lua_State* L)
{
	if (gu->spectating)
		PrintMessage(L, luaL_checksstring(L, 1));

	return 0;
}


/*** @function Spring.SendMessageToPlayer
 * @param playerID integer
 * @param message string
 * @return nil
 */
int LuaUnsyncedCtrl::SendMessageToPlayer(lua_State* L)
{
	if (luaL_checkint(L, 1) == gu->myPlayerNum)
		PrintMessage(L, luaL_checksstring(L, 2));

	return 0;
}


/*** @function Spring.SendMessageToTeam
 * @param teamID integer
 * @param message string
 * @return nil
 */
int LuaUnsyncedCtrl::SendMessageToTeam(lua_State* L)
{
	if (luaL_checkint(L, 1) == gu->myTeam)
		PrintMessage(L, luaL_checksstring(L, 2));

	return 0;
}


/*** @function Spring.SendMessageToAllyTeam
 * @param allyID integer
 * @param message string
 * @return nil
 */
int LuaUnsyncedCtrl::SendMessageToAllyTeam(lua_State* L)
{
	if (luaL_checkint(L, 1) == gu->myAllyTeam)
		PrintMessage(L, luaL_checksstring(L, 2));

	return 0;
}


/******************************************************************************
 * Sounds
 * @section sounds
******************************************************************************/


/*** Loads a SoundDefs file, the format is the same as in `gamedata/sounds.lua`.
 *
 * @function Spring.LoadSoundDef
 * @param soundfile string
 * @return boolean success
 */
int LuaUnsyncedCtrl::LoadSoundDef(lua_State* L)
{
	LuaParser soundDefsParser(luaL_checksstring(L, 1), SPRING_VFS_ZIP_FIRST, SPRING_VFS_ZIP_FIRST);

	const bool retval = sound->LoadSoundDefs(&soundDefsParser);
	const bool synced = CLuaHandle::GetHandleSynced(L);

	lua_pushboolean(L, retval && !synced);
	return 1;
}

/***
 * @alias SoundChannel
 * | "general" # 0
 * | "battle" # Same as `"sfx" | 1`
 * | "sfx" # Same as `"battle" | 1`
 * | "unitreply" # Same as `"voice" | 2`
 * | "voice" # Same as `"unitreply" | 2`
 * | "userinterface" # Same as "ui" | 3`
 * | "ui" # Same as "userinterface" | 3`
 * | 0 # General
 * | 1 # SFX
 * | 2 # Voice
 * | 3 # User interface
 */

/*** @function Spring.PlaySoundFile
 * @param soundfile string
 * @param volume number? (Default: 1.0)
 * @param posx number?
 * @param posy number?
 * @param posz number?
 * @param speedx number?
 * @param speedy number?
 * @param speedz number?
 * @param channel SoundChannel? (Default: `0|"general"`)
 * @return boolean playSound
 */
int LuaUnsyncedCtrl::PlaySoundFile(lua_State* L)
{
	const int args = lua_gettop(L);
	      int index = 3;

	const unsigned int soundID = sound->GetSoundId(luaL_checksstring(L, 1));

	if (soundID == 0) {
		lua_pushboolean(L, false);
		return 1;
	}

	const float volume = luaL_optfloat(L, 2, 1.0f);

	float3 pos;
	float3 speed;

	if (args >= 5 && lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
		pos = float3(lua_tofloat(L, 3), lua_tofloat(L, 4), lua_tofloat(L, 5));
		index += 3;

		if (args >= 8 && lua_isnumber(L, 6) && lua_isnumber(L, 7) && lua_isnumber(L, 8)) {
			speed = float3(lua_tofloat(L, 6), lua_tofloat(L, 7), lua_tofloat(L, 8));
			index += 3;
		}
	}

	// last argument (with and without pos/speed arguments) is the optional channel
	IAudioChannel* channel = Channels::General;

	if (args >= index) {
		if (lua_isstring(L, index)) {
			switch (hashStringLower(lua_tostring(L, index))) {
				case hashStringLower("battle"):
				case hashStringLower("sfx"   ): {
					channel = Channels::Battle;
				} break;

				case hashStringLower("unitreply"):
				case hashStringLower("voice"    ): {
					channel = Channels::UnitReply;
				} break;

				case hashStringLower("userinterface"):
				case hashStringLower("ui"           ): {
					channel = Channels::UserInterface;
				} break;

				default: {
				} break;
			}
		} else if (lua_isnumber(L, index)) {
			switch (lua_toint(L, index)) {
				case  1: { channel = Channels::Battle       ; } break;
				case  2: { channel = Channels::UnitReply    ; } break;
				case  3: { channel = Channels::UserInterface; } break;
				default: {                                    } break;
			}
		}
	}

	if (index < 6) {
		channel->PlaySample(soundID, volume);

		lua_pushboolean(L, !CLuaHandle::GetHandleSynced(L));
		return 1;
	}

	if (index >= 9) {
		channel->PlaySample(soundID, pos, speed, volume);
	} else {
		channel->PlaySample(soundID, pos, volume);
	}

	lua_pushboolean(L, !CLuaHandle::GetHandleSynced(L));
	return 1;
}


/*** Allows to play an Ogg Vorbis (.OGG) and mp3 compressed sound file.
 *
 * @function Spring.PlaySoundStream
 *
 * Multiple sound streams may be played at once.
 *
 * @param oggfile string
 * @param volume number? (Default: 1.0)
 * @param enqueue boolean?
 *
 * @return boolean success
*/
int LuaUnsyncedCtrl::PlaySoundStream(lua_State* L)
{
	// file, volume, enqueue
	Channels::BGMusic->StreamPlay(luaL_checksstring(L, 1), luaL_optnumber(L, 2, 1.0f), luaL_optboolean(L, 3, false));

	// .ogg files don't have sound ID's generated
	// for them (yet), so we always succeed here
	lua_pushboolean(L, !CLuaHandle::GetHandleSynced(L));
	return 1;
}


/*** Terminates any SoundStream currently running.
 *
 * @function Spring.StopSoundStream
 * @return nil
 */
int LuaUnsyncedCtrl::StopSoundStream(lua_State*)
{
	Channels::BGMusic->StreamStop();
	return 0;
}


/*** Pause any SoundStream currently running.
 *
 * @function Spring.PauseSoundStream
 * @return nil
 */
int LuaUnsyncedCtrl::PauseSoundStream(lua_State*)
{
	Channels::BGMusic->StreamPause();
	return 0;
}


/*** Set volume for SoundStream
 *
 * @function Spring.SetSoundStreamVolume
 * @param volume number
 * @return nil
 */
int LuaUnsyncedCtrl::SetSoundStreamVolume(lua_State* L)
{
	Channels::BGMusic->SetVolume(luaL_checkfloat(L, 1));
	return 0;
}


/*** @function Spring.SetSoundEffectParams
 */
int LuaUnsyncedCtrl::SetSoundEffectParams(lua_State* L)
{
#if !defined(HEADLESS) && !defined(NO_SOUND)
	if (!efx.Supported())
		return 0;

	//! only a preset name given?
	if (lua_israwstring(L, 1)) {
		efx.SetPreset(lua_tostring(L, 1), false);
		return 0;
	}

	if (!lua_istable(L, 1))
		luaL_error(L, "Incorrect arguments to SetSoundEffectParams()");

	//! first parse the 'preset' key (so all following params use it as base and override it)
	lua_pushliteral(L, "preset");
	lua_gettable(L, -2);
	if (lua_israwstring(L, -1)) {
		std::string presetname = lua_tostring(L, -1);
		efx.SetPreset(presetname, false, false);
	}
	lua_pop(L, 1);


	EAXSfxProps& efxprops = efx.sfxProperties;


	//! parse pass filter
	lua_pushliteral(L, "passfilter");
	lua_gettable(L, -2);
	if (lua_istable(L, -1)) {
		for (lua_pushnil(L); lua_next(L, -2) != 0; lua_pop(L, 1)) {
			if (!lua_israwstring(L, -2))
				continue;

			const string key = StringToLower(lua_tostring(L, -2));
			const auto it = nameToALFilterParam.find(key);

			if (it == nameToALFilterParam.end())
				continue;

			ALuint param = it->second;

			if (!lua_isnumber(L, -1))
				continue;
			if (alParamType[param] != EFXParamTypes::FLOAT)
				continue;

			efxprops.filter_props_f[param] = lua_tofloat(L, -1);
		}
	}
	lua_pop(L, 1);

	//! parse EAX Reverb
	lua_pushliteral(L, "reverb");
	lua_gettable(L, -2);
	if (lua_istable(L, -1)) {
		for (lua_pushnil(L); lua_next(L, -2) != 0; lua_pop(L, 1)) {
			if (!lua_israwstring(L, -2))
				continue;

			const string key = StringToLower(lua_tostring(L, -2));
			const auto it = nameToALParam.find(key);

			if (it == nameToALParam.end())
				continue;

			const ALuint param = it->second;

			if (lua_istable(L, -1)) {
				if (alParamType[param] == EFXParamTypes::VECTOR) {
					float3 v;

					if (LuaUtils::ParseFloatArray(L, -1, &v[0], 3) >= 3)
						efxprops.reverb_props_v[param] = v;
				}

				continue;
			}

			if (lua_isnumber(L, -1)) {
				if (alParamType[param] == EFXParamTypes::FLOAT)
					efxprops.reverb_props_f[param] = lua_tofloat(L, -1);

				continue;
			}

			if (lua_isboolean(L, -1)) {
				if (alParamType[param] == EFXParamTypes::BOOL)
					efxprops.reverb_props_i[param] = lua_toboolean(L, -1);

				continue;
			}
		}
	}
	lua_pop(L, 1);

	efx.CommitEffects();
#endif /// !defined(HEADLESS) && !defined(NO_SOUND)

	return 0;
}


/******************************************************************************/
/******************************************************************************/

/***
 *
 * @function Spring.AddWorldIcon
 * @param cmdID integer
 * @param posX number
 * @param posY number
 * @param posZ number
 * @return nil
 */
int LuaUnsyncedCtrl::AddWorldIcon(lua_State* L)
{
	const int cmdID = luaL_checkint(L, 1);
	const float3 pos(luaL_checkfloat(L, 2),
	                 luaL_checkfloat(L, 3),
	                 luaL_checkfloat(L, 4));
	cursorIcons.AddIcon(cmdID, pos);
	return 0;
}


/***
 *
 * @function Spring.AddWorldText
 * @param text string
 * @param posX number
 * @param posY number
 * @param posZ number
 * @return nil
 */
int LuaUnsyncedCtrl::AddWorldText(lua_State* L)
{
	const string text = luaL_checksstring(L, 1);
	const float3 pos(luaL_checkfloat(L, 2),
	                 luaL_checkfloat(L, 3),
	                 luaL_checkfloat(L, 4));
	cursorIcons.AddIconText(text, pos);
	return 0;
}


/***
 *
 * @function Spring.AddWorldUnit
 * @param unitDefID integer
 * @param posX number
 * @param posY number
 * @param posZ number
 * @param teamID integer
 * @param facing FacingInteger
 * @return nil
 */
int LuaUnsyncedCtrl::AddWorldUnit(lua_State* L)
{
	const int unitDefID = luaL_checkint(L, 1);

	if (!unitDefHandler->IsValidUnitDefID(unitDefID))
		return 0;

	const float3 pos(luaL_checkfloat(L, 2),
	                 luaL_checkfloat(L, 3),
	                 luaL_checkfloat(L, 4));

	const int teamId = luaL_checkint(L, 5);
	const int facing = luaL_checkint(L, 6);

	if (!teamHandler.IsValidTeam(teamId))
		return 0;

	cursorIcons.AddBuildIcon(-unitDefID, pos, teamId, facing);
	return 0;
}


/***
 * @function Spring.DrawUnitCommands
 * @param unitID integer
 */
/***
 * @function Spring.DrawUnitCommands
 * @param unitIDs integer[] Unit ids.
 * @param tableOrArray false|nil Set to `true` if the unit IDs should be read from the keys of `unitIDs`.
 */
/***
 * @function Spring.DrawUnitCommands
 * @param unitIDs table<integer, any> Table with unit IDs as keys.
 * @param tableOrArray true Set to `false` if the unit IDs should be read from the values of `unitIDs`.
 * @return nil
 */
int LuaUnsyncedCtrl::DrawUnitCommands(lua_State* L)
{
	if (lua_istable(L, 1)) {
		// second arg indicates if table is a map
		const int unitArg = luaL_optboolean(L, 2, false)? -2 : -1;
		const int queueDrawDepth = luaL_optnumber(L, 3, -1); //same value for all

		constexpr int tableIdx = 1;
		for (lua_pushnil(L); lua_next(L, tableIdx) != 0; lua_pop(L, 1)) {
			if (!lua_israwnumber(L, -2))
				continue;

			const CUnit* unit = ParseAllyUnit(L, __func__, unitArg);

			if (unit == nullptr)
				continue;

			commandDrawer->AddLuaQueuedUnit(unit, queueDrawDepth);
		}
	} else {
		const CUnit* unit = ParseAllyUnit(L, __func__, 1);

		if (unit != nullptr) {
			const int queueDrawDepth = luaL_optnumber(L, 2, -1);
			commandDrawer->AddLuaQueuedUnit(unit, queueDrawDepth);
		}
	}

	return 0;
}


/******************************************************************************
 * Camera
 * @section camera
 *****************************************************************************/

static CCameraController::StateMap ParseCamStateMap(lua_State* L, int tableIdx)
{
	CCameraController::StateMap camState;

	for (lua_pushnil(L); lua_next(L, tableIdx) != 0; lua_pop(L, 1)) {
		if (!lua_israwstring(L, -2))
			continue;

		const string key = lua_tostring(L, -2);

		if (lua_isnumber(L, -1)) {
			camState[key] = lua_tofloat(L, -1);
		}
		else if (lua_isboolean(L, -1)) {
			camState[key] = lua_toboolean(L, -1) ? +1.0f : -1.0f;
		}
	}

	return camState;
}


/*** For Spring Engine XZ represents horizontal, from north west corner of map and Y vertical, from water level and rising.
 *
 * @function Spring.SetCameraTarget
 *
 * @param x number
 * @param y number
 * @param z number
 * @param transTime number?
 * @return nil
 */
int LuaUnsyncedCtrl::SetCameraTarget(lua_State* L)
{
	if (mouse == nullptr)
		return 0;

	float4 targetPos = {
		luaL_checkfloat(L, 1),
		luaL_checkfloat(L, 2),
		luaL_checkfloat(L, 3),
		luaL_optfloat(L, 4, 0.5f),
	};
	const float3 targetDir = {
		luaL_optfloat(L, 5, (camera->GetDir()).x),
		luaL_optfloat(L, 6, (camera->GetDir()).y),
		luaL_optfloat(L, 7, (camera->GetDir()).z),
	};

	if (targetPos.w < 0.0f) {
		targetPos.w = 0.0f;
	}
	camHandler->GetCurrentController().SetPos(targetPos);
	camHandler->GetCurrentController().SetDir(targetDir);
	camHandler->CameraTransition(targetPos.w);

	return 0;
}


/***
 *
 * @function Spring.SetCameraOffset
 *
 * @param posX number? (Default: `0`)
 * @param posY number? (Default: `0`)
 * @param posZ number? (Default: `0`)
 * @param tiltX number? (Default: `0`)
 * @param tiltY number? (Default: `0`)
 * @param tiltZ number? (Default: `0`)
 * @return nil
 */
int LuaUnsyncedCtrl::SetCameraOffset(lua_State* L)
{
	camera->posOffset.x = luaL_optfloat(L, 1, 0.0f);
	camera->posOffset.y = luaL_optfloat(L, 2, 0.0f);
	camera->posOffset.z = luaL_optfloat(L, 3, 0.0f);
	camera->tiltOffset.x = luaL_optfloat(L, 4, 0.0f);
	camera->tiltOffset.y = luaL_optfloat(L, 5, 0.0f);
	camera->tiltOffset.z = luaL_optfloat(L, 6, 0.0f);

	return 0;
}


/*** Set camera state.
 *
 * @function Spring.SetCameraState
 *
 * @param cameraState CameraState The fields must be consistent with the name/mode and current/new camera mode.
 * 
 * @param transitionTime number? (Default: `0`) in nanoseconds
 *
 * @param transitionTimeFactor number?
 * Multiplicative factor applied to this and all subsequent transition times for
 * this camera mode.
 *
 * Defaults to "CamTimeFactor" springsetting unless set previously.
 *
 * @param transitionTimeExponent number?
 * Tween factor applied to this and all subsequent transitions for this camera
 * mode.
 *
 * Defaults to "CamTimeExponent" springsetting unless set previously.
 *
 * @return boolean set `true` when applied without errors, otherwise `false`.
 */
int LuaUnsyncedCtrl::SetCameraState(lua_State* L)
{
	// ??
	if (mouse == nullptr)
		return 0;

	const bool hasState = lua_istable(L, 1);

	if (!(hasState || lua_isnil(L, 1)))
		luaL_error(L, "[%s([ stateTable[, camTransTime[, transTimeFactor[, transTimeExpon] ] ] ])] incorrect arguments", __func__);

	camHandler->SetTransitionParams(luaL_optfloat(L, 3, camHandler->GetTransitionTimeFactor()), luaL_optfloat(L, 4, camHandler->GetTransitionTimeExponent()));

	const bool retval = camHandler->SetState(hasState ? ParseCamStateMap(L, 1) : camHandler->GetState());
	camHandler->CameraTransition(luaL_optfloat(L, 2, 0.0f));
	const bool synced = CLuaHandle::GetHandleSynced(L);

	// always push false in synced
	lua_pushboolean(L, retval && !synced);
	return 1;
}

/*** Runs Dolly Camera
 *
 * @function Spring.RunDollyCamera
 * @param runtime number Runtime in milliseconds.
 * @return nil
 */
int LuaUnsyncedCtrl::RunDollyCamera(lua_State* L)
{
	float runtime = luaL_checkfloat(L, 1);

	camHandler->GetDollyController().Run(runtime);

	return 0;
}

/*** Pause Dolly Camera
 *
 * @function Spring.PauseDollyCamera
 * @param fraction number Fraction of the total runtime to pause at, 0 to 1 inclusive. A null value pauses at current percent
 * @return nil
 */
int LuaUnsyncedCtrl::PauseDollyCamera(lua_State* L)
{
	float percent = luaL_optfloat(L, 1, -1);

	camHandler->GetDollyController().Pause(percent);

	return 0;
}

/*** Resume Dolly Camera
 *
 * @function Spring.ResumeDollyCamera
 * @return nil
 */
int LuaUnsyncedCtrl::ResumeDollyCamera(lua_State* L)
{
	camHandler->GetDollyController().Resume();

	return 0;
}

/*** Sets Dolly Camera Position
 *
 * @function Spring.SetDollyCameraPosition
 * @param x number
 * @param y number
 * @param z number
 * @return nil
 */
int LuaUnsyncedCtrl::SetDollyCameraPosition(lua_State* L)
{
	float x = luaL_checkfloat(L, 1);
	float y = luaL_checkfloat(L, 2);
	float z = luaL_checkfloat(L, 3);

	camHandler->GetDollyController().SetPosition(float3{x, y, z});

	return 0;
}

/*** NURBS control point.
 *
 * @class ControlPoint
 * @x_helper
 * 
 * @field [1] number x
 * @field [2] number y
 * @field [3] number z
 * @field [4] number weight
 */

/*** Sets Dolly Camera movement Curve
 *
 * @function Spring.SetDollyCameraCurve
 * @param degree number
 * @param cpoints ControlPoint[] NURBS control point positions.
 * @param knots table
 * @return nil
 */
int LuaUnsyncedCtrl::SetDollyCameraCurve(lua_State* L)
{
	int degree = luaL_checkint(L, 1);

	std::vector<float4> cpoints{};
	std::vector<float> knots{};

	LuaUtils::ParseFloat4Vector(L, 2, cpoints);
	LuaUtils::ParseFloatVector(L, 3, knots);

	camHandler->GetDollyController().SetNURBS(degree, cpoints, knots);

	return 0;
}

/*** Sets Dolly Camera movement mode
 *
 * @function Spring.SetDollyCameraMode
 * @param mode 1|2 `1` static position, `2` nurbs curve
 * @return nil
 */
int LuaUnsyncedCtrl::SetDollyCameraMode(lua_State* L)
{
	int mode = luaL_checkint(L, 1);

	camHandler->GetDollyController().SetMode(mode);

	return 0;
}

/*** Sets Dolly Camera movement curve to world relative or look target relative
 *
 * @function Spring.SetDollyCameraRelativeMode
 * @param relativeMode number `1` world, `2` look target
 * @return nil
 */
int LuaUnsyncedCtrl::SetDollyCameraRelativeMode(lua_State* L)
{
	int mode = luaL_checkint(L, 1);

	camHandler->GetDollyController().SetRelativeMode(mode);

	return 0;
}


/*** Sets Dolly Camera Look Curve
 *
 * @function Spring.SetDollyCameraLookCurve
 * @param degree number
 * @param cpoints ControlPoint[] NURBS control point positions.
 * @param knots table
 * @return nil
 */
int LuaUnsyncedCtrl::SetDollyCameraLookCurve(lua_State* L)
{
	int degree = luaL_checkint(L, 1);

	std::vector<float4> cpoints{};
	std::vector<float> knots{};

	LuaUtils::ParseFloat4Vector(L, 2, cpoints);
	LuaUtils::ParseFloatVector(L, 3, knots);

	camHandler->GetDollyController().SetLookMode(CDollyController::DOLLY_LOOKMODE_CURVE);
	camHandler->GetDollyController().SetLookCurve(degree, cpoints, knots);

	return 0;
}

/*** Sets Dolly Camera Look Position
 *
 * @function Spring.SetDollyCameraLookPosition
 * @param x number
 * @param y number
 * @param z number
 * @return nil
 */
int LuaUnsyncedCtrl::SetDollyCameraLookPosition(lua_State* L)
{
	float x = luaL_checkfloat(L, 1);
	float y = luaL_checkfloat(L, 2);
	float z = luaL_checkfloat(L, 3);

	camHandler->GetDollyController().SetLookMode(CDollyController::DOLLY_LOOKMODE_POSITION);
	camHandler->GetDollyController().SetLookPosition(float3(x, y, z));

	return 0;
}

/*** Sets target unit for Dolly Camera to look towards
 *
 * @function Spring.SetDollyCameraLookUnit
 * @param unitID integer The unit to look at.
 * @return nil
 */
int LuaUnsyncedCtrl::SetDollyCameraLookUnit(lua_State* L)
{
	int unitid = luaL_checkint(L, 1);

	camHandler->GetDollyController().SetLookMode(CDollyController::DOLLY_LOOKMODE_UNIT);
	camHandler->GetDollyController().SetLookUnit(unitid);

	return 0;
}


/******************************************************************************
 * Unit Selection
 * @section unit_selection
******************************************************************************/


/*** Selects a single unit
 *
 * @function Spring.SelectUnit
 * @param unitID integer?
 * @param append boolean? (Default: `false`) Append to current selection.
 * @return nil
 */
int LuaUnsyncedCtrl::SelectUnit(lua_State* L)
{
	if (!luaL_optboolean(L, 2, false))
		selectedUnitsHandler.ClearSelected();

	if (lua_isnoneornil(L, 1))
		return 0;

	CUnit* const unit = ParseSelectUnit(L, __func__, 1);
	if (unit == nullptr)
		return 0;

	selectedUnitsHandler.AddUnit(unit);

	return 0;
}

/***
 *
 * @function Spring.DeselectUnit
 * @param unitID integer
 * @return nil
 */
int LuaUnsyncedCtrl::DeselectUnit(lua_State* L)
{
	CUnit* const unit = ParseSelectUnit(L, __func__, 1);
	if (unit == nullptr)
		return 0;

	selectedUnitsHandler.RemoveUnit(unit);

	return 0;
}

static int TableSelectionCommonFunc(lua_State* L, int unitIndexInTable, bool isSelect, const char *caller)
{
	if (!lua_istable(L, 1))
		luaL_error(L, "[%s] 1st argument must be a table", caller);

	if (isSelect && !luaL_optboolean(L, 2, false))
		selectedUnitsHandler.ClearSelected();

	for (lua_pushnil(L); lua_next(L, 1); lua_pop(L, 1)) {
		if (!lua_israwnumber(L, unitIndexInTable))
			continue;

		const auto unit = ParseSelectUnit(L, __func__, unitIndexInTable);
		if (unit == nullptr)
			continue;

		isSelect
			? selectedUnitsHandler.AddUnit(unit)
			: selectedUnitsHandler.RemoveUnit(unit)
		;
	}

	return 0;
}

/*** Deselects multiple units.
 *
 * @function Spring.DeselectUnitArray
 * @param unitIDs integer[] Table with unit IDs as values.
 * @return nil
 */
int LuaUnsyncedCtrl::DeselectUnitArray(lua_State* L)
{
	return TableSelectionCommonFunc(L, -1, false, __func__);
}

/*** Deselects multiple units.
 *
 * @function Spring.DeselectUnitMap
 * @param unitMap table<integer, any> Table with unit IDs as keys.
 * @return nil
 */
int LuaUnsyncedCtrl::DeselectUnitMap(lua_State* L)
{
	return TableSelectionCommonFunc(L, -2, false, __func__);
}

/*** Selects multiple units, or appends to selection. Accepts a table with unitIDs as values
 *
 * @function Spring.SelectUnitArray
 * @param unitIDs integer[] Table with unit IDs as values.
 * @param append boolean? (Default: `false`) append to current selection
 * @return nil
 */
int LuaUnsyncedCtrl::SelectUnitArray(lua_State* L)
{
	return TableSelectionCommonFunc(L, -1, true, __func__);
}

/*** Selects multiple units, or appends to selection. Accepts a table with unitIDs as keys
 *
 * @function Spring.SelectUnitMap
 * @param unitMap table<integer, any> Table with unit IDs as keys.
 * @param append boolean? (Default: `false`) append to current selection
 * @return nil
 */
int LuaUnsyncedCtrl::SelectUnitMap(lua_State* L)
{
	return TableSelectionCommonFunc(L, -2, true, __func__);
}



/******************************************************************************
 * Lighting
 * @section lighting
******************************************************************************/

/*** Parameters for lighting
 *
 * @class LightParams
 * @x_helper
 * @field position { px: number, py: number, pz: number }
 * @field direction { dx: number, dy: number, dz: number }
 * @field ambientColor { red: number, green: number, blue: number }
 * @field diffuseColor { red: number, green: number, blue: number }
 * @field specularColor { red: number, green: number, blue: number }
 * @field intensityWeight { ambientWeight: number, diffuseWeight: number, specularWeight: number }
 *
 * @field ambientDecayRate { ambientRedDecay: number, ambientGreenDecay: number, ambientBlueDecay: number }
 * Per-frame decay of `ambientColor` (spread over TTL frames)
 *
 * @field diffuseDecayRate { diffuseRedDecay: number, diffuseGreenDecay: number, diffuseBlueDecay: number }
 * Per-frame decay of `diffuseColor` (spread over TTL frames)
 *
 * @field specularDecayRate { specularRedDecay: number, specularGreenDecay: number, specularBlueDecay: number }
 * Per-frame decay of `specularColor` (spread over TTL frames)
 * @field decayFunctionType { ambientDecayType: number, diffuseDecayType: number, specularDecayType: number }
 * If value is `0.0` then the `*DecayRate` values will be interpreted as linear, otherwise exponential.
 *
 * @field radius number
 * @field fov number
 * @field ttl number
 * @field priority number
 * @field ignoreLOS boolean
 */

static bool ParseLight(lua_State* L, GL::Light& light, const int tblIdx, const char* caller)
{
	if (!lua_istable(L, tblIdx)) {
		luaL_error(L, "[%s] argument %d must be a table!", caller, tblIdx);
		return false;
	}

	for (lua_pushnil(L); lua_next(L, tblIdx) != 0; lua_pop(L, 1)) {
		if (!lua_israwstring(L, -2))
			continue;

		const char* key = lua_tostring(L, -2);

		switch (lua_type(L, -1)) {
			case LUA_TTABLE: {
				float array[3] = {0.0f, 0.0f, 0.0f};

				if (LuaUtils::ParseFloatArray(L, -1, array, 3) < 3)
					continue;

				switch (hashString(key)) {
					case hashString("position"): {
						light.SetPosition(array);
					} break;
					case hashString("direction"): {
						light.SetDirection(array);
					} break;

					case hashString("ambientColor"): {
						light.SetAmbientColor(array);
					} break;
					case hashString("diffuseColor"): {
						light.SetDiffuseColor(array);
					} break;
					case hashString("specularColor"): {
						light.SetSpecularColor(array);
					} break;

					case hashString("intensityWeight"): {
						light.SetIntensityWeight(array);
					} break;
					case hashString("attenuation"): {
						light.SetAttenuation(array);
					} break;

					case hashString("ambientDecayRate"): {
						light.SetAmbientDecayRate(array);
					} break;
					case hashString("diffuseDecayRate"): {
						light.SetDiffuseDecayRate(array);
					} break;
					case hashString("specularDecayRate"): {
						light.SetSpecularDecayRate(array);
					} break;
					case hashString("decayFunctionType"): {
						light.SetDecayFunctionType(array);
					} break;

					default: {
					} break;
				}

				continue;
			} break;

			case LUA_TNUMBER: {
				switch (hashString(key)) {
					case hashString("radius"): {
						light.SetRadius(std::max(1.0f, lua_tofloat(L, -1)));
					} break;
					case hashString("fov"): {
						light.SetFOV(std::max(0.0f, std::min(180.0f, lua_tofloat(L, -1))));
					} break;
					case hashString("ttl"): {
						light.SetTTL(lua_tofloat(L, -1));
					} break;
					case hashString("priority"): {
						light.SetPriority(lua_tofloat(L, -1));
					} break;
					default: {
					} break;
				}

				continue;
			}

			case LUA_TBOOLEAN: {
				switch (hashString(key)) {
					case hashString("ignoreLOS"): {
						light.SetIgnoreLOS(lua_toboolean(L, -1));
					} break;
					case hashString("localSpace"): {
						light.SetLocalSpace(lua_toboolean(L, -1));
					} break;
					default: {
					} break;
				}

				continue;
			}

			default: {
			} break;
		}
	}

	return true;
}


/***
 * @function Spring.AddMapLight
 *
 * requires MaxDynamicMapLights > 0
 *
 * @param lightParams LightParams
 * @return integer lightHandle
 */
int LuaUnsyncedCtrl::AddMapLight(lua_State* L)
{
	if (CLuaHandle::GetHandleSynced(L) || !CLuaHandle::GetHandleFullRead(L))
		return 0;

	GL::LightHandler* lightHandler = readMap->GetGroundDrawer()->GetLightHandler();
	GL::Light light;

	unsigned int lightHandle = -1U;

	if (lightHandler != nullptr && ParseLight(L, light, 1, __func__))
		lightHandle = lightHandler->AddLight(light);

	lua_pushnumber(L, lightHandle);
	return 1;
}


/***
 * @function Spring.AddModelLight
 *
 * requires MaxDynamicMapLights > 0
 *
 * @param lightParams LightParams
 * @return number lightHandle
 */
int LuaUnsyncedCtrl::AddModelLight(lua_State* L)
{
	if (CLuaHandle::GetHandleSynced(L) || !CLuaHandle::GetHandleFullRead(L))
		return 0;

	GL::LightHandler* lightHandler = unitDrawer->GetLightHandler();
	GL::Light light;

	unsigned int lightHandle = -1U;

	if (lightHandler != nullptr && ParseLight(L, light, 1, __func__))
		lightHandle = lightHandler->AddLight(light);

	lua_pushnumber(L, lightHandle);
	return 1;
}


/***
 * @function Spring.UpdateMapLight
 *
 * @param lightHandle number
 * @param lightParams LightParams
 * @return boolean success
 */
int LuaUnsyncedCtrl::UpdateMapLight(lua_State* L)
{
	const unsigned int lightHandle = luaL_checkint(L, 1);

	if (CLuaHandle::GetHandleSynced(L) || !CLuaHandle::GetHandleFullRead(L))
		return 0;

	GL::LightHandler* lightHandler = readMap->GetGroundDrawer()->GetLightHandler();
	GL::Light* light = (lightHandler != nullptr)? lightHandler->GetLight(lightHandle): nullptr;

	lua_pushboolean(L, (light != nullptr && ParseLight(L, *light, 2, __func__)));
	return 1;
}


/***
 * @function Spring.UpdateModelLight
 *
 * @param lightHandle number
 * @param lightParams LightParams
 * @return boolean success
 */
int LuaUnsyncedCtrl::UpdateModelLight(lua_State* L)
{
	const unsigned int lightHandle = luaL_checkint(L, 1);

	if (CLuaHandle::GetHandleSynced(L) || !CLuaHandle::GetHandleFullRead(L))
		return 0;

	GL::LightHandler* lightHandler = unitDrawer->GetLightHandler();
	GL::Light* light = (lightHandler != nullptr)? lightHandler->GetLight(lightHandle): nullptr;

	lua_pushboolean(L, (light != nullptr && ParseLight(L, *light, 2, __func__)));
	return 1;
}


/***
 * @function Spring.AddLightTrackingTarget
 */
static bool AddLightTrackingTarget(lua_State* L, GL::Light* light, bool trackEnable, bool trackUnit, const char* caller)
{
	bool ret = false;

	if (trackUnit) {
		// interpret argument #2 as a unit ID
		CUnit* unit = ParseAllyUnit(L, caller, 2);

		if (unit != nullptr) {
			if (trackEnable) {
				if (light->GetTrackObject() == nullptr) {
					light->AddDeathDependence(unit, DEPENDENCE_LIGHT);
					light->SetTrackObject(unit);
					light->SetTrackType(GL::Light::TRACK_TYPE_UNIT);
					ret = true;
				}
			} else {
				// assume <light> was tracking <unit>
				if (light->GetTrackObject() == unit) {
					light->DeleteDeathDependence(unit, DEPENDENCE_LIGHT);
					light->SetTrackObject(nullptr);
					ret = true;
				}
			}
		}
	} else {
		// interpret argument #2 as a projectile ID
		//
		// only track synced projectiles (LuaSynced
		// does not know about unsynced ID's anyway)
		CProjectile* proj = ParseRawProjectile(L, caller, 2, true);

		if (proj != nullptr) {
			if (trackEnable) {
				if (light->GetTrackObject() == nullptr) {
					light->AddDeathDependence(proj, DEPENDENCE_LIGHT);
					light->SetTrackObject(proj);
					light->SetTrackType(GL::Light::TRACK_TYPE_PROJ);
					ret = true;
				}
			} else {
				// assume <light> was tracking <proj>
				if (light->GetTrackObject() == proj) {
					light->DeleteDeathDependence(proj, DEPENDENCE_LIGHT);
					light->SetTrackObject(nullptr);
					ret = true;
				}
			}
		}
	}

	return ret;
}

/*** Set a map-illuminating light to start/stop tracking the position of a moving object (unit or projectile)
 *
 * @function Spring.SetMapLightTrackingState
 *
 * @param lightHandle number
 * @param unitOrProjectileID integer
 * @param enableTracking boolean
 * @param unitOrProjectile boolean
 * @return boolean success
 */
int LuaUnsyncedCtrl::SetMapLightTrackingState(lua_State* L)
{
	if (CLuaHandle::GetHandleSynced(L) || !CLuaHandle::GetHandleFullRead(L))
		return 0;

	if (!lua_isnumber(L, 2)) {
		luaL_error(L, "[%s] 1st and 2nd arguments should be numbers, 3rd and 4th should be booleans", __func__);
		return 0;
	}

	const unsigned int lightHandle = luaL_checkint(L, 1);
	const bool trackEnable = luaL_optboolean(L, 3, true);
	const bool trackUnit = luaL_optboolean(L, 4, true);

	GL::LightHandler* lightHandler = readMap->GetGroundDrawer()->GetLightHandler();
	GL::Light* light = (lightHandler != nullptr)? lightHandler->GetLight(lightHandle): nullptr;

	bool ret = false;

	if (light != nullptr)
		ret = AddLightTrackingTarget(L, light, trackEnable, trackUnit, __func__);

	lua_pushboolean(L, ret);
	return 1;
}

/*** Set a model-illuminating light to start/stop tracking the position of a moving object (unit or projectile)
 *
 * @function Spring.SetModelLightTrackingState
 *
 * @param lightHandle number
 * @param unitOrProjectileID integer
 * @param enableTracking boolean
 * @param unitOrProjectile boolean
 * @return boolean success
 */
int LuaUnsyncedCtrl::SetModelLightTrackingState(lua_State* L)
{
	if (CLuaHandle::GetHandleSynced(L) || !CLuaHandle::GetHandleFullRead(L))
		return 0;

	if (!lua_isnumber(L, 2)) {
		luaL_error(L, "[%s] 1st and 2nd arguments should be numbers, 3rd and 4th should be booleans", __func__);
		return 0;
	}

	const unsigned int lightHandle = luaL_checkint(L, 1);
	const bool trackEnable = luaL_optboolean(L, 3, true);
	const bool trackUnit = luaL_optboolean(L, 4, true);

	GL::LightHandler* lightHandler = unitDrawer->GetLightHandler();
	GL::Light* light = (lightHandler != nullptr)? lightHandler->GetLight(lightHandle): nullptr;
	bool ret = false;

	if (light != nullptr)
		ret = AddLightTrackingTarget(L, light, trackEnable, trackUnit, __func__);

	lua_pushboolean(L, ret);
	return 1;
}


/******************************************************************************
 * Ingame Console
 * @section console
******************************************************************************/


/*** @function Spring.SetMapShader
 *
 * The ID's must refer to valid programs returned by `gl.CreateShader`.
 * Passing in a value of 0 will cause the respective shader to revert back to its engine default.
 * Custom map shaders that declare a uniform ivec2 named "texSquare" can sample from the default diffuse texture(s), which are always bound to TU 0.
 *
 * @param standardShaderID integer
 * @param deferredShaderID integer
 * @return nil
 */
int LuaUnsyncedCtrl::SetMapShader(lua_State* L)
{
	if (CLuaHandle::GetHandleSynced(L))
		return 0;

	const LuaShaders& shaders = CLuaHandle::GetActiveShaders(L);

	CBaseGroundDrawer* groundDrawer = readMap->GetGroundDrawer();
	LuaMapShaderData luaMapShaderData;

	for (unsigned int i = 0; i < 2; i++) {
		luaMapShaderData.shaderIDs[i] = shaders.GetProgramName(lua_tonumber(L, i + 1));
	}

	groundDrawer->SetLuaShader(&luaMapShaderData);
	return 0;
}


/*** @function Spring.SetMapSquareTexture
 * @param texSqrX number
 * @param texSqrY number
 * @param luaTexName string
 * @return boolean success
 */
int LuaUnsyncedCtrl::SetMapSquareTexture(lua_State* L)
{
	if (CLuaHandle::GetHandleSynced(L))
		return 0;

	const int texSquareX = luaL_checkint(L, 1);
	const int texSquareY = luaL_checkint(L, 2);
	const std::string& texName = luaL_checkstring(L, 3);

	CBaseGroundDrawer* groundDrawer = readMap->GetGroundDrawer();
	CBaseGroundTextures* groundTextures = groundDrawer->GetGroundTextures();

	if (groundTextures == nullptr) {
		lua_pushboolean(L, false);
		return 1;
	}
	if (texName.empty()) {
		// restore default texture for this square
		lua_pushboolean(L, groundTextures->SetSquareLuaTexture(texSquareX, texSquareY, 0));
		return 1;
	}

	const LuaTextures& luaTextures = CLuaHandle::GetActiveTextures(L);

	const    LuaTextures::Texture*   luaTexture = nullptr;
	const CNamedTextures::TexInfo* namedTexture = nullptr;

	if ((luaTexture = luaTextures.GetInfo(texName)) != nullptr) {
		if (luaTexture->xsize != luaTexture->ysize) {
			// square textures only
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, groundTextures->SetSquareLuaTexture(texSquareX, texSquareY, luaTexture->id));
		return 1;
	}

	if ((namedTexture = CNamedTextures::GetInfo(texName)) != nullptr) {
		if (namedTexture->xsize != namedTexture->ysize) {
			// square textures only
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, groundTextures->SetSquareLuaTexture(texSquareX, texSquareY, namedTexture->id));
		return 1;
	}

	lua_pushboolean(L, false);
	return 1;
}


static MapTextureData ParseLuaTextureData(lua_State* L, bool mapTex)
{
	MapTextureData luaTexData;

	const std::string& texType = mapTex? luaL_checkstring(L, 1): "";
	const std::string& texName = mapTex? luaL_checkstring(L, 2): luaL_checkstring(L, 1);

	if (mapTex) {
		// convert type=LUATEX_* to MAP_*
		luaTexData.type = LuaOpenGLUtils::GetLuaMatTextureType(texType) - LuaMatTexture::LUATEX_SMF_GRASS;
		// MAP_SSMF_SPLAT_NORMAL_TEX needs a num
		luaTexData.num = luaL_optint(L, 3, 0);
	}

	// empty name causes a revert to default
	if (!texName.empty()) {
		const LuaTextures& luaTextures = CLuaHandle::GetActiveTextures(L);

		const    LuaTextures::Texture*   luaTexture = nullptr;
		const CNamedTextures::TexInfo* namedTexture = nullptr;

		if ((luaTexData.id == 0) && ((luaTexture = luaTextures.GetInfo(texName)) != nullptr)) {
			luaTexData.id     = luaTexture->id;
			luaTexData.size.x = luaTexture->xsize;
			luaTexData.size.y = luaTexture->ysize;
		}
		if ((luaTexData.id == 0) && ((namedTexture = CNamedTextures::GetInfo(texName)) != nullptr)) {
			luaTexData.id     = namedTexture->id;
			luaTexData.size.x = namedTexture->xsize;
			luaTexData.size.y = namedTexture->ysize;
		}
	}

	return luaTexData;
}


/*** @function Spring.SetMapShadingTexture
 * @param texType string
 * @param texName string
 * @return boolean success
 * @usage Spring.SetMapShadingTexture("$ssmf_specular", "name_of_my_shiny_texture")
 */
int LuaUnsyncedCtrl::SetMapShadingTexture(lua_State* L)
{
	if (CLuaHandle::GetHandleSynced(L))
		return 0;

	if (readMap != nullptr) {
		lua_pushboolean(L, readMap->SetLuaTexture(ParseLuaTextureData(L, true)));
	} else {
		lua_pushboolean(L, false);
	}

	return 1;
}


/*** @function Spring.SetSkyBoxTexture
 * @param texName string
 * @return nil
 */
int LuaUnsyncedCtrl::SetSkyBoxTexture(lua_State* L)
{
	if (CLuaHandle::GetHandleSynced(L))
		return 0;

	if (const auto& sky = ISky::GetSky(); sky != nullptr)
		sky->SetLuaTexture(ParseLuaTextureData(L, false));

	return 0;
}


/******************************************************************************
 * Unit custom rendering
 * @section unitcustomrendering
******************************************************************************/


/***
 *
 * @function Spring.SetUnitNoDraw
 * @param unitID integer
 * @param noDraw boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitNoDraw(lua_State* L)
{
	CUnit* unit = ParseCtrlUnit(L, __func__, 1);

	if (unit == nullptr)
		return 0;

	unit->noDraw = luaL_checkboolean(L, 2);
	return 0;
}


/***
 *
 * @function Spring.SetUnitEngineDrawMask
 * @param unitID integer
 * @param drawMask number
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitEngineDrawMask(lua_State* L)
{
	CUnit* unit = ParseCtrlUnit(L, __func__, 1);

	if (unit == nullptr)
		return 0;

	unit->engineDrawMask = static_cast<uint8_t>(luaL_checkint(L, 2));
	return 0;
}


/***
 *
 * @function Spring.SetUnitAlwaysUpdateMatrix
 * @param unitID integer
 * @param alwaysUpdateMatrix boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitAlwaysUpdateMatrix(lua_State* L)
{
	CUnit* unit = ParseCtrlUnit(L, __func__, 1);

	if (unit == nullptr)
		return 0;

	unit->alwaysUpdateMat = luaL_checkboolean(L, 2);
	return 0;
}


/***
 *
 * @function Spring.SetUnitNoMinimap
 * @param unitID integer
 * @param unitNoMinimap boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitNoMinimap(lua_State* L)
{
	CUnit* unit = ParseCtrlUnit(L, __func__, 1);

	if (unit == nullptr)
		return 0;

	unit->noMinimap = luaL_checkboolean(L, 2);
	return 0;
}

/***
 * @function Spring.SetMiniMapRotation
 * @param rotation number amount in radians
 * @return nil
 */
int LuaUnsyncedCtrl::SetMiniMapRotation(lua_State* L)
{
	
	const float radians = luaL_checkfloat(L, 1);
	
	if (minimap == nullptr)
		return 0;
	
	if (minimap->minimapCanFlip)
		return 0;

	// Get the signed quadrant of the angle.
	const float quad = radians / math::HALFPI;

	const float wrapped = std::fmod(std::fmod(quad, 4.0f) + 4.0f, 4.0f);

	// Wrap it into range [0, 3]
	const int rotation = static_cast<int>(std::round(wrapped)) % 4;

	minimap->SetRotation(CMiniMap::RotationOptions(rotation));

	return 0;
}


/***
 *
 * @function Spring.SetUnitNoGroup
 * @param unitID integer
 * @param unitNoGroup boolean Whether unit can be added to selection groups
 */
int LuaUnsyncedCtrl::SetUnitNoGroup(lua_State* L)
{
	CUnit* unit = ParseCtrlUnit(L, __func__, 1);

	if (unit == nullptr)
		return 0;

	unit->noGroup = luaL_checkboolean(L, 2);

	if (unit->noGroup) {
		unit->SetGroup(nullptr);
	}
	return 0;
}


/***
 *
 * @function Spring.SetUnitNoSelect
 * @param unitID integer
 * @param unitNoSelect boolean whether unit can be selected or not
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitNoSelect(lua_State* L)
{
	CUnit* unit = ParseCtrlUnit(L, __func__, 1);

	if (unit == nullptr)
		return 0;

	unit->noSelect = luaL_checkboolean(L, 2);

	// deselect the unit if it's selected and shouldn't be
	if (unit->noSelect) {
		const auto& selUnits = selectedUnitsHandler.selectedUnits;

		if (selUnits.find(unit->id) != selUnits.end()) {
			selectedUnitsHandler.RemoveUnit(unit);
		}
	}
	return 0;
}


/***
 *
 * @function Spring.SetUnitLeaveTracks
 * @param unitID integer
 * @param unitLeaveTracks boolean whether unit leaves tracks on movement
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitLeaveTracks(lua_State* L)
{
	CUnit* unit = ParseCtrlUnit(L, __func__, 1);

	if (unit == nullptr)
		return 0;

	groundDecals->SetUnitLeaveTracks(unit, lua_toboolean(L, 2));
	return 0;
}


/***
 *
 * @function Spring.SetUnitSelectionVolumeData
 * @param unitID integer
 * @param featureID integer
 * @param scaleX number
 * @param scaleY number
 * @param scaleZ number
 * @param offsetX number
 * @param offsetY number
 * @param offsetZ number
 * @param vType number
 * @param tType number
 * @param Axis number
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitSelectionVolumeData(lua_State* L)
{
	CUnit* unit = ParseCtrlUnit(L, __func__, 1);

	if (unit == nullptr)
		return 0;

	return LuaUtils::ParseColVolData(L, 2, &unit->selectionVolume);
}


/******************************************************************************
 * Features
 * @section features
******************************************************************************/


/***
 *
 * @function Spring.SetFeatureNoDraw
 *
 * @param featureID integer
 * @param noDraw boolean
 *
 * @return nil
 */
int LuaUnsyncedCtrl::SetFeatureNoDraw(lua_State* L)
{
	CFeature* feature = ParseCtrlFeature(L, __func__, 1);

	if (feature == nullptr)
		return 0;

	feature->noDraw = luaL_checkboolean(L, 2);
	return 0;
}


/***
 *
 * @function Spring.SetFeatureEngineDrawMask
 * @param featureID integer
 * @param engineDrawMask number
 * @return nil
 */
int LuaUnsyncedCtrl::SetFeatureEngineDrawMask(lua_State* L)
{
	CFeature* feature = ParseCtrlFeature(L, __func__, 1);

	if (feature == nullptr)
		return 0;

	feature->engineDrawMask = static_cast<uint8_t>(luaL_checkint(L, 2));
	return 0;
}


/***
 *
 * @function Spring.SetFeatureAlwaysUpdateMatrix
 * @param featureID integer
 * @param alwaysUpdateMat number
 * @return nil
 */
int LuaUnsyncedCtrl::SetFeatureAlwaysUpdateMatrix(lua_State* L)
{
	CFeature* feature = ParseCtrlFeature(L, __func__, 1);

	if (feature == nullptr)
		return 0;

	feature->alwaysUpdateMat = luaL_checkboolean(L, 2);
	return 0;
}


/*** Control whether a feature will fade or not when zoomed out.
 *
 * @function Spring.SetFeatureFade
 *
 * @param featureID integer
 * @param allow boolean
 *
 * @return nil
 */
int LuaUnsyncedCtrl::SetFeatureFade(lua_State* L)
{
	CFeature* feature = ParseCtrlFeature(L, __func__, 1);

	if (feature == nullptr)
		return 0;

	feature->alphaFade = luaL_checkboolean(L, 2);
	return 0;
}


/***
 *
 * @function Spring.SetFeatureSelectionVolumeData
 *
 * @param featureID integer
 * @param scaleX number
 * @param scaleY number
 * @param scaleZ number
 * @param offsetX number
 * @param offsetY number
 * @param offsetZ number
 * @param vType number
 * @param tType number
 * @param Axis number
 * @return nil
 */
int LuaUnsyncedCtrl::SetFeatureSelectionVolumeData(lua_State* L)
{
	CFeature* feature = ParseCtrlFeature(L, __func__, 1);

	if (feature == nullptr)
		return 0;


	return LuaUtils::ParseColVolData(L, 2, &feature->selectionVolume);
}


/******************************************************************************
 * Unit Icons
 * @section unit_icons
******************************************************************************/


/***
 *
 * @function Spring.AddUnitIcon
 *
 * @param iconName string
 * @param texFile string
 * @param size number?
 * @param dist number?
 * @param radAdjust number?
 *
 * @return boolean added
 */
int LuaUnsyncedCtrl::AddUnitIcon(lua_State* L)
{
	if (CLuaHandle::GetHandleSynced(L))
		return 0;

	const string iconName  = luaL_checkstring(L, 1);
	const string texName   = luaL_checkstring(L, 2);

	const float  size      = luaL_optnumber(L, 3, 1.0f);
	const float  dist      = luaL_optnumber(L, 4, 1.0f);

	const bool   radAdjust = luaL_optboolean(L, 5, false);

	lua_pushboolean(L, icon::iconHandler.AddIcon(iconName, texName, size, dist, radAdjust));
	return 1;
}


/***
 *
 * @function Spring.FreeUnitIcon
 *
 * @param iconName string
 *
 * @return boolean? freed
 */
int LuaUnsyncedCtrl::FreeUnitIcon(lua_State* L)
{
	if (CLuaHandle::GetHandleSynced(L))
		return 0;

	lua_pushboolean(L, icon::iconHandler.FreeIcon(luaL_checkstring(L, 1)));
	return 1;
}


/***
 *
 * @function Spring.UnitIconSetDraw
 * Use Spring.SetUnitIconDraw instead.
 * @deprecated
 * @param unitID integer
 * @param drawIcon boolean
 * @return nil
 */
int LuaUnsyncedCtrl::UnitIconSetDraw(lua_State* L)
{
	LOG_DEPRECATED("Spring.UnitIconSetDraw is deprecated. Please use Spring.SetUnitIconDraw instead.");
	return LuaUnsyncedCtrl::SetUnitIconDraw(L);
}


/***
 *
 * @function Spring.SetUnitIconDraw
 * @param unitID integer
 * @param drawIcon boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitIconDraw(lua_State* L)
{
	CUnit* unit = ParseCtrlUnit(L, __func__, 1);

	if (unit == nullptr)
		return 0;

	unit->drawIcon = luaL_checkboolean(L, 2);
	return 0;
}


/***
 *
 * @function Spring.SetUnitDefIcon
 *
 * @param unitDefID integer
 * @param iconName string
 *
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitDefIcon(lua_State* L)
{
	const UnitDef* ud = unitDefHandler->GetUnitDefByID(luaL_checkint(L, 1));

	if (ud == nullptr)
		return 0;

	ud->iconType = icon::iconHandler.GetIcon(luaL_checksstring(L, 2));

	// set decoys to the same icon
	if (ud->decoyDef != nullptr)
		ud->decoyDef->iconType = ud->iconType;

	// spring::unordered_map<int, std::vector<int> >
	const auto& decoyMap = unitDefHandler->GetDecoyDefIDs();
	const auto decoyMapIt = decoyMap.find((ud->decoyDef != nullptr)? ud->decoyDef->id: ud->id);

	if (decoyMapIt != decoyMap.end()) {
		const auto& decoySet = decoyMapIt->second;

		for (const int decoyDefID: decoySet) {
			const UnitDef* decoyDef = unitDefHandler->GetUnitDefByID(decoyDefID);
			decoyDef->iconType = ud->iconType;
		}
	}

	unitDrawer->UpdateUnitDefMiniMapIcons(ud);
	return 0;
}


/***
 *
 * @function Spring.SetUnitDefImage
 *
 * @param unitDefID integer
 * @param image string luaTexture|texFile
 *
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitDefImage(lua_State* L)
{
	const UnitDef* ud = unitDefHandler->GetUnitDefByID(luaL_checkint(L, 1));

	if (ud == nullptr)
		return 0;

	if (lua_isnoneornil(L, 2)) {
		// reset to default texture
		CUnitDrawer::SetUnitDefImage(ud, ud->buildPicName);
		return 0;
	}

	if (!lua_israwstring(L, 2))
		return 0;

	const std::string& texName = lua_tostring(L, 2);

	if (texName[0] != LuaTextures::prefix) { // '!'
		CUnitDrawer::SetUnitDefImage(ud, texName);
		return 0;
	}

	const LuaTextures& textures = CLuaHandle::GetActiveTextures(L);
	const LuaTextures::Texture* tex = textures.GetInfo(texName);

	if (tex == nullptr)
		return 0;

	CUnitDrawer::SetUnitDefImage(ud, tex->id, tex->xsize, tex->ysize);
	return 0;
}


/******************************************************************************
 * Virtual File System
 * @section vfs
 *
 * Prefer using `VFS` whenever possible
 ******************************************************************************/

// TODO: move this to LuaVFS?

/***
 *
 * @function Spring.ExtractModArchiveFile
 * @param modfile string
 * @return boolean extracted
 */
int LuaUnsyncedCtrl::ExtractModArchiveFile(lua_State* L)
{
	const string path = luaL_checkstring(L, 1);

	CFileHandler vfsFile(path, SPRING_VFS_ZIP);
	CFileHandler rawFile(path, SPRING_VFS_RAW);

	if (!vfsFile.FileExists()) {
		luaL_error(L, "file \"%s\" not found in mod archive", path.c_str());
		return 0;
	}

	if (rawFile.FileExists()) {
		luaL_error(L, "cannot extract file \"%s\": already exists", path.c_str());
		return 0;
	}


	std::string dname = FileSystem::GetDirectory(path);
	std::string fname = FileSystem::GetFilename(path);

#ifdef _WIN32
	const size_t s = dname.size();
	// get rid of any trailing slashes (CreateDirectory()
	// fails on at least XP and Vista if they are present,
	// ie. it creates the dir but actually returns false)
	if ((s > 0) && ((dname[s - 1] == '/') || (dname[s - 1] == '\\')))
		dname = dname.substr(0, s - 1);
#endif

	if (!dname.empty() && !FileSystem::CreateDirectory(dname))
		luaL_error(L, "Could not create directory \"%s\" for file \"%s\"", dname.c_str(), fname.c_str());


	std::vector<uint8_t> buffer;
	std::fstream fstr(path.c_str(), std::ios::out | std::ios::binary);

	if (!vfsFile.IsBuffered()) {
		buffer.resize(vfsFile.FileSize(), 0);
		vfsFile.Read(buffer.data(), buffer.size());
	} else {
		buffer = std::move(vfsFile.GetBuffer());
	}

	fstr.write((const char*) buffer.data(), buffer.size());
	fstr.close();

	if (!dname.empty()) {
		LOG("[%s] extracted file \"%s\" to directory \"%s\"", __func__, fname.c_str(), dname.c_str());
	} else {
		LOG("[%s] extracted file \"%s\"", __func__, fname.c_str());
	}

	lua_pushboolean(L, true);
	return 1;
}


/***
 *
 * @function Spring.CreateDir
 * @param path string
 * @return boolean? dirCreated
 */
int LuaUnsyncedCtrl::CreateDir(lua_State* L)
{
	const std::string& dir = luaL_checkstring(L, 1);

	// keep directories within the Spring directory
	if (dir[0] == '/' || dir[0] == '\\' || dir[0] == '~')
		luaL_error(L, "[%s][1] invalid access: %s", __func__, dir.c_str());
	if (dir[0] == ' ' || dir[0] == '\t')
		luaL_error(L, "[%s][2] invalid access: %s", __func__, dir.c_str());

	if (strstr(dir.c_str(), "..") != nullptr)
		luaL_error(L, "[%s][3] invalid access: %s", __func__, dir.c_str());

	if (dir.size() > 1 && dir[1] == ':')
		luaL_error(L, "[%s][4] invalid access: %s", __func__, dir.c_str());

	lua_pushboolean(L, FileSystem::CreateDirectory(dir));
	return 1;
}


/******************************************************************************
 * GUI
 * @section gui
******************************************************************************/


static int SetActiveCommandByIndex(lua_State* L)
{
	if (guihandler == nullptr)
		return 0;

	const int args = lua_gettop(L); // number of arguments
	const int cmdIndex = lua_toint(L, 1) - CMD_INDEX_OFFSET;
	const int button = luaL_optint(L, 2, 1); // LMB

	if (args <= 2) {
		lua_pushboolean(L, guihandler->SetActiveCommand(cmdIndex, button != SDL_BUTTON_LEFT));
		return 1;
	}

	const bool lmb   = luaL_checkboolean(L, 3);
	const bool rmb   = luaL_checkboolean(L, 4);
	const bool alt   = luaL_checkboolean(L, 5);
	const bool ctrl  = luaL_checkboolean(L, 6);
	const bool meta  = luaL_checkboolean(L, 7);
	const bool shift = luaL_checkboolean(L, 8);

	const bool success = guihandler->SetActiveCommand(cmdIndex, button, lmb, rmb, alt, ctrl, meta, shift);
	lua_pushboolean(L, success);
	return 1;
}


static int SetActiveCommandByAction(lua_State* L)
{
	if (guihandler == nullptr)
		return 0;

	const int args = lua_gettop(L); // number of arguments
	const string text = lua_tostring(L, 1);
	const Action action(text);

	CKeySet ks;
	if (args >= 2)
		ks.Parse(lua_tostring(L, 2));

	lua_pushboolean(L, guihandler->SetActiveCommand(action, ks, 0));
	return 1;
}


/***
 * @function Spring.SetActiveCommand
 * @param action string
 * @param actionExtra string?
 * @return boolean? commandSet
 */

/*** @function Spring.SetActiveCommand
 * @param cmdIndex number
 * @param button number? (Default: `1`)
 * @param leftClick boolean?
 * @param rightClick boolean?
 * @param alt boolean?
 * @param ctrl boolean?
 * @param meta boolean?
 * @param shift boolean?
 * @return boolean? commandSet
 */
int LuaUnsyncedCtrl::SetActiveCommand(lua_State* L)
{
	if (guihandler == nullptr)
		return 0;

	if (lua_isnoneornil(L, 1)) {
		lua_pushboolean(L, guihandler->SetActiveCommand(-1, false));
		return 1;
	}

	if (lua_isnumber(L, 1))
		return SetActiveCommandByIndex(L);

	if (lua_isstring(L, 1))
		return SetActiveCommandByAction(L);

	luaL_error(L, "[%s] incorrect argument type", __func__);
	return 0;
}


/*** @function Spring.LoadCmdColorsConfig
 * @param config string
 * @return nil
 */
int LuaUnsyncedCtrl::LoadCmdColorsConfig(lua_State* L)
{
	cmdColors.LoadConfigFromString(luaL_checkstring(L, 1));
	return 0;
}


/*** @function Spring.LoadCtrlPanelConfig
 * @param config string
 * @return nil
 */
int LuaUnsyncedCtrl::LoadCtrlPanelConfig(lua_State* L)
{
	if (guihandler == nullptr)
		return 0;

	guihandler->ReloadConfigFromString(luaL_checkstring(L, 1));
	return 0;
}


/*** @function Spring.ForceLayoutUpdate
 * @return nil
 */
int LuaUnsyncedCtrl::ForceLayoutUpdate(lua_State* L)
{
	if (guihandler == nullptr)
		return 0;

	guihandler->ForceLayoutUpdate();
	return 0;
}


/***  Disables the "Selected Units x" box in the GUI.
 *
 * @function Spring.SetDrawSelectionInfo
 * @param enable boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetDrawSelectionInfo(lua_State* L)
{
	if (guihandler != nullptr)
		guihandler->SetDrawSelectionInfo(luaL_checkboolean(L, 1));

	return 0;
}


/***
 *
 * @function Spring.SetBoxSelectionByEngine
 * @param state boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetBoxSelectionByEngine(lua_State* L)
{
	bool b = luaL_checkboolean(L, 1);
	selectedUnitsHandler.SetBoxSelectionHandledByEngine(b);
	return 0;
}


/***
 *
 * @function Spring.SetTeamColor
 * @param teamID integer
 * @param r number
 * @param g number
 * @param b number
 * @return nil
 */
int LuaUnsyncedCtrl::SetTeamColor(lua_State* L)
{
	const int teamID = luaL_checkint(L, 1);
	if (!teamHandler.IsValidTeam(teamID))
		return 0;

	CTeam* team = teamHandler.Team(teamID);
	if (team == nullptr)
		return 0;

	team->color[0] = (unsigned char)(std::clamp(luaL_checkfloat(L, 2      ), 0.0f, 1.0f) * 255.0f);
	team->color[1] = (unsigned char)(std::clamp(luaL_checkfloat(L, 3      ), 0.0f, 1.0f) * 255.0f);
	team->color[2] = (unsigned char)(std::clamp(luaL_checkfloat(L, 4      ), 0.0f, 1.0f) * 255.0f);
	team->color[3] = (unsigned char)(std::clamp(luaL_optfloat  (L, 5, 1.0f), 0.0f, 1.0f) * 255.0f);
	return 0;
}


/*** Changes/creates the cursor of a single CursorCmd.
 *
 * @function Spring.AssignMouseCursor
 *
 * @param cmdName string
 * @param iconFileName string not the full filename, instead it is like this:
 *     Wanted filename: Anims/cursorattack_0.bmp
 *     => iconFileName: cursorattack
 * @param overwrite boolean? (Default: `true`)
 * @param hotSpotTopLeft boolean? (Default: `false`)
 * @return boolean? assigned
 */
int LuaUnsyncedCtrl::AssignMouseCursor(lua_State* L)
{
	const std::string& cmdName  = luaL_checksstring(L, 1);
	const std::string& fileName = luaL_checksstring(L, 2);

	const CMouseCursor::HotSpot hotSpot = (luaL_optboolean(L, 4, false))? CMouseCursor::TopLeft: CMouseCursor::Center;

	const bool retval = mouse->AssignMouseCursor(cmdName, fileName, hotSpot, luaL_optboolean(L, 3, true));
	const bool synced = CLuaHandle::GetHandleSynced(L);

	lua_pushboolean(L, retval && !synced);
	return 1;
}


/*** Mass replace all occurrences of the cursor in all CursorCmds.
 *
 * @function Spring.ReplaceMouseCursor
 * @param oldFileName string
 * @param newFileName string
 * @param hotSpotTopLeft boolean? (Default: `false`)
 * @return boolean? assigned
 */
int LuaUnsyncedCtrl::ReplaceMouseCursor(lua_State* L)
{
	const string oldName = luaL_checksstring(L, 1);
	const string newName = luaL_checksstring(L, 2);

	CMouseCursor::HotSpot hotSpot = CMouseCursor::Center;
	if (luaL_optboolean(L, 3, false))
		hotSpot = CMouseCursor::TopLeft;

	const bool retval = mouse->ReplaceMouseCursor(oldName, newName, hotSpot);
	const bool synced = CLuaHandle::GetHandleSynced(L);

	lua_pushboolean(L, retval && !synced);
	return 1;
}


/*** Register your custom cmd so it gets visible in the unit's cmd queue
 *
 * @function Spring.SetCustomCommandDrawData
 * @param cmdID integer
 * @param cmdReference string|integer|nil The name or ID of an icon for command. Pass `nil` to clear draw data for command.
 * @param color rgba? (Default: white)
 * @param showArea boolean? (Default: `false`)
 * @return nil
 */
int LuaUnsyncedCtrl::SetCustomCommandDrawData(lua_State* L)
{
	const int cmdID = luaL_checkint(L, 1);

	int iconID = 0;
	if (lua_israwnumber(L, 2)) {
		iconID = lua_toint(L, 2);
	}
	else if (lua_israwstring(L, 2)) {
		iconID = cmdID;
		const string icon = lua_tostring(L, 2);
		cursorIcons.SetCustomType(cmdID, icon);
	}
	else if (lua_isnoneornil(L, 2)) {
		cursorIcons.SetCustomType(cmdID, "");
		cmdColors.ClearCustomCmdData(cmdID);
		return 0;
	}
	else {
		luaL_error(L, "Incorrect arguments to SetCustomCommandDrawData");
	}

	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	/*const int size =*/ LuaUtils::ParseFloatArray(L, 3, color, 4);

	const bool showArea = luaL_optboolean(L, 4, false);

	cmdColors.SetCustomCmdData(cmdID, iconID, color, showArea);
	return 0;
}


/******************************************************************************
 * Mouse
 * @section mouse
******************************************************************************/


/*** @function Spring.WarpMouse
 * @param x number
 * @param y number
 * @return nil
 */
int LuaUnsyncedCtrl::WarpMouse(lua_State* L)
{
	const int x = luaL_checkint(L, 1);
	const int y = globalRendering->viewSizeY - luaL_checkint(L, 2) - 1;
	mouse->WarpMouse(x, y);
	return 0;
}


/*** @function Spring.SetMouseCursor
 * @param cursorName string
 * @param cursorScale number? (Default: `1.0`)
 * @return nil
 */
int LuaUnsyncedCtrl::SetMouseCursor(lua_State* L)
{
	const std::string& cursorName = luaL_checkstring(L, 1);
	const float cursorScale = luaL_optfloat(L, 2, 1.0f);

	mouse->ChangeCursor(cursorName, cursorScale);

	return 0;
}


/******************************************************************************
 * LOS Colors
 * @section loscolors
******************************************************************************/

/*** @function Spring.SetLosViewColors
 * @param always rgb
 * @param LOS rgb
 * @param radar rgb
 * @param jam rgb
 * @param radar2 rgb
 * @return nil
 */
int LuaUnsyncedCtrl::SetLosViewColors(lua_State* L)
{
	float alwaysColor[3];
	float losColor[3];
	float radarColor[3];
	float jamColor[3];
	float radarColor2[3];

	if (
		(LuaUtils::ParseFloatArray(L, 1, alwaysColor, 3) != 3) ||
		(LuaUtils::ParseFloatArray(L, 2, losColor, 3) != 3) ||
		(LuaUtils::ParseFloatArray(L, 3, radarColor, 3) != 3) ||
		(LuaUtils::ParseFloatArray(L, 4, jamColor, 3) != 3) ||
		(LuaUtils::ParseFloatArray(L, 5, radarColor2, 3) != 3)
	) {
		luaL_error(L, "Incorrect arguments to SetLosViewColors()");
	}
	const int scale = CBaseGroundDrawer::losColorScale;

	CBaseGroundDrawer* gd = readMap->GetGroundDrawer();
	gd->alwaysColor[0]  = (int)(scale * alwaysColor[0]);
	gd->alwaysColor[1]  = (int)(scale * alwaysColor[1]);
	gd->alwaysColor[2]  = (int)(scale * alwaysColor[2]);
	gd->losColor[0]     = (int)(scale * losColor[0]);
	gd->losColor[1]     = (int)(scale * losColor[1]);
	gd->losColor[2]     = (int)(scale * losColor[2]);
	gd->radarColor[0]   = (int)(scale * radarColor[0]);
	gd->radarColor[1]   = (int)(scale * radarColor[1]);
	gd->radarColor[2]   = (int)(scale * radarColor[2]);
	gd->jamColor[0]     = (int)(scale * jamColor[0]);
	gd->jamColor[1]     = (int)(scale * jamColor[1]);
	gd->jamColor[2]     = (int)(scale * jamColor[2]);
	gd->radarColor2[0]  = (int)(scale * radarColor2[0]);
	gd->radarColor2[1]  = (int)(scale * radarColor2[1]);
	gd->radarColor2[2]  = (int)(scale * radarColor2[2]);
	infoTextureHandler->SetMode(infoTextureHandler->GetMode());
	return 0;
}


/***
 *
 * @function Spring.SetNanoProjectileParams
 * @param rotVal number? (Default: `0`) in degrees
 * @param rotVel number? (Default: `0`) in degrees
 * @param rotAcc number? (Default: `0`) in degrees
 * @param rotValRng number? (Default: `0`) in degrees
 * @param rotVelRng number? (Default: `0`) in degrees
 * @param rotAccRng number? (Default: `0`) in degrees
 * @return nil
 */
int LuaUnsyncedCtrl::SetNanoProjectileParams(lua_State* L)
{
	CNanoProjectile::rotVal0 = luaL_optfloat(L, 1, 0.0f) * (math::DEG_TO_RAD                            );
	CNanoProjectile::rotVel0 = luaL_optfloat(L, 2, 0.0f) * (math::DEG_TO_RAD / GAME_SPEED               );
	CNanoProjectile::rotAcc0 = luaL_optfloat(L, 3, 0.0f) * (math::DEG_TO_RAD / (GAME_SPEED * GAME_SPEED));

	CNanoProjectile::rotValRng0 = luaL_optfloat(L, 4, 0.0f) * (math::DEG_TO_RAD                            );
	CNanoProjectile::rotVelRng0 = luaL_optfloat(L, 5, 0.0f) * (math::DEG_TO_RAD / GAME_SPEED               );
	CNanoProjectile::rotAccRng0 = luaL_optfloat(L, 6, 0.0f) * (math::DEG_TO_RAD / (GAME_SPEED * GAME_SPEED));

	return 0;
}


/******************************************************************************
 * Engine Config
 *
 * @section engineconfig
 *
 * The following functions read the engine configs saved in `Springsettings.cfg`, a version-ed instance of these or a custom file supplied on the command line.
******************************************************************************/


static constexpr const char* ConfigReadOnlyAdjectives[] = { "read-only", "deprecated" };

/***
 *
 * @function Spring.SetConfigInt
 * @param name string
 * @param value integer
 * @param useOverlay boolean? (Default: `false`) If `true`, the value will only be set in memory, and not be restored for the next game.
 * @return nil
 */
int LuaUnsyncedCtrl::SetConfigInt(lua_State* L)
{
	const std::string& key = luaL_checkstring(L, 1);

	// don't allow to change a read-only variable
	if (configHandler->IsReadOnly(key)) {
		const auto deprecated = configHandler->IsDeprecated(key);
		LOG_L(L_ERROR, "[%s] key \"%s\" is %s", __func__, key.c_str(), ConfigReadOnlyAdjectives[deprecated]);
		return 0;
	}

	const int val = luaL_checkint(L, 2);
	const bool uo = luaL_optboolean(L, 3, false);

	configHandler->EnableWriting(globalConfig.luaWritableConfigFile);
	configHandler->Set(key, val, uo);
	configHandler->EnableWriting(true);
	return 0;
}


/***
 *
 * @function Spring.SetConfigFloat
 * @param name string
 * @param value number
 * @param useOverlay boolean? (Default: `false`) If `true`, the value will only be set in memory, and not be restored for the next game.
 * @return nil
 */
int LuaUnsyncedCtrl::SetConfigFloat(lua_State* L)
{
	const std::string& key = luaL_checkstring(L, 1);

	if (configHandler->IsReadOnly(key)) {
		const auto deprecated = configHandler->IsDeprecated(key);
		LOG_L(L_ERROR, "[%s] key \"%s\" is %s", __func__, key.c_str(), ConfigReadOnlyAdjectives[deprecated]);
		return 0;
	}

	configHandler->EnableWriting(globalConfig.luaWritableConfigFile);
	configHandler->Set(key, luaL_checkfloat(L, 2), luaL_optboolean(L, 3, false));
	configHandler->EnableWriting(true);
	return 0;
}

/***
 *
 * @function Spring.SetConfigString
 * @param name string
 * @param value string
 * @param useOverlay boolean? (Default: `false`) If `true`, the value will only be set in memory, and not be restored for the next game.
 * @return nil
 */
int LuaUnsyncedCtrl::SetConfigString(lua_State* L)
{
	const std::string& key = luaL_checkstring(L, 1);
	const std::string& val = luaL_checkstring(L, 2);

	if (configHandler->IsReadOnly(key)) {
		const auto deprecated = configHandler->IsDeprecated(key);
		LOG_L(L_ERROR, "[%s] key \"%s\" is %s", __func__, key.c_str(), ConfigReadOnlyAdjectives[deprecated]);
		return 0;
	}

	configHandler->EnableWriting(globalConfig.luaWritableConfigFile);
	configHandler->SetString(key, val, luaL_optboolean(L, 3, false));
	configHandler->EnableWriting(true);
	return 0;
}


/******************************************************************************/

static int ReloadOrRestart(const std::string& springArgs, const std::string& scriptText, bool newProcess) {
	const std::string springFullName = Platform::GetProcessExecutableFile();
	const std::string scriptFullName = dataDirLocater.GetWriteDirPath() + "script.txt";

	if (!newProcess) {
		// signal SpringApp
		gameSetup->reloadScript = scriptText;
		gu->globalReload = true;

		LOG("[%s] Spring \"%s\" should be reloading", __func__, springFullName.c_str());
		return 0;
	}

	std::array<std::string, 32> processArgs;

	processArgs[0] = springFullName;
	processArgs[1] = " ";

	#if 0
	// arguments to Spring binary given by Lua code, if any
	if (!springArgs.empty())
		processArgs[1] = springArgs;
	#endif

	if (!scriptText.empty()) {
		// create file 'script.txt' with contents given by Lua code
		std::ofstream scriptFile(scriptFullName.c_str());

		scriptFile.write(scriptText.c_str(), scriptText.size());
		scriptFile.close();

		processArgs[2] = scriptFullName;
	}

	#ifdef _WIN32
	// else OpenAL crashes when using execvp
	ISound::Shutdown(false);
	#endif
	// close local socket to avoid "bind: Address already in use"
	spring::SafeDelete(gameServer);

	LOG("[%s] Spring \"%s\" should be restarting", __func__, springFullName.c_str());
	Platform::ExecuteProcess(processArgs, newProcess);

	// only reached on execvp failure
	return 1;
}




/*** Closes the application
 *
 * @function Spring.Quit
 * @return nil
 */
int LuaUnsyncedCtrl::Quit(lua_State* L)
{
	gu->globalQuit = true;
	return 0;
}

/******************************************************************************
 * Unit Group
 * @section unitgroup
******************************************************************************/

/***
 *
 * @function Spring.SetUnitGroup
 * @param unitID integer
 * @param groupID integer the group number to be assigned, or -1 for deassignment
 * @return nil
 */
int LuaUnsyncedCtrl::SetUnitGroup(lua_State* L)
{
	if (gs->noHelperAIs)
		return 0;

	CUnit* unit = ParseRawUnit(L, __func__, 1);

	if (unit == nullptr)
		return 0;

	const int groupID = luaL_checkint(L, 2);

	if (groupID == -1) {
		unit->SetGroup(nullptr);
		return 0;
	}

	if (!uiGroupHandlers[gu->myTeam].HasGroup(groupID))
		return 0;

	CGroup* group = uiGroupHandlers[gu->myTeam].GetGroup(groupID);

	if (group != nullptr)
		unit->SetGroup(group);

	return 0;
}


/******************************************************************************
 * Give Order
 * @section giveorder
******************************************************************************/

static void ParseUnitMap(lua_State* L, const char* caller,
                         int table, vector<int>& unitIDs)
{
	if (!lua_istable(L, table))
		luaL_error(L, "%s(): error parsing unit map", caller);

	for (lua_pushnil(L); lua_next(L, table) != 0; lua_pop(L, 1)) {
		if (lua_israwnumber(L, -2)) {
			CUnit* unit = ParseCtrlUnit(L, __func__, -2); // the key

			if (unit != nullptr && !unit->noSelect)
				unitIDs.push_back(unit->id);
		}
	}
}


static void ParseUnitArray(lua_State* L, const char* caller,
                           int table, vector<int>& unitIDs)
{
	if (!lua_istable(L, table))
		luaL_error(L, "%s(): error parsing unit array", caller);

	for (lua_pushnil(L); lua_next(L, table) != 0; lua_pop(L, 1)) {
		if (lua_israwnumber(L, -2) && lua_isnumber(L, -1)) {   // avoid 'n'
			CUnit* unit = ParseCtrlUnit(L, __func__, -1); // the value

			if (unit != nullptr && !unit->noSelect)
				unitIDs.push_back(unit->id);
		}
	}
}


/******************************************************************************/

static bool CanGiveOrders(const lua_State* L)
{
	if (gs->PreSimFrame())
		return false;

	if (gs->noHelperAIs)
		return false;

	const int ctrlTeam = CLuaHandle::GetHandleCtrlTeam(L);

	if (gu->GetMyPlayer()->CanControlTeam(ctrlTeam))
		return true;

	// FIXME ? (correct? warning / error?)
	return (!gu->spectating && (ctrlTeam == gu->myTeam) && (ctrlTeam >= 0));
}



/***
 * Give order to selected units.
 *
 * @function Spring.GiveOrder
 * @param cmdID CMD|integer The command ID.
 * @param params CreateCommandParams Parameters for the given command.
 * @param options CreateCommandOptions?
 * @param timeout integer? Absolute frame number. The command will be discarded after this frame. Only respected by mobile units.
 * @return boolean
 */
int LuaUnsyncedCtrl::GiveOrder(lua_State* L)
{
	if (!CanGiveOrders(L)) {
		lua_pushboolean(L, false);
		return 1;
	}

	selectedUnitsHandler.GiveCommand(LuaUtils::ParseCommand(L, __func__, 1));

	lua_pushboolean(L, true);
	return 1;
}


/***
 * Give order to specific unit.
 *
 * @function Spring.GiveOrderToUnit
 * @param unitID integer
 * @param cmdID CMD|integer The command ID.
 * @param params CreateCommandParams? Parameters for the given command.
 * @param options CreateCommandOptions?
 * @param timeout integer? Absolute frame number. The command will be discarded after this frame. Only respected by mobile units.
 * @return boolean
 */
int LuaUnsyncedCtrl::GiveOrderToUnit(lua_State* L)
{
	if (!CanGiveOrders(L)) {
		lua_pushboolean(L, false);
		return 1;
	}

	const CUnit* unit = ParseCtrlUnit(L, __func__, 1);

	if (unit == nullptr || unit->noSelect) {
		lua_pushboolean(L, false);
		return 1;
	}

	const Command cmd = LuaUtils::ParseCommand(L, __func__, 2);

	clientNet->Send(CBaseNetProtocol::Get().SendAICommand(gu->myPlayerNum, MAX_AIS, MAX_TEAMS, unit->id, cmd.GetID(false), cmd.GetID(true), cmd.GetTimeOut(), cmd.GetOpts(), cmd.GetNumParams(), cmd.GetParams()));

	lua_pushboolean(L, true);
	return 1;
}


/***
 * Give order to multiple units, specified by table keys.
 *
 * @function Spring.GiveOrderToUnitMap
 * @param unitMap table<integer, any> A table with unit IDs as keys.
 * @param cmdID CMD|integer The command ID.
 * @param params CreateCommandParams? Parameters for the given command.
 * @param options CreateCommandOptions?
 * @param timeout integer? Absolute frame number. The command will be discarded after this frame. Only respected by mobile units.
 * @return boolean orderGiven
 */
int LuaUnsyncedCtrl::GiveOrderToUnitMap(lua_State* L)
{
	if (!CanGiveOrders(L)) {
		lua_pushboolean(L, false);
		return 1;
	}

	// unitIDs
	vector<int> unitIDs;
	ParseUnitMap(L, __func__, 1, unitIDs);

	if (unitIDs.empty()) {
		lua_pushboolean(L, false);
		return 1;
	}

	selectedUnitsHandler.SendCommandsToUnits(unitIDs, {LuaUtils::ParseCommand(L, __func__, 2)});

	lua_pushboolean(L, true);
	return 1;
}


/***
 * Give order to an array of units.
 *
 * @function Spring.GiveOrderToUnitArray
 * @param unitIDs integer[] Array of unit IDs.
 * @param cmdID CMD|integer The command ID.
 * @param params CreateCommandParams? Parameters for the given command.
 * @param options CreateCommandOptions?
 * @param timeout integer? Absolute frame number. The command will be discarded after this frame. Only respected by mobile units.
 * @return boolean ordersGiven `true` if any orders were sent, otherwise `false`.
 */
int LuaUnsyncedCtrl::GiveOrderToUnitArray(lua_State* L)
{
	if (!CanGiveOrders(L)) {
		lua_pushboolean(L, false);
		return 1;
	}

	// unitIDs
	vector<int> unitIDs;
	ParseUnitArray(L, __func__, 1, unitIDs);

	if (unitIDs.empty()) {
		lua_pushboolean(L, false);
		return 1;
	}

	selectedUnitsHandler.SendCommandsToUnits(unitIDs, {LuaUtils::ParseCommand(L, __func__, 2)});

	lua_pushboolean(L, true);
	return 1;
}

/***
 *
 * @function Spring.GiveOrderArrayToUnit
 * @param unitID integer Unit ID.
 * @param commands CreateCommand[]
 * @return boolean ordersGiven `true` if any orders were sent, otherwise `false`.
 */
int LuaUnsyncedCtrl::GiveOrderArrayToUnit(lua_State* L)
{
	if (!CanGiveOrders(L)) {
		lua_pushboolean(L, false);
		return 1;
	}

	const CUnit* const unit = ParseCtrlUnit(L, __func__, 1);
	if (unit == nullptr || unit->noSelect) {
		lua_pushboolean(L, false);
		return 1;
	}
	vector<int> unitIDs {unit->id};

	vector<Command> commands;
	LuaUtils::ParseCommandArray(L, __func__, 2, commands);
	if (commands.empty()) {
		lua_pushboolean(L, false);
		return 1;
	}

	selectedUnitsHandler.SendCommandsToUnits(unitIDs, commands);
	lua_pushboolean(L, true);
	return 1;
}


/***
 *
 * @function Spring.GiveOrderArrayToUnitMap
 * @param unitMap table<integer, any> A table with unit IDs as keys.
 * @param commands CreateCommand[]
 * @return boolean ordersGiven `true` if any orders were sent, otherwise `false`.
 */
int LuaUnsyncedCtrl::GiveOrderArrayToUnitMap(lua_State* L)
{
	if (!CanGiveOrders(L)) {
		lua_pushboolean(L, false);
		return 1;
	}

	// unitIDs
	vector<int> unitIDs;
	ParseUnitMap(L, __func__, 1, unitIDs);

	// commands
	vector<Command> commands;
	LuaUtils::ParseCommandArray(L, __func__, 2, commands);

	if (unitIDs.empty() || commands.empty()) {
		lua_pushboolean(L, false);
		return 1;
	}

	selectedUnitsHandler.SendCommandsToUnits(unitIDs, commands);

	lua_pushboolean(L, true);
	return 1;
}


/***
 * @function Spring.GiveOrderArrayToUnitArray
 * @param unitIDs integer[] Array of unit IDs.
 * @param commands CreateCommand[]
 * @param pairwise boolean? (Default: `false`) When `false`, assign all commands to each unit.
 *
 * When `true`, assign commands according to index between units and cmds arrays.
 *
 * If `len(unitArray) < len(cmdArray)` only the first `len(unitArray)` commands
 * will be assigned, and vice-versa.
 *
 * @return boolean ordersGiven `true` if any orders were sent, otherwise `false`.
 */
int LuaUnsyncedCtrl::GiveOrderArrayToUnitArray(lua_State* L)
{
	if (!CanGiveOrders(L)) {
		lua_pushboolean(L, false);
		return 1;
	}

	const int args = lua_gettop(L); // number of arguments

	// unitIDs
	vector<int> unitIDs;
	ParseUnitArray(L, __func__, 1, unitIDs);

	// commands
	vector<Command> commands;
	LuaUtils::ParseCommandArray(L, __func__, 2, commands);

	if (unitIDs.empty() || commands.empty()) {
		lua_pushboolean(L, false);
		return 1;
	}

	selectedUnitsHandler.SendCommandsToUnits(unitIDs, commands, (args >= 3 && lua_toboolean(L, 3)));

	lua_pushboolean(L, true);
	return 1;
}


/***
 *
 * @function Spring.SetBuildSpacing
 * @param spacing number
 * @return nil
 */
int LuaUnsyncedCtrl::SetBuildSpacing(lua_State* L)
{
	if (guihandler != nullptr)
		guihandler->SetBuildSpacing(luaL_checkinteger(L, 1));

	return 0;
}


/***
 *
 * @function Spring.SetBuildFacing
 * @param facing FacingInteger
 * @return nil
 */
int LuaUnsyncedCtrl::SetBuildFacing(lua_State* L)
{
	if (guihandler != nullptr)
		guihandler->SetBuildFacing(luaL_checkint(L, 1));

	return 0;
}


/******************************************************************************
 * UI
 * @section ui
 * Very important! (allows synced inter-lua-environment communications)
******************************************************************************/


/*** @function Spring.SendLuaUIMsg
 * @param message string
 * @param mode string "s"/"specs" | "a"/"allies"
 * @return nil
 */
int LuaUnsyncedCtrl::SendLuaUIMsg(lua_State* L)
{
	const std::string msg = luaL_checksstring(L, 1);
	const std::vector<std::uint8_t> data(msg.begin(), msg.end());

	const char* mode = luaL_optstring(L, 2, "");

	if (mode[0] != 0 && mode[0] != 'a' && mode[0] != 's')
		luaL_error(L, "Unknown SendLuaUIMsg() mode");

	try {
		clientNet->Send(CBaseNetProtocol::Get().SendLuaMsg(gu->myPlayerNum, LUA_HANDLE_ORDER_UI, mode[0], data));
	} catch (const netcode::PackPacketException& ex) {
		luaL_error(L, "SendLuaUIMsg() packet error: %s", ex.what());
	}

	return 0;
}


/*** @function Spring.SendLuaGaiaMsg
 * @param message string
 * @return nil
 */
int LuaUnsyncedCtrl::SendLuaGaiaMsg(lua_State* L)
{
	const std::string msg = luaL_checksstring(L, 1);
	const std::vector<std::uint8_t> data(msg.begin(), msg.end());

	try {
		clientNet->Send(CBaseNetProtocol::Get().SendLuaMsg(gu->myPlayerNum, LUA_HANDLE_ORDER_GAIA, 0, data));
	} catch (const netcode::PackPacketException& ex) {
		luaL_error(L, "SendLuaGaiaMsg() packet error: %s", ex.what());
	}

	return 0;
}


/*** @function Spring.SendLuaRulesMsg
 * @param message string
 * @return nil
 */
int LuaUnsyncedCtrl::SendLuaRulesMsg(lua_State* L)
{
	const std::string msg = luaL_checksstring(L, 1);
	const std::vector<std::uint8_t> data(msg.begin(), msg.end());

	try {
		clientNet->Send(CBaseNetProtocol::Get().SendLuaMsg(gu->myPlayerNum, LUA_HANDLE_ORDER_RULES, 0, data));
	} catch (const netcode::PackPacketException& ex) {
		luaL_error(L, "SendLuaRulesMsg() packet error: %s", ex.what());
	}
	return 0;
}


/***
 *
 * @function Spring.SendLuaMenuMsg
 *
 * @param msg string
 */
int LuaUnsyncedCtrl::SendLuaMenuMsg(lua_State* L)
{
	if (luaMenu != nullptr)
		luaMenu->RecvLuaMsg(luaL_checksstring(L, 1), gu->myPlayerNum);

	return 0;
}


/******************************************************************************
 * Sharing
 * @section sharing
******************************************************************************/


/***
 *
 * @function Spring.SetShareLevel
 *
 * @param resource string metal | energy
 * @param shareLevel number
 * @return nil
 */
int LuaUnsyncedCtrl::SetShareLevel(lua_State* L)
{
	if (gu->spectating || gs->noHelperAIs || gs->PreSimFrame())
		return 0;


	const char* shareType = lua_tostring(L, 1);
	const float shareLevel = std::clamp(luaL_checkfloat(L, 2), 0.0f, 1.0f);

	if (shareType[0] == 'm') {
		clientNet->Send(CBaseNetProtocol::Get().SendSetShare(gu->myPlayerNum, gu->myTeam, shareLevel, teamHandler.Team(gu->myTeam)->resShare.energy));
		return 0;
	}
	if (shareType[0] == 'e') {
		clientNet->Send(CBaseNetProtocol::Get().SendSetShare(gu->myPlayerNum, gu->myTeam, teamHandler.Team(gu->myTeam)->resShare.metal, shareLevel));
		return 0;
	}

	LOG_L(L_WARNING, "[%s] unknown resource-type \"%s\"", __func__, shareType);
	return 0;
}


/***
 *
 * @function Spring.ShareResources
 *
 * @param teamID integer
 * @param units string
 * @return nil
 */

/***
 *
 * @function Spring.ShareResources
 *
 * @param teamID integer
 * @param resource string metal | energy
 * @param amount number
 * @return nil
 */
int LuaUnsyncedCtrl::ShareResources(lua_State* L)
{
	if (gu->spectating || gs->noHelperAIs || gs->PreSimFrame())
		return 0;

	const int args = lua_gettop(L); // number of arguments
	if ((args < 2) || !lua_isnumber(L, 1) || !lua_isstring(L, 2))
		luaL_error(L, "Incorrect arguments to ShareResources()");

	const int teamID = lua_toint(L, 1);
	if (!teamHandler.IsValidTeam(teamID))
		return 0;

	const CTeam* team = teamHandler.Team(teamID);
	if ((team == nullptr) || team->isDead)
		return 0;

	const char* type = lua_tostring(L, 2);
	if (type[0] == 'u') {
		selectedUnitsHandler.SendSelect();
		clientNet->Send(CBaseNetProtocol::Get().SendShare(gu->myPlayerNum, teamID, 1, 0.0f, 0.0f));
		selectedUnitsHandler.ClearSelected();
		return 0;
	}

	if (!lua_isnumber(L, 3))
		luaL_error(L, "Incorrect third argument to ShareResources() for the specified resource");

	if (type[0] == 'm') {
		clientNet->Send(CBaseNetProtocol::Get().SendShare(gu->myPlayerNum, teamID, 0, lua_tofloat(L, 3), 0.0f));
		return 0;
	}
	if (type[0] == 'e') {
		clientNet->Send(CBaseNetProtocol::Get().SendShare(gu->myPlayerNum, teamID, 0, 0.0f, lua_tofloat(L, 3)));
		return 0;
	}

	return 0;
}


/******************************************************************************
 * UI
 * @section ui
******************************************************************************/


/*** @function Spring.SetLastMessagePosition
 * @param x number
 * @param y number
 * @param z number
 * @return nil
 */
int LuaUnsyncedCtrl::SetLastMessagePosition(lua_State* L)
{
	const float3 pos(luaL_checkfloat(L, 1),
	                 luaL_checkfloat(L, 2),
	                 luaL_checkfloat(L, 3));

	eventHandler.LastMessagePosition(pos);
	return 0;
}


/******************************************************************************
 * Markers
 * @section markers
******************************************************************************/


/*** @function Spring.MarkerAddPoint
 * @param x number
 * @param y number
 * @param z number
 * @param text string? (Default: `""`)
 * @param localOnly boolean?
 * @return nil
 */
int LuaUnsyncedCtrl::MarkerAddPoint(lua_State* L)
{
	if (inMapDrawer == nullptr)
		return 0;

	const float3 pos(luaL_checkfloat(L, 1),
	                 luaL_checkfloat(L, 2),
	                 luaL_checkfloat(L, 3));
	const string text = luaL_optstring(L, 4, "");
	const bool onlyLocal = luaL_optboolean(L, 5, true);

	if (onlyLocal) {
		inMapDrawerModel->AddPoint(pos, text, luaL_optnumber(L, 6, gu->myPlayerNum));
	} else {
		inMapDrawer->SendPoint(pos, text, true);
	}

	return 0;
}


/*** @function Spring.MarkerAddLine
 * @param x1 number
 * @param y1 number
 * @param z1 number
 * @param x2 number
 * @param y2 number
 * @param z2 number
 * @param localOnly boolean? (Default: `false`)
 * @param playerId number?
 * @return nil
 */
int LuaUnsyncedCtrl::MarkerAddLine(lua_State* L)
{
	if (inMapDrawer == nullptr)
		return 0;

	const float3 pos1(luaL_checkfloat(L, 1),
	                  luaL_checkfloat(L, 2),
	                  luaL_checkfloat(L, 3));
	const float3 pos2(luaL_checkfloat(L, 4),
	                  luaL_checkfloat(L, 5),
	                  luaL_checkfloat(L, 6));
	const bool onlyLocal = luaL_optboolean(L, 7, false);

	if (onlyLocal) {
		inMapDrawerModel->AddLine(pos1, pos2, luaL_optnumber(L, 8, gu->myPlayerNum));
	} else {
		inMapDrawer->SendLine(pos1, pos2, true);
	}

	return 0;
}


/*** @function Spring.MarkerErasePosition
 *
 * Issue an erase command for markers on the map.
 *
 * @param x number
 * @param y number
 * @param z number
 * @param unused nil This argument is ignored.
 * @param localOnly boolean? (Default: `false`) do not issue a network message, erase only for the current player
 * @param playerId number? when not specified it uses the issuer playerId
 * @param alwaysErase boolean? (Default: `false`) erase any marker when `localOnly` and current player is spectating. Allows spectators to erase players markers locally
 * @return nil
 */
int LuaUnsyncedCtrl::MarkerErasePosition(lua_State* L)
{
	if (inMapDrawer == nullptr)
		return 0;

	const float3 pos(luaL_checkfloat(L, 1),
	                 luaL_checkfloat(L, 2),
	                 luaL_checkfloat(L, 3));

	/* Argument 4 is going to be radius
	 * after some future refactoring. */

	const bool onlyLocal = luaL_optboolean(L, 5, false);
	if (onlyLocal) {
		// always erase if onlyLocal and current player is spectator
		const bool alwaysErase = luaL_optboolean(L, 7, false) && gu->spectating;
		inMapDrawerModel->EraseNear(pos, luaL_optnumber(L, 6, gu->myPlayerNum), alwaysErase);
	} else {
		inMapDrawer->SendErase(pos);
	}

	return 0;
}


/******************************************************************************
 * Sun
 * @section sun
******************************************************************************/

/***
 * @class AtmosphereParams
 * @x_helper
 * @field fogStart number
 * @field fogEnd number
 * @field sunColor rgba
 * @field skyColor rgba
 * @field cloudColor rgba
 * @field skyAxisAngle xyzw rotation axis and angle in radians of skybox orientation
 */

/*** Set atmosphere parameters
 *
 * @function Spring.SetAtmosphere
 * @param params AtmosphereParams
 */
int LuaUnsyncedCtrl::SetAtmosphere(lua_State* L)
{
	if (!lua_istable(L, 1))
		luaL_error(L, "Incorrect arguments to SetAtmosphere()");

	const auto& sky = ISky::GetSky();
	for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1)) {
		if (!lua_israwstring(L, -2))
			continue;

		const char* key = lua_tostring(L, -2);

		if (lua_istable(L, -1)) {
			float4 values;
			LuaUtils::ParseFloatArray(L, -1, &values[0], 4);

			switch (hashString(key)) {
				case hashString("fogColor"): {
					sky->fogColor = values;
				} break;
				case hashString("skyColor"): {
					sky->skyColor = values;
				} break;
				case hashString("sunColor"): {
					sky->sunColor = values;
				} break;
				case hashString("cloudColor"): {
					sky->cloudColor = values;
				} break;
				case hashString("skyAxisAngle"): {
					sky->SetSkyAxisAngle(values);
				} break;
				default: {
					luaL_error(L, "[%s] unknown array key %s", __func__, key);
				} break;
			}

			continue;
		}

		if (lua_isnumber(L, -1)) {
			switch (hashString(key)) {
				case hashString("fogStart"): {
					sky->fogStart = lua_tofloat(L, -1);
				} break;
				case hashString("fogEnd"): {
					sky->fogEnd = lua_tofloat(L, -1);
				} break;
				default: {
					luaL_error(L, "[%s] unknown scalar key %s", __func__, key);
				} break;
			}

			continue;
		}
	}

	sky->SetUpdated();

	return 0;
}


/***
 *
 * @function Spring.SetSunDirection
 * @param dirX number
 * @param dirY number
 * @param dirZ number
 * @param intensity number? (Default: `1.0`)
 * @return nil
 */
int LuaUnsyncedCtrl::SetSunDirection(lua_State* L)
{
	auto dir = float3(luaL_checkfloat(L, 1), luaL_checkfloat(L, 2), luaL_checkfloat(L, 3));
	auto intensity = luaL_optfloat(L, 4, 1.0f); // seems broken atm, only toggles shadows off when set to 0
	ISky::GetSky()->GetLight()->SetLightDir(float4(dir.SafeNormalize(), intensity));
	return 0;
}


/***
 * Modify sun lighting parameters.
 *
 * ```lua
 * Spring.SetSunLighting({groundAmbientColor = {1, 0.1, 1}, groundDiffuseColor = {1, 0.1, 1} })
 * ```
 *
 * @function Spring.SetSunLighting
 * @param params { groundAmbientColor: rgb, groundDiffuseColor: rgb }
 */
int LuaUnsyncedCtrl::SetSunLighting(lua_State* L)
{
	if (!lua_istable(L, 1))
		luaL_error(L, "[%s] argument should be a table", __func__);

	CSunLighting sl = *sunLighting;

	for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1)) {
		if (!lua_israwstring(L, -2))
			continue;

		const char* key = lua_tostring(L, -2);

		if (lua_istable(L, -1)) {
			float4 color;
			LuaUtils::ParseFloatArray(L, -1, &color[0], 4);

			if (sl.SetValue(key, color))
				continue;

			luaL_error(L, "[%s] unknown array key %s", __func__, key);
		}

		if (lua_isnumber(L, -1)) {
			if (sl.SetValue(key, lua_tofloat(L, -1)))
				continue;

			luaL_error(L, "[%s] unknown scalar key %s", __func__, key);
		}
	}

	*sunLighting = sl;
	sunLighting->SetUpdated();
	return 0;
}


/*** Map rendering params
 *
 * @class MapRenderingParams
 * @x_helper
 * @field splatTexMults rgba
 * @field splatTexScales rgba
 * @field voidWater boolean
 * @field voidGround boolean
 * @field splatDetailNormalDiffuseAlpha boolean
 */


/*** Allows to change map rendering params at runtime.
 *
 * @function Spring.SetMapRenderingParams
 * @param params MapRenderingParams
 * @return nil
 */
int LuaUnsyncedCtrl::SetMapRenderingParams(lua_State* L)
{
	if (!lua_istable(L, 1))
		luaL_error(L, "[%s] incorrect arguments");

	for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1)) {
		if (!lua_israwstring(L, -2))
			continue;

		const char* key = lua_tostring(L, -2);

		if (lua_istable(L, -1)) {
			float values[4];
			const int size = LuaUtils::ParseFloatArray(L, -1, values, 4);

			if (size < 4)
				luaL_error(L, "[%s] unexpected size %d for array key \"%s\"", __func__, size, key);

			switch (hashString(key)) {
				case hashString("splatTexScales"): {
					mapRendering->splatTexScales = values;
				} break;
				case hashString("splatTexMults"): {
					mapRendering->splatTexMults = values;
				} break;
				default: {
					luaL_error(L, "[%s] unknown array key \"%s\"", __func__, key);
				} break;
			}

			continue;
		}

		if (lua_isboolean(L, -1)) {
			const bool value = lua_toboolean(L, -1);

			switch (hashString(key)) {
				case hashString("voidWater"): {
					mapRendering->voidWater = value;
				} break;
				case hashString("voidGround"): {
					mapRendering->voidGround = value;
				} break;
				case hashString("splatDetailNormalDiffuseAlpha"): {
					mapRendering->splatDetailNormalDiffuseAlpha = value;
				} break;
				default: {
					luaL_error(L, "[%s] unknown boolean key \"%s\"", __func__, key);
				} break;
			}
		}
	}

	CBaseGroundDrawer* groundDrawer = readMap->GetGroundDrawer();
	groundDrawer->UpdateRenderState();

	return 0;
}


/***
 *
 * @function Spring.ForceTesselationUpdate
 * @param normal boolean? (Default: `true`)
 * @param shadow boolean? (Default: `false`)
 * @return boolean updated
 */
int LuaUnsyncedCtrl::ForceTesselationUpdate(lua_State* L)
{
	CSMFGroundDrawer* smfDrawer = dynamic_cast<CSMFGroundDrawer*>(readMap->GetGroundDrawer());

	if (smfDrawer == nullptr) {
		lua_pushboolean(L, false);
		return 1;
	}

	CRoamMeshDrawer* roamMeshDrawer = dynamic_cast<CRoamMeshDrawer*>(smfDrawer->GetMeshDrawer());
	if (roamMeshDrawer == nullptr) {
		lua_pushboolean(L, false);
		return 1;
	}

	CRoamMeshDrawer::ForceNextTesselation(
		luaL_optboolean(L, 1, true ),
		luaL_optboolean(L, 2, false)
	);

	lua_pushboolean(L, true);
	return 1;
}


/******************************************************************************
 * AI
 * @section ai
******************************************************************************/


/*** @function Spring.SendSkirmishAIMessage
 * @param aiTeam number
 * @param message string
 * @return boolean? ai_processed
 */
int LuaUnsyncedCtrl::SendSkirmishAIMessage(lua_State* L) {
	if (CLuaHandle::GetHandleSynced(L))
		return 0;

	const int aiTeam = luaL_checkint(L, 1);
	const char* inData = luaL_checkstring(L, 2);

	std::vector<const char*> outData;

	luaL_checkstack(L, 2, __func__);
	lua_pushboolean(L, eoh->SendLuaMessages(aiTeam, inData, outData));

	// push the AI response(s)
	lua_createtable(L, outData.size(), 0);
	for (unsigned int n = 0; n < outData.size(); n++) {
		lua_pushstring(L, outData[n]);
		lua_rawseti(L, -2, n + 1);
	}

	return 2;
}


/******************************************************************************
 * Developers
 * @section developers
******************************************************************************/


/*** @function Spring.SetLogSectionFilterLevel
 * @param sectionName string
 * @param logLevel ?string|number
 * @return nil
 */
int LuaUnsyncedCtrl::SetLogSectionFilterLevel(lua_State* L) {
	const int loglevel = LuaUtils::ParseLogLevel(L, 2);

	if (loglevel < 0)
		return luaL_error(L, "Incorrect arguments to Spring.SetLogSectionFilterLevel(logsection, loglevel)");

	log_frontend_register_runtime_section(loglevel, luaL_checkstring(L, 1));
	return 0;
}

/*** @function Spring.GarbageCollectCtrl
 *
 * @param itersPerBatch integer?
 * @param numStepsPerIter integer?
 * @param minStepsPerIter integer?
 * @param maxStepsPerIter integer?
 * @param minLoopRunTime number?
 * @param maxLoopRunTime number?
 * @param baseRunTimeMult number?
 * @param baseMemLoadMult number?
 * @return nil
 */
int LuaUnsyncedCtrl::GarbageCollectCtrl(lua_State* L) {
	luaContextData* ctxData = GetLuaContextData(L);
	SLuaGarbageCollectCtrl& gcCtrl = ctxData->gcCtrl;

	gcCtrl.itersPerBatch = std::max(0, luaL_optint(L, 1, gcCtrl.itersPerBatch));

	gcCtrl.numStepsPerIter = std::max(0, luaL_optint(L, 2, gcCtrl.numStepsPerIter));
	gcCtrl.minStepsPerIter = std::max(0, luaL_optint(L, 3, gcCtrl.minStepsPerIter));
	gcCtrl.maxStepsPerIter = std::max(0, luaL_optint(L, 4, gcCtrl.maxStepsPerIter));

	gcCtrl.minLoopRunTime = std::max(0.0f, luaL_optfloat(L, 5, gcCtrl.minLoopRunTime));
	gcCtrl.maxLoopRunTime = std::max(0.0f, luaL_optfloat(L, 6, gcCtrl.maxLoopRunTime));

	gcCtrl.baseRunTimeMult = std::max(0.0f, luaL_optfloat(L, 7, gcCtrl.baseRunTimeMult));
	gcCtrl.baseMemLoadMult = std::max(0.0f, luaL_optfloat(L, 8, gcCtrl.baseMemLoadMult));

	return 0;
}


/*** @function Spring.SetAutoShowMetal
 * @param autoShow boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetAutoShowMetal(lua_State* L)
{
	guihandler->autoShowMetal = luaL_checkboolean(L, 1);
	return 0;
}


/*** @function Spring.SetDrawSky
 * @param drawSky boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetDrawSky(lua_State* L)
{
	globalRendering->drawSky = !!luaL_checkboolean(L, 1);
	return 0;
}


/*** @function Spring.SetDrawWater
 * @param drawWater boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetDrawWater(lua_State* L)
{
	globalRendering->drawWater = !!luaL_checkboolean(L, 1);
	return 0;
}


/*** @function Spring.SetDrawGround
 * @param drawGround boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetDrawGround(lua_State* L)
{
	globalRendering->drawGround = !!luaL_checkboolean(L, 1);
	return 0;
}


/*** @function Spring.SetDrawGroundDeferred
 * @param drawGroundDeferred boolean
 * @param drawGroundForward boolean? allows disabling of the forward pass
 * @return nil
 */
int LuaUnsyncedCtrl::SetDrawGroundDeferred(lua_State* L)
{
	CBaseGroundDrawer* gd = readMap->GetGroundDrawer();

	gd->SetDrawDeferredPass(luaL_checkboolean(L, 1));
	gd->SetDrawForwardPass(luaL_optboolean(L, 2, gd->DrawForward()));

	lua_pushboolean(L, gd->DrawDeferred());
	lua_pushboolean(L, gd->DrawForward());
	return 2;
}

/*** @function Spring.SetDrawModelsDeferred
 * @param drawUnitsDeferred boolean
 * @param drawFeaturesDeferred boolean
 * @param drawUnitsForward boolean? allows disabling of the respective forward passes
 * @param drawFeaturesForward boolean? allows disabling of the respective forward passes
 * @return nil
 */
int LuaUnsyncedCtrl::SetDrawModelsDeferred(lua_State* L)
{
	// NOTE the argument ordering
	unitDrawer->SetDrawDeferredPass(luaL_checkboolean(L, 1));
	unitDrawer->SetDrawForwardPass(luaL_optboolean(L, 3, unitDrawer->DrawForward()));

	featureDrawer->SetDrawDeferredPass(luaL_checkboolean(L, 2));
	featureDrawer->SetDrawForwardPass(luaL_optboolean(L, 4, featureDrawer->DrawForward()));

	lua_pushboolean(L,    unitDrawer->DrawDeferred());
	lua_pushboolean(L, featureDrawer->DrawDeferred());
	lua_pushboolean(L,    unitDrawer->DrawForward());
	lua_pushboolean(L, featureDrawer->DrawForward());
	return 4;
}


/*** This doesn't actually record the game in any way, it just regulates the framerate and interpolations.
 *
 * @function Spring.SetVideoCapturingMode
 * @param allowCaptureMode boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetVideoCapturingMode(lua_State* L)
{
	videoCapturing->SetAllowRecord(luaL_checkboolean(L, 1));
	return 0;
}


/*** @function Spring.SetVideoCapturingTimeOffset
 * @param timeOffset boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetVideoCapturingTimeOffset(lua_State* L)
{
	videoCapturing->SetTimeOffset(luaL_checkfloat(L, 1));
	return 0;
}


/***
 * Water params
 *
 * @class WaterParams
 * @x_helper
 * @field absorb rgb
 * @field baseColor rgb
 * @field minColor rgb
 * @field surfaceColor rgb
 * @field diffuseColor rgb
 * @field specularColor rgb
 * @field planeColor rgb
 * @field texture string file
 * @field foamTexture string file
 * @field normalTexture string file
 * @field damage number
 * @field repeatX number
 * @field repeatY number
 * @field surfaceAlpha number
 * @field ambientFactor number
 * @field diffuseFactor number
 * @field specularFactor number
 * @field specularPower number
 * @field fresnelMin number
 * @field fresnelMax number
 * @field fresnelPower number
 * @field reflectionDistortion number
 * @field blurBase number
 * @field blurExponent number
 * @field perlinStartFreq number
 * @field perlinLacunarity number
 * @field perlinAmplitude number
 * @field windSpeed number
 * @field waveOffsetFactor number
 * @field waveLength number
 * @field waveFoamDistortion number
 * @field waveFoamIntensity number
 * @field causticsResolution number
 * @field causticsStrength number
 * @field numTiles integer
 * @field shoreWaves boolean
 * @field forceRendering boolean
 * @field hasWaterPlane boolean
 */

/***
 * Does not need cheating enabled.
 *
 * Allows to change water params (mostly `BumpWater` ones) at runtime. You may
 * want to set `BumpWaterUseUniforms` in your `springrc` to 1, then you don't even
 * need to restart `BumpWater` via `/water 4`.
 *
 * @function Spring.SetWaterParams
 * @param waterParams WaterParams
 * @return nil
 */
int LuaUnsyncedCtrl::SetWaterParams(lua_State* L)
{
	if (!lua_istable(L, 1))
		luaL_error(L, "[%s] incorrect arguments", __func__);

	for (lua_pushnil(L); lua_next(L, 1) != 0; lua_pop(L, 1)) {
		if (!lua_israwstring(L, -2))
			continue;

		const char* key = lua_tostring(L, -2);

		switch (lua_type(L, -1)) {
			case LUA_TTABLE: {
				float color[3];
				const int size = LuaUtils::ParseFloatArray(L, -1, color, 3);

				if (size < 3)
					luaL_error(L, "[%s] unexpected size %d for array key %s", __func__, size, key);

				switch (hashString(key)) {
					case hashString("absorb"): {
						waterRendering->absorb = color;
					} break;
					case hashString("baseColor"): {
						waterRendering->baseColor = color;
					} break;
					case hashString("minColor"): {
						waterRendering->minColor = color;
					} break;
					case hashString("surfaceColor"): {
						waterRendering->surfaceColor = color;
					} break;
					case hashString("diffuseColor"): {
						waterRendering->diffuseColor = color;
					} break;
					case hashString("specularColor"): {
						waterRendering->specularColor = color;
					} break;
					case hashString("planeColor"): {
						waterRendering->planeColor.x = color[0];
						waterRendering->planeColor.y = color[1];
						waterRendering->planeColor.z = color[2];
					} break;
					default: {
						luaL_error(L, "[%s] unknown array key \"%s\"", __func__, key);
					} break;
				}

				continue;
			} break;

			case LUA_TSTRING: {
				const std::string value = lua_tostring(L, -1);

				switch (hashString(key)) {
					case hashString("texture"): {
						waterRendering->texture = value;
					} break;
					case hashString("foamTexture"): {
						waterRendering->foamTexture = value;
					} break;
					case hashString("normalTexture"): {
						waterRendering->normalTexture = value;
					} break;
					default: {
						luaL_error(L, "[%s] unknown string key \"%s\"", __func__, key);
					} break;
				}

				continue;
			} break;

			case LUA_TNUMBER: {
				const float value = lua_tofloat(L, -1);

				switch (hashString(key)) {
					case hashString("repeatX"): {
						waterRendering->repeatX = value;
					} break;
					case hashString("repeatY"): {
						waterRendering->repeatY = value;
					} break;

					case hashString("surfaceAlpha"): {
						waterRendering->surfaceAlpha = value;
					} break;

					case hashString("ambientFactor"): {
						waterRendering->ambientFactor = value;
					} break;
					case hashString("diffuseFactor"): {
						waterRendering->diffuseFactor = value;
					} break;
					case hashString("specularFactor"): {
						waterRendering->specularFactor = value;
					} break;
					case hashString("specularPower"): {
						waterRendering->specularPower = value;
					} break;

					case hashString("fresnelMin"): {
						waterRendering->fresnelMin = value;
					} break;
					case hashString("fresnelMax"): {
						waterRendering->fresnelMax = value;
					} break;
					case hashString("fresnelPower"): {
						waterRendering->fresnelPower = value;
					} break;

					case hashString("reflectionDistortion"): {
						waterRendering->reflDistortion = value;
					} break;

					case hashString("blurBase"): {
						waterRendering->blurBase = value;
					} break;
					case hashString("blurExponent"): {
						waterRendering->blurExponent = value;
					} break;

					case hashString("perlinStartFreq"): {
						waterRendering->perlinStartFreq = value;
					} break;
					case hashString("perlinLacunarity"): {
						waterRendering->perlinLacunarity = value;
					} break;
					case hashString("perlinAmplitude"): {
						waterRendering->perlinAmplitude = value;
					} break;

					case hashString("windSpeed"): {
						waterRendering->windSpeed = value;
					} break;

					case hashString("waveOffsetFactor"): {
						waterRendering->waveOffsetFactor = value;
					} break;

					case hashString("waveLength"): {
						waterRendering->waveLength = value;
					} break;

					case hashString("waveFoamDistortion"): {
						waterRendering->waveFoamDistortion = value;
					} break;

					case hashString("waveFoamIntensity"): {
						waterRendering->waveFoamIntensity = value;
					} break;

					case hashString("causticsResolution"): {
						waterRendering->causticsResolution = value;
					} break;

					case hashString("causticsStrength"): {
						waterRendering->causticsStrength = value;
					} break;

					case hashString("numTiles"): {
						waterRendering->numTiles = (unsigned char)value;
					} break;
					default: {
						luaL_error(L, "[%s] unknown scalar key \"%s\"", __func__, key);
					} break;
				}

				continue;
			} break;

			case LUA_TBOOLEAN: {
				const bool value = lua_toboolean(L, -1);

				switch (hashString(key)) {
					case hashString("shoreWaves"): {
						waterRendering->shoreWaves = value;
					} break;
					case hashString("forceRendering"): {
						waterRendering->forceRendering = value;
					} break;
					case hashString("hasWaterPlane"): {
						waterRendering->hasWaterPlane = value;
					} break;
					default: {
						luaL_error(L, "[%s] unknown boolean key \"%s\"", __func__, key);
					} break;
				}

				continue;
			} break;

			default: {
			} break;
		}
	}
	auto waterID = static_cast<int>(IWater::GetWater()->GetID());
	IWater::KillWater();
	IWater::SetWater(waterID);
	waterRendering->SetUpdated();

	return 0;
}


/******************************************************************************
 * Preload
 * @section preload
******************************************************************************/


/*** @function Spring.PreloadUnitDefModel
 *
 * Allow the engine to load the unit's model (and texture) in a background thread.
 * Wreckages and buildOptions of a unit are automatically preloaded.
 *
 * @param unitDefID integer
 * @return nil
 */
int LuaUnsyncedCtrl::PreloadUnitDefModel(lua_State* L) {
	const UnitDef* ud = unitDefHandler->GetUnitDefByID(luaL_checkint(L, 1));

	if (ud == nullptr)
		return 0;

	ud->PreloadModel();
	return 0;
}


/*** @function Spring.PreloadFeatureDefModel
 *
 * @param featureDefID integer
 * @return nil
 */
int LuaUnsyncedCtrl::PreloadFeatureDefModel(lua_State* L) {
	const FeatureDef* fd = featureDefHandler->GetFeatureDefByID(luaL_checkint(L, 1));

	if (fd == nullptr)
		return 0;

	fd->PreloadModel();
	return 0;
}


/*** @function Spring.PreloadSoundItem
 *
 * @param name string
 * @return nil
 */
int LuaUnsyncedCtrl::PreloadSoundItem(lua_State* L)
{
	// always push false in synced context
	const bool retval = sound->PreloadSoundItem(luaL_checkstring(L, 1));
	const bool synced = CLuaHandle::GetHandleSynced(L);

	lua_pushboolean(L, retval && !synced);
	return 1;
}


/*** @function Spring.LoadModelTextures
 *
 * @param modelName string
 * @return boolean? success
 */
int LuaUnsyncedCtrl::LoadModelTextures(lua_State* L)
{
	const std::string modelName = luaL_optstring(L, 1, "");
	if (modelName.empty()) {
		lua_pushnil(L);
		return 1;
	}

	for (S3DModel& model : modelLoader.GetModelsVec()) {
		if (model.name == modelName) {
			if (model.type == MODELTYPE_3DO) {
				lua_pushboolean(L, false);
				return 1;
			}
			textureHandlerS3O.LoadTexture(&model);
			lua_pushboolean(L, true);
			return 1;
		}
	}

	lua_pushboolean(L, false);
	return 1;
}


/******************************************************************************
 * Decals
 * @section decals
******************************************************************************/


/***
 *
 * @function Spring.CreateGroundDecal
 * @return nil|number decalID
 */
int LuaUnsyncedCtrl::CreateGroundDecal(lua_State* L)
{
	const uint32_t id = groundDecals->CreateLuaDecal();
	if (id > 0) {
		lua_pushnumber(L, id);
		return 1;
	}
	return 0;
}


/***
 *
 * @function Spring.DestroyGroundDecal
 * @param decalID integer
 * @return boolean delSuccess
 */
int LuaUnsyncedCtrl::DestroyGroundDecal(lua_State* L)
{
	lua_pushboolean(L, groundDecals->DeleteLuaDecal(luaL_checkint(L, 1)));
	return 1;
}


/***
 *
 * @function Spring.SetGroundDecalPosAndDims
 * @param decalID integer
 * @param midPosX number? (Default: currMidPosX)
 * @param midPosZ number? (Default: currMidPosZ)
 * @param sizeX number? (Default: currSizeX)
 * @param sizeZ number? (Default: currSizeZ)
 * @param projCubeHeight number? (Default: calculateProjCubeHeight)
 * @return boolean decalSet
 */
int LuaUnsyncedCtrl::SetGroundDecalPosAndDims(lua_State* L)
{
	auto* decal = groundDecals->GetDecalById(luaL_checkint(L, 1));
	if (!decal) {
		lua_pushboolean(L, false);
		return 1;
	}

	const float2 midPointCurr = (decal->posTL + decal->posTR + decal->posBR + decal->posBL) * 0.25f;

	const float2 midPoint {
		luaL_optfloat(L, 2, midPointCurr.x),
		luaL_optfloat(L, 3, midPointCurr.y)
	};

	const float sizex = luaL_optfloat(L, 4, (decal->posTL.Distance(decal->posTR) + decal->posBL.Distance(decal->posBR)) * 0.25f);
	const float sizez = luaL_optfloat(L, 5, (decal->posTL.Distance(decal->posBL) + decal->posTR.Distance(decal->posBR)) * 0.25f);

	const auto posTL = midPoint + float2(-sizex, -sizez);
	const auto posTR = midPoint + float2( sizex, -sizez);
	const auto posBR = midPoint + float2( sizex,  sizez);
	const auto posBL = midPoint + float2(-sizex,  sizez);

	decal->posTL = posTL;
	decal->posTR = posTR;
	decal->posBR = posBR;
	decal->posBL = posBL;
	decal->height = luaL_optfloat(L, 6, math::sqrt(sizex * sizex + sizez * sizez));

	lua_pushboolean(L, true);
	return 1;
}

/***
 * @class xz
 * @x_helper
 * @field x number
 * @field y number
 */
/***
 *
 * @function Spring.SetGroundDecalQuadPosAndHeight
 *
 * Use for non-rectangular decals
 *
 * @param decalID integer
 * @param posTL xz? (Default: currPosTL)
 * @param posTR xz? (Default: currPosTR)
 * @param posBR xz? (Default: currPosBR)
 * @param posBL xz? (Default: currPosBL)
 * @param projCubeHeight number? (Default: calculateProjCubeHeight)
 * @return boolean decalSet
 */
int LuaUnsyncedCtrl::SetGroundDecalQuadPosAndHeight(lua_State* L)
{
	auto* decal = groundDecals->GetDecalById(luaL_checkint(L, 1));
	if (!decal) {
		lua_pushboolean(L, false);
		return 1;
	}

	decal->posTL = float2{ luaL_optfloat(L, 2, decal->posTL.x), luaL_optfloat(L, 3, decal->posTL.y) };
	decal->posTR = float2{ luaL_optfloat(L, 4, decal->posTR.x), luaL_optfloat(L, 5, decal->posTR.y) };
	decal->posBR = float2{ luaL_optfloat(L, 6, decal->posBR.x), luaL_optfloat(L, 7, decal->posBR.y) };
	decal->posBL = float2{ luaL_optfloat(L, 8, decal->posBL.x), luaL_optfloat(L, 9, decal->posBL.y) };

	const float sizex = (decal->posTL.Distance(decal->posTR) + decal->posBL.Distance(decal->posBR)) * 0.25f;
	const float sizez = (decal->posTL.Distance(decal->posBL) + decal->posTR.Distance(decal->posBR)) * 0.25f;

	decal->height = luaL_optfloat(L, 10, math::sqrt(sizex * sizex + sizez * sizez));

	lua_pushboolean(L, true);
	return 1;
}

/***
 *
 * @function Spring.SetGroundDecalRotation
 * @param decalID integer
 * @param rot number? (Default: random) in radians
 * @return boolean decalSet
 */
int LuaUnsyncedCtrl::SetGroundDecalRotation(lua_State* L)
{
	auto* decal = groundDecals->GetDecalById(luaL_checkint(L, 1));
	if (!decal) {
		lua_pushboolean(L, false);
		return 1;
	}

	decal->rot = luaL_optfloat(L, 2, guRNG.NextFloat() * math::TWOPI);

	lua_pushboolean(L, true);
	return 1;
}


/***
 *
 * @function Spring.SetGroundDecalTexture
 * @param decalID integer
 * @param textureName string The texture has to be on the atlas which seems to mean it's defined as an explosion, unit tracks, or building plate decal on some unit already (no arbitrary textures)
 * @param isMainTex boolean? (Default: `true`) If false, it sets the normals/glow map
 * @return nil|boolean decalSet
 */
int LuaUnsyncedCtrl::SetGroundDecalTexture(lua_State* L)
{
	lua_pushboolean(L,
		groundDecals->SetDecalTexture(luaL_checkint(L, 1), luaL_checksstring(L, 2), luaL_optboolean(L, 3, false))
	);
	return 1;
}

/***
 *
 * @function Spring.SetGroundDecalTextureParams
 * @param decalID integer
 * @param texWrapDistance number? (Default: currTexWrapDistance) if non-zero sets the mode to repeat the texture along the left-right direction of the decal every texWrapFactor elmos
 * @param texTraveledDistance number? (Default: currTexTraveledDistance) shifts the texture repetition defined by texWrapFactor so the texture of a next line in the continuous multiline can start where the previous finished. For that it should collect all elmo lengths of the previously set multiline segments.
 * @return nil|boolean decalSet
 */
int LuaUnsyncedCtrl::SetGroundDecalTextureParams(lua_State* L)
{
	auto* decal = groundDecals->GetDecalById(luaL_checkint(L, 1));
	if (!decal) {
		lua_pushboolean(L, false);
		return 1;
	}

	decal->uvWrapDistance     = luaL_optfloat(L, 2, decal->uvWrapDistance);
	decal->uvTraveledDistance = luaL_optfloat(L, 3, decal->uvTraveledDistance);

	lua_pushboolean(L, true);
	return 1;
}


/***
 *
 * @function Spring.SetGroundDecalAlpha
 * @param decalID integer
 * @param alpha number? (Default: currAlpha) Between 0 and 1
 * @param alphaFalloff number? (Default: currAlphaFalloff) Between 0 and 1, per second
 * @return boolean decalSet
 */
int LuaUnsyncedCtrl::SetGroundDecalAlpha(lua_State* L)
{
	auto* decal = groundDecals->GetDecalById(luaL_checkint(L, 1));
	if (!decal) {
		lua_pushboolean(L, false);
		return 1;
	}

	decal->alpha = luaL_optfloat(L, 2, decal->alpha);
	decal->alphaFalloff = luaL_optfloat(L, 3, decal->alphaFalloff * GAME_SPEED) / GAME_SPEED;

	lua_pushboolean(L, true);
	return 1;
}

/***
 *
 * @function Spring.SetGroundDecalNormal
 * Sets projection cube normal to orient in 3D space.
 * In case the normal (0,0,0) then normal is picked from the terrain
 * @param decalID integer
 * @param normalX number? (Default: `0`)
 * @param normalY number? (Default: `0`)
 * @param normalZ number? (Default: `0`)
 * @return boolean decalSet
 */
int LuaUnsyncedCtrl::SetGroundDecalNormal(lua_State* L)
{
	auto* decal = groundDecals->GetDecalById(luaL_checkint(L, 1));
	if (!decal) {
		lua_pushboolean(L, false);
		return 1;
	}

	float3 forcedNormal{
		luaL_optfloat(L, 2, 0.0f),
		luaL_optfloat(L, 3, 0.0f),
		luaL_optfloat(L, 4, 0.0f)
	};
	forcedNormal.SafeNormalize();

	decal->forcedNormal = forcedNormal;

	lua_pushboolean(L, true);
	return 1;
}

/***
 *
 * @function Spring.SetGroundDecalTint
 * Sets the tint of the ground decal. Color = 2 * textureColor * tintColor
 * Respectively a color of (0.5, 0.5, 0.5, 0.5) is effectively no tint
 * @param decalID integer
 * @param tintColR number? (Default: curTintColR)
 * @param tintColG number? (Default: curTintColG)
 * @param tintColB number? (Default: curTintColB)
 * @param tintColA number? (Default: curTintColA)
 * @return boolean decalSet
 */
int LuaUnsyncedCtrl::SetGroundDecalTint(lua_State* L)
{
	auto* decal = groundDecals->GetDecalById(luaL_checkint(L, 1));
	if (!decal) {
		lua_pushboolean(L, false);
		return 1;
	}

	float4 tintColor = decal->tintColor;
	tintColor.r = luaL_optfloat(L, 2, tintColor.r);
	tintColor.g = luaL_optfloat(L, 3, tintColor.g);
	tintColor.b = luaL_optfloat(L, 4, tintColor.b);
	tintColor.a = luaL_optfloat(L, 5, tintColor.a);

	decal->tintColor = SColor{ tintColor };

	lua_pushboolean(L, true);
	return 1;
}

/***
 *
 * @function Spring.SetGroundDecalMisc
 * Sets varios secondary parameters of a decal
 * @param decalID integer
 * @param dotElimExp number? (Default: curValue) pow(max(dot(decalProjVector, SurfaceNormal), 0.0), dotElimExp), used to reduce decal artifacts on surfaces non-collinear with the projection vector
 * @param refHeight number? (Default: curValue)
 * @param minHeight number? (Default: curValue)
 * @param maxHeight number? (Default: curValue)
 * @param forceHeightMode number? (Default: curValue) in case forceHeightMode==1.0 ==> force relative height: midPoint.y = refHeight + clamp(midPoint.y - refHeight, minHeight); forceHeightMode==2.0 ==> force absolute height: midPoint.y = midPoint.y, clamp(midPoint.y, minHeight, maxHeight); other forceHeightMode values do not enforce the height of the center position
 * @return boolean decalSet
 */
int LuaUnsyncedCtrl::SetGroundDecalMisc(lua_State* L)
{
	auto* decal = groundDecals->GetDecalById(luaL_checkint(L, 1));
	if (!decal) {
		lua_pushboolean(L, false);
		return 1;
	}

	decal->dotElimExp = luaL_optfloat(L, 2, decal->dotElimExp);
	decal->refHeight = luaL_optfloat(L, 3, decal->refHeight);
	decal->minHeight = luaL_optfloat(L, 4, decal->minHeight);
	decal->maxHeight = luaL_optfloat(L, 5, decal->maxHeight);
	decal->forceHeightMode = luaL_optfloat(L, 6, decal->forceHeightMode);

	lua_pushboolean(L, true);
	return 1;
}

/***
 *
 * @function Spring.SetGroundDecalCreationFrame
 *
 * Use separate min and max for "gradient" style decals such as tank tracks
 *
 * @param decalID integer
 * @param creationFrameMin number? (Default: currCreationFrameMin)
 * @param creationFrameMax number? (Default: currCreationFrameMax)
 * @return boolean decalSet
 */
int LuaUnsyncedCtrl::SetGroundDecalCreationFrame(lua_State* L)
{
	auto* decal = groundDecals->GetDecalById(luaL_checkint(L, 1));
	if (!decal) {
		lua_pushboolean(L, false);
		return 1;
	}

	decal->createFrameMin = luaL_optfloat(L, 2, decal->createFrameMin);
	decal->createFrameMax = luaL_optfloat(L, 3, decal->createFrameMax);

	lua_pushboolean(L, true);
	return 1;
}


/******************************************************************************
 * SDL Text
 * @section sdltext
******************************************************************************/


/***
 *
 * @function Spring.SDLSetTextInputRect
 * @param x number
 * @param y number
 * @param width number
 * @param height number
 * @return nil
 */
int LuaUnsyncedCtrl::SDLSetTextInputRect(lua_State* L)
{
	SDL_Rect textWindow;
	textWindow.x = luaL_checkint(L, 1);
	textWindow.y = luaL_checkint(L, 2);
	textWindow.w = luaL_checkint(L, 3);
	textWindow.h = luaL_checkint(L, 4);
	SDL_SetTextInputRect(&textWindow);
	return 0;
}

/***
 *
 * @function Spring.SDLStartTextInput
 * @return nil
 */
int LuaUnsyncedCtrl::SDLStartTextInput(lua_State* L)
{
	SDL_StartTextInput();
	return 0;
}

/***
 *
 * @function Spring.SDLStopTextInput
 * @return nil
 */
int LuaUnsyncedCtrl::SDLStopTextInput(lua_State* L)
{
	SDL_StopTextInput();
	return 0;
}


/******************************************************************************
 * Window Management
 * @section window
******************************************************************************/

/***
 *
 * @function Spring.SetWindowGeometry
 * @param displayIndex number
 * @param winRelPosX number
 * @param winRelPosY number
 * @param winSizeX number
 * @param winSizeY number
 * @param fullScreen boolean
 * @param borderless boolean
 * @return nil
 */
int LuaUnsyncedCtrl::SetWindowGeometry(lua_State* L)
{
	const int displayIndex = luaL_checkint(L, 1) - 1;
	const int winRelPosX = luaL_checkint(L, 2);
	const int winRelPosY = luaL_checkint(L, 3);
	const int winSizeX = luaL_checkint(L, 4);
	const int winSizeY = luaL_checkint(L, 5);
	const bool fullScreen = luaL_checkboolean(L, 6);
	const bool borderless = luaL_checkboolean(L, 7);

	const bool r = globalRendering->SetWindowPosHelper(displayIndex, winRelPosX, winRelPosY, winSizeX, winSizeY, fullScreen, borderless);

	if (!r)
		luaL_error(L, "[%s] Invalid function parameters\n", __func__);

	return 0;
}

/***
 *
 * @function Spring.SetWindowMinimized
 * @return boolean minimized
 */
int LuaUnsyncedCtrl::SetWindowMinimized(lua_State* L)
{
	lua_pushboolean(L, globalRendering->SetWindowMinimized());
	return 1;
}

/***
 *
 * @function Spring.SetWindowMaximized
 * @return boolean maximized
 */
int LuaUnsyncedCtrl::SetWindowMaximized(lua_State* L)
{
	lua_pushboolean(L, globalRendering->SetWindowMaximized());
	return 1;
}


/******************************************************************************
 * Misc
 * @section misc
******************************************************************************/


/*** @function Spring.Reload
 * @param startScript string the CONTENT of the script.txt spring should use to start.
 * @return nil
 */
int LuaUnsyncedCtrl::Reload(lua_State* L)
{
	return (ReloadOrRestart("", luaL_checkstring(L, 1), false));
}


/*** @function Spring.Restart
 *
 * If this call returns, something went wrong
 *
 * @param commandline_args string commandline arguments passed to spring executable.
 * @param startScript string
 * @return nil
 */
int LuaUnsyncedCtrl::Restart(lua_State* L)
{
	// same as Reload now, cl-args are always ignored
	return (ReloadOrRestart(luaL_checkstring(L, 1), luaL_checkstring(L, 2), false));
}


/*** Launches a new Spring instance without terminating the existing one.
 *
 * @function Spring.Start
 *
 * If this call returns, something went wrong
 *
 * @param commandline_args string commandline arguments passed to spring executable.
 * @param startScript string the CONTENT of the script.txt spring should use to start (if empty, no start-script is added, you can still point spring to your custom script.txt when you add the file-path to commandline_args.
 * @return nil
 */
int LuaUnsyncedCtrl::Start(lua_State* L)
{
	if (ReloadOrRestart(luaL_checkstring(L, 1), luaL_checkstring(L, 2), true) != 0) {
		lua_pushboolean(L, false);
		return 1;
	}

	return 0;
}


/*** Sets the icon for the process which is seen in the OS task-bar and other places (default: spring-logo).
 *
 * @function Spring.SetWMIcon
 *
 * Note: has to be 24bit or 32bit.
 * Note: on windows, it has to be 32x32 pixels in size (recommended for cross-platform)
 * Note: *.bmp images have to be in BGR format (default for m$ ones).
 * Note: *.ico images are not supported.
 *
 * @param iconFileName string
 * @return nil
 */
int LuaUnsyncedCtrl::SetWMIcon(lua_State* L)
{
	const std::string iconFileName = luaL_checksstring(L, 1);
	const bool forceResolution = luaL_optboolean(L, 2, false);

	CBitmap iconTexture;

	if (iconTexture.Load(iconFileName)) {
		WindowManagerHelper::SetIcon(&iconTexture, forceResolution);
	} else {
		luaL_error(L, "Failed to load image from file \"%s\"", iconFileName.c_str());
	}

	return 0;
}


/*** Set the window title for the process
 *
 * @function Spring.SetWMCaption
 *
 * @param title string (Default: `"Spring <version>"`)
 * @param titleShort string? (Default: `"Spring <version>"`) displayed in the OS task-bar .
 *
 * > [!NOTE]
 * > shortTitle is only ever possibly used under X11 (Linux & OS X), but not
 * > with QT (KDE) and never under Windows.
 *
 * @return nil
 */
int LuaUnsyncedCtrl::SetWMCaption(lua_State* L)
{
	globalRendering->SetWindowTitle(luaL_checksstring(L, 1));
	return 0;
}


/*** @function Spring.ClearWatchDogTimer
 * @param threadName string? (Default: main)
 * @return nil
 */
int LuaUnsyncedCtrl::ClearWatchDogTimer(lua_State* L) {
	if (lua_gettop(L) == 0) {
		// clear for current thread
		Watchdog::ClearTimer();
		return 0;
	}

	if (lua_isstring(L, 1)) {
		Watchdog::ClearTimer(lua_tostring(L, 1), luaL_optboolean(L, 2, false));
	} else {
		Watchdog::ClearTimer("main", luaL_optboolean(L, 2, false));
	}

	return 0;
}


/*** @function Spring.SetClipboard
 * @param text string
 * @return nil
 */
int LuaUnsyncedCtrl::SetClipboard(lua_State* L)
{
	SDL_SetClipboardText(luaL_checkstring(L, 1));
	return 0;
}


/*** Relinquish control of the game loading thread and OpenGL context back to the UI (LuaIntro).
 *
 * @function Spring.Yield
 *
 * Should be called after each widget/unsynced gadget is loaded in widget/gadget handler. Use it to draw screen updates and process windows events.
 *
 * @usage
 * local wantYield = Spring.Yield and Spring.Yield() -- nil check: not present in synced
 * for wupget in pairs(wupgetsToLoad) do
 *   loadWupget(wupget)
 *   wantYield = wantYield and Spring.Yield()
 * end
 *
 * @return boolean when true caller should continue calling `Spring.Yield` during the widgets/gadgets load, when false it shouldn't call it any longer.
 */
int LuaUnsyncedCtrl::Yield(lua_State* L)
{
	if (CLoadLock::GetThreadSafety() == false) {
		lua_pushboolean(L, false); //hint Lua might stop calling Yield
		return 1;
	}

	auto& mtx = CLoadLock::GetMutex();
	mtx.unlock();
	std::this_thread::yield();
	mtx.lock();
	Watchdog::ClearTimer(WDT_LOAD);

	lua_pushboolean(L, true); //hint Lua should keep calling Yield
	return 1;
}
