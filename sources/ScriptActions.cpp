/*
 * File:   ScriptActions.cpp
 * Author: b4n92uid
 *
 * Created on 28 mai 2011, 20:58
 */

#include "ScriptActions.h"
#include "GameManager.h"
#include "SoundManager.h"

#include "MapElement.h"
#include "Player.h"

#include "ScriptFunctions.h"

using namespace std;
using namespace tbe;

ScriptActions::ScriptActions(GameManager* gameManager)
{
    m_gameManager = gameManager;

    m_lua = lua_open();

    luaL_openlibs(m_lua);

    lua_register(m_lua, "randomPosition", script::randomPosition);

    lua_register(m_lua, "setPosition", script::setPosition);
    lua_register(m_lua, "getPosistion", script::getPosistion);

    lua_register(m_lua, "setVelocity", script::setVelocity);
    lua_register(m_lua, "getVelocity", script::getVelocity);

    lua_register(m_lua, "setForce", script::setForce);
    lua_register(m_lua, "upForce", script::upForce);
    lua_register(m_lua, "getForce", script::getForce);

    lua_register(m_lua, "impulse", script::impulse);

    lua_register(m_lua, "setHealth", script::setHealth);
    lua_register(m_lua, "upHealth", script::upHealth);
    lua_register(m_lua, "getHealth", script::getHealth);

    lua_register(m_lua, "setEnergy", script::setEnergy);
    lua_register(m_lua, "upEnergy", script::upEnergy);
    lua_register(m_lua, "getEnergy", script::getEnergy);

    lua_register(m_lua, "selectPower", script::selectPower);
    lua_register(m_lua, "selectedPower", script::selectedPower);

    lua_register(m_lua, "setAmmo", script::setAmmo);
    lua_register(m_lua, "upAmmo", script::upAmmo);
    lua_register(m_lua, "getAmmo", script::getAmmo);

    lua_register(m_lua, "setWeapon", script::setWeapon);
    lua_register(m_lua, "getWeapon", script::getWeapon);
    lua_register(m_lua, "dropWeapon", script::dropWeapon);

    lua_register(m_lua, "setScore", script::setScore);
    lua_register(m_lua, "upScore", script::upScore);
    lua_register(m_lua, "getScore", script::getScore);

    lua_register(m_lua, "loadSound", script::loadSound);
    lua_register(m_lua, "playSound", script::playSound);

    lua_register(m_lua, "loadMusic", script::loadMusic);
    lua_register(m_lua, "playMusic", script::playMusic);
    lua_register(m_lua, "pauseMusic", script::pauseMusic);
    lua_register(m_lua, "stopMusic", script::stopMusic);

    lua_register(m_lua, "isViewed", script::isViewed);

    lua_register(m_lua, "rayCast", script::rayCast);

    lua_register(m_lua, "randomPosition", script::randomPosition);

    lua_register(m_lua, "diriction", script::diriction);

    lua_register(m_lua, "nearestPlayer", script::nearestPlayer);
    lua_register(m_lua, "farestPlayer", script::farestPlayer);

    lua_register(m_lua, "normalize", script::normalize);

    lua_register(m_lua, "setData", script::setData);

    lua_register(m_lua, "getDataVec", script::getDataVec);
    lua_register(m_lua, "getDataFloat", script::getDataFloat);
    lua_register(m_lua, "getDataInt", script::getDataInt);
    lua_register(m_lua, "getDataString", script::getDataString);

    lua_register(m_lua, "getSceneDataString", script::getSceneDataString);
    lua_register(m_lua, "getSceneDataInt", script::getSceneDataInt);
    lua_register(m_lua, "getSceneDataFloat", script::getSceneDataFloat);
    lua_register(m_lua, "getSceneDataVec", script::getSceneDataVec);

    lua_register(m_lua, "highestScore", script::highestScore);
    lua_register(m_lua, "lowestScore", script::lowestScore);

    lua_register(m_lua, "shoot", script::shoot);
    lua_register(m_lua, "jump", script::jump);
    lua_register(m_lua, "dammage", script::dammage);
    lua_register(m_lua, "power", script::power);

    lua_register(m_lua, "display", script::display);
    lua_register(m_lua, "status", script::status);
    lua_register(m_lua, "gameover", script::gameover);

    lua_register(m_lua, "registerCollid", script::registerCollid);
    lua_register(m_lua, "registerPlayerHook", script::registerPlayerHook);
    lua_register(m_lua, "registerGlobalHook", script::registerGlobalHook);

    lua_register(m_lua, "playerList", script::playerList);

    lua_register(m_lua, "elementsList", script::elementsList);
    lua_register(m_lua, "elementsRand", script::elementsRand);
    lua_register(m_lua, "elementsFirst", script::elementsFirst);

    lua_register(m_lua, "createPlayer", script::createPlayer);
    lua_register(m_lua, "deletePlayer", script::deletePlayer);

    lua_register(m_lua, "killPlayer", script::killPlayer);

    lua_register(m_lua, "visibility", script::visibility);

    lua_register(m_lua, "physics", script::physics);

    lua_register(m_lua, "setFragScore", script::setFragScore);
    lua_register(m_lua, "fragScore", script::fragScore);

    lua_register(m_lua, "setHitScore", script::setHitScore);
    lua_register(m_lua, "hitScore", script::hitScore);

    lua_register(m_lua, "setMaxPlayerHealth", script::setMaxPlayerHealth);
    lua_register(m_lua, "maxPlayerHealth", script::maxPlayerHealth);
}

ScriptActions::~ScriptActions()
{
    lua_close(m_lua);
}

void ScriptActions::processCollid(Player* player, MapElement* elem)
{
    if(m_collidRec.count(elem->getId()))
    {
        callCollidCallback(m_collidRec[elem->getId()], player, elem);
    }
}

void ScriptActions::callCollidCallback(std::string funcname, Player* player, MapElement* elem)
{
    lua_getglobal(m_lua, funcname.c_str());

    if(!lua_isfunction(m_lua, -1))
    {
        lua_pop(m_lua, 1);
        throw tbe::Exception("ScriptActions::call; undefined function (%s)", funcname.c_str());
    }

    lua_pushinteger(m_lua, (lua_Integer)player);
    lua_pushinteger(m_lua, (lua_Integer)elem);
    lua_call(m_lua, 2, 0);
}

void ScriptActions::call(std::string funcname)
{
    lua_getglobal(m_lua, funcname.c_str());

    if(!lua_isfunction(m_lua, -1))
    {
        lua_pop(m_lua, 1);
        throw tbe::Exception("ScriptActions::call; undefined function (%s)", funcname.c_str());
    }

    lua_call(m_lua, 0, 0);
}

void ScriptActions::load(std::string scriptpath)
{
    cout << "Loading script " << scriptpath << endl;

    lua_pushinteger(m_lua, (lua_Integer)m_gameManager);
    lua_setglobal(m_lua, GAMEMANAGER_INTERNALE_NAME);

    lua_pushinteger(m_lua, (lua_Integer)this);
    lua_setglobal(m_lua, SCRIPTMANAGER_INTERNALE_NAME);

    if(luaL_dofile(m_lua, scriptpath.c_str()) != 0)
        throw tbe::Exception("ScriptActions::load; %s (%s)", lua_tostring(m_lua, -1), scriptpath.c_str());
}

void ScriptActions::registerCollid(std::string id, std::string funcname)
{
    m_collidRec[id] = funcname;
}
