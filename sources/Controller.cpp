/* 
 * File:   Controller.cpp
 * Author: b4n92uid
 * 
 * Created on 15 juin 2010, 14:54
 */

#include "Controller.h"
#include "PlayManager.h"
#include "Player.h"

Controller::Controller(PlayManager* playManager)
{
    m_playManager = playManager;
    m_worldSettings = playManager->manager.app->globalSettings.physics;
}

Controller::~Controller()
{
}
