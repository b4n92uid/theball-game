/*
 * File:   BulletTime.h
 * Author: b4n92uid
 *
 * Created on 27 juillet 2010, 13:48
 */

#ifndef BULLETTIME_H
#define	BULLETTIME_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>
#include <fmod.hpp>

#include "Power.h"

class Weapon;
class Player;

class BulletTime : public Power
{
public:
    BulletTime(GameManager* gameManager);
    virtual ~BulletTime();

    void process();

    static void clearSingleTone(GameManager* gm);

protected:
    void replaceRemainingCallback();
    void internalActivate(tbe::Vector3f target);
    void internalDiactivate();

protected:
    static void applyForceAndTorqueCallback(const NewtonBody* body, float, int);
    static void soundEffect(FMOD_CHANNEL* channel);
    static bool reduceDammage(Player* hitted, Player* shooter, int dammage);

protected:
    Weapon* m_usedWeapon;
    std::map<NewtonBody*, NewtonApplyForceAndTorque> m_callbacks;

protected:
    static tbe::ppe::ColorEffect* m_ppeffect;

private :
    float m_internalEnergy;
};

#endif	/* BULLETTIME_H */

