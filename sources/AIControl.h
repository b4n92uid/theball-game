/*
 * File:   AIManager.h
 * Author: b4n92uid
 *
 * Created on 16 décembre 2009, 18:45
 */

#ifndef _AIMANAGER_H
#define	_AIMANAGER_H

#include <Tbe.h>

#include "Settings.h"
#include "Controller.h"
#include "Player.h"

class GameManager;
class Player;
class Item;

class AIControl : public Controller
{
public:
    AIControl(GameManager* playManager);
    ~AIControl();

    typedef std::map<Player*, AIControl*> Map;

protected:
    Player* m_targetPlayer;
    MapElement* m_targetOtp;

    tbe::AABB m_mapAABB;

    tbe::ticks::Clock m_switchWeapon;
    tbe::ticks::Clock m_switchTarget;

    tbe::Vector3f m_strikePos;
    tbe::Vector3f m_targetPos;

    float m_minDistToShoot;
    float m_minDistToSwith;
    float m_strikPrimiter;

    Settings::Ai& m_aiParams;

    tbe::Vector3f m_lastPos;
    tbe::ticks::Clock m_lastPosClock;

    tbe::ticks::Clock m_gustClock;
    unsigned m_gustCount;
};

#endif	/* _AIMANAGER_H */

