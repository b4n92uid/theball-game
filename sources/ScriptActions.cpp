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

    lua_register(m_lua, "randomPosition", randomPosition);

    lua_register(m_lua, "position", position);
    lua_register(m_lua, "posistionOf", posistionOf);

    lua_register(m_lua, "velocity", velocity);
    lua_register(m_lua, "velocityOf", velocityOf);

    lua_register(m_lua, "impulse", impulse);

    lua_register(m_lua, "health", health);
    lua_register(m_lua, "healthUp", healthUp);
    lua_register(m_lua, "healthOf", healthOf);

    lua_register(m_lua, "energy", energy);
    lua_register(m_lua, "energyUp", energyUp);
    lua_register(m_lua, "energyOf", energyOf);

    lua_register(m_lua, "loadSound", loadSound);
    lua_register(m_lua, "sound", sound);

    lua_register(m_lua, "registerCollid", ::registerCollid);

    lua_register(m_lua, "diriction", diriction);
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
