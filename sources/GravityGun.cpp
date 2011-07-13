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

using namespace std;
using namespace tbe;
using namespace scene;

GravityGun::GravityGun(GameManager* gameManager) : Power(gameManager)
{
    m_slot = 3;
    m_name = "GravityGun";

    m_attached = NULL;
    m_lastAttached = NULL;

    m_soundManager->registerSound("power.gravitygun.catch", m_settings("audio.gravitygun-catch"));
    m_soundManager->registerSound("power.gravitygun.throw", m_settings("audio.gravitygun-throw"));
}

GravityGun::~GravityGun()
{
    diactivate();
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

    float length = m_owner->getVisualBody()->getAabb().getLength() / 2
            + m_attached->getVisualBody()->getAabb().getLength() / 2;

    stay += Quaternion(Vector3f(0, -M_PI_2, 0)) * m_gameManager->getViewDirection() * length;

    stay.y += length;

    Vector3f direction = stay - attachpbody->getPos();
    Vector3f velocity = attachpbody->getVelocity();

    float magnitude = direction.getMagnitude();

    /*
     * Ralentit l'objet pour eviter une trop grande vittese
     * quand celui ci est pret du joueur
     */
    if(magnitude < 2)
        attachpbody->setVelocity(velocity * (magnitude / 2));

    // Calcule de la force de mouvement
    Vector3f force = direction.normalize() * attachpbody->getMasse() * 128;

    // Calcule de la contre force
    force -= velocity.normalize() * attachpbody->getMasse() * 64;

    /*
     * Pour eviter le teremblement de l'objet attacher
     * qaund la distance cible est tres petite
     */
    if(magnitude < 0.2)
        force *= magnitude;

    attachpbody->setApplyForce(force);
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

            m_attached->makeTransparent(true, 0.5);
            m_attached->makeLighted(false);

            break;
        }
    }
}

void GravityGun::internalDiactivate()
{
    if(!m_attached)
        return;

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
