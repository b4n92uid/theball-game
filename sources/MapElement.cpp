/*
 * File:   MapElement.cpp
 * Author: b4n92uid
 *
 * Created on 26 mai 2011, 22:53
 */

#include "MapElement.h"
#include "GameManager.h"
#include "SoundManager.h"

MapElement::MapElement(GameManager* gameManager)
{
    m_gameManager = gameManager;
    m_soundManager = m_gameManager->manager.sound;
    m_worldSettings = m_gameManager->manager.app->globalSettings.world;

    m_physicBody = NULL;
    m_visualBody = NULL;
}

MapElement::~MapElement()
{
}

void MapElement::setVisualBody(tbe::scene::Node* visualBody)
{
    this->m_visualBody = visualBody;
}

tbe::scene::Node* MapElement::getVisualBody() const
{
    return m_visualBody;
}

void MapElement::setPhysicBody(tbe::scene::NewtonNode* physicBody)
{
    this->m_physicBody = physicBody;
}

tbe::scene::NewtonNode* MapElement::getPhysicBody() const
{
    return m_physicBody;
}

void MapElement::setId(std::string id)
{
    this->m_id = id;
}

std::string MapElement::getId() const
{
    return m_id;
}

bool MapElement::isCollidWithStaticWorld(tbe::scene::NewtonNode* body, Array staticObjects)
{
    for(unsigned i = 0; i < staticObjects.size(); i++)
        if(body->isCollidWith(staticObjects[i]->getPhysicBody()))
            return true;

    return false;
}

void MapElement::applyForceAndTorqueCallback(const NewtonBody* body, float, int)
{
    MapElement* elem = static_cast<MapElement*>(NewtonBodyGetUserData(body));

    elem->getPhysicBody()->applyForceAndTorque();
}
