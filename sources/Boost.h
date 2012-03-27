/*
 * File:   Boost.h
 * Author: b4n92uid
 *
 * Created on 5 juillet 2011, 21:52
 */

#ifndef BOOST_H
#define	BOOST_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>

#include "Power.h"

class Player;

class Boost : public Power
{
public:
    Boost(GameManager* gameManager);
    virtual ~Boost();

    void process();

    static void clearSingleTone(GameManager* gm);

protected:
    void internalActivate(tbe::Vector3f target);
    void internalDiactivate();

private:
    bool boostForce(Player* player, tbe::Vector3f diriction);

private:
    tbe::ticks::Clock m_cadency;
    int m_signalid;
    
private:
    static tbe::ppe::MotionBlurEffect* m_ppeffect;
};

#endif	/* BOOST_H */

