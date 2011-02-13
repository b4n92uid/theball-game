/* 
 * File:   TeamModeAi.cpp
 * Author: b4n92uid
 * 
 * Created on 15 juin 2010, 17:25
 */

#include "TeamModeAi.h"

#include "PlayManager.h"
#include "Player.h"

using namespace std;
using namespace tbe;

TeamModeAi::TeamModeAi(PlayTeamManager* playerManager) : AIControl(playerManager)
{
    m_playManager = playerManager;
}

TeamModeAi::~TeamModeAi()
{
}

void TeamModeAi::Process(Player* player)
{
    const Player::Array players = m_playManager->GetTargetsOf(player);
    const Item::Array& items = m_playManager->map.items;
    const DynamicObject::Array& dynamicObjects = m_playManager->map.dynamicObjects;

    Vector3f addForce;
    Vector3f playerPos = player->GetPos();

    // Test de collision avec la map

    //    const StaticObject::Array& staticObjects = m_playManager->map.staticObjects;
    //
    //    bool isCollidWithMap = false;
    //
    //    for(unsigned i = 0; i < staticObjects.size(); i++)
    //        if(isCollidWithMap = player->GetPhysicBody()->IsCollidWith(staticObjects[i]->GetPhysicBody()))
    //            break;

    // Recherche d'item

    m_targetOtp = NULL;

    for(unsigned i = 0; i < dynamicObjects.size(); i++)
    {
        if(dynamicObjects[i]->GetPos() - playerPos < m_aiParams.dynamicInteraction)
            m_targetOtp = dynamicObjects[i];
    }

    for(unsigned i = 0; i < items.size(); i++)
    {
        if(items[i]->IsNeeded(player))
        {
            m_targetOtp = items[i];
            break;
        }
    }

    // Priorité aux items
    if(m_targetOtp)
    {
        addForce = m_targetOtp->GetPos();
    }

    else
    {
        // Mete a jour la position de la cible
        if(m_targetPlayer)
            m_targetPos = m_targetPlayer->GetPos();

        // Recherche d'une nouvelle cible
        // Si Temps écouler
        // || Trop pres
        // || Pas de cible
        // || La cible est mort
        if(m_switchTarget.IsEsplanedTime(m_aiParams.switchTargetTime)
           || m_strikePos - playerPos < m_minDistToSwith
           || !m_targetPlayer
           || m_targetPlayer->IsKilled())
        {
            Vector3f minDist = m_mapAABB.max - m_mapAABB.min;

            for(unsigned i = 0; i < players.size(); i++)

                if(!players[i]->IsKilled() && m_mapAABB.IsInner(players[i])
                   && players[i]->IsVisibleFromIA())
                {
                    Vector3f testedDist = players[i]->GetPos() - playerPos;

                    if(testedDist < minDist)
                    {
                        minDist = testedDist;
                        m_targetPlayer = players[i];
                    }
                }

            if(m_targetPlayer)
            {
                m_targetPos = m_targetPlayer->GetPos();

                // Recherche d'une position d'attack
                do
                    m_strikePos = tools::rand(m_targetPos - m_strikPrimiter,
                                              m_targetPos + m_strikPrimiter);
                while(!m_mapAABB.IsInner(m_strikePos));

                m_strikePos.y = 0;
            }
        }

        addForce = m_strikePos;
    }

    // Routine d'attack
    if(m_targetPlayer && m_targetPos - playerPos < m_minDistToShoot)
    {
        Vector3f targetPos = m_targetPos;

        if(m_targetPlayer == m_playManager->GetUserPlayer()
           && m_playManager->GetBullettime()->IsActive())
        {
            Vector3f velocity;
            NewtonBodyGetVelocity(m_targetPlayer->GetPhysicBody()->GetBody(), velocity);

            targetPos = m_targetPos - velocity;
        }

        if(m_gustCount < m_aiParams.shootGustCount)
        {
            if(player->Shoot(m_targetPos))
                m_gustCount++;
        }
        else if(m_gustClock.IsEsplanedTime(m_aiParams.shootGustTime))
            m_gustCount = 0;
    }

    if((AABB(-1, 1) + m_lastPos).IsInner(player)
       && m_lastPosClock.IsEsplanedTime(2000))
    {
        do
        {
            m_strikePos = tools::rand(m_playManager->map.aabb);
            m_strikePos.y = 1;
            m_strikePos = m_playManager->parallelscene.newton->FindFloor(m_strikePos);
        }
        while(m_strikePos.y < m_playManager->map.aabb.min.y);

        addForce = m_strikePos;
    }
    else
        m_lastPos = playerPos;

    // Changement d'arme
    if(player->GetCurWeapon()->IsEmpty())
        player->SwitchUpWeapon();

    // Mouvement
    addForce = (addForce - playerPos).Normalize() * m_worldSettings.playerMoveSpeed;
    addForce.y = 0;

    player->GetPhysicBody()->SetApplyForce(addForce);
}
