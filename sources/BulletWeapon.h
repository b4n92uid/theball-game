/*
 * File:   BulletWeapon.h
 * Author: b4n92uid
 *
 * Created on 3 juillet 2011, 21:49
 */

#ifndef BULLETWEAPON_H
#define	BULLETWEAPON_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>
#include <fmod.h>

#include "Weapon.h"

class GameManager;
class Player;
class Bullet;

class BulletWeapon : public Weapon, public tbe::scene::ParticlesEmiter
{
public:
    BulletWeapon(GameManager* playManager);
    virtual ~BulletWeapon();

    void setShootSize(unsigned shootSize);
    unsigned getShootSize() const;

    void setShootSpeed(float shootSpeed);
    float getShootSpeed() const;

    void setupBullet(tbe::scene::Particle& p);

protected:
    void process();
    virtual void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos) = 0;

protected:
    std::vector<Bullet*> m_bulletArray;
    unsigned m_shootSize;
    float m_shootSpeed;
};

class WeaponBlaster : public BulletWeapon
{
public:
    WeaponBlaster(GameManager* playManager);

protected:
    void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

class WeaponShotgun : public BulletWeapon
{
public:
    WeaponShotgun(GameManager* playManager);
protected:
    void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

class WeaponFinder : public BulletWeapon
{
public:
    WeaponFinder(GameManager* playManager);
protected:
    void process();
    void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

class WeaponFusion : public BulletWeapon
{
public:
    WeaponFusion(GameManager* playManager);
protected:
    void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

#endif	/* BULLETWEAPON_H */

