/*
 * File:   ScriptFunctions.h
 * Author: b4n92uid
 *
 * Created on 30 mai 2011, 23:25
 */

#ifndef SCRIPTFUNCTIONS_H
#define	SCRIPTFUNCTIONS_H

#include "ScriptManager.h"

#define GAMEMANAGER_INTERNALE_NAME "_gameManager"
#define SCRIPTMANAGER_INTERNALE_NAME "_scriptManager"
#define SCRIPTPATH_INTERNALE_NAME "_scriptPath"

namespace script
{

// General

int include(lua_State* lua);

// Translation

int setPosition(lua_State* lua);
int getPosition(lua_State* lua);

int setVelocity(lua_State* lua);
int getVelocity(lua_State* lua);

int setForce(lua_State* lua);
int upForce(lua_State* lua);
int getForce(lua_State* lua);

int impulse(lua_State* lua);

int stopMotion(lua_State* lua);

// Material

int freeze(lua_State* lua);
int unfreeze(lua_State* lua);

int setGhost(lua_State* lua);
int setImmunity(lua_State* lua);

// Player Stat

int getNickName(lua_State* lua);

int setHealth(lua_State* lua);
int upHealth(lua_State* lua);
int getHealth(lua_State* lua);

int setEnergy(lua_State* lua);
int upEnergy(lua_State* lua);
int getEnergy(lua_State* lua);

int setAmmo(lua_State* lua);
int upAmmo(lua_State* lua);
int getAmmo(lua_State* lua);

int attachPower(lua_State* lua);
int selectPower(lua_State* lua);
int switchPower(lua_State* lua);
int getSelectedPower(lua_State* lua);

int attachWeapon(lua_State* lua);
int selectWeapon(lua_State* lua);
int getSelectedWeapon(lua_State* lua);
int switchWeapon(lua_State* lua);

int move(lua_State* lua);
int jump(lua_State* lua);
int shoot(lua_State* lua);
int power(lua_State* lua);
int dammage(lua_State* lua);

int createPlayers(lua_State* lua);
int killPlayer(lua_State* lua);
int isKilledPlayer(lua_State* lua);
int deletePlayer(lua_State* lua);

// Audio

int loadSound(lua_State* lua);
int playSound(lua_State* lua);

int loadMusic(lua_State* lua);
int playMusic(lua_State* lua);
int pauseMusic(lua_State* lua);
int stopMusic(lua_State* lua);

// Compute

int rayCast(lua_State* lua);

int randomPosition(lua_State* lua);

int nearestPlayer(lua_State* lua);
int farestPlayer(lua_State* lua);

int isViewed(lua_State* lua);

int diriction(lua_State* lua);

int length(lua_State* lua);

int normalize(lua_State* lua);

// User data

int setString(lua_State* lua);
int getString(lua_State* lua);

int setNumber(lua_State* lua);
int getNumber(lua_State* lua);

int setVector(lua_State* lua);
int getVector(lua_State* lua);

int getSceneString(lua_State* lua);

// HUD

int display(lua_State* lua);
int status(lua_State* lua);

// Management

int playerList(lua_State* lua);

int getElement(lua_State* lua);
int getElementsList(lua_State* lua);
int getElementsRand(lua_State* lua);

int registerCollid(lua_State* lua);
int registerGlobalHook(lua_State* lua);
int registerPlayerHook(lua_State* lua);

int gameover(lua_State* lua);

int setTimeout(lua_State* lua);
int setInterval(lua_State* lua);

// PPE

int glowEnable(lua_State* lua);
int glowSettings(lua_State* lua);

// Trigger

int whiteFlash(lua_State* lua);
int earthQuake(lua_State* lua);

}

#endif	/* SCRIPTFUNCTIONS_H */

