/*
 * File:   Boost.h
 * Author: b4n92uid
 *
 * Created on 5 juillet 2011, 21:52
 */

#ifndef BOOST_H
#define	BOOST_H

#include <map>

#include <NewtonBall/NewtonBall.h>

#include "ColorEffect.h"
#include "Power.h"
#include "Weapon.h"

class Player;

class Boost : public Power
{
public:
    Boost(GameManager* gameManager);
    virtual ~Boost();

    void process();

protected:
    void internalActivate(tbe::Vector3f target);
    void internalDiactivate();

private:
    bool boostForce(Player* player, tbe::Vector3f diriction);

private:
    static tbe::ppe::MotionBlurEffect* m_ppeffect;
};

#endif	/* BOOST_H */

