/*
 * File:   Sniper.h
 * Author: b4n92uid
 *
 * Created on 18 janvier 2013, 20:21
 */

#ifndef SNIPER_H
#define	SNIPER_H

#include <Tbe.h>
#include <fmod.h>

#include "Weapon.h"

class GameManager;
class Player;

class WeaponSniper : public Weapon, public tbe::scene::LaserBeam
{
public:
    WeaponSniper(GameManager* gameManager);
    ~WeaponSniper();

protected:
    void process();
    void processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos);

private:
    float m_life;
};

#endif	/* SNIPER_H */
