/*
 * File:   ScriptFunctions.cpp
 * Author: b4n92uid
 *
 * Created on 30 mai 2011, 23:19
 */

#include "ScriptFunctions.h"

#include "GameManager.h"
#include "SoundManager.h"

#include "MapElement.h"
#include "Player.h"

using namespace std;
using namespace tbe;

namespace script
{

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

int setPosition(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);

    Vector3f vec = lua_tovector3(lua, 2);

    elem->getPhysicBody()->setPos(vec);

    return 0;
}

int getPosistion(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushvector3(lua, player->getPhysicBody()->getPos());

    return 1;
}

int setVelocity(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);

    Vector3f vec = lua_tovector3(lua, 2);

    elem->getPhysicBody()->setVelocity(vec);

    return 0;
}

int getVelocity(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushvector3(lua, player->getPhysicBody()->getVelocity());

    return 1;
}

int setForce(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    Vector3f vec = lua_tovector3(lua, 2);

    elem->getPhysicBody()->setApplyForce(vec);

    return 0;
}

int upForce(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    Vector3f vec = lua_tovector3(lua, 2);

    elem->getPhysicBody()->setApplyForce(elem->getPhysicBody()->getApplyForce() + vec);

    return 0;
}

int getForce(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    lua_pushvector3(lua, elem->getPhysicBody()->getApplyForce());

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

int setHealth(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    player->setLife(lua_tonumber(lua, 1));

    return 0;
}

int upHealth(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    player->upLife(lua_tonumber(lua, 1));

    return 0;
}

int getHealth(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushnumber(lua, player->getLife());

    return 1;
}

int setEnergy(lua_State* lua)
{
    return 0;
}

int upEnergy(lua_State* lua)
{
    return 0;
}

int getEnergy(lua_State* lua)
{
    return 0;
}

int selectPower(lua_State* lua)
{
    return 0;
}

int selectedPower(lua_State* lua)
{
    return 0;
}

int setAmmo(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    int value = lua_tointeger(lua, 2);

    player->getCurWeapon()->setAmmoCount(value);

    return 0;
}

int upAmmo(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    int value = lua_tointeger(lua, 2);

    player->getCurWeapon()->UpAmmoCount(value);

    return 0;
}

int getAmmo(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushinteger(lua, player->getCurWeapon()->getAmmoCount());

    return 1;
}

int setWeapon(lua_State* lua)
{
    return 0;
}

int getWeapon(lua_State* lua)
{
    return 0;
}

int dropWeapon(lua_State* lua)
{
    return 0;
}

int setScore(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    int value = lua_tointeger(lua, 2);

    player->setScore(value);

    return 0;
}

int upScore(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    int value = lua_tointeger(lua, 2);

    player->upScore(value);

    return 0;
}

int getScore(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushinteger(lua, player->getScore());

    return 1;
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

int playSound(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    string id = lua_tostring(lua, 1);
    MapElement* elem = lua_toelem(lua, 2);

    ge->manager.sound->playSound(id, elem);

    return 0;
}

int randomPosition(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    Vector3f pos = ge->getRandomPosOnTheFloor();

    lua_pushvector3(lua, pos);

    return 1;
}

int loadMusic(lua_State* lua)
{
    return 0;
}

int playMusic(lua_State* lua)
{
    return 0;
}

int pauseMusic(lua_State* lua)
{
    return 0;
}

int stopMusic(lua_State* lua)
{
    return 0;
}

int diriction(lua_State* lua)
{
    Vector3f pos1 = lua_tovector3(lua, 1);
    Vector3f pos2 = lua_tovector3(lua, 2);

    lua_pushvector3(lua, pos2 - pos1);

    return 1;
}

int nearestPlayer(lua_State* lua)
{
    return 0;
}

int farestPlayer(lua_State* lua)
{
    return 0;
}

int normalize(lua_State* lua)
{
    return 0;
}

int setData(lua_State* lua)
{
    return 0;
}

int getDataVec(lua_State* lua)
{
    return 0;
}

int getDataFloat(lua_State* lua)
{
    return 0;
}

int getDataInt(lua_State* lua)
{
    return 0;
}

int getDataString(lua_State* lua)
{
    return 0;
}

int getSceneDataString(lua_State* lua)
{
    return 0;
}

int getSceneDataInt(lua_State* lua)
{
    return 0;
}

int getSceneDataFloat(lua_State* lua)
{
    return 0;
}

int getSceneDataVec(lua_State* lua)
{
    return 0;
}

int highestScore(lua_State* lua)
{
    return 0;
}

int lowestScore(lua_State* lua)
{
    return 0;
}

int shoot(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    Vector3f vec = lua_tovector3(lua, 2);

    player->shoot(vec);

    return 0;
}

int jump(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    player->jump();

    return 0;
}

int boost(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    player->boost();

    return 0;
}

int dammage(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    int value = lua_tointeger(lua, 2);

    Bullet b(getGameManager(lua));
    b.setDammage(value);
    player->takeDammage(&b);

    return 0;
}

int display(lua_State* lua)
{
    return 0;
}

int status(lua_State* lua)
{
    return 0;
}

int gameover(lua_State* lua)
{
    return 0;
}

int registerCollid(lua_State* lua)
{
    ScriptActions* sm = getScriptManager(lua);

    string id = lua_tostring(lua, 1);
    string fn = lua_tostring(lua, 2);

    sm->registerCollid(id, fn);

    return 0;
}

int playerList(lua_State* lua)
{
    return 0;
}

int elements(lua_State* lua)
{
    return 0;
}

int killPlayer(lua_State* lua)
{
    return 0;
}

int visibility(lua_State* lua)
{
    return 0;
}

int physics(lua_State* lua)
{
    return 0;
}

int setFragScore(lua_State* lua)
{
    return 0;
}

int fragScore(lua_State* lua)
{
    return 0;
}

int setHitScore(lua_State* lua)
{
    return 0;
}

int hitScore(lua_State* lua)
{
    return 0;
}

int setMaxPlayerHealth(lua_State* lua)
{
    return 0;
}

int maxPlayerHealth(lua_State* lua)
{
    return 0;
}

}
