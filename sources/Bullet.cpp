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

Bullet::Bullet(GameManager* gameManager) : MapElement(gameManager)
{
    Settings::World& worldSettings = gameManager->manager.app->globalSettings.world;

    m_life = 300;
    m_dammage = 0;
    m_weapon = NULL;
    m_size = worldSettings.weaponSize;
    m_masse = worldSettings.weaponMasse;
}

Bullet::~Bullet()
{
    if(m_physicBody) delete m_physicBody;
    if(m_visualBody) delete m_visualBody;
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

void Bullet::setMasse(float masse)
{
    this->m_masse = masse;
}

float Bullet::getMasse() const
{
    return m_masse;
}

void Bullet::setSize(float size)
{
    this->m_size = size;
}

float Bullet::getSize() const
{
    return m_size;
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

    m_visualBody = new BullNode;
    m_visualBody->setPos(startpos);

    m_physicBody = new NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->setUpdatedMatrix(&m_visualBody->getMatrix());
    m_physicBody->buildSphereNode(m_size, m_masse);
    m_physicBody->setApplyGravity(false);

    NewtonBodySetForceAndTorqueCallback(m_physicBody->getBody(), MapElement::applyForceAndTorqueCallback);
    NewtonBodySetTransformCallback(m_physicBody->getBody(), MapElement::applyTransformCallback);

    NewtonBodySetUserData(m_physicBody->getBody(), this);
    NewtonBodySetContinuousCollisionMode(m_physicBody->getBody(), true);
    NewtonBodyAddImpulse(m_physicBody->getBody(), impulse, m_startPos);

    m_gameManager->manager.material->addBullet(this);
}
