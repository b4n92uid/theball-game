/*
 * File:   Weapon.h
 * Author: b4n92uid
 *
 * Created on 9 novembre 2009, 13:58
 */

#ifndef _WEAPONENGINE_H
#define	_WEAPONENGINE_H

#include <Tbe.h>
#include <BulletBall/BulletBall.h>
#include <fmod.h>

#include "Settings.h"

class GameManager;
class SoundManager;
class Player;

class Weapon
{
public:
    Weapon(GameManager* playManager);
    virtual ~Weapon();

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

    void setMaxAmmoDammage(unsigned maxAmmoDammage);
    unsigned getMaxAmmoDammage() const;

    void setFireSound(std::string fireSound);

    std::string getWeaponName() const;
    void setWeaponName(std::string weaponName);

    bool isEmpty();

    unsigned getSlot() const;

    Weapon* clone();

protected:
    virtual void process() = 0;
    virtual void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos) = 0;

protected:
    GameManager* m_playManager;
    SoundManager* m_soundManager;

    Player* m_shooter;

    std::string m_weaponName;

    unsigned m_maxAmmoDammage;
    unsigned m_maxAmmoCount;
    unsigned m_ammoCount;

    unsigned m_slot;

    unsigned m_shootCadency;

    tbe::ticks::Clock m_shootCadencyClock;

    tbe::AABB m_mapAABB;
    std::string m_soundID;

    const Settings& m_settings;
};

#endif	/* _WEAPONENGINE_H */
