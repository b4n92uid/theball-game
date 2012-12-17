/*
 * File:   MaterialManager.cpp
 * Author: b4n92uid
 *
 * Created on 10 novembre 2009, 15:04
 */

#include "MaterialManager.h"

#include "AppManager.h"
#include "GameManager.h"
#include "ScriptManager.h"

#include "Weapon.h"
#include "Bullet.h"
#include "Player.h"
#include "MapElement.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

class CollidFilterCallback : public btOverlapFilterCallback
{
public:
    MaterialManager* materialManager;
    GameManager* gameManager;

    CollidFilterCallback(MaterialManager * matmng)
    {
        materialManager = matmng;
        gameManager = matmng->m_gameManager;
    }

    virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy * proxy1) const
    {
        bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
        collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);

        if(proxy0->m_clientObject && proxy1->m_clientObject)
        {
            btCollisionObject* body0 = static_cast<btCollisionObject*>(proxy0->m_clientObject);
            btCollisionObject* body1 = static_cast<btCollisionObject*>(proxy1->m_clientObject);

            MapElement* elem0 = static_cast<MapElement*>(body0->getUserPointer());
            MapElement* elem1 = static_cast<MapElement*>(body1->getUserPointer());

            std::vector<MaterialManager::CallbackSettings>& callbacks = materialManager->m_callbacks;

            for(int i = 0; i < callbacks.size(); i++)
                if(callbacks[i].a == elem0 && callbacks[i].b == elem1 ||
                   callbacks[i].a == elem1 && callbacks[i].b == elem0)
                {
                    return callbacks[i].cb->collide();
                }
        }

        return collides;
    }
};

MaterialManager::MaterialManager(GameManager * gameManager)
{
    m_gameManager = gameManager;

    m_world = m_gameManager->parallelscene.physics->getWorld();

    btOverlapFilterCallback * filterCallback = new CollidFilterCallback(this);
    m_world->getPairCache()->setOverlapFilterCallback(filterCallback);
}

MaterialManager::~MaterialManager()
{
    clearCallbacks();
}

struct BulletOnPlayerCallback : public MaterialManager::MaterialCallback
{
    Bullet* bullet;
    Player* player;

    tbe::ticks::Clock cheerClock;
    int cheerCount;

    BulletOnPlayerCallback(Bullet* bullet, Player * player)
    {
        this->bullet = bullet;
        this->player = player;
        this->cheerCount = 0;
    }

    bool collide()
    {

        Player* striker = bullet->getWeapon()->getShooter();
        GameManager* playManager = striker->getGameManager();

        if(player == striker)
            return false;

        player->takeDammage(bullet->getDammage(), striker);

        bullet->setLife(0);

        if(striker == playManager->getUserPlayer() && player->isKilled())
        {
            cheerCount++;

            if(cheerCount >= 2)
            {
                if(!cheerClock.isEsplanedTime(4000))
                    switch(cheerCount)
                    {
                        case 2: playManager->display("Double kill");
                            break;
                        case 3: playManager->display("Triple kill");
                            break;
                        case 4: playManager->display("Carnage");
                            break;
                        case 5: playManager->display("The One");
                            break;
                        case 6: playManager->display("War Warrior");
                            break;
                        case 7: playManager->display("God Of War !!!");
                            break;
                        default: playManager->display("...");
                            break;
                    }
                else
                    cheerCount = 0;

                cheerClock.snapShoot();
            }
        }

        return false;
    }
};

struct BulletOnMapCallback : public MaterialManager::MaterialCallback
{
    Bullet* bullet;
    MapElement* elem;

    BulletOnMapCallback(Bullet* bullet, MapElement * elem)
    {
        this->bullet = bullet;
        this->elem = elem;
    }

    bool collide()
    {
        bullet->setLife(0);
        return true;
    }
};

struct PlayerOnMapCallback : public MaterialManager::MaterialCallback
{
    Player* player;
    MapElement* selem;

    PlayerOnMapCallback(Player* player, MapElement * selem)
    {
        this->player = player;
        this->selem = selem;
    }

    bool collide()
    {
        GameManager* ge = player->getGameManager();

        Vector3f veloc = selem->getPhysicBody()->getVelocity() * selem->getPhysicBody()->getMasse();

        Vector3f cveloc = player->getPhysicBody()->getVelocity() * player->getPhysicBody()->getMasse();

        if(veloc > cveloc)
        {
            veloc -= cveloc;

            if(veloc > 200)
                player->takeDammage(veloc.getMagnitude() - 200);
        }

        ge->manager.script->processCollid(player, selem);

        return true;
    }

};

