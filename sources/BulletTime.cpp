/*
 * File:   BulletTime.cpp
 * Author: b4n92uid
 *
 * Created on 27 juillet 2010, 13:48
 */

#include "BulletTime.h"

#include "Define.h"

#include "Player.h"
#include "GameManager.h"
#include "SoundManager.h"

using namespace tbe;
using namespace tbe::scene;

BulletTime::BulletTime(GameManager* gameManager) : Power(gameManager)
{
    m_name = "BulletTime";

    m_ppeffect = NULL;

    if(m_gameManager->manager.app->globalSettings.video.usePpe)
    {
        using namespace ppe;

        m_ppeffect = new ColorEffect;
        m_ppeffect->setInternalPass(true);
        m_ppeffect->setRttFrameSize(256);
        m_ppeffect->setFusionMode(ColorEffect::BLACK_WHITE);
        m_ppeffect->setColor(Vector4f(1));
        m_ppeffect->setEnable(false);

        m_gameManager->manager.ppe->addPostEffect("blettimeEffect", m_ppeffect);
    }
}

BulletTime::~BulletTime()
{
}

void BulletTime::process()
{
    int value = m_owner->getEnergy();

    if(m_active)
    {
        if(value > 0)
        {
            NewtonWorld* nworld = m_gameManager->parallelscene.newton->getNewtonWorld();

            NewtonBody* body = NewtonWorldGetFirstBody(nworld);

            while(body)
            {
                Vector3f force;
                NewtonBodyGetForce(body, force);
                force *= 0.5;

                NewtonBodySetForce(body, force);

                body = NewtonWorldGetNextBody(nworld, body);
            }

            value -= 2;
        }

        else
            m_active = false;
    }

    m_owner->setEnergy(value);
}

void BulletTime::activate(tbe::Vector3f target)
{
    Power::activate(target);

    // FMOD_Channel_SetVolume(m_gameManager->map.musicChannel, 0.5);

    m_soundManager->playSound("bullettime", m_owner);

    if(m_ppeffect)
        m_ppeffect->setEnable(true);

    NewtonWorld* nworld = m_gameManager->parallelscene.newton->getNewtonWorld();

    NewtonBody* body = NewtonWorldGetFirstBody(nworld);

    while(body)
    {
        Vector3f vel;
        NewtonBodyGetVelocity(body, vel);

        vel *= 0.5;
        NewtonBodySetVelocity(body, vel);

        body = NewtonWorldGetNextBody(nworld, body);
    }
}

void BulletTime::diactivate()
{
    Power::diactivate();

    // FMOD_Channel_SetVolume(m_gameManager->map.musicChannel, 1.0);

    if(m_ppeffect)
        m_ppeffect->setEnable(false);

    NewtonWorld* nworld = m_gameManager->parallelscene.newton->getNewtonWorld();

    NewtonBody* body = NewtonWorldGetFirstBody(nworld);

    while(body)
    {
        Vector3f vel;
        NewtonBodyGetVelocity(body, vel);

        vel *= 2.0;
        NewtonBodySetVelocity(body, vel);

        body = NewtonWorldGetNextBody(nworld, body);
    }
}
