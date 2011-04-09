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
    m_worldSettings = m_gameManager->manager.app->globalSettings.world;

    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->setUpdatedMatrix(&m_matrix);

    m_physicBody->setParent(this);
}

Object::~Object()
{
}

void Object::setPhysicBody(tbe::scene::NewtonNode* physicBody)
{
    this->m_physicBody = physicBody;
}

tbe::scene::NewtonNode* Object::getPhysicBody() const
{
    return m_physicBody;
}
