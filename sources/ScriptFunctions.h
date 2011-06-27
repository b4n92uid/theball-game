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

namespace script
{

// Position

int setPosition(lua_State* lua);
int getPosition(lua_State* lua);

// Moves

int setVelocity(lua_State* lua);
int getVelocity(lua_State* lua);

int setForce(lua_State* lua);
int upForce(lua_State* lua);
int getForce(lua_State* lua);

int impulse(lua_State* lua);

// Player Stat

int setHealth(lua_State* lua);
int upHealth(lua_State* lua);
int getHealth(lua_State* lua);

int setEnergy(lua_State* lua);
int upEnergy(lua_State* lua);
int getEnergy(lua_State* lua);

int selectPower(lua_State* lua);
int selectedPower(lua_State* lua);

int setAmmo(lua_State* lua);
int upAmmo(lua_State* lua);
int getAmmo(lua_State* lua);

int setWeapon(lua_State* lua);
int getWeapon(lua_State* lua);
int dropWeapon(lua_State* lua);

int setScore(lua_State* lua);
int upScore(lua_State* lua);
int getScore(lua_State* lua);

// Audio

int loadSound(lua_State* lua);
int playSound(lua_State* lua);

int loadMusic(lua_State* lua);
int playMusic(lua_State* lua);
int pauseMusic(lua_State* lua);
int stopMusic(lua_State* lua);

// Compute

int isViewed(lua_State* lua);

int rayCast(lua_State* lua);

int randomPosition(lua_State* lua);

int diriction(lua_State* lua);

int nearestPlayer(lua_State* lua);
int farestPlayer(lua_State* lua);

int normalize(lua_State* lua);

// User data

int setData(lua_State* lua);

int getDataVec(lua_State* lua);
int getDataFloat(lua_State* lua);
int getDataInt(lua_State* lua);
int getDataString(lua_State* lua);

int getSceneDataString(lua_State* lua);
int getSceneDataInt(lua_State* lua);
int getSceneDataFloat(lua_State* lua);
int getSceneDataVec(lua_State* lua);

// Game Stat

int highestScore(lua_State* lua);
int lowestScore(lua_State* lua);

// Actions

int shoot(lua_State* lua);
int jump(lua_State* lua);
int dammage(lua_State* lua);
int power(lua_State* lua);

// HUD

int display(lua_State* lua);
int status(lua_State* lua);
int gameover(lua_State* lua);

// Management

int registerGlobalHook(lua_State* lua);
int registerPlayerHook(lua_State* lua);
int registerCollid(lua_State* lua);

int playerList(lua_State* lua);

int elementsList(lua_State* lua);
int elementsRand(lua_State* lua);
int elementsFirst(lua_State* lua);

int createPlayer(lua_State* lua);
int deletePlayer(lua_State* lua);

int killPlayer(lua_State* lua);

int visibility(lua_State* lua);

int physics(lua_State* lua);

int setFragScore(lua_State* lua);
int fragScore(lua_State* lua);

int setHitScore(lua_State* lua);
int hitScore(lua_State* lua);

int setMaxPlayerHealth(lua_State* lua);
int maxPlayerHealth(lua_State* lua);

}

#endif	/* SCRIPTFUNCTIONS_H */

