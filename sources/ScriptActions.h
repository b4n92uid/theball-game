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

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class GameManager;

class ScriptActions
{
public:
    ScriptActions(GameManager* gameManager);
    virtual ~ScriptActions();

    void load(std::string scriptpath);

    void call(std::string funcname);

    void process(std::string type1, std::string type2);

    friend int registerCollid(lua_State* lua);

private:
    lua_State* m_lua;
    GameManager* m_gameManager;

    struct CollidRec
    {
        std::string type1;
        std::string type2;
        std::string funcname;
    };

    std::vector<CollidRec> m_collidRec;
};

#endif	/* SCRIPTACTIONS_H */

