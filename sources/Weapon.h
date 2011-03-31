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
#include "Object.h"

class PlayManager;
class Player;
class Bullet;
class Weapon;

class Bullet : public tbe::scene::NewtonNode
{
public:

    Bullet(PlayManager* playManager);

    void Shoot(tbe::Vector3f startpos, tbe::Vector3f targetpos, float shootspeed, float accuracy = 0);

    bool IsDeadAmmo();

    void SetLife(int life);
    int GetLife() const;

    void SetWeapon(Weapon* weapon);
    Weapon* GetWeapon() const;

    void SetShootSpeed(float shootSpeed);
    float GetShootSpeed() const;

    void SetDammage(int dammage);
    int GetDammage() const;

    void SetShootDiri(tbe::Vector3f shootDiri);
    tbe::Vector3f GetShootDiri() const;

    void SetTargetPos(tbe::Vector3f targetPos);
    tbe::Vector3f GetTargetPos() const;

    void SetStartPos(tbe::Vector3f startPos);
    tbe::Vector3f GetStartPos() const;

    typedef std::vector<Bullet*> Array;

protected:
    PlayManager* m_playManager;
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
    Weapon(PlayManager* playManager);
    Weapon(const Weapon& copy);
    virtual ~Weapon();

    Weapon & operator=(const Weapon& copy);

    bool operator==(const Weapon& copy);

    bool Shoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);

    void SetShooter(Player* shooter);
    Player* GetShooter() const;

    void SetShootCadency(unsigned shootCadency);
    unsigned GetShootCadency() const;

    bool UpAmmoCount(int ammoCount);
    void SetAmmoCount(unsigned ammoCount);
    unsigned GetAmmoCount() const;

    void SetMaxAmmoCount(unsigned maxAmmoCount);
    unsigned GetMaxAmmoCount() const;

    void SetShootSize(unsigned shootSize);
    unsigned GetShootSize() const;

    void SetMaxAmmoDammage(unsigned maxAmmoDammage);
    unsigned GetMaxAmmoDammage() const;

    void SetShootSpeed(float shootSpeed);
    float GetShootSpeed() const;

    void SetFireSound(std::string fireSound);

    std::string GetWeaponName() const;
    void SetWeaponName(std::string weaponName);

    bool IsEmpty();

    unsigned GetSlot() const;

    void SetupBullet(tbe::scene::Particle& p);

    tbe::scene::Node* Clone();

    typedef std::vector<Weapon*> Array;

protected:
    virtual void ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos) = 0;
    virtual void Process();

protected:
    PlayManager* m_playManager;
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
    WeaponBlaster(PlayManager* playManager);

protected:
    void ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

class WeaponShotgun : public Weapon
{
public:
    WeaponShotgun(PlayManager* playManager);
protected:
    void ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

class WeaponFinder : public Weapon
{
public:
    WeaponFinder(PlayManager* playManager);
protected:
    void Process();
    void ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

class WeaponBomb : public Weapon
{
public:
    WeaponBomb(PlayManager* playManager);
protected:
    void ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);
};

#endif	/* _WEAPONENGINE_H */

