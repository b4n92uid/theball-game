/*
 * File:   ScriptManager.cpp
 * Author: b4n92uid
 *
 * Created on 28 mai 2011, 20:58
 */

#include "ScriptManager.h"
#include "GameManager.h"
#include "SoundManager.h"

#include "MapElement.h"
#include "Player.h"

#include "ScriptFunctions.h"

using namespace std;
using namespace tbe;

ScriptManager::ScriptManager(GameManager* gameManager)
{
    m_gameManager = gameManager;

    m_lua = lua_open();

    luaL_openlibs(m_lua);

    lua_register(m_lua, "include", safeLuaCall<script::include>);

    lua_register(m_lua, "setFloorPosition", safeLuaCall<script::setFloorPosition>);
    lua_register(m_lua, "isStayInFloor", safeLuaCall<script::isStayInFloor>);

    lua_register(m_lua, "setPosition", safeLuaCall<script::setPosition>);
    lua_register(m_lua, "getPosition", safeLuaCall<script::getPosition>);

    lua_register(m_lua, "setVelocity", safeLuaCall<script::setVelocity>);
    lua_register(m_lua, "getVelocity", safeLuaCall<script::getVelocity>);

    lua_register(m_lua, "setForce", safeLuaCall<script::setForce>);
    lua_register(m_lua, "upForce", safeLuaCall<script::upForce>);
    lua_register(m_lua, "getForce", safeLuaCall<script::getForce>);

    lua_register(m_lua, "unfreeze", safeLuaCall<script::unfreeze>);
    lua_register(m_lua, "freeze", safeLuaCall<script::freeze>);
    lua_register(m_lua, "impulse", safeLuaCall<script::impulse>);
    lua_register(m_lua, "stopMotion", safeLuaCall<script::stopMotion>);

    lua_register(m_lua, "setImmunity", safeLuaCall<script::setImmunity>);
    lua_register(m_lua, "setGhost", safeLuaCall<script::setGhost>);

    lua_register(m_lua, "setTextureFrame", safeLuaCall<script::setTextureFrame>);
    lua_register(m_lua, "childOf", safeLuaCall<script::childOf>);

    lua_register(m_lua, "getNickName", safeLuaCall<script::getNickName>);

    lua_register(m_lua, "setHealth", safeLuaCall<script::setHealth>);
    lua_register(m_lua, "upHealth", safeLuaCall<script::upHealth>);
    lua_register(m_lua, "getHealth", safeLuaCall<script::getHealth>);

    lua_register(m_lua, "setEnergy", safeLuaCall<script::setEnergy>);
    lua_register(m_lua, "upEnergy", safeLuaCall<script::upEnergy>);
    lua_register(m_lua, "getEnergy", safeLuaCall<script::getEnergy>);

    lua_register(m_lua, "attachPower", safeLuaCall<script::attachPower>);
    lua_register(m_lua, "selectPower", safeLuaCall<script::selectPower>);
    lua_register(m_lua, "switchPower", safeLuaCall<script::switchPower>);
    lua_register(m_lua, "getSelectedPower", safeLuaCall<script::getSelectedPower>);

    lua_register(m_lua, "attachWeapon", safeLuaCall<script::attachWeapon>);
    lua_register(m_lua, "selectWeapon", safeLuaCall<script::selectWeapon>);
    lua_register(m_lua, "getSelectedWeapon", safeLuaCall<script::getSelectedWeapon>);
    lua_register(m_lua, "switchWeapon", safeLuaCall<script::switchWeapon>);

    lua_register(m_lua, "setAmmo", safeLuaCall<script::setAmmo>);
    lua_register(m_lua, "upAmmo", safeLuaCall<script::upAmmo>);
    lua_register(m_lua, "getAmmo", safeLuaCall<script::getAmmo>);

    lua_register(m_lua, "loadSound", safeLuaCall<script::loadSound>);
    lua_register(m_lua, "playSound", safeLuaCall<script::playSound>);
    lua_register(m_lua, "stopSound", safeLuaCall<script::stopSound>);
    lua_register(m_lua, "volSound", safeLuaCall<script::volSound>);
    lua_register(m_lua, "isPlaySound", safeLuaCall<script::isPlaySound>);

    lua_register(m_lua, "loadMusic", safeLuaCall<script::loadMusic>);
    lua_register(m_lua, "playMusic", safeLuaCall<script::playMusic>);
    lua_register(m_lua, "pauseMusic", safeLuaCall<script::pauseMusic>);
    lua_register(m_lua, "stopMusic", safeLuaCall<script::stopMusic>);

    lua_register(m_lua, "rayCast", safeLuaCall<script::rayCast>);

    lua_register(m_lua, "randomPosition", safeLuaCall<script::randomPosition>);

    lua_register(m_lua, "nearestPlayer", safeLuaCall<script::nearestPlayer>);
    lua_register(m_lua, "farestPlayer", safeLuaCall<script::farestPlayer>);

    lua_register(m_lua, "isViewed", safeLuaCall<script::isViewed>);

    lua_register(m_lua, "normalize", safeLuaCall<script::normalize>);

    lua_register(m_lua, "diriction", safeLuaCall<script::diriction>);

    lua_register(m_lua, "length", safeLuaCall<script::length>);

    lua_register(m_lua, "setString", safeLuaCall<script::setString>);
    lua_register(m_lua, "getString", safeLuaCall<script::getString>);

    lua_register(m_lua, "setNumber", safeLuaCall<script::setNumber>);
    lua_register(m_lua, "getNumber", safeLuaCall<script::getNumber>);

    lua_register(m_lua, "setEntity", safeLuaCall<script::setEntity>);
    lua_register(m_lua, "getEntity", safeLuaCall<script::getEntity>);

    lua_register(m_lua, "setVector", safeLuaCall<script::setVector>);
    lua_register(m_lua, "getVector", safeLuaCall<script::getVector>);

    lua_register(m_lua, "getSceneString", safeLuaCall<script::getSceneString>);

    lua_register(m_lua, "move", safeLuaCall<script::move>);
    lua_register(m_lua, "jump", safeLuaCall<script::jump>);
    lua_register(m_lua, "shoot", safeLuaCall<script::shoot>);
    lua_register(m_lua, "power", safeLuaCall<script::power>);
    lua_register(m_lua, "dammage", safeLuaCall<script::dammage>);

    lua_register(m_lua, "createPlayer", safeLuaCall<script::createPlayer>);
    lua_register(m_lua, "killPlayer", safeLuaCall<script::killPlayer>);
    lua_register(m_lua, "killAllPlayers", safeLuaCall<script::killAllPlayers>);
    lua_register(m_lua, "kickPlayer", safeLuaCall<script::kickPlayer>);
    lua_register(m_lua, "kickAllPlayers", safeLuaCall<script::kickAllPlayers>);
    lua_register(m_lua, "isKilledPlayer", safeLuaCall<script::isKilledPlayer>);

    lua_register(m_lua, "display", safeLuaCall<script::display>);
    lua_register(m_lua, "status", safeLuaCall<script::status>);

    lua_register(m_lua, "playerList", safeLuaCall<script::playerList>);
    lua_register(m_lua, "userPlayer", safeLuaCall<script::userPlayer>);

    lua_register(m_lua, "getElement", safeLuaCall<script::getElement>);
    lua_register(m_lua, "getElementsList", safeLuaCall<script::getElementsList>);
    lua_register(m_lua, "getElementsRand", safeLuaCall<script::getElementsRand>);

    lua_register(m_lua, "registerElementCollid", script::registerElementCollid);
    lua_register(m_lua, "registerAreaCollid", script::registerAreaCollid);

    lua_register(m_lua, "registerGlobalHook", script::registerGlobalHook);
    lua_register(m_lua, "registerPlayerHook", script::registerPlayerHook);

    lua_register(m_lua, "gameover", script::gameover);

    lua_register(m_lua, "setInterval", script::setInterval);
    lua_register(m_lua, "setTimeout", script::setTimeout);

    lua_register(m_lua, "glowEnable", script::glowEnable);
    lua_register(m_lua, "glowSettings", script::glowSettings);

    lua_register(m_lua, "whiteFlash", script::whiteFlash);
    lua_register(m_lua, "earthQuake", script::earthQuake);
}

