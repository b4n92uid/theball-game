/*
 * File:   Weapon.h
 * Author: b4n92uid
 *
 * Created on 9 novembre 2009, 13:58
 */

#ifndef _WEAPONENGINE_H
#define	_WEAPONENGINE_H

#include <Tbe.h>
#include <Newton/Newton.h>
#include <fmod.h>

#include "Settings.h"

class PlayManager;
class Player;
class Ammo;
class Weapon;

class Ammo : public tbe::scene::Node, public tbe::scene::NewtonNode
{
public:

    Ammo(Weapon* weapon);

    void Shoot(tbe::Vector3f startpos, tbe::Vector3f shootdiri, float shootspeed);

    bool IsDeadAmmo();

    void SetLife(int life);
    int GetLife() const;

    void SetParent(Weapon* shooter);
    Weapon* GetParent() const;

    void SetDiripos(tbe::Vector3f diripos);
    tbe::Vector3f GetDiripos() const;

    void SetShootSpeed(float shootSpeed);
    float GetShootSpeed() const;
    
    void SetDammage(int dammage);
    int GetDammage() const;

protected:
    PlayManager* m_playManager;
    Weapon* m_weapon;
    int m_dammage;
    int m_life;
    float m_shootSpeed;
    tbe::Vector3f m_startPos;
    tbe::Vector3f m_shootDiri;
};

class Weapon : public tbe::scene::ParticlesEmiter
{
public:
    Weapon(PlayManager* playManager);

    Weapon(const Weapon& copy);

    virtual ~Weapon();

    Weapon & operator=(const Weapon& copy);

    void Shoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);

    void SetShootCadency(unsigned shootCadency);
    unsigned GetShootCadency() const;

    bool UpAmmoCount(int ammoCount);
    void SetAmmoCount(unsigned ammoCount);
    unsigned GetAmmoCount() const;

    void SetMaxAmmoCount(unsigned maxAmmoCount);
    unsigned GetMaxAmmoCount() const;

    void SetMaxAmmoDammage(unsigned maxAmmoDammage);
    unsigned GetMaxAmmoDammage() const;

    void SetShootSpeed(float shootSpeed);
    float GetShootSpeed() const;

    void SetShooter(Player* shooter);
    Player* GetShooter() const;

    void SetFireSound(std::string fireSound);
    FSOUND_SAMPLE* GetFireSound() const;

    std::string GetWeaponName() const;
    void SetWeaponName(std::string weaponName);

    bool IsEmpty();

    typedef std::map<std::string, Weapon*> Map;

protected:
    virtual void ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos) = 0;
    virtual void Process();

protected:
    std::string m_weaponName;

    unsigned m_maxAmmoDammage;
    unsigned m_maxAmmoCount;
    unsigned m_ammoCount;

    float m_shootSpeed;

    unsigned m_shootCadency;

    tbe::ticks::Clock m_shootCadencyClock;

    std::vector<Ammo*> m_ammosPack;

    tbe::AABB m_mapAABB;

    PlayManager* m_playManager;
    Player* m_parent;

    FSOUND_SAMPLE* m_fireSound;

    Settings::Physics m_worldSettings;
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

