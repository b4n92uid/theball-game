/*
 * File:   AloneModeAi.cpp
 * Author: b4n92uid
 *
 * Created on 15 juin 2010, 17:25
 */

#include "AloneModeAi.h"

#include "GameManager.h"
#include "Player.h"

using namespace std;
using namespace tbe;

AloneModeAi::AloneModeAi(GameManager* playManager) : AIControl(playManager)
{
}

AloneModeAi::~AloneModeAi()
{
}

void AloneModeAi::process(Player* player)
{
    /*
     IA Process Alone :
     ------------------

     Rechrche une collision avec la map : isCollidWithMap

     Rechrche un item nécéssaire : m_targetItem

     Si m_targetItem
     {
        Si isCollidWithMap
            Mouvement vers m_targetItem.pos
     }

     Sinon
     {
        Si userPlayer.pos - playerPos < m_minDistToShoot
            Tire sur m_targetPos

        Si isCollidWithMap
            Mouvement vers m_strikePos

        Si AI_SWITCH_WEAPON_TIME
            Changement d'arme
     }
     */

    if(!m_targetPlayer)
        m_targetPlayer = m_playManager->getUserPlayer();

    Vector3f addForce;
    Vector3f playerPos = player->getVisualBody()->getPos();

    /*
        StaticObject::Array& staticObjects = m_playManager->map.staticObjects;

        bool isCollidWithMap = false;

        for(unsigned i = 0; i < staticObjects.size(); i++)
            if(isCollidWithMap = player->getPhysicBody()->IsCollidWith(staticObjects[i]->getPhysicBody()))
                break;
     */

    m_targetPos = m_targetPlayer->getVisualBody()->getPos();

    if(m_switchTarget.isEsplanedTime(m_aiParams.switchTargetTime)
       || m_strikePos - playerPos < m_minDistToSwith)
    {
        do
            m_strikePos = tools::rand(m_targetPos - m_strikPrimiter, m_targetPos + m_strikPrimiter);
        while(!m_mapAABB.isInner(m_strikePos));

        m_strikePos.y = 0;
    }

    if(m_targetPos - playerPos < m_minDistToShoot)
    {
        Vector3f targetPos = m_targetPos;

        if(m_targetPlayer == m_playManager->getUserPlayer()
           && m_playManager->getBullettime()->isActive())
        {
            Vector3f velocity;
            NewtonBodyGetVelocity(m_targetPlayer->getPhysicBody()->getBody(), velocity);

            targetPos = m_targetPos - velocity;
        }

        if(m_gustCount < m_aiParams.shootGustCount)
        {
            Vector3f margin = tools::rand(AABB(m_aiParams.shootAccuracy));
            if(player->shoot(m_targetPos + margin))
                m_gustCount++;
        }
        else if(m_gustClock.isEsplanedTime(m_aiParams.shootGustTime))
            m_gustCount = 0;
    }

    if((AABB(-1, 1) + m_lastPos).isInner(player->getVisualBody())
       && m_lastPosClock.isEsplanedTime(2000))
    {
        do
        {
            m_strikePos = tools::rand(m_playManager->map.aabb);
            m_strikePos.y = 1;
            m_strikePos = m_playManager->parallelscene.newton->findFloor(m_strikePos);
        }
        while(m_strikePos.y < m_playManager->map.aabb.min.y);

        addForce = m_strikePos;
    }
    else
        m_lastPos = playerPos;

    if(player->getCurWeapon()->isEmpty())
        player->switchUpWeapon();

    addForce = m_strikePos;
    addForce = (addForce - playerPos).normalize() * m_worldSettings.playerMoveSpeed;
    addForce.y = 0;

    player->getPhysicBody()->setApplyForce(addForce);
}
