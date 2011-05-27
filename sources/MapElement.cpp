/*
 * File:   MapElement.cpp
 * Author: b4n92uid
 *
 * Created on 26 mai 2011, 22:53
 */

#include "MapElement.h"
#include "GameManager.h"
#include "SoundManager.h"

#include <lua.h>

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
