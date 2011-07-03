/*
 * File:   Weapon.cpp
 * Author: b4n92uid
 *
 * Created on 9 septembre 2009, 13:57
 */

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

// Weapon ----------------------------------------------------------------------

Weapon::Weapon(GameManager* playManager)
{
    m_playManager = playManager;
    m_worldSettings = m_playManager->manager.app->globalSettings.world;
    m_soundManager = m_playManager->manager.sound;

    m_shooter = NULL;

    m_maxAmmoDammage = 0;
    m_maxAmmoCount = 0;

    m_ammoCount = 0;

    m_shootCadency = 0;

    m_mapAABB = m_playManager->map.aabb;

    m_slot = 0;

    m_weaponName = "Weapon";
}

Weapon::~Weapon()
{

}

Weapon* Weapon::clone()
{
    return NULL;
}

bool Weapon::operator==(const Weapon& copy)
{
    return m_weaponName == copy.m_weaponName;
}

void Weapon::setFireSound(std::string fireSound)
{
    fspath path(fireSound);
    m_soundID = path.stem();

    m_soundManager->registerSound(m_soundID, fireSound);
}

bool Weapon::isEmpty()
{
    return (m_ammoCount == 0);
}

unsigned Weapon::getSlot() const
{
    return m_slot;
}

void Weapon::setShooter(Player* shooter)
{
    this->m_shooter = shooter;
}

Player* Weapon::getShooter() const
{
    return m_shooter;
}

std::string Weapon::getWeaponName() const
{
    return m_weaponName;
}

void Weapon::setWeaponName(std::string weaponName)
{
    this->m_weaponName = weaponName;
}

bool Weapon::shoot(Vector3f startpos, Vector3f targetpos)
{
    // Controle des munitions
    if(m_ammoCount <= 0)
    {
        if(m_shooter->clocks.shoot.isEsplanedTime(1000))
            m_soundManager->playSound("noAvailable", m_shooter);

        return false;
    }

    if(!m_shootCadencyClock.isEsplanedTime(m_shootCadency))
        return false;

    m_soundManager->playSound(m_soundID, m_shooter);

    m_ammoCount--;

    processShoot(startpos, targetpos);

    return true;
}

void Weapon::setShootCadency(unsigned shootCadency)
{
    this->m_shootCadency = shootCadency;
}

unsigned Weapon::getShootCadency() const
{
    return m_shootCadency;
}

bool Weapon::UpAmmoCount(int ammoCount)
{
    if(m_ammoCount >= m_maxAmmoCount)
        return false;

    if(m_ammoCount + ammoCount > m_maxAmmoCount)
        m_ammoCount = m_maxAmmoCount;

    else
        m_ammoCount += ammoCount;

    return true;
}

void Weapon::setAmmoCount(unsigned ammoCount)
{

    if(ammoCount > m_maxAmmoCount)
        ammoCount = m_maxAmmoCount;

    this->m_ammoCount = ammoCount;
}

unsigned Weapon::getAmmoCount() const
{

    return m_ammoCount;
}

void Weapon::setMaxAmmoCount(unsigned maxAmmoCount)
{

    this->m_maxAmmoCount = maxAmmoCount;
}

unsigned Weapon::getMaxAmmoCount() const
{

    return m_maxAmmoCount;
}

void Weapon::setMaxAmmoDammage(unsigned maxAmmoDammage)
{

    this->m_maxAmmoDammage = maxAmmoDammage;
}

unsigned Weapon::getMaxAmmoDammage() const
{

    return m_maxAmmoDammage;
}
