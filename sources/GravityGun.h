/*
 * File:   GravityGun.h
 * Author: b4n92uid
 *
 * Created on 6 juillet 2011, 18:55
 */

#ifndef GRAVITYGUN_H
#define	GRAVITYGUN_H

#include <map>

#include <NewtonBall/NewtonBall.h>

#include "Power.h"

class GameManager;
class MapElement;
class Player;

class GravityGun : public Power
{
public:
    GravityGun(GameManager* gameManager);
    virtual ~GravityGun();

    void process();

    virtual void activate(tbe::Vector3f target);
    virtual void diactivate();

private:
    MapElement* m_attached;
    MapElement* m_lastAttached;
    float m_internalEnergy;
    tbe::scene::ParticlesEmiter* m_highlighter;
};

#endif	/* GRAVITYGUN_H */

