/*
 * File:   ScriptActions.h
 * Author: b4n92uid
 *
 * Created on 28 mai 2011, 20:58
 */

#ifndef SCRIPTACTIONS_H
#define	SCRIPTACTIONS_H

#include <string>

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

private:
    lua_State* m_lua;
    GameManager* m_gameManager;
};

#endif	/* SCRIPTACTIONS_H */

