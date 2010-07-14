/* 
 * File:   FragModeAi.cpp
 * Author: b4n92uid
 * 
 * Created on 15 juin 2010, 17:23
 */

#include "FragModeAi.h"

#include "PlayManager.h"
#include "Player.h"

using namespace std;
using namespace tbe;

FragModeAi::FragModeAi(PlayManager* playManager) : AIControl(playManager)
{
}

FragModeAi::~FragModeAi()
{
}

void FragModeAi::Process(Player* player)
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

    if(player->IsKilled())
        return;

    Player::Array& players = m_playManager->players;
    Item::Array& items = m_playManager->map.items;
    StaticObject::Array& staticObjects = m_playManager->map.staticObjects;
    DynamicObject::Array& dynamicObjects = m_playManager->map.dynamicObjects;

    Vector3f addForce;
    Vector3f playerPos = player->NewtonNode::GetPos();

    // -------------------------------------------------------------------------

    bool isCollidWithMap = false;

    for(unsigned i = 0; i < staticObjects.size(); i++)
        if(isCollidWithMap = player->IsCollidWith(staticObjects[i]))
            break;

    // -------------------------------------------------------------------------

    m_targetOtp = NULL;

    for(unsigned i = 0; i < dynamicObjects.size(); i++)
    {
        if(dynamicObjects[i]->NewtonNode::GetPos() - playerPos < AI_DYNAMIC_INTERACTION)
            m_targetOtp = dynamicObjects[i];
    }

    for(unsigned i = 0; i < items.size(); i++)
    {
        if(items[i]->IsNeeded(player))
            m_targetOtp = items[i];
    }

    // -------------------------------------------------------------------------

    if(m_targetOtp)
    {
        addForce = m_targetOtp->NewtonNode::GetPos();
    }

    else
    {
        if(m_targetPlayer)
            m_targetPos = m_targetPlayer->NewtonNode::GetPos();

        if(m_switchTarget.IsEsplanedTime(AI_SWITCH_TARGET_TIME)
           || m_strikePos - playerPos < m_minDistToSwith
           || !m_targetPlayer
           || m_targetPlayer->IsKilled()
           )
        {
            Vector3f minDist = m_mapAABB.max - m_mapAABB.min;

            for(unsigned i = 0; i < players.size(); i++)
                if(players[i] != player && !players[i]->IsKilled() && m_mapAABB.IsInner(players[i]))
                {
                    Vector3f testedDist = players[i]->NewtonNode::GetPos() - playerPos;

                    if(testedDist < minDist)
                    {
                        minDist = testedDist;
                        m_targetPlayer = players[i];
                    }
                }

            m_targetPos = m_targetPlayer->NewtonNode::GetPos();

            do
                m_strikePos = tools::rand(m_targetPos - m_strikPrimiter, m_targetPos + m_strikPrimiter);
            while(!m_mapAABB.IsInner(m_strikePos));

            m_strikePos.y = 0;
        }


        addForce = m_strikePos;
    }

    if(m_targetPlayer)
        if(m_targetPos - playerPos < m_minDistToShoot)
            if(player->IsBullettimeMotion() && m_targetPlayer == m_playManager->GetUserPlayer())
            {
                Vector3f velocity;
                NewtonBodyGetVelocity(m_targetPlayer->GetBody(), velocity);

                player->Shoot(m_targetPos - velocity);
            }
            else
            {
                player->Shoot(m_targetPos);
            }

    if(player->GetCurWeapon()->IsEmpty())
        player->SwitchUpWeapon();

    addForce = (addForce - playerPos).Normalize() * m_worldSettings.playerMoveSpeed;
    addForce.y = 0;

    player->SetApplyForce(addForce);
}
