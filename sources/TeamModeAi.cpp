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
    if(player->IsKilled())
        return;

    Player::Array& players = find(m_playManager->blueTeamPlayers.begin(),
                                  m_playManager->blueTeamPlayers.end(),
                                  player) == m_playManager->blueTeamPlayers.end()
            ? m_playManager->blueTeamPlayers
            : m_playManager->redTeamPlayers;

    Item::Array& items = m_playManager->map.items;

    StaticObject::Array& staticObjects = m_playManager->map.staticObjects;
    DynamicObject::Array& dynamicObjects = m_playManager->map.dynamicObjects;

    Vector3f addForce;
    Vector3f playerPos = player->NewtonNode::GetPos();

    // Test de collision avec la map

    bool isCollidWithMap = false;

    for(unsigned i = 0; i < staticObjects.size(); i++)
        if(isCollidWithMap = player->IsCollidWith(staticObjects[i]))
            break;

    // Recherche d'item

    m_targetOtp = NULL;

    for(unsigned i = 0; i < dynamicObjects.size(); i++)
    {
        if(dynamicObjects[i]->NewtonNode::GetPos() - playerPos < AI_DYNAMIC_INTERACTION)
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
        addForce = m_targetOtp->NewtonNode::GetPos();
    }

    else
    {
        // Mete a jour la position de la cible
        if(m_targetPlayer)
            m_targetPos = m_targetPlayer->NewtonNode::GetPos();

        // Recherche d'une nouvelle cible
        // Si Temps écouler
        // || Trop pres
        // || Pas de cible
        // || La cible est mort
        if(m_switchTarget.IsEsplanedTime(AI_SWITCH_TARGET_TIME)
           || m_strikePos - playerPos < m_minDistToSwith
           || !m_targetPlayer
           || m_targetPlayer->IsKilled())
        {
            Vector3f minDist = m_mapAABB.max - m_mapAABB.min;

            for(unsigned i = 0; i < players.size(); i++)
                if(players[i] != player && !players[i]->IsKilled()
                   && m_mapAABB.IsInner(players[i]))
                {
                    Vector3f testedDist = players[i]->NewtonNode::GetPos() - playerPos;

                    if(testedDist < minDist)
                    {
                        minDist = testedDist;
                        m_targetPlayer = players[i];
                    }
                }

            m_targetPos = m_targetPlayer->NewtonNode::GetPos();

            // Recherche d'une position d'attack
            do
                m_strikePos = tools::rand(m_targetPos - m_strikPrimiter,
                                          m_targetPos + m_strikPrimiter);
            while(!m_mapAABB.IsInner(m_strikePos));

            m_strikePos.y = 0;
        }

        addForce = m_strikePos;
    }

    // Routine d'attack
    if(m_targetPlayer)
        if(m_targetPos - playerPos < m_minDistToShoot)
            if(m_targetPlayer == m_playManager->GetUserPlayer() && m_playManager->GetBullettime()->IsActive())
            {
                Vector3f velocity;
                NewtonBodyGetVelocity(m_targetPlayer->GetBody(), velocity);

                player->Shoot(m_targetPos - velocity);
            }
            else
            {
                player->Shoot(m_targetPos);
            }

    // Changement d'arme
    if(player->GetCurWeapon()->IsEmpty())
        player->SwitchUpWeapon();

    // Mouvement
    addForce = (addForce - playerPos).Normalize() * m_worldSettings.playerMoveSpeed;
    addForce.y = 0;

    player->SetApplyForce(addForce);
}
