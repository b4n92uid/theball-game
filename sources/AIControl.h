/* 
 * File:   AIManager.h
 * Author: b4n92uid
 *
 * Created on 16 d�cembre 2009, 18:45
 */

#ifndef _AIMANAGER_H
#define	_AIMANAGER_H

#include <Tbe.h>

#include "Settings.h"
#include "Controller.h"
#include "Player.h"

class PlayManager;
class Player;
class Item;

class AIControl : public Controller
{
public:
    AIControl(PlayManager* playManager);
    ~AIControl();

    typedef std::map<Player*, AIControl*> Map;

protected:
    Player* m_targetPlayer;
    Object* m_targetOtp;

    tbe::AABB m_mapAABB;

    tbe::ticks::Clock m_switchWeapon;
    tbe::ticks::Clock m_switchTarget;

    tbe::Vector3f m_strikePos;
    tbe::Vector3f m_targetPos;

    float m_minDistToShoot;
    float m_minDistToSwith;
    float m_strikPrimiter;

    Settings::Ai& m_aiParams;
};

#endif	/* _AIMANAGER_H */

