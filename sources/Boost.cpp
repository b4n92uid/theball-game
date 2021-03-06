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
    m_signalid = 0;

    Settings::Video& vidsets = m_gameManager->manager.app->globalSettings.video;

    if(vidsets.ppeUse && !m_ppeffect)
    {
        m_ppeffect = new ppe::MotionBlurEffect;
        m_ppeffect->setEnable(false);
        m_ppeffect->setIntensity(0.75);
        m_gameManager->manager.ppe->addPostEffect("boost", m_ppeffect);
    }

    m_gameManager->manager.sound->registerSound("boost", m_settings("audio.boost"));
}

Boost::~Boost()
{
    diactivate();
}

void Boost::clearSingleTone(GameManager* gm)
{
    gm->manager.ppe->deletePostEffect(m_ppeffect);

    m_ppeffect = NULL;
}

void Boost::process()
{
    int value = m_owner->getEnergy();
    value -= 4;

    m_owner->setEnergy(value);
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

void Boost::internalActivate(tbe::Vector3f target)
{
    if(math::isZero(m_owner->getPhysicBody()->getApplyForce())
       // || !m_owner->isStayDown()
       || !m_cadency.isEsplanedTime(500))
    {
        m_active = false;
        return;
    }

    m_soundManager->playSound("boost", m_owner);

    if(m_ppeffect)
        m_ppeffect->setEnable(true);

    m_signalid = std::time(0);
    // m_owner->onMove.connect(m_signalid, boost::bind(&Boost::boostForce, this, _1, _2));

    NewtonNode* pbody = m_owner->getPhysicBody();

    Vector3f veloc = pbody->getApplyForce().normalize()*64;

    // NewtonBodySetContinuousCollisionMode(pbody->getBody(), true);
    NewtonBodyAddImpulse(pbody->getBody(), veloc, pbody->getPos(), 1.0f/60.0f);
}

void Boost::internalDiactivate()
{
    if(m_ppeffect)
        m_ppeffect->setEnable(false);

    NewtonNode* pbody = m_owner->getPhysicBody();

    // m_owner->onMove.disconnect(m_signalid);
    // NewtonBodySetContinuousCollisionMode(pbody->getBody(), false);

    m_cadency.snapShoot();
}
