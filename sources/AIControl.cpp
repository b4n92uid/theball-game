/* 
 * File:   AIManager.cpp
 * Author: b4n92uid
 * 
 * Created on 16 décembre 2009, 18:45
 */

#include "AIControl.h"

#include "PlayManager.h"
#include "Player.h"
#include "Item.h"
#include "BldParser.h"

#include "Define.h"

using namespace tbe;
using namespace tbe::scene;

// AIManager -------------------------------------------------------------------

AIControl::AIControl(PlayManager* playManager) : Controller(playManager)
{
    m_targetPlayer = NULL;
    m_targetOtp = NULL;

    m_mapAABB = m_playManager->map.aabb;

    m_minDistToShoot = (m_mapAABB.max - m_mapAABB.min).GetMagnitude() / 8.0f;
    m_strikPrimiter = m_minDistToShoot;
    m_minDistToSwith = m_strikPrimiter / 8.0f;
}

AIControl::~AIControl()
{
}
