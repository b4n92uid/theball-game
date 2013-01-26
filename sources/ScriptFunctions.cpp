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

#include "Bullet.h"
#include "BulletWeapon.h"

#include "GravityGun.h"
#include "BulletTime.h"
#include "Boost.h"

#include <boost/signals.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/format.hpp>

#define foreach BOOST_FOREACH

using namespace std;
using namespace tbe;

namespace script
{

inline string getScriptPath(lua_State* lua)
{
    lua_getglobal(lua, SCRIPTPATH_INTERNALE_NAME);

    string path = lua_tostring(lua, -1);

    return path;
}

inline GameManager* getGameManager(lua_State* lua)
{
    lua_getglobal(lua, GAMEMANAGER_INTERNALE_NAME);

    void* ptr = (void*)(long)lua_tonumber(lua, -1);

    lua_pop(lua, 1);

    return(GameManager*)ptr;
}

inline ScriptManager* getScriptManager(lua_State* lua)
{
    lua_getglobal(lua, SCRIPTMANAGER_INTERNALE_NAME);

    void* ptr = (void*)(long)lua_tonumber(lua, -1);

    lua_pop(lua, 1);

    return(ScriptManager*)ptr;
}

bool __check(MapElement* elem, GameManager* gm)
{
    return tools::find(gm->map.mapElements, elem)
            || tools::find(gm->map.staticElements, elem)
            || tools::find(gm->getPlayers(), elem);
}

bool __check(StaticElement* elem, GameManager* gm)
{
    return tools::find(gm->map.staticElements, elem);
}

bool __check(Player* elem, GameManager* gm)
{
    return tools::find(gm->getPlayers(), elem);
}

#define check(e) \
     \
    if(!e || !__check(e, getGameManager(lua))) \
    { \
        lua_Debug ar; \
        lua_getstack(lua, 1, &ar); \
        lua_getinfo(lua, "nSl", &ar); \
 \
        cout << boost::format("LUA: %1%:%2%@%3% -> %4% undefined element (%5%)") \
        % ar.short_src % ar.currentline % ar.name % __FUNCTION__ % e  << endl; \
\
        return 0; \
    } \

#define invalidArg(e) { cout << "LUA: " << __FUNCTION__ << ": invalid argument (" << e << ")" << endl; return 0; }

int safeLuaCallCommon(lua_State* L, lua_CFunction func)
{
    int result = 0;

    try
    {
        result = func(L);
    }

    catch(const tbe::Exception& e)
    {
        std::cout << "LUA: " << e.what() << std::endl;
    }

    catch(std::exception& e)
    {
        std::cout << "LUA: " << e.what() << std::endl;
    }

    catch(...)
    {
        std::cout << "LUA: Unhandled exception catch" << std::endl;
    }

    return result;
}

int include(lua_State* lua)
{
    string scriptpath = getScriptPath(lua);

    string include = tools::pathScope(scriptpath, lua_tostring(lua, 1), true);

    if(luaL_dofile(lua, include.c_str()) != 0)
        cout << "LUA: " << lua_tostring(lua, -1) << endl;

    return 0;
}

int isStayInFloor(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushboolean(lua, player->isStayDown());

    return 1;
}

int setFloorPosition(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    Vector3f vec = lua_tovector3(lua, 2);

    GameManager* gm = getGameManager(lua);

    if(gm->parallelscene.meshs->findFloor(vec.x, vec.y, vec.z))
    {
        vec.y += -elem->getVisualBody()->getAabb().min.y;

        if(elem->getPhysicBody())
            elem->getPhysicBody()->setPos(vec);
        else if(elem->getVisualBody())
            elem->getVisualBody()->setPos(vec);
    }

    return 0;
}

int setPosition(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    Vector3f vec = lua_tovector3(lua, 2);

    if(elem->getPhysicBody())
        elem->getPhysicBody()->setPos(vec);
    else if(elem->getVisualBody())
        elem->getVisualBody()->setPos(vec);

    return 0;
}

int getPosition(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);

