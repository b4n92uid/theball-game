/*
 * File:   BulletTime.cpp
 * Author: b4n92uid
 *
 * Created on 27 juillet 2010, 13:48
 */

#include "BulletTime.h"

#include "Define.h"

#include "GameManager.h"
#include "SoundManager.h"
#include "MapElement.h"
#include "Player.h"
#include "Weapon.h"

using namespace tbe;
using namespace tbe::scene;

ppe::ColorEffect* BulletTime::m_ppeffect = NULL;

BulletTime::BulletTime(GameManager* gameManager) : Power(gameManager)
{
    m_slot = 1;
    m_name = "BulletTime";

    m_usedWeapon = NULL;

    if(!m_ppeffect)
    {
        using namespace ppe;

        m_ppeffect = new ColorEffect;
        m_ppeffect->setRttFrameSize(256);
        m_ppeffect->setFusionMode(ColorEffect::BLACK_WHITE);
        m_ppeffect->setColor(Vector4f(1));
        m_ppeffect->setEnable(false);

        m_gameManager->manager.ppe->addPostEffect("bullettime", m_ppeffect);
    }

    m_gameManager->manager.sound->registerSound("bullettime", m_settings("audio.bullettime"));
}

BulletTime::~BulletTime()
{
    diactivate();
}

void BulletTime::clearSingleTone(GameManager* gm)
{
    gm->manager.ppe->deletePostEffect(m_ppeffect);

    m_ppeffect = NULL;
}

void BulletTime::applyForceAndTorqueCallback(const NewtonBody* body, float, int)
{
    MapElement* elem = static_cast<MapElement*>(NewtonBodyGetUserData(body));

    NewtonNode* nnode = elem->getPhysicBody();

    Vector3f applyForce = nnode->getApplyForce();
    Vector3f applyTorque = nnode->getApplyTorque();
    float masse = nnode->getMasse();

    if(nnode->isApplyGravity())
        applyForce.y -= masse * 9.81 * nnode->getParallelScene()->getGravity() * 0.1;

    applyForce *= 0.1;
    applyTorque *= 0.1;

    NewtonBodySetForce(nnode->getBody(), applyForce);
    NewtonBodySetTorque(nnode->getBody(), applyTorque);

    nnode->setApplyForce(0);
    nnode->setApplyTorque(0);
}

void BulletTime::process()
{
    m_internalEnergy -= 0.5;

    m_owner->setEnergy(m_internalEnergy);

    if(m_owner->getCurWeapon() && m_owner->getCurWeapon() != m_usedWeapon)
    {
        m_usedWeapon->setShootCadency(m_usedWeapon->getShootCadency()*0.1);
        m_usedWeapon = m_owner->getCurWeapon();
        m_usedWeapon->setShootCadency(m_usedWeapon->getShootCadency()*10);
    }

    NewtonWorld* nworld = m_gameManager->parallelscene.newton->getNewtonWorld();
    NewtonBody* body = NewtonWorldGetFirstBody(nworld);

    while(body)
    {
        NewtonApplyForceAndTorque callback = NewtonBodyGetForceAndTorqueCallback(body);

        if(!m_callbacks.count(body) && callback)
        {
            Vector3f vel, omg;
            NewtonBodyGetVelocity(body, vel);
            NewtonBodyGetOmega(body, omg);

            vel *= 0.1;
            omg *= 0.1;
            NewtonBodySetVelocity(body, vel);
            NewtonBodySetOmega(body, omg);

            m_callbacks[body] = callback;
            NewtonBodySetForceAndTorqueCallback(body, applyForceAndTorqueCallback);
        }

        body = NewtonWorldGetNextBody(nworld, body);
    }
}

void BulletTime::soundEffect(FMOD_CHANNEL* channel)
{
    FMOD_Channel_SetFrequency(channel, 22050);
}

void BulletTime::internalActivate(tbe::Vector3f target)
{
    m_internalEnergy = m_owner->getEnergy();

    // FMOD_Channel_SetVolume(m_gameManager->map.musicChannel, 0.5);
    m_soundManager->processSoundEffect.connect(soundEffect);
    m_soundManager->playSound("bullettime", m_owner);

    m_settings.world.playerJumpForce *= 0.1;

    m_ppeffect->setEnable(true);

    m_usedWeapon = m_owner->getCurWeapon();

    if(m_usedWeapon)
        m_usedWeapon->setShootCadency(m_usedWeapon->getShootCadency()*10);

    NewtonWorld* nworld = m_gameManager->parallelscene.newton->getNewtonWorld();
    NewtonBody* body = NewtonWorldGetFirstBody(nworld);

    m_callbacks.clear();

    while(body)
    {
        Vector3f vel, omg;
        NewtonBodyGetVelocity(body, vel);
        NewtonBodyGetOmega(body, omg);

        vel *= 0.1;
        omg *= 0.1;
        NewtonBodySetVelocity(body, vel);
        NewtonBodySetOmega(body, omg);

        NewtonApplyForceAndTorque callback = NewtonBodyGetForceAndTorqueCallback(body);

        if(callback)
        {
            m_callbacks[body] = callback;
            NewtonBodySetForceAndTorqueCallback(body, applyForceAndTorqueCallback);
        }

        body = NewtonWorldGetNextBody(nworld, body);
    }
}

void BulletTime::internalDiactivate()
{
    m_soundManager->processSoundEffect.disconnect(soundEffect);
    // FMOD_Channel_SetVolume(m_gameManager->map.musicChannel, 1.0);

    m_ppeffect->setEnable(false);

    m_settings.world.playerJumpForce *= 10;

    m_usedWeapon->setShootCadency(m_usedWeapon->getShootCadency()*0.1);

    NewtonWorld* nworld = m_gameManager->parallelscene.newton->getNewtonWorld();
    NewtonBody* body = NewtonWorldGetFirstBody(nworld);

    while(body)
    {
        Vector3f vel, omg;
        NewtonBodyGetVelocity(body, vel);
        NewtonBodyGetOmega(body, omg);

        vel *= 10.0;
        omg *= 10.0;
        NewtonBodySetVelocity(body, vel);
        NewtonBodySetOmega(body, omg);

        if(m_callbacks.count(body))
            NewtonBodySetForceAndTorqueCallback(body, m_callbacks[body]);

        body = NewtonWorldGetNextBody(nworld, body);
    }

    m_callbacks.clear();
}
