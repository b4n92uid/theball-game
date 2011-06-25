/*
 * File:   Weapon.h
 * Author: b4n92uid
 *
 * Created on 9 novembre 2009, 13:58
 */

#ifndef _WEAPONENGINE_H
#define	_WEAPONENGINE_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>
#include <fmod.h>

#include "Settings.h"
#include "MapElement.h"
#include "SoundManager.h"

class GameManager;
class Player;
class Bullet;
class Weapon;

class Bullet : public MapElement
{
public:

    Bullet(GameManager* playManager);
    ~Bullet();

    void shoot(tbe::Vector3f startpos, tbe::Vector3f targetpos, float shootspeed, float accuracy = 0);

    bool isDeadAmmo();

    void setLife(int life);
    int getLife() const;

    void setWeapon(Weapon* weapon);
    Weapon* getWeapon() const;

    void setShootSpeed(float shootSpeed);
    float getShootSpeed() const;

    void setDammage(int dammage);
    int getDammage() const;

    void process();

    void setShootDiri(tbe::Vector3f shootDiri);
    tbe::Vector3f getShootDiri() const;

    void setTargetPos(tbe::Vector3f targetPos);
    tbe::Vector3f getTargetPos() const;

    void setStartPos(tbe::Vector3f startPos);
    tbe::Vector3f getStartPos() const;

    typedef std::vector<Bullet*> Array;

protected:
    Weapon* m_weapon;
    int m_dammage;
    int m_life;
    float m_shootSpeed;
    tbe::Vector3f m_startPos;
    tbe::Vector3f m_targetPos;
    tbe::Vector3f m_shootDiri;
};

class Weapon : public tbe::scene::ParticlesEmiter
{
public:
    Weapon(GameManager* playManager);
    Weapon(const Weapon& copy);
    virtual ~Weapon();

    Weapon & operator=(const Weapon& copy);

    bool operator==(const Weapon& copy);

    bool shoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);

    void setShooter(Player* shooter);
    Player* getShooter() const;

    void setShootCadency(unsigned shootCadency);
    unsigned getShootCadency() const;

    bool UpAmmoCount(int ammoCount);
    void setAmmoCount(unsigned ammoCount);
    unsigned getAmmoCount() const;

    void setMaxAmmoCount(unsigned maxAmmoCount);
    unsigned getMaxAmmoCount() const;

    void setShootSize(unsigned shootSize);
    unsigned getShootSize() const;

    void setMaxAmmoDammage(unsigned maxAmmoDammage);
    unsigned getMaxAmmoDammage() const;

    void setShootSpeed(float shootSpeed);
    float getShootSpeed() const;

    void setFireSound(std::string fireSound);

    std::string getWeaponName() const;
    void setWeaponName(std::string weaponName);

    bool isEmpty();

    unsigned getSlot() const;

    void setupBullet(tbe::scene::Particle& p);

    Weapon* clone();

    typedef std::vector<Weapon*> Array;

protected:
    virtual void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos) = 0;
    virtual void process();

protected:
    GameManager* m_playManager;
    SoundManager* m_soundManager;

    Player* m_shooter;

    Settings::World m_worldSettings;

    Bullet::Array m_bulletArray;

    std::string m_weaponName;

    unsigned m_maxAmmoDammage;
    unsigned m_maxAmmoCount;
    unsigned m_ammoCount;

    unsigned m_slot;
    unsigned m_shootSize;

    float m_shootSpeed;

    unsigned m_shootCadency;

    tbe::ticks::Clock m_shootCadencyClock;

    tbe::AABB m_mapAABB;
    std::string m_soundID;
};

class WeaponBlaster : public Weapon
{
public:
    WeaponBlaster(GameManager* playManager);

protected:
    void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

class WeaponShotgun : public Weapon
{
public:
    WeaponShotgun(GameManager* playManager);
protected:
    void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

class WeaponFinder : public Weapon
{
public:
    WeaponFinder(GameManager* playManager);
protected:
    void process();
    void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

class WeaponBomb : public Weapon
{
public:
    WeaponBomb(GameManager* playManager);
protected:
    void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

#endif	/* _WEAPONENGINE_H */
