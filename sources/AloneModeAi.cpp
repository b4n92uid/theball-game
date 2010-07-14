/* 
 * File:   AloneModeAi.cpp
 * Author: b4n92uid
 * 
 * Created on 15 juin 2010, 17:25
 */

#include "AloneModeAi.h"

#include "PlayManager.h"
#include "Player.h"

using namespace std;
using namespace tbe;

AloneModeAi::AloneModeAi(PlayManager* playManager) : AIControl(playManager)
{
}

AloneModeAi::~AloneModeAi()
{
}

void AloneModeAi::Process(Player* player)
{
    /*
     IA Process Alone :
     ------------------

     Rechrche une collision avec la map : isCollidWithMap

     Rechrche un item nÚcÚssaire : m_targetItem

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

    if(player->IsKilled())
        return;

    if(!m_targetPlayer)
        m_targetPlayer = m_playManager->GetUserPlayer();

    StaticObject::Array& staticObjects = m_playManager->map.staticObjects;

    Vector3f addForce;
    Vector3f playerPos = player->NewtonNode::GetPos();

    // -------------------------------------------------------------------------

    bool isCollidWithMap = false;

    for(unsigned i = 0; i < staticObjects.size(); i++)
        if(isCollidWithMap = player->IsCollidWith(staticObjects[i]))
            break;

    // -------------------------------------------------------------------------

    m_targetPos = m_targetPlayer->NewtonNode::GetPos();

    if(m_switchTarget.IsEsplanedTime(AI_SWITCH_TARGET_TIME)
       || m_strikePos - playerPos < m_minDistToSwith)
    {
        do
            m_strikePos = tools::rand(m_targetPos - m_strikPrimiter, m_targetPos + m_strikPrimiter);
        while(!m_mapAABB.IsInner(m_strikePos));

        m_strikePos.y = 0;
    }

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

    addForce = m_strikePos;
    addForce = (addForce - playerPos).Normalize() * m_worldSettings.playerMoveSpeed;
    addForce.y = 0;

    player->SetApplyForce(addForce);
}
