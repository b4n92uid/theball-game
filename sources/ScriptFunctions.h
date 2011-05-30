/*
 * File:   ScriptFunctions.h
 * Author: b4n92uid
 *
 * Created on 30 mai 2011, 23:25
 */

#ifndef SCRIPTFUNCTIONS_H
#define	SCRIPTFUNCTIONS_H

#include "ScriptActions.h"

#define GAMEMANAGER_INTERNALE_NAME "_gameManager"
#define SCRIPTMANAGER_INTERNALE_NAME "_scriptManager"

int randomPosition(lua_State* lua);

int position(lua_State* lua);

int posistionOf(lua_State* lua);

int velocity(lua_State* lua);

int velocityOf(lua_State* lua);

int impulse(lua_State* lua);

int health(lua_State* lua);

int healthUp(lua_State* lua);

int healthOf(lua_State* lua);

int energy(lua_State* lua);

int energyUp(lua_State* lua);

int energyOf(lua_State* lua);

int loadSound(lua_State* lua);

int sound(lua_State* lua);

int registerCollid(lua_State* lua);

int diriction(lua_State* lua);

#endif	/* SCRIPTFUNCTIONS_H */

