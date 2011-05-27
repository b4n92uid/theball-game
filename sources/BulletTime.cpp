/*
 * File:   BulletTime.cpp
 * Author: b4n92uid
 *
 * Created on 27 juillet 2010, 13:48
 */

#include "BulletTime.h"
#include "Define.h"

#include "SoundManager.h"
#include "GameManager.h"
#include "Player.h"

using namespace tbe;
using namespace tbe::scene;

BulletTime::BulletTime(GameManager* gameManager)
{
    m_active = false;
    m_value = 1;
    m_gameManager = gameManager;
    m_userPlayer = m_gameManager->getUserPlayer();
    m_soundManager = m_gameManager->manager.sound;
}

BulletTime::~BulletTime()
{
}

void BulletTime::process()
{
    NewtonParallelScene* newton = m_gameManager->parallelscene.newton;

    if(m_active)
    {
        if(m_value > 0.0f)
        {
            m_value -= m_gameManager->worldSettings.bullettimeDown;
            newton->setWorldTimestep(newton->getWorldTimestep() / m_gameManager->worldSettings.bullettimeFactor);
        }

        else
            setActive(false);
    }

    else
    {
        if(m_value < 1.0f)
            m_value += m_gameManager->worldSettings.bullettimeUp;
    }
}

void BulletTime::setActive(bool active)
{
    this->m_active = active;

    if(m_value <= 0)
        m_active = false;

    if(m_active)
    {
        FMOD_Channel_SetVolume(m_gameManager->map.musicChannel, 0.5);

        m_soundManager->play("bullettime", m_userPlayer);

        m_gameManager->hudBullettime(true);
    }

    else
    {
        FMOD_Channel_SetVolume(m_gameManager->map.musicChannel, 1.0);

        m_gameManager->hudBullettime(false);
    }
}

bool BulletTime::isActive() const
{
    return m_active;
}

void BulletTime::setValue(float value)
{
    this->m_value = value;
}

float BulletTime::getValue() const
{
    return m_value;
}
