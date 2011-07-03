/*
 * File:   BulletWeapon.cpp
 * Author: b4n92uid
 *
 * Created on 3 juillet 2011, 21:49
 */

#include "BulletWeapon.h"

#include "Weapon.h"

#include "GameManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"

#include "MapElement.h"
#include "Player.h"
#include "Item.h"
#include "Bullet.h"

#include "Define.h"

#include <fmod_errors.h>

typedef boost::filesystem::path fspath;

using namespace std;
using namespace tbe;
using namespace tbe::scene;

BulletWeapon::BulletWeapon(GameManager* gameManager) :
Weapon(gameManager),
ParticlesEmiter(gameManager->parallelscene.particles)
{
    m_shootSize = 1;
    m_shootSpeed = 0;

    setBulletSize(0.75);

    m_playManager->manager.scene->getRootNode()->addChild(this);
}

BulletWeapon::~BulletWeapon()
{
    for(unsigned i = 0; i < m_bulletArray.size(); i++)
        delete m_bulletArray[i];
}

void BulletWeapon::setShootSize(unsigned shootSize)
{
    this->m_shootSize = shootSize;
}

unsigned BulletWeapon::getShootSize() const
{
    return m_shootSize;
}

void BulletWeapon::setShootSpeed(float shootSpeed)
{
    this->m_shootSpeed = shootSpeed;
}

float BulletWeapon::getShootSpeed() const
{
    return m_shootSpeed;
}

void BulletWeapon::setupBullet(Particle& p)
{
    p = Particle();
    p.life = 1;
    p.color = 1;
}

void BulletWeapon::process()
{
    unsigned requsetSize = m_ammoCount * m_shootSize + m_bulletArray.size();

    if(requsetSize > m_number)
    {
        setNumber(requsetSize);
        build();
    }

    Particle* particles = beginParticlesPosProcess();

    for(unsigned i = 0; i < m_bulletArray.size(); i++)
    {
        if(m_bulletArray[i]->isDeadAmmo())
        {
            delete m_bulletArray[i], m_bulletArray[i] = NULL;
        }

        else
        {
            m_bulletArray[i]->process();
            particles[i].pos = m_bulletArray[i]->getPhysicBody()->getPos();
        }
    }

    endParticlesPosProcess();

    Bullet::Array::iterator newend = remove(m_bulletArray.begin(), m_bulletArray.end(), (Bullet*)NULL);
    m_bulletArray.erase(newend, m_bulletArray.end());

    setDrawNumber(m_bulletArray.size());
}

// WeaponBlaster ---------------------------------------------------------------

WeaponBlaster::WeaponBlaster(GameManager* playManager) : BulletWeapon(playManager)
{
    setMaxAmmoCount(200);
    setAmmoCount(180);
    setMaxAmmoDammage(10);
    setShootCadency(64);
    setShootSpeed(64);
    setFireSound(SOUND_BLASTER);

    setTexture(WEAPON_BLASTER);
    setNumber(200);
    build();

    setWeaponName("Blaster");

    m_slot = 1;
}

void WeaponBlaster::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    startpos += math::rand(Vector3f(-m_worldSettings.playerSize * 0.5),
                           Vector3f(m_worldSettings.playerSize * 0.5));

    // Creation du tire
    Bullet* fire = new Bullet(m_playManager);
    fire->setWeapon(this);
    fire->setDammage(math::rand(1, m_maxAmmoDammage));
    fire->shoot(startpos, targetpos, m_shootSpeed);

    m_bulletArray.push_back(fire);
}

// WeaponShotgun ---------------------------------------------------------------

WeaponShotgun::WeaponShotgun(GameManager* playManager) : BulletWeapon(playManager)
{
    setMaxAmmoCount(50);
    setAmmoCount(40);
    setMaxAmmoDammage(75);
    setShootCadency(512);
    setShootSpeed(64);
    setFireSound(SOUND_SHOTGUN);
    setShootSize(7);

    setTexture(WEAPON_SHOTGUN);
    setNumber(50 * 7);
    build();

    setWeaponName("Shotgun");

    m_slot = 2;
}

