/*
 * File:   MaterialManager.h
 * Author: b4n92uid
 *
 * Created on 10 novembre 2009, 15:04
 */

#ifndef _MATERIALMANAGER_H
#define	_MATERIALMANAGER_H

#include <NewtonBall/NewtonBall.h>

class Bullet;
class Player;
class MapElement;
class AreaElement;
class GameManager;

class MaterialManager
{
public:
    MaterialManager(GameManager* playManager);
    virtual ~MaterialManager();

    void setGhost(MapElement* body, bool state);
    void setImmunity(Player* body, bool state);

    void addBullet(Bullet* body);
    void addPlayer(Player* body);
    void addElement(MapElement* body);

    void process();

    int callbackBulletOnPlayerAABBOverlape(const NewtonMaterial*, const NewtonBody*, const NewtonBody*, int);
    void callbackBulletOnMapContactsProcess(const NewtonJoint* contact, dFloat, int);
    void callbackPlayerOnStaticContactsProcess(const NewtonJoint* contact, dFloat, int);

protected:
    NewtonWorld* m_world;
    GameManager* m_gameManager;

    int m_elementsGroupe;
    int m_bulletGroupe;
    int m_playersGroupe;
    int m_immunityGroupe;
    int m_ghostGroupe;

    std::map<Player*, AreaElement*> m_triggerCollidState;

    std::map<tbe::scene::NewtonNode*, int> m_ghostState;
    std::map<tbe::scene::NewtonNode*, int> m_immunityState;
};

#endif	/* _MATERIALMANAGER_H */

