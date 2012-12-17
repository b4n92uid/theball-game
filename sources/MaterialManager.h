/*
 * File:   MaterialManager.h
 * Author: b4n92uid
 *
 * Created on 10 novembre 2009, 15:04
 */

#ifndef _MATERIALMANAGER_H
#define	_MATERIALMANAGER_H

#include <BulletBall/BulletBall.h>

#include "Player.h"

#include "Bullet.h"

#include "MapElement.h"

class Bullet;
class Player;
class MapElement;
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
    void addArea(AreaElement* body);

    bool isBullet(MapElement* body);
    bool isPlayer(MapElement* body);
    bool isElement(MapElement* body);
    bool isArea(MapElement* body);

    class MaterialCallback
    {
    public:
        virtual bool collide() = 0;
    };

    void registerCallback(MapElement* a, MapElement* b, MaterialCallback* cb);
    void clearCallbacks();

    void process();

    friend class CollidFilterCallback;

protected:
    btDiscreteDynamicsWorld* m_world;
    GameManager* m_gameManager;
    std::vector<MapElement*> m_ghostGroupe;
    std::vector<MapElement*> m_elementsGroupe;
    std::vector<AreaElement*> m_areaGroupe;
    std::vector<Bullet*> m_bulletGroupe;
    std::vector<Player*> m_playersGroupe;
    std::vector<Player*> m_immunityGroupe;

//    typedef std::map<std::pair<MapElement*, MapElement*>, MaterialCallback*> MapCallback;

    struct CallbackSettings
    {
        MapElement* a;
        MapElement* b;
        MaterialCallback* cb;
    };

    std::vector<CallbackSettings> m_callbacks;

    std::map<tbe::scene::BulletNode*, int> m_ghostState;
    std::map<tbe::scene::BulletNode*, int> m_immunityState;
};

#endif	/* _MATERIALMANAGER_H */

