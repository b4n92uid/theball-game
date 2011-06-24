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

int setPosition(lua_State* lua); /// elem, pos
int getPosistion(lua_State* lua); /// elem

// Moves

int setVelocity(lua_State* lua); /// elem, vec
int getVelocity(lua_State* lua); /// elem

int setForce(lua_State* lua); /// elem, vec
int upForce(lua_State* lua); /// elem, vec
int getForce(lua_State* lua); /// elem

int impulse(lua_State* lua); /// elem, vec

// Player Stat

int setHealth(lua_State* lua); /// player, value
int upHealth(lua_State* lua); /// player, value
int getHealth(lua_State* lua); /// player

int setEnergy(lua_State* lua); /// player, value
int upEnergy(lua_State* lua); /// player, value
int getEnergy(lua_State* lua); /// player

int selectPower(lua_State* lua); /// player, name
int selectedPower(lua_State* lua); /// player

int setAmmo(lua_State* lua); /// player, value
int upAmmo(lua_State* lua); /// player, value
int getAmmo(lua_State* lua); /// player

int setWeapon(lua_State* lua); /// player, name
int getWeapon(lua_State* lua); /// player
int dropWeapon(lua_State* lua); /// player, name

int setScore(lua_State* lua); /// player, value
int upScore(lua_State* lua); /// player, value
int getScore(lua_State* lua); /// player

// Audio

int loadSound(lua_State* lua); /// id, path
int playSound(lua_State* lua); /// id, elem

int loadMusic(lua_State* lua); /// id, path
int playMusic(lua_State* lua); /// id
int pauseMusic(lua_State* lua); /// id
int stopMusic(lua_State* lua); /// id

// Compute

int randomPosition(lua_State* lua); /// pos, radius

int diriction(lua_State* lua); /// elem1, elem2

int nearestPlayer(lua_State* lua); /// player
int farestPlayer(lua_State* lua); /// player

int normalize(lua_State* lua); /// vec

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
int boost(lua_State* lua);
int dammage(lua_State* lua);

// HUD

int display(lua_State* lua);
int status(lua_State* lua);
int gameover(lua_State* lua);

// Management

int registerCollid(lua_State* lua);
int playerList(lua_State* lua);

int elements(lua_State* lua);

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

