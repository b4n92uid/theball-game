/*
 * File:   MapElement.cpp
 * Author: b4n92uid
 *
 * Created on 26 mai 2011, 22:53
 */

#include "MapElement.h"
#include "GameManager.h"
#include "SoundManager.h"

using namespace tbe;

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

void MapElement::resetInitialMatrix()
{
    if(m_physicBody)
    {
        stopMotion();
        m_physicBody->setMatrix(m_initialMatrix);
    }
    else if(m_visualBody)
        m_visualBody->setMatrix(m_initialMatrix);
}

void MapElement::stopMotion()
{
    m_physicBody->setVelocity(0);
    m_physicBody->setOmega(0);
    m_physicBody->setApplyForce(0);
    m_physicBody->setApplyTorque(0);
}

GameManager* MapElement::getGameManager() const
{
    return m_gameManager;
}

void MapElement::applyForceAndTorqueCallback(const NewtonBody* body, float, int)
{
    MapElement* elem = static_cast<MapElement*>(NewtonBodyGetUserData(body));

    elem->getPhysicBody()->applyForceAndTorque();
}

AreaElement::AreaElement(GameManager* gameManager, std::string id, tbe::Vector3f pos, float radius) : MapElement(gameManager)
{
    m_id = id;

    m_visualBody = new scene::BullNode;
    m_visualBody->setPos(pos);

    m_radius = radius;

    //    m_physicBody = new scene::NewtonNode(gameManager->parallelscene.newton, m_visualBody);
    //    m_physicBody->buildSphereNode(size, 0);
    //    m_physicBody->setPos(pos);
    //    m_physicBody->setParent(m_visualBody);

    //    NewtonBodySetUserData(m_physicBody->getBody(), this);
}

void AreaElement::setRadius(float radius)
{
    this->m_radius = radius;
}

float AreaElement::getRadius() const
{
    return m_radius;
}
