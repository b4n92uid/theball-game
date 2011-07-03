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

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#define foreach BOOST_FOREACH

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

inline ScriptManager* getScriptManager(lua_State* lua)
{
    lua_getglobal(lua, SCRIPTMANAGER_INTERNALE_NAME);

    void* ptr = (void*)(long)lua_tonumber(lua, -1);

    lua_pop(lua, 1);

    return (ScriptManager*)ptr;
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

template<typename T> void lua_pushtable(lua_State* lua, const vector<T>& vec)
{
    lua_newtable(lua);

    for(unsigned i = 0; i < vec.size(); i++)
    {
        lua_pushnumber(lua, i);
        lua_pushnumber(lua, (lua_Integer)vec[i]);
        lua_settable(lua, -3);
    }
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

void lua_pushstring(lua_State *L, string s)
{
    lua_pushstring(L, s.c_str());
}

int setPosition(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);

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

    if(elem->getPhysicBody())
        lua_pushvector3(lua, elem->getPhysicBody()->getPos());
    else if(elem->getVisualBody())
        lua_pushvector3(lua, elem->getVisualBody()->getPos());

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

int playSound(lua_State* lua)
{
    GameManager* ge = getGameManager(lua);

    string id = lua_tostring(lua, 1);
    MapElement* elem = lua_toelem(lua, 2);

    ge->manager.sound->playSound(id, elem);

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
                    < p2->getPhysicBody()->getPos() - cp->getPhysicBody()->getPos())
                return true;

            else
                return false;
        }
    }

    Player* cp;
};

int nearestPlayer(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    Player* player = lua_toplayer(lua, 1);

    const Player::Array target = gm->getPlayers();

    Player* nearest = *min_element(target.begin(), target.end(), NearestPlayer(player));

    if(nearest->isKilled())
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

int setElemDataS(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);

    string key = lua_tostring(lua, 2);
    string value = lua_tostring(lua, 3);

    elem->getVisualBody()->setUserData(key, value);

    return 0;
}

int getElemDataS(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
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

int setElemDataN(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);

    string key = lua_tostring(lua, 2);
    float value = lua_tonumber(lua, 3);

    elem->getVisualBody()->setUserData(key, value);

    return 0;
}

int getElemDataN(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
    string key = lua_tostring(lua, 2);

    if(elem->getVisualBody()->hasUserData(key))
    {
        Any value = elem->getVisualBody()->getUserData(key);
        lua_pushnumber(lua, value.getValue<float> ());
    }
    else
        lua_pushnil(lua);

    return 1;
}

int setElemDataV(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);

    string key = lua_tostring(lua, 2);
    Vector3f value = lua_tovector3(lua, 3);

    elem->getVisualBody()->setUserData(key, value);

    return 0;
}

int getElemDataV(lua_State* lua)
{
    MapElement* elem = lua_toelem(lua, 1);
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

int getSceneData(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string key = lua_tostring(lua, 1);

    string value = gm->manager.parser->getAdditionalString(key);

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

int createPlayers(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    if(lua_isnumber(lua, 1))
    {
        int count = lua_tonumber(lua, 1);

        for(int i = 0; i < count; i++)
        {
            unsigned selectPlayer = math::rand(0, gm->manager.app->globalSettings.availablePlayer.size());

            Settings::PlayerInfo& pi = gm->manager.app->globalSettings.availablePlayer[selectPlayer];

            unsigned selectName = math::rand(0, gm->manager.app->globalSettings.botNames.size());

            Player* player = new Player(gm, gm->manager.app->globalSettings.botNames[selectName], pi.model);

            gm->registerPlayer(player);

            gm->manager.scene->getRootNode()->addChild(player->getVisualBody());
        }
    }
    else if(lua_isstring(lua, 1) && lua_isstring(lua, 2))
    {
        string name = lua_tostring(lua, 1);
        string model = lua_tostring(lua, 2);

        Player* player = new Player(gm, name, model);

        gm->registerPlayer(player);

        gm->manager.scene->getRootNode()->addChild(player->getVisualBody());
    }

    return 0;
}

int deletePlayer(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    Player* player = lua_toplayer(lua, 1);

    gm->unregisterPlayer(player);
    delete player;

    return 0;
}

int isKilledPlayer(lua_State* lua)
{
    Player* player = lua_toplayer(lua, 1);

    lua_pushboolean(lua, player->isKilled());
    return 1;
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

int display(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

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

    lua_pushtable(lua, gm->getPlayers());

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

int registerGlobalHook(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string type = lua_tostring(lua, 1);
    string func = lua_tostring(lua, 2);

    if(type == "frame")
        gm->onEachFrame.connect(FrameHook(lua, func));

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
        lua_call(lua, 1, 1);
    }

    string callback;
    lua_State* lua;
};

int registerPlayerHook(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string type = lua_tostring(lua, 1);
    string func = lua_tostring(lua, 2);

    const Player::Array& players = gm->getPlayers();

    BOOST_FOREACH(Player* player, players)
    {
        if(type == "ai")
        {
            Controller* ctrl = player->getAttachedCotroller();

            if(!ctrl)
            {
                ctrl = new AIControl(gm);
                ctrl->onAi.connect(AIHook(lua, func));

                player->attachController(ctrl);
            }

        }

        else if(type == "respawn")
        {
            player->onRespawn.connect(RespawnHook(lua, func));
            player->onRespawn(player);
        }

        else if(type == "dammage")
            player->onDammage.connect(DammageHook(lua, func));

        else if(type == "killed")
            player->onKilled.connect(KilledHook(lua, func));

        else if(type == "shoot")
            player->onShoot.connect(ShootHook(lua, func));

        else if(type == "power")
            player->onPower.connect(PowerHook(lua, func));

        /*
        if(type == "jump")
            player->onJump.connect();
        if(type == "move")
            player->onMove.connect();
         */
    }

    return 0;
}

int registerCollid(lua_State* lua)
{
    ScriptManager* sm = getScriptManager(lua);

    string id = lua_tostring(lua, 1);
    string fn = lua_tostring(lua, 2);

    sm->registerCollid(id, fn);

    return 0;
}

int gameover(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    gm->setGameOver(lua_toplayer(lua, 1), lua_tostring(lua, 2));

    return 0;
}

int ghost(lua_State* lua)
{
    return 0;
}

struct Interval
{

    Interval(lua_State* l, string c, int t)
    {
        lua = l;
        callback = c;
        time = t;
    }

    void operator()(Player * userplayer)
    {
        if(clock.isEsplanedTime(time))
        {
            lua_getglobal(lua, callback.c_str());
            lua_pushplayer(lua, userplayer);
            lua_call(lua, 1, 0);
        }
    }

    ticks::Clock clock;
    lua_State* lua;
    string callback;
    int time;
};

int setInterval(lua_State* lua)
{
    GameManager* gm = getGameManager(lua);

    string callback = lua_tostring(lua, 1);
    int time = lua_tointeger(lua, 2);

    gm->onEachFrame.connect(Interval(lua, callback, time));

    return 0;
}

}
