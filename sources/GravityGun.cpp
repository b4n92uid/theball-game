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
}

GravityGun::~GravityGun()
{
}

void GravityGun::process()
{
    if(!m_attached)
        return;

    m_internalEnergy -= 0.1;

    m_owner->setEnergy(roundf(m_internalEnergy));

    Vector3f stay = m_owner->getPhysicBody()->getPos();
    stay += (m_gameManager->getShootTarget() - m_owner->getPhysicBody()->getPos()).normalize() * 2;
    stay.y = m_owner->getPhysicBody()->getPos().y + 0.25;

    Vector3f direction = stay - m_attached->getPhysicBody()->getPos();

    float magnitude = direction.getMagnitude();
    magnitude = (magnitude > 0 ? magnitude : 0.1f);

    direction.normalize();

    Vector3f force = direction * 32 * magnitude;

    // force += m_owner->getPhysicBody()->getApplyForce();

    m_attached->getPhysicBody()->setApplyForce(force);

    if(magnitude < 4)
    {
        Vector3f velocity = m_attached->getPhysicBody()->getVelocity();

        if(velocity > 32)
            velocity.normalize() *= 32;

        float factor = magnitude * 0.1;

        if(factor < 0.8)
            factor = 0.8;

        m_attached->getPhysicBody()->setVelocity(velocity * factor);
    }
}

void GravityGun::activate(tbe::Vector3f target)
{
    if(m_active)
        return;

    Power::activate(target);

    m_internalEnergy = m_owner->getEnergy();

    foreach(MapElement* elem, m_gameManager->map.mapElements)
    {
        if(elem->getPhysicBody() && elem->getPhysicBody()->getMasse() > 0
           && elem->getVisualBody()->getAbsolutAabb().add(2).isInner(target))
        {
            m_attached = elem;
            m_attached->getPhysicBody()->setApplyGravity(false);
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

    m_attached->getPhysicBody()->setApplyGravity(true);

    if(m_internalEnergy > 1)
    {
        Vector3f targetWay = m_gameManager->getShootTarget() - m_attached->getPhysicBody()->getPos();

        if(targetWay > 4)
            NewtonBodyAddImpulse(m_attached->getPhysicBody()->getBody(),
                                 targetWay.normalize() * 64,
                                 m_attached->getPhysicBody()->getPos());
    }

    m_attached = NULL;
}