    check(elem);

    if(elem->getPhysicBody())
        lua_pushvector3(lua, elem->getPhysicBody()->getPos());

    else if(elem->getVisualBody())
        lua_pushvector3(lua, elem->getVisualBody()->getPos());

    return 1;
}

int setVelocity(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

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
    check(elem);

    Vector3f vec = lua_tovector3(lua, 2);

    elem->getPhysicBody()->setApplyForce(vec);

    return 0;
}

int upForce(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    Vector3f vec = lua_tovector3(lua, 2);

    elem->getPhysicBody()->setApplyForce(elem->getPhysicBody()->getApplyForce() + vec);

    return 0;
}

int getForce(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    lua_pushvector3(lua, elem->getPhysicBody()->getApplyForce());

    return 1;
}

int impulse(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    Vector3f vec = lua_tovector3(lua, 2);

    float force = lua_tonumber(lua, 3);

    NewtonBodyAddImpulse(elem->getPhysicBody()->getBody(),
                         vec*force,
                         elem->getPhysicBody()->getPos());

    return 0;
}

int freeze(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    elem->getPhysicBody()->setVelocity(0);
    elem->getPhysicBody()->setOmega(0);
    elem->getPhysicBody()->setApplyForce(0);
    elem->getPhysicBody()->setApplyTorque(0);
    elem->getPhysicBody()->setApplyGravity(false);

    return 0;
}

int unfreeze(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    elem->getPhysicBody()->setApplyGravity(true);

    return 0;
}

int stopMotion(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    elem->stopMotion();

    return 0;
}

int setGhost(lua_State* lua)
{
    StaticElement* elem = lua_tostatic(lua, 1);
    bool value = lua_toboolean(lua, 2);

    elem->setGhost(value);

    return 0;
}

int setImmunity(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    bool value = lua_toboolean(lua, 2);

    player->setImmunity(value);

    return 0;
}

int setTextureFrame(lua_State* lua)
{
    StaticElement* elem = lua_tostatic(lua, 1);
    check(elem);

    scene::Mesh* mesh = elem->getVisualBody();

    Vector2i part;
    part.x = lua_tointeger(lua, 2);
    part.y = lua_tointeger(lua, 3);

    mesh->getMaterial(0)->setTexturePart(part);

    return 0;
}

int childOf(lua_State* lua)
{
    StaticElement* elem = lua_tostatic(lua, 1);
    check(elem);

    scene::Node* node = elem->getVisualBody()->getChild(lua_tointeger(lua, 2));

    MapElement* child = getGameManager(lua)->getInterface(node);

    lua_pushelement(lua, child);

    return 1;
}

int getNickName(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushstring(lua, player->getName());
    return 1;
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
    Player* player = lua_toplayer(lua, 1);
    player->setEnergy(lua_tonumber(lua, 1));

    return 0;
}

int upEnergy(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    player->upEnergy(lua_tonumber(lua, 1));

    return 0;
}

int getEnergy(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushnumber(lua, player->getEnergy());

    return 0;
}

int setAmmo(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    int value = lua_tointeger(lua, 2);

    if(player->getCurWeapon())
        player->getCurWeapon()->setAmmoCount(value);

    return 0;
}

int upAmmo(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    int value = lua_tointeger(lua, 2);

    if(player->getCurWeapon())
        player->getCurWeapon()->UpAmmoCount(value);

    return 0;
}

int getAmmo(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    if(player->getCurWeapon())
        lua_pushinteger(lua, player->getCurWeapon()->getAmmoCount());

    return 1;
}

int attachPower(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    Player* player = lua_toplayer(lua, 1);
    string id = lua_tostring(lua, 2);

    if(id == "BulletTime")
        player->addPower(new BulletTime(gm));

    else if(id == "Boost")
        player->addPower(new Boost(gm));

    else if(id == "GravityGun")
        player->addPower(new GravityGun(gm));

    else
        invalidArg(id);


    return 0;
}

