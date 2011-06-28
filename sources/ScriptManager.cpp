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

    lua_register(m_lua, "setPosition", script::setPosition);
    lua_register(m_lua, "getPosition", script::getPosition);
}

ScriptManager::~ScriptManager()
{
    lua_close(m_lua);
}

void ScriptManager::processCollid(Player* player, MapElement* elem)
{
    if(m_collidRec.count(elem->getId()))
    {
        callCollidCallback(m_collidRec[elem->getId()], player, elem);
    }
}

void ScriptManager::callCollidCallback(std::string funcname, Player* player, MapElement* elem)
{
    lua_getglobal(m_lua, funcname.c_str());

    if(!lua_isfunction(m_lua, -1))
    {
        lua_pop(m_lua, 1);
        throw tbe::Exception("ScriptManager::call; undefined function (%s)", funcname.c_str());
    }

    lua_pushinteger(m_lua, (lua_Integer)player);
    lua_pushinteger(m_lua, (lua_Integer)elem);
    lua_call(m_lua, 2, 0);
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

    if(luaL_dofile(m_lua, scriptpath.c_str()) != 0)
        throw tbe::Exception("ScriptManager::load; %s (%s)", lua_tostring(m_lua, -1), scriptpath.c_str());
}

void ScriptManager::registerCollid(std::string id, std::string funcname)
{
    m_collidRec[id] = funcname;
}
