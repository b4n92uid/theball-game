/*
 * File:   ScriptFunctions.h
 * Author: b4n92uid
 *
 * Created on 30 mai 2011, 23:25
 */

#ifndef SCRIPTFUNCTIONS_H
#define	SCRIPTFUNCTIONS_H

#include "ScriptManager.h"

#define GAMEMANAGER_INTERNALE_NAME "_gameManager"
#define SCRIPTMANAGER_INTERNALE_NAME "_scriptManager"
#define SCRIPTPATH_INTERNALE_NAME "_scriptPath"

class StaticElement;

inline void lua_pushelement(lua_State* lua, MapElement* e)
{
    lua_pushinteger(lua, (lua_Integer)e);
}

inline void lua_pushplayer(lua_State* lua, Player* p)
{
    lua_pushinteger(lua, (lua_Integer)p);
}

inline Player* lua_toplayer(lua_State* lua, int argpos)
{
    return reinterpret_cast<Player*>((long)lua_tonumber(lua, argpos));
}

inline MapElement* lua_toelem(lua_State* lua, int argpos)
{
    return reinterpret_cast<MapElement*>((long)lua_tonumber(lua, argpos));
}

inline StaticElement* lua_tostatic(lua_State* lua, int argpos)
{
    return reinterpret_cast<StaticElement*>((long)lua_tonumber(lua, argpos));
}

inline void lua_pushvector3(lua_State* lua, tbe::Vector3f vec)
{
    lua_newtable(lua);

    lua_pushstring(lua, "x");
    lua_pushnumber(lua, vec.x);
    lua_settable(lua, -3);

    lua_pushstring(lua, "y");
    lua_pushnumber(lua, vec.y);
    lua_settable(lua, -3);

    lua_pushstring(lua, "z");
    lua_pushnumber(lua, vec.z);
    lua_settable(lua, -3);
}

template<typename T>
void lua_pushtable(lua_State* lua, const std::vector<T>& vec)
{
    if(vec.empty())
        return;

    lua_newtable(lua);

    for(unsigned i = 0; i < vec.size(); i++)
    {
        lua_pushnumber(lua, i);
        lua_pushnumber(lua, (lua_Integer)vec[i]);
        lua_settable(lua, -3);
    }
}

inline tbe::Vector3f lua_tovector3(lua_State* lua, int argpos)
{
    tbe::Vector3f vec;

    lua_pushstring(lua, "x");
    lua_gettable(lua, argpos);
    vec.x = lua_tonumber(lua, -1);
    lua_pop(lua, 1);

    lua_pushstring(lua, "y");
    lua_gettable(lua, argpos);
    vec.y = lua_tonumber(lua, -1);
    lua_pop(lua, 1);

    lua_pushstring(lua, "z");
    lua_gettable(lua, argpos);
    vec.z = lua_tonumber(lua, -1);
    lua_pop(lua, 1);

    return vec;
}

inline void lua_pushstring(lua_State *L, std::string s)
{
    lua_pushstring(L, s.c_str());
}

namespace script
{

int safeLuaCallCommon(lua_State* L, lua_CFunction func);

// General

int include(lua_State* lua);

// Translation

int setFloorPosition(lua_State* lua);
int isStayInFloor(lua_State* lua);

int setPosition(lua_State* lua);
int getPosition(lua_State* lua);

int setVelocity(lua_State* lua);
int getVelocity(lua_State* lua);

int setForce(lua_State* lua);
int upForce(lua_State* lua);
int getForce(lua_State* lua);

int impulse(lua_State* lua);

int stopMotion(lua_State* lua);

// Material

int loadMaterial(lua_State* lua);
int attachMaterial(lua_State* lua);

int freeze(lua_State* lua);
int unfreeze(lua_State* lua);

int setGhost(lua_State* lua);
int setImmunity(lua_State* lua);

int setTextureFrame(lua_State* lua);

int childOf(lua_State* lua);

// Player Stat

int getNickName(lua_State* lua);

int setHealth(lua_State* lua);
int upHealth(lua_State* lua);
int getHealth(lua_State* lua);

int setEnergy(lua_State* lua);
int upEnergy(lua_State* lua);
int getEnergy(lua_State* lua);

int setAmmo(lua_State* lua);
int upAmmo(lua_State* lua);
int getAmmo(lua_State* lua);

int attachPower(lua_State* lua);
int selectPower(lua_State* lua);
int switchPower(lua_State* lua);
int getSelectedPower(lua_State* lua);

int attachWeapon(lua_State* lua);
int selectWeapon(lua_State* lua);
int getSelectedWeapon(lua_State* lua);
int switchWeapon(lua_State* lua);

int move(lua_State* lua);
int jump(lua_State* lua);
int shoot(lua_State* lua);
int power(lua_State* lua);
int dammage(lua_State* lua);

int createPlayer(lua_State* lua);
int kickPlayer(lua_State* lua);
int kickAllPlayers(lua_State* lua);
int killPlayer(lua_State* lua);
int killAllPlayers(lua_State* lua);
int isKilledPlayer(lua_State* lua);

// Audio

int loadSound(lua_State* lua);
int playSound(lua_State* lua);
int stopSound(lua_State* lua);
int isPlaySound(lua_State* lua);
int volSound(lua_State* lua);

int loadMusic(lua_State* lua);
int playMusic(lua_State* lua);
int pauseMusic(lua_State* lua);
int stopMusic(lua_State* lua);

// Compute

int rayCast(lua_State* lua);

int randomPosition(lua_State* lua);

int nearestPlayer(lua_State* lua);
int farestPlayer(lua_State* lua);

int isViewed(lua_State* lua);

int diriction(lua_State* lua);

int length(lua_State* lua);

int normalize(lua_State* lua);

// User data

int setString(lua_State* lua);
int getString(lua_State* lua);

int setNumber(lua_State* lua);
int getNumber(lua_State* lua);

int setEntity(lua_State* lua);
int getEntity(lua_State* lua);

int setVector(lua_State* lua);
int getVector(lua_State* lua);

int getSceneString(lua_State* lua);

// HUD

int display(lua_State* lua);
int status(lua_State* lua);

// Management

int playerList(lua_State* lua);
int userPlayer(lua_State* lua);

int getElement(lua_State* lua);
int getElementsList(lua_State* lua);
int getElementsRand(lua_State* lua);

int registerElementCollid(lua_State* lua);
int registerAreaCollid(lua_State* lua);

int registerGlobalHook(lua_State* lua);
int registerPlayerHook(lua_State* lua);

int gameover(lua_State* lua);

int setTimeout(lua_State* lua);
int setInterval(lua_State* lua);

// PPE

int glowEnable(lua_State* lua);
int glowSettings(lua_State* lua);

// Trigger

int whiteFlash(lua_State* lua);
int earthQuake(lua_State* lua);

}

template<lua_CFunction func> int safeLuaCall(lua_State* L)
{
    return script::safeLuaCallCommon(L, func);
}

#endif	/* SCRIPTFUNCTIONS_H */