int selectPower(lua_State* lua)
{
    return 0;
}

int switchPower(lua_State* lua)
{
    return 0;
}

int getSelectedPower(lua_State* lua)
{
    return 0;
}

int attachWeapon(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    Player* player = lua_toplayer(lua, 1);
    string id = lua_tostring(lua, 2);

    Weapon* weapon = NULL;

    if(id == "Blaster")
        weapon = new WeaponBlaster(gm);

    else if(id == "ShotGun")
        weapon = new WeaponShotgun(gm);

    else if(id == "Finder")
        weapon = new WeaponFinder(gm);

    else if(id == "Fusion")
        weapon = new WeaponFusion(gm);

    else
        invalidArg(id);

    if(lua_isnumber(lua, 3))
        weapon->setAmmoCount(lua_tointeger(lua, 3));

    if(lua_isnumber(lua, 4))
        weapon->setMaxAmmoDammage(lua_tointeger(lua, 4));

    player->addWeapon(weapon);

    return 0;
}

int selectWeapon(lua_State* lua)
{
    return 0;
}

int getSelectedWeapon(lua_State* lua)
{
    return 0;
}

int switchWeapon(lua_State* lua)
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

int volSound(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    string id = lua_tostring(lua, 1);

    float vol = lua_tonumber(lua, 2);

    ge->manager.sound->volSound(id, vol);

    return 0;
}

int isPlaySound(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    string id = lua_tostring(lua, 1);

    bool isplay = ge->manager.sound->isPlaySound(id);

    lua_pushboolean(lua, isplay);

    return 1;
}

int stopSound(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    string id = lua_tostring(lua, 1);

    ge->manager.sound->stopSound(id);

    return 0;
}

int playSound(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    string id = lua_tostring(lua, 1);

    MapElement* elem = lua_toelem(lua, 2);

    int loop = lua_tointeger(lua, 3);

    float vol = lua_isnoneornil(lua, 4) ? 1 : lua_tonumber(lua, 4);

    ge->manager.sound->playSound(id, elem, loop, vol);

    return 0;
}

int loadMusic(lua_State* lua)
{
    string id = lua_tostring(lua, 1);
    string path = lua_tostring(lua, 2);

    GameManager* ge = getGameManager(lua);

    ge->manager.sound->registerMusic(id, path);

    return 0;
}

int playMusic(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    string id = lua_tostring(lua, 1);

    ge->manager.sound->playMusic(id);

    return 0;
}

int pauseMusic(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    string id = lua_tostring(lua, 1);

    ge->manager.sound->pauseMusic(id);

    return 0;
}

int stopMusic(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    string id = lua_tostring(lua, 1);

    ge->manager.sound->stopMusic(id);

    return 0;
}

int rayCast(lua_State* lua)
{
    return 1;
}

int isViewed(lua_State* lua)
{
    return 1;
}

int randomPosition(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    if(lua_istable(lua, 1), lua_isnumber(lua, 2))
    {
        Vector3f pos = lua_tovector3(lua, 1);
        float radius = lua_tonumber(lua, 2);

        Vector3f randpos = ge->getRandomPosOnTheFloor(pos, radius);
        lua_pushvector3(lua, randpos);
    }

    else
    {
        Vector3f randpos = ge->getRandomPosOnTheFloor();
        lua_pushvector3(lua, randpos);
    }

    return 1;
}

struct NearestPlayer
{

    NearestPlayer(Player * p)
    {
        cp = p;
    }

    bool operator()(Player* p1, Player * p2)
    {
        if(p2->isKilled())
            return true;

        if(p1->isKilled())
            return false;

        else if(p1->getPhysicBody()->getPos() - cp->getPhysicBody()->getPos()
                < p2->getPhysicBody()->getPos() - cp->getPhysicBody()->getPos())
            return true;

        else
            return false;
    }

