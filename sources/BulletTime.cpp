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
#include "PlayManager.h"
#include "Player.h"

using namespace tbe;
using namespace tbe::scene;

BulletTime::BulletTime(PlayManager* playManager)
{
    m_active = false;
    m_value = 1;
    m_playManager = playManager;
    m_userPlayer = m_playManager->GetUserPlayer();
    m_soundManager = m_playManager->manager.sound;
}

BulletTime::~BulletTime()
{
}

void BulletTime::Process()
{
    NewtonParallelScene* newton = m_playManager->parallelscene.newton;

    if(m_active)
    {
        if(m_value > 0.0f)
        {
            m_value -= 0.006;
            newton->SetWorldTimestep(newton->GetWorldTimestep() / 512.0f);
        }

        else
            SetActive(false);
    }

    else
    {
        if(m_value < 1.0f)
            m_value += 0.004;
    }
}

void BulletTime::SetActive(bool active)
{
    this->m_active = active;

    if(m_value <= 0)
        m_active = false;

    if(m_active)
    {
        FMOD_Channel_SetVolume(m_playManager->map.musicChannel, 0.5);

        m_soundManager->Play("bullettime", m_userPlayer);

        m_playManager->HudBullettime(true);
    }

    else
    {
        FMOD_Channel_SetVolume(m_playManager->map.musicChannel, 1.0);

        m_playManager->HudBullettime(false);
    }
}

bool BulletTime::IsActive() const
{
    return m_active;
}

void BulletTime::SetValue(float value)
{
    this->m_value = value;
}

float BulletTime::GetValue() const
{
    return m_value;
}
