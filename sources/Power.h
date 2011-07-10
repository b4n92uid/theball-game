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

#include "Settings.h"

#include <Tbe.h>

class Player;
class GameManager;
class SoundManager;

class Power
{
public:
    Power(GameManager* gameManager);
    virtual ~Power();

    /**
     * Cette fonction agis pandant l'état actife du pouvoir
     * elle est aussi en charge de la consomation d'energie (ex:value -= 2)
     * et la désactivation de celui ci dans le cas d'épuisement de l'énergie
     */
    virtual void process() = 0;

    void activate(tbe::Vector3f target);
    void diactivate();

    bool isActive() const;

    void setOwner(Player* owner);
    Player* getOwner() const;

    void setName(std::string name);
    std::string getName() const;

    unsigned getSlot() const;

    typedef std::vector<Power*> Array;

protected:
    virtual void internalActivate(tbe::Vector3f target) = 0;
    virtual void internalDiactivate() = 0;

protected:
    bool m_active;
    unsigned m_slot;

protected:
    Player* m_owner;
    GameManager* m_gameManager;
    SoundManager* m_soundManager;
    std::string m_name;
    Settings& m_settings;
};

#endif	/* POWER_H */
