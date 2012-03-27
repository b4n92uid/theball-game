/*
 * File:   AIManager.cpp
 * Author: b4n92uid
 *
 * Created on 16 décembre 2009, 18:45
 */

#include "AIControl.h"

#include "Define.h"
#include "MapElement.h"
#include "Player.h"
#include "GameManager.h"

using namespace tbe;
using namespace tbe::scene;

// AIManager -------------------------------------------------------------------

AIControl::AIControl(GameManager* playManager) : Controller(playManager)
{
}

AIControl::~AIControl()
{
}

void AIControl::process(Player* player)
{
    onAi(player);
}
