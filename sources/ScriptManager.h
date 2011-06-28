/*
 * File:   ScriptManager.h
 * Author: b4n92uid
 *
 * Created on 28 mai 2011, 20:58
 */

#ifndef SCRIPTMANAGER_H
#define	SCRIPTMANAGER_H

#include <string>
#include <vector>
#include <map>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class Player;
class MapElement;
class GameManager;

class ScriptManager
{
public:
    ScriptManager(GameManager* gameManager);
    virtual ~ScriptManager();

    void load(std::string scriptpath);

    void call(std::string funcname);

    void callCollidCallback(std::string funcname, Player* player, MapElement* elem);

    void processCollid(Player* player, MapElement* elem);

    void registerCollid(std::string id, std::string funcname);

private:
    lua_State* m_lua;
    GameManager* m_gameManager;

    std::map<std::string, std::string> m_collidRec;
};

#endif	/* ScriptManager_H */

