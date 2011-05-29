/*
 * File:   ScriptActions.cpp
 * Author: b4n92uid
 *
 * Created on 28 mai 2011, 20:58
 */

#include "ScriptActions.h"
#include "GameManager.h"
#include "SoundManager.h"

#define GAMEMANAGER_INTERNALE_NAME "_gameManager"
#define SCRIPTMANAGER_INTERNALE_NAME "_scriptManager"

using namespace std;
using namespace tbe;

inline GameManager* getGameManager(lua_State* lua)
{
    lua_getglobal(lua, GAMEMANAGER_INTERNALE_NAME);

    void* ptr = (void*)lua_tointeger(lua, -1);

    lua_pop(lua, 1);

    return (GameManager*)ptr;
}

inline ScriptActions* getScriptManager(lua_State* lua)
{
    lua_getglobal(lua, SCRIPTMANAGER_INTERNALE_NAME);

    void* ptr = (void*)lua_tointeger(lua, -1);

    lua_pop(lua, 1);

    return (ScriptActions*)ptr;
}

int randomPosition(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int position(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int posistionOf(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int velocity(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int velocityOf(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int impulse(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int health(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int healthUp(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int healthOf(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int energy(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int energyUp(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int energyOf(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int loadSound(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    const char *id = NULL, *filepath = NULL;

    if(lua_isstring(lua, 1))
        id = lua_tostring(lua, 1);

    if(lua_isstring(lua, 2))
        filepath = lua_tostring(lua, 2);

    ge->manager.sound->registerSound(id, filepath);
}

int sound(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);
}

int registerCollid(lua_State* lua)
{
    ScriptActions* sm = getScriptManager(lua);

    ScriptActions::CollidRec record = {
        lua_tostring(lua, 1),
        lua_tostring(lua, 2),
        lua_tostring(lua, 3),
    };

    sm->m_collidRec.push_back(record);
}

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

    lua_register(m_lua, "registerCollid", registerCollid);
}

ScriptActions::~ScriptActions()
{
}

void ScriptActions::process(std::string type1, std::string type2)
{
    if(m_lastFalseResult.type1 == type1 && m_lastFalseResult.type2 == type2)
        return;


    bool nothing = true;

    for(unsigned i = 0; i < m_collidRec.size(); i++)
    {
        if(type1 == m_collidRec[i].type1 && type2 == m_collidRec[i].type2)
        {
            call(m_collidRec[i].funcname);
            nothing = false;
            break;
        }
    }

    if(nothing)
    {
        m_lastFalseResult.type1 = type1;
        m_lastFalseResult.type2 = type2;
    }
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

    lua_settop(m_lua, 0);

    lua_pushinteger(m_lua, (lua_Integer)m_gameManager);
    lua_setglobal(m_lua, GAMEMANAGER_INTERNALE_NAME);

    lua_pushinteger(m_lua, (lua_Integer)this);
    lua_setglobal(m_lua, SCRIPTMANAGER_INTERNALE_NAME);

    if(luaL_dofile(m_lua, scriptpath.c_str()) != 0)
        throw tbe::Exception("ScriptActions::load; %s (%s)", lua_tostring(m_lua, -1), scriptpath.c_str());
}
