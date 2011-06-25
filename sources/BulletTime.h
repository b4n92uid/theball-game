/*
 * File:   BulletTime.h
 * Author: b4n92uid
 *
 * Created on 27 juillet 2010, 13:48
 */

#ifndef BULLETTIME_H
#define	BULLETTIME_H

#include <map>

#include "ColorEffect.h"
#include "Power.h"

class Player;

class BulletTime : public Power
{
public:
    BulletTime(GameManager* gameManager);
    virtual ~BulletTime();

    void process();

    virtual void activate(tbe::Vector3f target);
    virtual void diactivate();

protected:
    tbe::ppe::ColorEffect* m_ppeffect;
};

#endif	/* BULLETTIME_H */

