/*
 * File:   Controller.cpp
 * Author: b4n92uid
 *
 * Created on 15 juin 2010, 14:54
 */

#include "Controller.h"

#include "GameManager.h"
#include "Player.h"

Controller::Controller(GameManager* playManager) :
m_worldSettings(playManager->manager.app->globalSettings.world)
{
    m_playManager = playManager;
}

Controller::~Controller()
{
}