void WeaponShotgun::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    for(int i = 0; i < 7; i++)
    {
        // Creation du tire
        Bullet * fire = new Bullet(m_playManager);
        fire->setWeapon(this);
        fire->setDammage(math::rand(1, m_maxAmmoDammage));
        fire->shoot(startpos, targetpos, m_shootSpeed, 0.1);

        m_bulletArray.push_back(fire);
    }
}

// WeaponBomb ------------------------------------------------------------------

WeaponBomb::WeaponBomb(GameManager* playManager) : BulletWeapon(playManager)
{
    setMaxAmmoCount(80);
    setAmmoCount(60);
    setMaxAmmoDammage(100);
    setShootCadency(512);
    setShootSpeed(8);
    setFireSound(SOUND_BOMB);
    setShootSize(8);

    setTexture(WEAPON_BOMB);
    setNumber(m_maxAmmoCount * m_shootSize);
    build();

    setWeaponName("Bomb");

    m_slot = 4;
}

void WeaponBomb::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    Vector3f relrot(0, 0, 0.25);

    for(unsigned i = 0; i < m_shootSize; i++)
    {
        relrot.rotate(45 * i, 0);

        Bullet * fire = new Bullet(m_playManager);
        fire->setWeapon(this);
        fire->setDammage(100);
        fire->shoot(startpos + relrot, startpos + (relrot * 4.0f), m_shootSpeed);

        m_bulletArray.push_back(fire);
    }
}

// WeaponFinder ----------------------------------------------------------------

WeaponFinder::WeaponFinder(GameManager* playManager) : BulletWeapon(playManager)
{
    setMaxAmmoCount(200);
    setAmmoCount(180);
    setMaxAmmoDammage(50);
    setShootCadency(64);
    setShootSpeed(64);
    setFireSound(SOUND_FINDER);

    setTexture(WEAPON_FINDER);
    setNumber(200);
    build();

    setWeaponName("Finder");

    m_slot = 3;
}

void WeaponFinder::process()
{
    Particle* particles = beginParticlesPosProcess();

    for(unsigned i = 0; i < m_bulletArray.size(); i++)
    {
        if(m_bulletArray[i]->isDeadAmmo())
        {
            delete m_bulletArray[i], m_bulletArray[i] = NULL;
        }

        else
        {
            m_bulletArray[i]->process();
            particles[i].pos = m_bulletArray[i]->getPhysicBody()->getPos();

            bool targetLocked = false;

            Vector3f minDist = m_mapAABB.max - m_mapAABB.min;

            const Player::Array& targets = m_playManager->getTargetsOf(m_shooter);

            for(unsigned j = 0; j < targets.size(); j++)
            {
                Vector3f ammodiri = m_bulletArray[i]->getPhysicBody()->getVelocity().normalize();
                Vector3f targetdiri = (targets[j]->getVisualBody()->getPos() - m_bulletArray[i]->getPhysicBody()->getPos()).normalize();

                if(Vector3f::dot(targetdiri, ammodiri) > 0.5f)
                {
                    minDist = min(targets[j]->getVisualBody()->getPos() - m_bulletArray[i]->getPhysicBody()->getPos(), minDist);
                    targetLocked = true;
                }
            }

            if(targetLocked)
            {
                m_bulletArray[i]->getPhysicBody()->setApplyForce(minDist.normalize() * m_shootSpeed / 2.0f);
            }
        }
    }

    endParticlesPosProcess();

    Bullet::Array::iterator newend = remove(m_bulletArray.begin(), m_bulletArray.end(), (Bullet*)NULL);
    m_bulletArray.erase(newend, m_bulletArray.end());

    setDrawNumber(m_bulletArray.size());
}

void WeaponFinder::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    // Creation du tire
    Bullet * fire = new Bullet(m_playManager);
    fire->setWeapon(this);
    fire->setDammage(math::rand(1, m_maxAmmoDammage));
    fire->shoot(startpos, targetpos, m_shootSpeed);

    m_bulletArray.push_back(fire);
}

