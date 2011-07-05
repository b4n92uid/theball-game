/*
 * File:   Boost.cpp
 * Author: b4n92uid
 *
 * Created on 5 juillet 2011, 21:52
 */

#include "Boost.h"

#include "Define.h"

#include "GameManager.h"
#include "SoundManager.h"
#include "MapElement.h"
#include "Player.h"

#include <boost/bind.hpp>

using namespace tbe;
using namespace tbe::scene;

ppe::MotionBlurEffect* Boost::m_ppeffect = NULL;

Boost::Boost(GameManager* gameManager) : Power(gameManager)
{
    m_slot = 2;
    m_name = "Boost";

    Settings::Video& vidsets = m_gameManager->manager.app->globalSettings.video;

    if(vidsets.ppeUse && !m_ppeffect)
    {
        m_ppeffect = new ppe::MotionBlurEffect;
        m_ppeffect->setEnable(false);
        m_ppeffect->setIntensity(0.75);
        m_gameManager->manager.ppe->addPostEffect("boost", m_ppeffect);
    }

    m_gameManager->manager.sound->registerSound("boost", "data/sfxart/boost.wav");
}

Boost::~Boost()
{
}

void Boost::process()
{
    if(m_active)
    {
        int value = m_owner->getEnergy();

        if(value > 0)
            value -= 4;

        else
            m_active = false;

        m_owner->setEnergy(value);
    }

}

bool Boost::boostForce(Player* player, tbe::Vector3f direction)
{
    Vector3f moveforce = m_gameManager->manager.app->globalSettings.world.playerMoveSpeed;
    moveforce = direction * moveforce * 4;

    if(moveforce > 2048)
        moveforce.normalize() *= 2048;

    moveforce.y = 0;

    player->getPhysicBody()->setApplyForce(moveforce);

    return false;
}

void Boost::activate(tbe::Vector3f target)
{
    if(m_active)
        return;

    Power::activate(target);

    m_soundManager->playSound("boost", m_owner);

    if(m_ppeffect)
        m_ppeffect->setEnable(true);

    m_owner->onMove.connect(boost::bind(&Boost::boostForce, this, _1, _2));

    NewtonNode* pbody = m_owner->getPhysicBody();

    Vector3f veloc = pbody->getApplyForce().normalize()*64;

    NewtonBodyAddImpulse(pbody->getBody(), veloc, pbody->getPos());
}

void Boost::diactivate()
{
    if(!m_active)
        return;

    Power::diactivate();

    if(m_ppeffect)
        m_ppeffect->setEnable(false);

    m_owner->onMove.disconnect(boost::bind(&Boost::boostForce, this, _1, _2));
}