    Player* cp;
};

int nearestPlayer(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    Player* player = lua_toplayer(lua, 1);

    Player::Array target = gm->getPlayers();

    tools::erase(target, player);

    Player* nearest = NULL;

    if(target.size() == 1)
        nearest = target[0];
    else
        nearest = *min_element(target.begin(), target.end(), NearestPlayer(player));

    if(!nearest || nearest->isKilled())
        lua_pushnil(lua);
    else
        lua_pushplayer(lua, nearest);

    return 1;
}

struct FarestPlayer
{

    FarestPlayer(Player * p)
    {
        cp = p;
    }

    bool operator()(Player* p1, Player * p2)
    {
        if(p1 == cp)
            return false;

        if(p2 == cp)
            return true;

        else
        {
            if(p2->isKilled())
                return true;

            if(p1->isKilled())
                return false;

            else if(p1->getPhysicBody()->getPos() - cp->getPhysicBody()->getPos()
                    > p2->getPhysicBody()->getPos() - cp->getPhysicBody()->getPos())
                return true;

            else
                return false;
        }
    }

    Player* cp;
};

int farestPlayer(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    Player* player = lua_toplayer(lua, 1);

    const Player::Array targets = gm->getPlayers();

    Player* farest = *min_element(targets.begin(), targets.end(), FarestPlayer(player));

    if(farest->isKilled())
        lua_pushnil(lua);
    else
        lua_pushplayer(lua, farest);

    return 1;
}

int diriction(lua_State* lua)
{
    Vector3f pos1 = lua_tovector3(lua, 1);
    Vector3f pos2 = lua_tovector3(lua, 2);

    lua_pushvector3(lua, pos2 - pos1);

    return 1;
}

int length(lua_State* lua)
{
    lua_pushnumber(lua, (lua_tovector3(lua, 1) - lua_tovector3(lua, 2)).getMagnitude());

    return 1;
}

int normalize(lua_State* lua)
{
    lua_pushvector3(lua, lua_tovector3(lua, 1).normalize());

    return 1;
}

int setString(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    string key = lua_tostring(lua, 2);
    string value = lua_tostring(lua, 3);

    elem->getVisualBody()->setUserData(key, value);

    return 0;
}

int getString(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    string key = lua_tostring(lua, 2);

    if(elem->getVisualBody()->hasUserData(key))
    {
        Any value = elem->getVisualBody()->getUserData(key);
        lua_pushstring(lua, value.getValue<string > ());
    }
    else
        lua_pushnil(lua);

    return 1;
}

int setNumber(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    string key = lua_tostring(lua, 2);
    double value = lua_tonumber(lua, 3);

    elem->getVisualBody()->setUserData(key, value);

    return 0;
}

int getNumber(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    string key = lua_tostring(lua, 2);

    if(elem->getVisualBody()->hasUserData(key))
    {
        Any value = elem->getVisualBody()->getUserData(key);
        lua_pushnumber(lua, value.getValue<double> ());
    }
    else
        lua_pushnil(lua);

    return 1;
}

int setEntity(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    string key = lua_tostring(lua, 2);
    long value = lua_tointeger(lua, 3);

    elem->getVisualBody()->setUserData(key, value);

    return 0;
}

int getEntity(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    string key = lua_tostring(lua, 2);

    if(elem->getVisualBody()->hasUserData(key))
    {
        Any value = elem->getVisualBody()->getUserData(key);
        lua_pushinteger(lua, value.getValue<long> ());
    }
    else
        lua_pushnil(lua);

    return 1;
}

int setVector(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    string key = lua_tostring(lua, 2);
    Vector3f value = lua_tovector3(lua, 3);

    elem->getVisualBody()->setUserData(key, value);

    return 0;
}

