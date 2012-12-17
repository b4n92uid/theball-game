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

    Settings::Video& vidsets = m_gameManager->manager.app->globalSettings.video;

    if(vidsets.ppeUse && !m_ppeffect)
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

    if(m_ppeffect)
        m_ppeffect->setEnable(true);

    m_usedWeapon = m_owner->getCurWeapon();
    m_usedWeapon->setShootCadency(m_usedWeapon->getShootCadency()*10);
}

void BulletTime::internalDiactivate()
{
    m_soundManager->processSoundEffect.disconnect(soundEffect);
    // FMOD_Channel_SetVolume(m_gameManager->map.musicChannel, 1.0);

    if(m_ppeffect)
        m_ppeffect->setEnable(false);

    m_usedWeapon->setShootCadency(m_usedWeapon->getShootCadency()*0.1);
}
