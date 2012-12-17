/*
 * File:   BulletWeapon.cpp
 * Author: b4n92uid
 *
 * Created on 3 juillet 2011, 21:49
 */

#include "BulletWeapon.h"

#include "GameManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"

#include "MapElement.h"
#include "Player.h"
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

    setBulletSize(0.5);

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
    m_slot = m_settings.weapons.get<int>("blaster.slot");
    setWeaponName(m_settings.weapons.get<string > ("blaster.name"));

    setMaxAmmoCount(m_settings.weapons.get<int>("blaster.maxAmmoCount"));
    setAmmoCount(m_settings.weapons.get<int>("blaster.ammoCount"));
    setMaxAmmoDammage(m_settings.weapons.get<int>("blaster.maxAmmoDammage"));
    setShootCadency(m_settings.weapons.get<int>("blaster.shootCadency"));
    setShootSpeed(m_settings.weapons.get<float>("blaster.shootSpeed"));
    setFireSound(m_settings.weapons.get<string > ("blaster.fireSound"));
    setTexture(m_settings.weapons.get<string > ("blaster.texture"));

    setNumber(m_maxAmmoCount * m_shootSize);
    build();

}

void WeaponBlaster::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    startpos += math::rand(Vector3f(-m_settings.world.playerSize * 0.5),
                           Vector3f(m_settings.world.playerSize * 0.5));

    // Creation du tire
    Bullet* fire = new Bullet(m_playManager);
    fire->setWeapon(this);
    fire->setDammage(math::rand(1, m_maxAmmoDammage));
    fire->shoot(startpos, targetpos, m_shootSpeed);

    m_bulletArray.push_back(fire);

    if(m_shooter == m_playManager->getUserPlayer())
        m_playManager->backImpulse(m_settings.weapons.get<float>("blaster.backIntensity"),
                                   m_settings.weapons.get<float>("blaster.backPush"));
}

// WeaponShotgun ---------------------------------------------------------------

WeaponShotgun::WeaponShotgun(GameManager* playManager) : BulletWeapon(playManager)
{
    m_slot = m_settings.weapons.get<int>("shotgun.slot");
    setWeaponName(m_settings.weapons.get<string > ("shotgun.name"));

    setMaxAmmoCount(m_settings.weapons.get<int>("shotgun.maxAmmoCount"));
    setAmmoCount(m_settings.weapons.get<int>("shotgun.ammoCount"));
    setMaxAmmoDammage(m_settings.weapons.get<int>("shotgun.maxAmmoDammage"));
    setShootCadency(m_settings.weapons.get<int>("shotgun.shootCadency"));
    setShootSpeed(m_settings.weapons.get<float>("shotgun.shootSpeed"));
    setFireSound(m_settings.weapons.get<string > ("shotgun.fireSound"));
    setTexture(m_settings.weapons.get<string > ("shotgun.texture"));
    setShootSize(m_settings.weapons.get<int>("shotgun.shootSize"));

    setNumber(m_maxAmmoCount * m_shootSize);
    build();
}

void WeaponShotgun::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    for(int i = 0; i < 7; i++)
    {
        // Creation du tire
        Bullet * fire = new Bullet(m_playManager);
        fire->setWeapon(this);
        fire->setDammage(math::rand(1, m_maxAmmoDammage));
        fire->shoot(startpos, targetpos, m_shootSpeed, 0.2);

        m_bulletArray.push_back(fire);
    }

    if(m_shooter == m_playManager->getUserPlayer())
        m_playManager->backImpulse(m_settings.weapons.get<float>("shotgun.backIntensity"),
                                   m_settings.weapons.get<float>("shotgun.backPush"));
}

// WeaponBomb ------------------------------------------------------------------

WeaponBomb::WeaponBomb(GameManager* playManager) : BulletWeapon(playManager)
{
    m_slot = m_settings.weapons.get<int>("bomb.slot");
    setWeaponName(m_settings.weapons.get<string > ("bomb.name"));

    setMaxAmmoCount(m_settings.weapons.get<int>("bomb.maxAmmoCount"));
    setAmmoCount(m_settings.weapons.get<int>("bomb.ammoCount"));
    setMaxAmmoDammage(m_settings.weapons.get<int>("bomb.maxAmmoDammage"));
    setShootCadency(m_settings.weapons.get<int>("bomb.shootCadency"));
    setShootSpeed(m_settings.weapons.get<float>("bomb.shootSpeed"));
    setFireSound(m_settings.weapons.get<string > ("bomb.fireSound"));
    setTexture(m_settings.weapons.get<string > ("bomb.texture"));
    setShootSize(m_settings.weapons.get<int>("bomb.shootSize"));

    setNumber(m_maxAmmoCount * m_shootSize);
    build();
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
    m_slot = m_settings.weapons.get<int>("finder.slot");
    setWeaponName(m_settings.weapons.get<string > ("finder.name"));

    setMaxAmmoCount(m_settings.weapons.get<int>("finder.maxAmmoCount"));
    setAmmoCount(m_settings.weapons.get<int>("finder.ammoCount"));
    setMaxAmmoDammage(m_settings.weapons.get<int>("finder.maxAmmoDammage"));
    setShootCadency(m_settings.weapons.get<int>("finder.shootCadency"));
    setShootSpeed(m_settings.weapons.get<float>("finder.shootSpeed"));
    setFireSound(m_settings.weapons.get<string > ("finder.fireSound"));
    setTexture(m_settings.weapons.get<string > ("finder.texture"));

    setNumber(m_maxAmmoCount * m_shootSize);
    build();
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
                if(targets[j]->isKilled())
                    continue;

                Vector3f ammodiri = m_bulletArray[i]->getPhysicBody()->getVelocity().normalize();
                Vector3f targetdiri = (targets[j]->getVisualBody()->getPos() - m_bulletArray[i]->getPhysicBody()->getPos()).normalize();

                if(Vector3f::dot(targetdiri, ammodiri) > 0.75f)
                {
                    minDist = min(targets[j]->getVisualBody()->getPos() - m_bulletArray[i]->getPhysicBody()->getPos(), minDist);
                    targetLocked = true;
                }
            }

            if(targetLocked)
            {
                BulletNode* nnode = m_bulletArray[i]->getPhysicBody();

                //                Vector3f force = minDist.normalize() * m_shootSpeed / 2;
                //                force -= nnode->getVelocity().normalize() * m_shootSpeed / 2;

                //                nnode->setApplyForce(force);
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
    fire->shoot(startpos, targetpos, 32);

    m_bulletArray.push_back(fire);

    if(m_shooter == m_playManager->getUserPlayer())
        m_playManager->backImpulse(m_settings.weapons.get<float>("finder.backIntensity"),
                                   m_settings.weapons.get<float>("finder.backPush"));
}
