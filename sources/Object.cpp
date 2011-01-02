/* 
 * File:   Object.cpp
 * Author: b4n92uid
 * 
 * Created on 7 juin 2010, 23:28
 */

#include "Object.h"
#include "GameManager.h"

Object::Object(GameManager* gameManager) : OBJMesh(gameManager->parallelscene.meshs)
{
    m_gameManager = gameManager;
    m_soundManager = m_gameManager->manager.sound;
    m_worldSettings = m_gameManager->manager.app->globalSettings.physics;

    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->SetUpdatedMatrix(&m_matrix);

    m_physicBody->SetParent(this);
}

Object::~Object()
{
}

void Object::SetPhysicBody(tbe::scene::NewtonNode* physicBody)
{
    this->m_physicBody = physicBody;
}

tbe::scene::NewtonNode* Object::GetPhysicBody() const
{
    return m_physicBody;
}
