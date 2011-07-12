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
#include "StaticElement.h"
#include "Player.h"

using namespace tbe;
using namespace tbe::scene;

GravityGun::GravityGun(GameManager* gameManager) : Power(gameManager)
{
    m_slot = 3;
    m_name = "GravityGun";

    m_attached = NULL;
    m_lastAttached = NULL;

    m_soundManager->registerSound("power.gravitygun.catch", m_settings("audio.gravitygun-catch"));
    m_soundManager->registerSound("power.gravitygun.throw", m_settings("audio.gravitygun-throw"));

    m_highlighter = new ParticlesEmiter(gameManager->parallelscene.particles);
    m_highlighter->setLifeDown(0.8);
    m_highlighter->setNumber(8);
    m_highlighter->setGravity(Vector3f(0, 0.01, 0));
    m_highlighter->setContinousMode(true);
    m_highlighter->setTexture(m_settings("powers.gravitygun"));
}

GravityGun::~GravityGun()
{
    diactivate();
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

    stay.y += m_attached->getVisualBody()->getAabb().getLength() / 2;
    stay.y += m_owner->getVisualBody()->getAabb().getLength() / 2;

    Vector3f direction = stay - attachpbody->getPos();
    Vector3f velocity = attachpbody->getVelocity();

    float magnitude = direction.getMagnitude();

    // Application de la force de mouvement
    Vector3f force = direction.normalize() * attachpbody->getMasse() * 128;
    force -= velocity.normalize() * attachpbody->getMasse() * 64;

    attachpbody->setApplyForce(force);

    if(magnitude < 1)
        attachpbody->setVelocity(velocity * magnitude);
}

void GravityGun::internalActivate(tbe::Vector3f target)
{
    NewtonNode* ownerpbody = m_owner->getPhysicBody();

    if(target - ownerpbody->getPos() > 32)
        return;

    m_internalEnergy = m_owner->getEnergy();

    foreach(StaticElement* elem, m_gameManager->map.staticElements)
    {
        NewtonNode* elempbody = elem->getPhysicBody();

        if(elempbody && elempbody->getMasse() > 0
           && elempbody->getMasse() < 4.1
           && elem->getVisualBody()->getAbsolutAabb().add(2).isInner(target))
        {
            m_attached = elem;

            elempbody->setApplyGravity(false);

            if(elempbody->getPos() - ownerpbody->getPos() > 8)
                NewtonBodyAddImpulse(elempbody->getBody(),
                                     Vector3f::Y(4) * elempbody->getMasse(),
                                     elempbody->getPos());

            if(m_lastAttached)
                NewtonBodySetContinuousCollisionMode(m_lastAttached->getPhysicBody()->getBody(), false);

            NewtonBodySetContinuousCollisionMode(elempbody->getBody(), true);

            m_soundManager->playSound("power.gravitygun.catch", m_attached, -1);

            m_highlighter->setBoxSize(m_attached->getVisualBody()->getAabb().getSize());
            m_highlighter->setPos(-m_attached->getVisualBody()->getAabb().getSize() / 2.0f);
            //            m_highlighter->build();

            m_attached->makeTransparent(true, 0.5);
            m_attached->makeLighted(false);
            m_attached->getVisualBody()->addChild(m_highlighter);

            break;
        }
    }
}

void GravityGun::internalDiactivate()
{
    if(!m_attached)
        return;

    m_attached->getVisualBody()->releaseChild(m_highlighter);

    NewtonNode* attachpbody = m_attached->getPhysicBody();
    attachpbody->setApplyGravity(true);

    m_attached->makeTransparent(false);
    m_attached->makeLighted(true);

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
