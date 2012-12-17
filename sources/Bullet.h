/*
 * File:   Bullet.h
 * Author: b4n92uid
 *
 * Created on 28 juin 2011, 17:27
 */

#ifndef BULLET_H
#define	BULLET_H

#include <Tbe.h>
#include <BulletBall/BulletBall.h>
#include <fmod.h>

#include "MapElement.h"

class GameManager;
class Player;
class Weapon;

class Bullet : public MapElement
{
public:

    Bullet(GameManager* playManager);
    ~Bullet();

    void shoot(tbe::Vector3f startpos, tbe::Vector3f targetpos, float shootspeed, float accuracy = 0);

    bool isDeadAmmo();

    void setLife(int life);
    int getLife() const;

    void setWeapon(Weapon* weapon);
    Weapon* getWeapon() const;

    void setShootSpeed(float shootSpeed);
    float getShootSpeed() const;

    void setDammage(int dammage);
    int getDammage() const;

    void process();

    void setShootDiri(tbe::Vector3f shootDiri);
    tbe::Vector3f getShootDiri() const;

    void setTargetPos(tbe::Vector3f targetPos);
    tbe::Vector3f getTargetPos() const;

    void setStartPos(tbe::Vector3f startPos);
    tbe::Vector3f getStartPos() const;

    typedef std::vector<Bullet*> Array;

protected:
    Weapon* m_weapon;
    int m_dammage;
    int m_life;
    float m_shootSpeed;
    tbe::Vector3f m_startPos;
    tbe::Vector3f m_targetPos;
    tbe::Vector3f m_shootDiri;
};


#endif	/* BULLET_H */

