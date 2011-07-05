/*
 * File:   Power.h
 * Author: b4n92uid
 *
 * Created on 25 juin 2011, 20:47
 */

#ifndef POWER_H
#define	POWER_H

#include <vector>
#include <string>

#include <Tbe.h>

class Player;
class GameManager;
class SoundManager;

class Power
{
public:
    Power(GameManager* gameManager);
    virtual ~Power();

    virtual void process() = 0;

    virtual void activate(tbe::Vector3f target);
    virtual void diactivate();

    virtual bool isActive() const;

    void setOwner(Player* owner);
    Player* getOwner() const;

    void setName(std::string name);
    std::string getName() const;

    unsigned getSlot() const;

    typedef std::vector<Power*> Array;

protected:
    bool m_active;
    unsigned m_slot;

protected:
    Player* m_owner;
    GameManager* m_gameManager;
    SoundManager* m_soundManager;
    std::string m_name;
};

#endif	/* POWER_H */
