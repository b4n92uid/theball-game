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

#define GAMEMANAGER_INTERNALE_NAME "_gameManager"
#define SCRIPTMANAGER_INTERNALE_NAME "_scriptManager"

using namespace std;
using namespace tbe;

inline GameManager* getGameManager(lua_State* lua)
{
    lua_getglobal(lua, GAMEMANAGER_INTERNALE_NAME);

    void* ptr = (void*)(long)lua_tonumber(lua, -1);

    lua_pop(lua, 1);

    return (GameManager*)ptr;
}

inline ScriptActions* getScriptManager(lua_State* lua)
{
    lua_getglobal(lua, SCRIPTMANAGER_INTERNALE_NAME);

    void* ptr = (void*)(long)lua_tonumber(lua, -1);

    lua_pop(lua, 1);

    return (ScriptActions*)ptr;
}

inline Player* lua_toplayer(lua_State* lua, int argpos)
{
    return reinterpret_cast<Player*>((long)lua_tonumber(lua, argpos));
}

inline MapElement* lua_toelem(lua_State* lua, int argpos)
{
    return reinterpret_cast<MapElement*>((long)lua_tonumber(lua, argpos));
}

inline void lua_pushvector3(lua_State* lua, Vector3f vec)
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

inline Vector3f lua_tovector3(lua_State* lua, int argpos)
{
    Vector3f vec;

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

int randomPosition(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    Vector3f pos = ge->getRandomPosOnTheFloor();

    lua_pushvector3(lua, pos);

    return 1;
}

int position(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);

    Vector3f vec = lua_tovector3(lua, 2);

    elem->getPhysicBody()->setPos(vec);

    return 0;
}

int posistionOf(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushvector3(lua, player->getPhysicBody()->getPos());

    return 1;
}

int velocity(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);

    Vector3f vec = lua_tovector3(lua, 2);

    elem->getPhysicBody()->setVelocity(vec);

    return 0;
}

int velocityOf(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushvector3(lua, player->getPhysicBody()->getVelocity());

    return 1;
}

int impulse(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);

    Vector3f vec = lua_tovector3(lua, 2);

    float force = lua_tonumber(lua, 3);

    NewtonBodyAddImpulse(elem->getPhysicBody()->getBody(),
                         vec*force,
                         elem->getPhysicBody()->getPos());

    return 0;
}

int health(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    player->setLife(lua_tonumber(lua, 1));

    return 0;
}

int healthUp(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    player->upLife(lua_tonumber(lua, 1));

    return 0;
}

int healthOf(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushnumber(lua, player->getLife());

    return 1;
}

int energy(lua_State* lua)
{
    return 0;
}

int energyUp(lua_State* lua)
{
    return 0;
}

int energyOf(lua_State* lua)
{
    return 0;
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

    return 0;
}

int sound(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    string id = lua_tostring(lua, 1);
    MapElement* elem = lua_toelem(lua, 2);

    ge->manager.sound->play(id, elem);

    return 0;
}

int registerCollid(lua_State* lua)
{
    ScriptActions* sm = getScriptManager(lua);

    string id = lua_tostring(lua, 1);
    string fn = lua_tostring(lua, 2);

    sm->m_collidRec[id] = fn;

    return 0;
}

void diriction(lua_State* lua)
{
    Vector3f pos1 = lua_tovector3(lua, 1);
    Vector3f pos2 = lua_tovector3(lua, 2);

    lua_pushvector3(lua, pos2 - pos1);

    return 1;
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

    lua_register(m_lua, "diriction", diriction);
}

ScriptActions::~ScriptActions()
{
    lua_close(m_lua);
}

void ScriptActions::process(Player* player, MapElement* elem)
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
