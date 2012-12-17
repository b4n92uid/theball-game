/*
 * File:   GravityGun.h
 * Author: b4n92uid
 *
 * Created on 6 juillet 2011, 18:55
 */

#ifndef GRAVITYGUN_H
#define	GRAVITYGUN_H

#include "Power.h"

class GameManager;
class StaticElement;
class Player;

class GravityGun : public Power
{
public:
    GravityGun(GameManager* gameManager);
    virtual ~GravityGun();

    void process();

protected:
    void internalActivate(tbe::Vector3f target);
    void internalDiactivate();

private:
    StaticElement* m_attached;
    float m_internalEnergy;
};

#endif	/* GRAVITYGUN_H */
