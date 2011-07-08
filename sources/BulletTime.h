/*
 * File:   BulletTime.h
 * Author: b4n92uid
 *
 * Created on 27 juillet 2010, 13:48
 */

#ifndef BULLETTIME_H
#define	BULLETTIME_H

#include <map>

#include <NewtonBall/NewtonBall.h>

#include "ColorEffect.h"
#include "Power.h"
#include "Weapon.h"

class Player;

class BulletTime : public Power
{
public:
    BulletTime(GameManager* gameManager);
    virtual ~BulletTime();

    void process();

protected:
    void internalActivate(tbe::Vector3f target);
    void internalDiactivate();

protected:
    static void applyForceAndTorqueCallback(const NewtonBody* body, float, int);
    static void soundEffect(FMOD_CHANNEL* channel);

protected:
    Weapon* m_usedWeapon;
    std::map<NewtonBody*, NewtonApplyForceAndTorque> m_callbacks;

protected:
    static tbe::gui::Image* m_screeneffect;
    static tbe::ppe::ColorEffect* m_ppeffect;
};

#endif	/* BULLETTIME_H */

