/*
 * File:   ScriptActions.h
 * Author: b4n92uid
 *
 * Created on 28 mai 2011, 20:58
 */

#ifndef SCRIPTACTIONS_H
#define	SCRIPTACTIONS_H

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

class ScriptActions
{
public:
    ScriptActions(GameManager* gameManager);
    virtual ~ScriptActions();

    void load(std::string scriptpath);

    void call(std::string funcname);

    void callCollidCallback(std::string funcname, Player* player, MapElement* elem);

    void process(Player* player, MapElement* elem);

    friend int registerCollid(lua_State* lua);

private:
    lua_State* m_lua;
    GameManager* m_gameManager;

    std::map<std::string, std::string> m_collidRec;
};

#endif	/* SCRIPTACTIONS_H */

