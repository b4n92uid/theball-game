/* 
 * File:   Object.cpp
 * Author: b4n92uid
 * 
 * Created on 7 juin 2010, 23:28
 */

#include "Object.h"
#include "GameManager.h"

Object::Object(GameManager* gameManager)
{
    m_gameManager = gameManager;
    m_soundManager = gameManager->manager.sound;
    m_worldSettings = gameManager->manager.app->globalSettings.physics;

    SetNewtonWorld(m_gameManager->parallelscene.newton->GetNewtonWorld());
    SetUpdatedMatrix(&m_matrix);
}

Object::~Object()
{
}