int getVector(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    check(elem);

    string key = lua_tostring(lua, 2);

    if(elem->getVisualBody()->hasUserData(key))
    {
        Any value = elem->getVisualBody()->getUserData(key);
        lua_pushvector3(lua, value.getValue<Vector3f > ());
    }
    else
        lua_pushnil(lua);

    return 1;
}

int getSceneString(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string key = lua_tostring(lua, 1);

    string value = gm->manager.app->getSceneParser()->getAdditionalString(key);

    lua_pushstring(lua, value);

    return 1;
}

int shoot(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    Vector3f vec = lua_tovector3(lua, 2);

    player->shoot(vec);

    return 0;
}

int move(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    Vector3f direction = lua_tovector3(lua, 2);

    player->move(direction);

    return 0;
}

int jump(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    player->jump();

    return 0;
}

int dammage(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    int value = lua_tointeger(lua, 2);

    player->takeDammage(value);

    return 0;
}

int power(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);
    Vector3f vec = lua_tovector3(lua, 2);
    bool stat = lua_toboolean(lua, 3);

    player->power(stat, vec);

    return 0;
}

int createPlayer(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string name = lua_tostring(lua, 1);
    string model = lua_tostring(lua, 2);

    Content::PlayerInfo* pi = new Content::PlayerInfo(gm->manager.app, model);
    Player* player = new Player(gm, name, pi);

    gm->manager.scene->getRootNode()->addChild(player->getVisualBody());

    gm->registerPlayer(player);

    lua_pushplayer(lua, player);

    return 1;
}

int isKilledPlayer(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    check(player);

    lua_pushboolean(lua, player->isKilled());

    return 1;
}

int kickPlayer(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    Player* player = lua_toplayer(lua, 1);

    check(player);

    gm->unregisterPlayer(player);

    delete player;

    return 0;
}

int kickAllPlayers(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    bool kickuser = false;

    if(lua_isboolean(lua, 1))
        kickuser = lua_toboolean(lua, 1);

    Player* upl = gm->getUserPlayer();

    Player::Array pls = gm->getPlayers();

    BOOST_FOREACH(Player* player, pls)
    {
        if(player == upl && !kickuser)
            continue;

        gm->unregisterPlayer(player, true);
    }

    return 0;
}

int killPlayer(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    Player* killer = NULL;

    if(lua_isnumber(lua, 2))
        killer = lua_toplayer(lua, 2);

    player->kill(killer);

    return 0;
}

int killAllPlayers(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    bool kickuser = false;

    if(lua_isboolean(lua, 1))
        kickuser = lua_toboolean(lua, 1);

    Player* killer = NULL;

    if(lua_isnumber(lua, 2))
        killer = lua_toplayer(lua, 2);

    Player* upl = gm->getUserPlayer();

    Player::Array pls = gm->getPlayers();

    BOOST_FOREACH(Player* player, pls)
    {
        if(player == upl && !kickuser)
            continue;

        player->kill(killer);
    }

    return 0;
}

int display(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    if(lua_isnumber(lua, 2))
        gm->display(lua_tostring(lua, 1), lua_tointeger(lua, 2));
    else
        gm->display(lua_tostring(lua, 1));

    return 0;
}

int status(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    gm->status(lua_tostring(lua, 1));

    return 0;
}

int playerList(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    Player::Array players = gm->getPlayers();

    lua_pushtable(lua, players);

    return 1;
}

int userPlayer(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    Player* player = gm->getUserPlayer();

    lua_pushplayer(lua, player);

    return 1;
}

int getElement(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string id = lua_tostring(lua, 1);

    BOOST_FOREACH(MapElement* elem, gm->map.mapElements)
    {
        if(elem->getId() == id)
        {
            lua_pushelement(lua, elem);
            return 1;
        }
    }

    lua_pushnil(lua);

    cout << "LUA: " << __FUNCTION__ << ": return nil for (" << id << ")" << endl;
                                                                                                                                                                                                                                                                                                    \
    return 1;
}