void MaterialManager::setGhost(MapElement* body, bool state)
{
    if(state)
        m_ghostGroupe.push_back(body);
    else
        tools::erase(m_ghostGroupe, body);
}

void MaterialManager::setImmunity(Player* body, bool state)
{
    if(state)
        m_immunityGroupe.push_back(body);
    else
        tools::erase(m_immunityGroupe, body);
}

void MaterialManager::addElement(MapElement* elem)
{
    m_elementsGroupe.push_back(elem);
}

void MaterialManager::addArea(AreaElement* body)
{
    m_areaGroupe.push_back(body);
}

void MaterialManager::addBullet(Bullet* body)
{
    m_bulletGroupe.push_back(body);
}

void MaterialManager::addPlayer(Player* player)
{
    m_playersGroupe.push_back(player);
}

bool MaterialManager::isBullet(MapElement* body)
{
    return tools::find(m_bulletGroupe, body);
}

bool MaterialManager::isPlayer(MapElement* body)
{
    return tools::find(m_playersGroupe, body);
}

bool MaterialManager::isElement(MapElement* body)
{
    return tools::find(m_elementsGroupe, body);
}

bool MaterialManager::isArea(MapElement* body)
{
    return tools::find(m_areaGroupe, body);
}

void MaterialManager::clearCallbacks()
{
    for(int i = 0; i < m_callbacks.size(); i++)
        delete m_callbacks[i].cb;

    m_callbacks.clear();
}

void MaterialManager::registerCallback(MapElement* a, MapElement* b, MaterialCallback* cb)
{
    CallbackSettings cbs;
    cbs.a = a;
    cbs.b = b;
    cbs.cb = cb;

    m_callbacks.push_back(cbs);
}

void MaterialManager::process()
{
    clearCallbacks();

    // Player with MapElement

    BOOST_FOREACH(Player* player, m_playersGroupe)
    {

        BOOST_FOREACH(MapElement* mapElement, m_elementsGroupe)
        {
            PlayerOnMapCallback* callback = new PlayerOnMapCallback(player, mapElement);
            registerCallback(player, mapElement, callback);
        }
    }

    // Bullet with Map

    BOOST_FOREACH(Bullet* bullet, m_bulletGroupe)
    {

        BOOST_FOREACH(MapElement* mapElement, m_elementsGroupe)
        {
            BulletOnMapCallback* callback = new BulletOnMapCallback(bullet, mapElement);
            registerCallback(bullet, mapElement, callback);
        }
    }

    // Bullet With Player

    BOOST_FOREACH(Bullet* bullet, m_bulletGroupe)
    {

        BOOST_FOREACH(Player* player, m_playersGroupe)
        {
            BulletOnPlayerCallback* callback = new BulletOnPlayerCallback(bullet, player);
            registerCallback(bullet, player, callback);
        }
    }

    // Player with AreaElement

    BOOST_FOREACH(Player* player, m_playersGroupe)
    {

        BOOST_FOREACH(AreaElement* area, m_areaGroupe)
        {
            Matrix4 pmatrix = player->getVisualBody()->getAbsoluteMatrix();
            Matrix4 amatrix = area->getVisualBody()->getAbsoluteMatrix();

            if(amatrix.getPos() - pmatrix.getPos() < area->getRadius())
                m_gameManager->manager.script->processCollid(player, area);
        }
    }

    /*
        int numManifolds = m_world->getDispatcher()->getNumManifolds();

        for(int i = 0; i < numManifolds; i++)
        {
            btPersistentManifold* contactManifold = m_world->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* obA = contactManifold->getBody0();
            const btCollisionObject* obB = contactManifold->getBody1();

            MapElement* elemB = static_cast<MapElement*>(obB->getUserPointer());
            MapElement* elemA = static_cast<MapElement*>(obA->getUserPointer());

            for(int i = 0; i < m_callbacks.size(); i++)
                if(m_callbacks[i].a == elemA && m_callbacks[i].b == elemB ||
                   m_callbacks[i].a == elemB && m_callbacks[i].b == elemA)
                {
                    if(!m_callbacks[i].cb->collide())
                        contactManifold->clearManifold();
                }
        }
     */
}
