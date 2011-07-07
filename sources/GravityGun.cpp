/*
 * File:   GravityGun.cpp
 * Author: b4n92uid
 *
 * Created on 6 juillet 2011, 18:55
 */

#include "GravityGun.h"

#include "Define.h"

#include "GameManager.h"
#include "SoundManager.h"
#include "MapElement.h"
#include "Player.h"

using namespace tbe;
using namespace tbe::scene;

GravityGun::GravityGun(GameManager* gameManager) : Power(gameManager)
{
    m_slot = 3;
    m_name = "GravityGun";

    m_attached = NULL;
    m_lastAttached = NULL;

    m_soundManager->registerSound("power.gravitygun.catch", "data/sfxart/powers/gravitygun-catch.wav");
    m_soundManager->registerSound("power.gravitygun.throw", "data/sfxart/powers/gravitygun-throw.wav");

    m_highlighter = new ParticlesEmiter(gameManager->parallelscene.particles);
    m_highlighter->setFreeMove(0.5);
    m_highlighter->setLifeDown(0.5);
    m_highlighter->setNumber(64);
    m_highlighter->setContinousMode(true);
    m_highlighter->setTexture("data/gfxart/particles/gravitygun.bmp");
}

GravityGun::~GravityGun()
{
    if(!m_highlighter->isAttached())
        delete m_highlighter;
}

void GravityGun::process()
{
    if(!m_attached)
        return;

    m_internalEnergy -= 0.1;

    m_owner->setEnergy(roundf(m_internalEnergy));

    NewtonNode* ownerpbody = m_owner->getPhysicBody();
    NewtonNode* attachpbody = m_attached->getPhysicBody();

    Vector3f stay = ownerpbody->getPos();
    //    stay += (m_gameManager->getShootTarget() - ownerpbody->getPos())
    //            .normalize() * m_attached->getVisualBody()->getAabb().getLength();
    stay.y += m_attached->getVisualBody()->getAabb().getLength();

    Vector3f direction = stay - attachpbody->getPos();

    Vector3f velocity = attachpbody->getVelocity();

    float magnitude = direction.getMagnitude();

    direction.normalize();

    if(magnitude < 4)
    {
        float factor = magnitude / 4;
        attachpbody->setVelocity(velocity * factor);
    }

    // Application de la force de mouvement

    Vector3f force = direction * 16 * magnitude;
    force += ownerpbody->getApplyForce();
    attachpbody->setApplyForce(force);
}

void GravityGun::activate(tbe::Vector3f target)
{
    if(m_active)
        return;

    NewtonNode* ownerpbody = m_owner->getPhysicBody();

    if(target - ownerpbody->getPos() > 32)
        return;

    Power::activate(target);

    m_internalEnergy = m_owner->getEnergy();

    foreach(MapElement* elem, m_gameManager->map.mapElements)
    {
        NewtonNode* elempbody = elem->getPhysicBody();

        if(elempbody && elempbody->getMasse() > 0
           && elempbody->getMasse() < 4.1
           && elem->getVisualBody()->getAbsolutAabb().add(2).isInner(target))
        {
            m_attached = elem;

            elempbody->setApplyGravity(false);

            NewtonBodyAddImpulse(elempbody->getBody(), Vector3f::Y(4), elempbody->getPos());

            if(m_lastAttached)
                NewtonBodySetContinuousCollisionMode(m_lastAttached->getPhysicBody()->getBody(), false);

            NewtonBodySetContinuousCollisionMode(elempbody->getBody(), true);

            m_soundManager->playSound("power.gravitygun.catch", m_attached, -1);

            m_highlighter->setBoxSize(m_attached->getVisualBody()->getAabb().getSize());
            m_highlighter->setPos(-m_attached->getVisualBody()->getAabb().getSize() / 2.0f);
            m_highlighter->build();

            m_attached->getVisualBody()->addChild(m_highlighter);

            break;
        }
    }
}

void GravityGun::diactivate()
{
    if(!m_active)
        return;

    Power::diactivate();

    if(!m_attached)
        return;

    NewtonNode* attachpbody = m_attached->getPhysicBody();

    attachpbody->setApplyGravity(true);

    m_attached->getVisualBody()->releaseChild(m_highlighter);

    if(m_internalEnergy > 1)
    {
        Vector3f targetWay = m_gameManager->getShootTarget() - attachpbody->getPos();

        if(targetWay > 4)
        {
            NewtonBodyAddImpulse(attachpbody->getBody(), targetWay.normalize() * 128, attachpbody->getPos());

            m_soundManager->playSound("power.gravitygun.throw", m_attached);
        }
    }

    m_soundManager->stopSound("power.gravitygun.catch");

    m_lastAttached = m_attached;
    m_attached = NULL;
}