ScriptManager::~ScriptManager()
{
    lua_close(m_lua);
}

void ScriptManager::processCollid(Player* player, MapElement* elem, float force, float normaleSpeed)
{
    if(m_collidRec.count(elem->getId()))
    {
        callCollidCallback(m_collidRec[elem->getId()], player, elem, force, normaleSpeed);
    }
}

void ScriptManager::callCollidCallback(std::string funcname, Player* player, MapElement* elem, float force, float normaleSpeed)
{
    lua_getglobal(m_lua, funcname.c_str());

    if(!lua_isfunction(m_lua, -1))
    {
        lua_pop(m_lua, 1);
        throw tbe::Exception("ScriptManager::call; undefined function (%s)", funcname.c_str());
    }

    lua_pushinteger(m_lua, (lua_Integer)player);
    lua_pushinteger(m_lua, (lua_Integer)elem);
    lua_pushnumber(m_lua, force);
    lua_pushnumber(m_lua, normaleSpeed);
    lua_call(m_lua, 4, 0);
}

void ScriptManager::call(std::string funcname)
{
    lua_getglobal(m_lua, funcname.c_str());

    if(!lua_isfunction(m_lua, -1))
    {
        lua_pop(m_lua, 1);
        throw tbe::Exception("ScriptManager::call; undefined function (%s)", funcname.c_str());
    }

    lua_call(m_lua, 0, 0);
}

void ScriptManager::load(std::string scriptpath)
{
    cout << "Loading script " << scriptpath << endl;

    lua_pushinteger(m_lua, (lua_Integer)m_gameManager);
    lua_setglobal(m_lua, GAMEMANAGER_INTERNALE_NAME);

    lua_pushinteger(m_lua, (lua_Integer)this);
    lua_setglobal(m_lua, SCRIPTMANAGER_INTERNALE_NAME);

    lua_pushstring(m_lua, scriptpath.c_str());
    lua_setglobal(m_lua, SCRIPTPATH_INTERNALE_NAME);

    if(luaL_dofile(m_lua, scriptpath.c_str()) != 0)
        throw tbe::Exception("ScriptManager::load; %s (%s)", lua_tostring(m_lua, -1), scriptpath.c_str());
}

void ScriptManager::registerCollid(std::string id, std::string funcname)
{
    m_collidRec[id] = funcname;
}

void ScriptManager::registerCollid(tbe::Vector3f pos, float radius, std::string funcname)
{
    string id = "dummy#" + tools::numToStr(time(0)) + tools::numToStr(rand());

    AreaElement* elem = new AreaElement(m_gameManager, id, pos, radius);

    m_gameManager->registerArea(elem);

    m_collidRec[id] = funcname;
}
