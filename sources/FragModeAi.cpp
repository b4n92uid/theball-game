/*
 * File:   FragModeAi.cpp
 * Author: b4n92uid
 *
 * Created on 15 juin 2010, 17:23
 */

#include "FragModeAi.h"

#include "GameManager.h"
#include "Player.h"

using namespace std;
using namespace tbe;

FragModeAi::FragModeAi(GameManager* playManager) : AIControl(playManager)
{
    m_gustCount = 0;
}

FragModeAi::~FragModeAi()
{
}

void FragModeAi::process(Player* player)
{
    /*
     IA Process Frag :
     -----------------

     Rechrche une collision avec la map : isCollidWithMap

     Rechrche un item nécéssaire : m_targetItem

     Si m_targetItem
     {
        Si isCollidWithMap
            addForce = m_targetItem.pos
     }

     Sinon
     {
        Mettre a jour m_tagerPos avec m_targetPlayer

        Si AI_SWITCH_TARGET_TIME || m_minDistToSwitch
            Rechreche la cible la plus proche : m_targetPlayer
            Mettre a jour m_tagerPos avec m_targetPlayer
            Spécifier un vecteur d'attack : m_strikePos

        Si m_targetPos - playerPos < m_minDistToShoot
            Tire sur m_targetPos

        Si isCollidWithMap
            addForce = m_strikePos

        Si AI_SWITCH_WEAPON_TIME
            Changement d'arme
     }

     Mouvement vers addForce
     */

    const Player::Array& players = m_playManager->getTargetsOf(player);

    const Item::Array& items = m_playManager->map.items;
    //    const StaticObject::Array& staticObjects = m_playManager->map.staticObjects;
    //    const DynamicObject::Array& dynamicObjects = m_playManager->map.dynamicObjects;

    Vector3f addForce;
    Vector3f playerPos = player->getVisualBody()->getPos();

    // -------------------------------------------------------------------------

    //    bool isCollidWithMap = false;
    //
    //    for(unsigned i = 0; i < staticObjects.size(); i++)
    //        if(isCollidWithMap = player->getPhysicBody()->IsCollidWith(staticObjects[i]->getPhysicBody()))
    //            break;

    // -------------------------------------------------------------------------

    /*
        m_targetOtp = NULL;

        for(unsigned i = 0; i < dynamicObjects.size(); i++)
        {
            if(dynamicObjects[i]->getPos() - playerPos < m_aiParams.dynamicInteraction)
                m_targetOtp = dynamicObjects[i];
        }

        for(unsigned i = 0; i < items.size(); i++)
        {
            if(items[i]->isNeeded(player))
                m_targetOtp = items[i];
        }
     */

    // -------------------------------------------------------------------------

    if(m_targetOtp)
    {
        addForce = m_targetOtp->getVisualBody()->getPos();
    }

    else
    {
        if(m_targetPlayer)
            m_targetPos = m_targetPlayer->getVisualBody()->getPos();

        if(m_switchTarget.isEsplanedTime(m_aiParams.switchTargetTime)
           || m_strikePos - playerPos < m_minDistToSwith
           || !m_targetPlayer
           || m_targetPlayer->isKilled()
           )
        {
            Vector3f minDist = m_mapAABB.max - m_mapAABB.min;

            for(unsigned i = 0; i < players.size(); i++)
                if(!players[i]->isKilled() && m_mapAABB.isInner(players[i]->getVisualBody())
                   && players[i]->isVisibleFromIA())
                {
                    Vector3f testedDist = players[i]->getVisualBody()->getPos() - playerPos;

                    if(testedDist < minDist)
                    {
                        minDist = testedDist;
                        m_targetPlayer = players[i];
                    }
                }

            if(m_targetPlayer)
            {
                m_targetPos = m_targetPlayer->getVisualBody()->getPos();

                do
                    m_strikePos = tools::rand(m_targetPos - m_strikPrimiter, m_targetPos + m_strikPrimiter);
                while(!m_mapAABB.isInner(m_strikePos));

                m_strikePos.y = 0;
            }
        }


        addForce = m_strikePos;
    }

    if(m_targetPlayer && m_targetPos - playerPos < m_minDistToShoot)
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

    addForce = (addForce - playerPos).normalize() * m_worldSettings.playerMoveSpeed;
    addForce.y = 0;

    player->getPhysicBody()->setApplyForce(addForce);
}