int getElementsList(lua_State* lua)
{
    using namespace boost;

    GameManager* gm = getGameManager(lua);

    if(lua_gettop(lua) == 0)
    {
        lua_pushtable(lua, gm->map.mapElements);
    }
    else
    {
        if(lua_istable(lua, 1) and lua_isnumber(lua, 2))
        {
            // Vector3f pos = lua_tovector(lua, 1);
            // float radius = lua_tonumber(lua, 2);

            // TODO pick element in area
        }
        else if(lua_isstring(lua, 1))
        {
            regex pattern(lua_tostring(lua, 1));

            int count = 0;

            if(lua_isnumber(lua, 2))
                count = lua_tonumber(lua, 2);

            MapElement::Array selection;

            BOOST_FOREACH(MapElement* elem, gm->map.mapElements)
            {
                if(regex_match(elem->getId(), pattern))
                    selection.push_back(elem);

                if(count > 0 && (int)selection.size() >= count)
                    break;
            }

            if(selection.empty())
                lua_pushnil(lua);
            else
                lua_pushtable(lua, selection);
        }

    }

    return 1;
}

int getElementsRand(lua_State* lua)
{
    using namespace boost;

    GameManager* gm = getGameManager(lua);

    regex pattern(lua_tostring(lua, 1));

    int count = 0;

    if(lua_isnumber(lua, 2))
        count = lua_tonumber(lua, 2);

    MapElement::Array selection;

    BOOST_FOREACH(MapElement* elem, gm->map.mapElements)
    {
        if(regex_match(elem->getId(), pattern))
            selection.push_back(elem);

        if(count > 0 && (int)selection.size() >= count)
            break;
    }

    if(selection.empty())
        lua_pushnil(lua);
    else
        lua_pushinteger(lua, (lua_Integer)selection[math::rand(0, selection.size())]);

    return 1;
}

struct ScoreHook
{

    ScoreHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    string operator()()
    {
        lua_getglobal(lua, callback.c_str());

        lua_call(lua, 0, 1);

        return lua_tostring(lua, -1);
    }

    string callback;
    lua_State* lua;
};

struct FrameHook
{

    FrameHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    void operator()(Player * userplayer)
    {
        lua_getglobal(lua, callback.c_str());
        lua_pushplayer(lua, userplayer);
        lua_call(lua, 1, 0);
    }

    string callback;
    lua_State* lua;
};

struct OutOfArenaHook
{

    OutOfArenaHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    bool operator()(Player * userplayer)
    {
        lua_getglobal(lua, callback.c_str());
        lua_pushplayer(lua, userplayer);
        lua_call(lua, 1, 1);

        return lua_toboolean(lua, -1);
    }

    string callback;
    lua_State* lua;
};

struct PlayerInitHook
{

    PlayerInitHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    bool operator()(Player * userplayer)
    {
        lua_getglobal(lua, callback.c_str());
        lua_pushplayer(lua, userplayer);
        lua_call(lua, 1, 0);
    }

    string callback;
    lua_State* lua;
};

struct StartGameHook
{

    StartGameHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    bool operator()(Player * userplayer)
    {
        lua_getglobal(lua, callback.c_str());
        lua_pushplayer(lua, userplayer);
        lua_call(lua, 1, 1);

        return lua_toboolean(lua, -1);
    }

    string callback;
    lua_State* lua;
};

int registerGlobalHook(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string type = lua_tostring(lua, 1);
    string func = lua_tostring(lua, 2);

    if(type == "player")
        gm->onPlayerInit.connect(PlayerInitHook(lua, func));

    else if(type == "frame")
        gm->onEachFrame.connect(FrameHook(lua, func));

    else if(type == "start")
        gm->onStartGame.connect(StartGameHook(lua, func));

    else if(type == "out")
        gm->onOutOfArena.connect(OutOfArenaHook(lua, func));

    return 0;
}

