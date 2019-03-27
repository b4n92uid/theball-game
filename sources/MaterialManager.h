/*
 * File:   BodyMaterialManager.h
 * Author: b4n92uid
 *
 * Created on 10 novembre 2009, 15:04
 */

#ifndef _BODYMATERIALMANAGER_H
#define	_BODYMATERIALMANAGER_H

#include <NewtonBall/NewtonBall.h>

class Bullet;
class Player;
class MapElement;
class AreaElement;
class GameManager;

class BodyMaterialManager
{
public:
    BodyMaterialManager(GameManager* playManager);
    virtual ~BodyMaterialManager();

    void setGhost(MapElement* body, bool state);
    void setImmunity(Player* body, bool state);

    void addBullet(Bullet* body);
    void addPlayer(Player* body);
    void addElement(MapElement* body);

    void process();

    int callbackBulletOnPlayerAABBOverlape(const NewtonBody*, const NewtonBody*);
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

#endif	/* _BODYMATERIALMANAGER_H */

