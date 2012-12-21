/*
 * File:   Bullet.cpp
 * Author: b4n92uid
 *
 * Created on 28 juin 2011, 17:27
 */

#include "Bullet.h"

#include "GameManager.h"
#include "Player.h"
#include "Weapon.h"
#include "MaterialManager.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

Bullet::Bullet(GameManager* playManager) : MapElement(playManager)
{
    m_life = 300;
    m_dammage = 0;
    m_weapon = NULL;
}

Bullet::~Bullet()
{
    delete m_physicBody;
    delete m_visualBody;
}

void Bullet::process()
{
    m_physicBody->process();
    m_visualBody->process();
}

void Bullet::setShootSpeed(float shootSpeed)
{
    this->m_shootSpeed = shootSpeed;
}

float Bullet::getShootSpeed() const
{
    return m_shootSpeed;
}

void Bullet::setDammage(int dammage)
{
    this->m_dammage = dammage;
}

int Bullet::getDammage() const
{
    return m_dammage;
}

void Bullet::setShootDiri(tbe::Vector3f shootDiri)
{
    this->m_shootDiri = shootDiri;
}

tbe::Vector3f Bullet::getShootDiri() const
{
    return m_shootDiri;
}

void Bullet::setTargetPos(tbe::Vector3f targetPos)
{
    this->m_targetPos = targetPos;
}

tbe::Vector3f Bullet::getTargetPos() const
{
    return m_targetPos;
}

void Bullet::setStartPos(tbe::Vector3f startPos)
{
    this->m_startPos = startPos;
}

tbe::Vector3f Bullet::getStartPos() const
{
    return m_startPos;
}

void Bullet::setWeapon(Weapon* weapon)
{
    this->m_weapon = weapon;
}

Weapon* Bullet::getWeapon() const
{
    return m_weapon;
}

void Bullet::setLife(int life)
{
    this->m_life = life;
}

int Bullet::getLife() const
{
    return m_life;
}

bool Bullet::isDeadAmmo()
{
    return(m_life <= 0 || !m_gameManager->map.aabb.isInner(m_visualBody->getPos()));
}

void Bullet::shoot(tbe::Vector3f startpos, tbe::Vector3f targetpos, float shootspeed, float accuracy)
{

    m_shootDiri = (targetpos - startpos).normalize();
    m_shootDiri += AABB(accuracy).randPos();
    m_shootDiri.normalize();

    m_startPos = startpos + m_shootDiri * 1;

    m_targetPos = targetpos;

    m_shootSpeed = shootspeed;

    Vector3f impulse = m_shootDiri * m_shootSpeed;

    BullNode* placehold = new BullNode;
    placehold->setPos(startpos);

    setVisualBody(placehold);

    Settings::World& worldSettings = m_gameManager->manager.app->globalSettings.world;

    NewtonNode* body = new NewtonNode(m_gameManager->parallelscene.newton);
    body->setUpdatedMatrix(&placehold->getMatrix());
    body->buildSphereNode(worldSettings.weaponSize, worldSettings.weaponMasse);
    body->setApplyGravity(false);

    NewtonBodySetForceAndTorqueCallback(body->getBody(), MapElement::applyForceAndTorqueCallback);
    NewtonBodySetTransformCallback(body->getBody(), MapElement::applyTransformCallback);

    NewtonBodySetUserData(body->getBody(), this);
    NewtonBodySetContinuousCollisionMode(body->getBody(), true);
    NewtonBodyAddImpulse(body->getBody(), impulse, m_startPos);

    setPhysicBody(body);

    m_gameManager->manager.material->addBullet(this);
}