struct RespawnHook
{

    RespawnHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    void operator()(Player * player)
    {
        lua_getglobal(lua, callback.c_str());

        lua_pushinteger(lua, (lua_Integer)player);
        lua_call(lua, 1, 0);
    }

    string callback;
    lua_State* lua;
};

struct PowerHook
{

    PowerHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    bool operator()(Player* player, bool stat, Vector3f target)
    {
        lua_getglobal(lua, callback.c_str());

        lua_pushinteger(lua, (lua_Integer)player);
        lua_pushboolean(lua, stat);
        lua_pushvector3(lua, target);
        lua_call(lua, 3, 1);

        return lua_toboolean(lua, -1);
    }

    string callback;
    lua_State* lua;
};

struct ShootHook
{

    ShootHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    bool operator()(Player* player, Vector3f target)
    {
        lua_getglobal(lua, callback.c_str());

        lua_pushinteger(lua, (lua_Integer)player);
        lua_pushvector3(lua, target);
        lua_call(lua, 2, 1);

        return lua_toboolean(lua, -1);
    }

    string callback;
    lua_State* lua;
};

struct KilledHook
{

    KilledHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    bool operator()(Player* player, Player * killer)
    {
        lua_getglobal(lua, callback.c_str());

        lua_pushplayer(lua, player);
        lua_pushplayer(lua, killer);
        lua_call(lua, 2, 1);

        return lua_toboolean(lua, -1);
    }

    string callback;
    lua_State* lua;
};

struct DammageHook
{

    DammageHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    bool operator()(Player* player, Player * shooter)
    {
        lua_getglobal(lua, callback.c_str());

        lua_pushplayer(lua, player);
        lua_pushplayer(lua, shooter);
        lua_call(lua, 2, 1);

        return lua_toboolean(lua, -1);
    }

    string callback;
    lua_State* lua;
};

struct AIHook
{

    AIHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    void operator()(Player * player)
    {
        lua_getglobal(lua, callback.c_str());

        lua_pushinteger(lua, (lua_Integer)player);
        lua_call(lua, 1, 0);
    }

    string callback;
    lua_State* lua;
};

struct JumpHook
{

    JumpHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    bool operator()(Player* player, bool allowed)
    {
        lua_getglobal(lua, callback.c_str());

        lua_pushplayer(lua, player);
        lua_pushboolean(lua, allowed);
        lua_call(lua, 2, 1);

        return lua_toboolean(lua, -1);
    }

    string callback;
    lua_State* lua;
};

struct MoveHook
{

    MoveHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    bool operator()(Player* player, Vector3f force)
    {
        lua_getglobal(lua, callback.c_str());

        lua_pushplayer(lua, player);
        lua_pushvector3(lua, force);
        lua_call(lua, 2, 1);

        return lua_toboolean(lua, -1);
    }

    string callback;
    lua_State* lua;
};

struct VelocityHook
{

    VelocityHook(lua_State* l, string f)
    {
        lua = l;
        callback = f;
    }

    bool operator()(Player* player, Vector3f vel)
    {
        lua_getglobal(lua, callback.c_str());

        lua_pushplayer(lua, player);
        lua_pushvector3(lua, vel);
        lua_call(lua, 2, 1);

        return lua_toboolean(lua, -1);
    }

    string callback;
    lua_State* lua;
};

