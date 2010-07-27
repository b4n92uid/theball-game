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
    m_userPlayer = playManager->GetUserPlayer();
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

    if(m_active)
    {
        m_playManager->manager.sound->Play("bullettime", m_userPlayer);

        m_playManager->hud.background.bullettime->SetEnable(true);
        m_playManager->hud.background.bullettime->SetOpacity(0.5);

        if(m_playManager->manager.app->globalSettings.video.usePpe)
            m_playManager->ppe.bullettime->SetEnable(true);
    }

    else
    {
        m_playManager->hud.background.bullettime->SetEnable(false);

        if(m_playManager->manager.app->globalSettings.video.usePpe)
            m_playManager->ppe.bullettime->SetEnable(false);
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