int registerPlayerHook(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string type = lua_tostring(lua, 1);
    string func = lua_tostring(lua, 2);

    Player::Array players;

    if(lua_isnumber(lua, 3))
        players.push_back(lua_toplayer(lua, 3));
    else
        players = gm->getPlayers();

    BOOST_FOREACH(Player* player, players)
    {
        if(type == "ai")
        {
            Controller* ctrl = player->getAttachedCotroller();

            if(!ctrl)
            {
                AIControl* ai = new AIControl(gm);
                ai->onAi.connect(AIHook(lua, func));

                player->attachController(ai);
            }

        }

        else if(type == "respawn")
            player->onRespawn.connect(RespawnHook(lua, func));

        else if(type == "dammage")
            player->onDammage.connect(DammageHook(lua, func));

        else if(type == "killed")
            player->onKilled.connect(KilledHook(lua, func));

        else if(type == "shoot")
            player->onShoot.connect(ShootHook(lua, func));

        else if(type == "power")
            player->onPower.connect(PowerHook(lua, func));

        else if(type == "jump")
            player->onJump.connect(JumpHook(lua, func));

        else if(type == "move")
            player->onMove.connect(MoveHook(lua, func));

        else if(type == "velocity")
            player->onVelocity.connect(VelocityHook(lua, func));
    }

    return 0;
}

int registerElementCollid(lua_State* lua)
{
    ScriptManager* sm = getScriptManager(lua);

    string id = lua_tostring(lua, 1);
    string fn = lua_tostring(lua, 2);

    sm->registerCollid(id, fn);

    return 0;
}

int registerAreaCollid(lua_State* lua)
{
    ScriptManager* sm = getScriptManager(lua);

    Vector3f pos = lua_tovector3(lua, 1);
    float radius = lua_tonumber(lua, 2);
    string fn = lua_tostring(lua, 3);

    sm->registerCollid(pos, radius, fn);

    return 0;
}

int gameover(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    gm->setGameOver(lua_toplayer(lua, 1), lua_tostring(lua, 2));

    return 0;
}

struct Timer
{

    Timer(lua_State* l, string c, int t)
    {
        lua = l;
        callback = c;
        time = t;
        shotid = 0;
    }

    void operator()(Player * userplayer)
    {
        if(clock.isEsplanedTime(time))
        {
            lua_getglobal(lua, callback.c_str());

            foreach(int v, objects)
            lua_pushinteger(lua, v);

            lua_call(lua, objects.size(), 0);

            if(shotid)
                userplayer->getGameManager()->onEachFrame.disconnect(shotid);
        }
    }

    lua_State* lua;

    ticks::Clock clock;

    string callback;
    int time;
    vector<int> objects;

    int shotid;
};

int setInterval(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string callback = lua_tostring(lua, 1);
    int time = lua_tointeger(lua, 2);

    Timer slot(lua, callback, time);

    for(int i = 3; lua_isnumber(lua, i); i++)
        slot.objects.push_back(lua_tointeger(lua, i));

    gm->onEachFrame.connect(slot);

    return 0;
}

int setTimeout(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string callback = lua_tostring(lua, 1);
    int time = lua_tointeger(lua, 2);

    Timer slot(lua, callback, time);
    slot.shotid = std::time(0);

    for(int i = 3; lua_isnumber(lua, i); i++)
        slot.objects.push_back(lua_tointeger(lua, i));

    gm->onEachFrame.connect(slot.shotid, slot);

    return 0;
}

int glowEnable(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    if(!gm->manager.app->globalSettings.video.ppeUse)
        return 0;

    bool stat = lua_toboolean(lua, 1);

    gm->glowEnable(stat);

    return 0;
}

int glowSettings(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    if(!gm->manager.app->globalSettings.video.ppeUse)
        return 0;

    float intensity = lua_tonumber(lua, 1);
    float thershold = lua_tonumber(lua, 2);
    float blurpass = lua_tonumber(lua, 3);

    gm->glowSettings(intensity, thershold, blurpass);

    return 0;
}

int whiteFlash(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    float init = lua_tonumber(lua, 1);
    float down = lua_tonumber(lua, 2);

    gm->whiteFlash(init, down);

    return 0;
}

int earthQuake(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    float init = lua_tonumber(lua, 1);
    float down = lua_tonumber(lua, 2);

    gm->earthQuake(init, down);

    return 0;
}

}
