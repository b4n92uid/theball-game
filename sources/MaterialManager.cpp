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

MaterialManager::MaterialManager(GameManager * gameManager)
{
    m_gameManager = gameManager;

    m_world = m_gameManager->parallelscene.newton->getNewtonWorld();

    m_bulletGroupe = NewtonMaterialCreateGroupID(m_world);
    m_playersGroupe = NewtonMaterialCreateGroupID(m_world);
    m_immunityGroupe = NewtonMaterialCreateGroupID(m_world);
    m_elementsGroupe = NewtonMaterialCreateGroupID(m_world);
    m_ghostGroupe = NewtonMaterialCreateGroupID(m_world);
    m_dummyGroupe = NewtonMaterialCreateGroupID(m_world);

    NewtonMaterialSetCollisionCallback(m_world, m_playersGroupe, m_dummyGroupe, this, PlayerOnDummyAABBOverlape, NULL);
    NewtonMaterialSetCollisionCallback(m_world, m_bulletGroupe, m_playersGroupe, this, BulletOnPlayerAABBOverlape, NULL);

    NewtonMaterialSetCollisionCallback(m_world, m_playersGroupe, m_elementsGroupe, this, NULL, PlayerOnStaticContactsProcess);
    NewtonMaterialSetCollisionCallback(m_world, m_bulletGroupe, m_elementsGroupe, this, NULL, BulletOnMapContactsProcess);

    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_ghostGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_bulletGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_playersGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_elementsGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_immunityGroupe, false);

    NewtonMaterialSetDefaultCollidable(m_world, m_immunityGroupe, m_bulletGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_immunityGroupe, m_immunityGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_immunityGroupe, m_playersGroupe, false);

    NewtonMaterialSetDefaultCollidable(m_world, m_dummyGroupe, m_dummyGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_dummyGroupe, m_bulletGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_dummyGroupe, m_elementsGroupe, false);

    NewtonMaterialSetDefaultFriction(m_world, m_playersGroupe, m_elementsGroupe, 256, 512);
    NewtonMaterialSetDefaultFriction(m_world, m_immunityGroupe, m_elementsGroupe, 256, 512);
}

MaterialManager::~MaterialManager()
{
    NewtonMaterialDestroyAllGroupID(m_world);
}

void MaterialManager::setGhost(MapElement* body, bool state)
{
    NewtonNode* xbody = body->getPhysicBody();

    if(state)
    {
        if(m_ghostState.count(xbody))
            return;

        int newtonMaterial = NewtonBodyGetMaterialGroupID(xbody->getBody());
        m_ghostState[xbody] = newtonMaterial;
        NewtonBodySetMaterialGroupID(xbody->getBody(), m_ghostGroupe);
    }

    else
    {
        if(!m_ghostState.count(xbody))
            return;

        NewtonBodySetMaterialGroupID(xbody->getBody(), m_ghostState[xbody]);
        m_ghostState.erase(xbody);
    }
}

void MaterialManager::setImmunity(Player* body, bool state)
{
    NewtonNode* xbody = body->getPhysicBody();

    if(state)
    {
        if(m_immunityState.count(xbody))
            return;

        int newtonMaterial = NewtonBodyGetMaterialGroupID(xbody->getBody());
        m_immunityState[xbody] = newtonMaterial;
        NewtonBodySetMaterialGroupID(xbody->getBody(), m_immunityGroupe);
    }

    else
    {
        if(!m_immunityState.count(xbody))
            return;

        NewtonBodySetMaterialGroupID(xbody->getBody(), m_immunityState[xbody]);
        m_immunityState.erase(xbody);
    }
}

void MaterialManager::addElement(MapElement* body)
{
    NewtonBodySetMaterialGroupID(body->getPhysicBody()->getBody(), m_elementsGroupe);
}

void MaterialManager::addDummy(MapElement* body)
{
    NewtonBodySetMaterialGroupID(body->getPhysicBody()->getBody(), m_dummyGroupe);
}

void MaterialManager::addBullet(Bullet* body)
{
    NewtonBodySetMaterialGroupID(body->getPhysicBody()->getBody(), m_bulletGroupe);
}

void MaterialManager::addPlayer(Player* body)
{
    NewtonBodySetMaterialGroupID(body->getPhysicBody()->getBody(), m_playersGroupe);
}

int MaterialManager::getPlayersGroupe() const
{
    return m_playersGroupe;
}

int MaterialManager::getWeaponsGroupe() const
{
    return m_bulletGroupe;
}

int MaterialManager::getElementsGroupe() const
{
    return m_elementsGroupe;
}

template<typename T> static T getUserData(const NewtonBody* body)
{
    return static_cast<T>(NewtonBodyGetUserData(body));
}

void MaterialManager::mPlayerOnStaticContactsProcess(const NewtonJoint* contact, dFloat, int)
{
    NewtonBody* body0 = NewtonJointGetBody0(contact);
    NewtonBody* body1 = NewtonJointGetBody1(contact);

    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1 || group0 == m_ghostGroupe || group1 == m_ghostGroupe)
        return;

    Player* player = NULL;
    MapElement* elem = NULL;

    if(group0 == m_playersGroupe || group0 == m_immunityGroupe)
    {
        player = getUserData<Player*>(body0);
        elem = getUserData<MapElement*>(body1);
    }

    else if(group1 == m_playersGroupe || group1 == m_immunityGroupe)
    {
        player = getUserData<Player*>(body1);
        elem = getUserData<MapElement*>(body0);
    }

    else
        return;

    GameManager* ge = player->getGameManager();

    Vector3f veloc = elem->getPhysicBody()->getVelocity() * elem->getPhysicBody()->getMasse();

    Vector3f cveloc = player->getPhysicBody()->getVelocity() * player->getPhysicBody()->getMasse();

    if(veloc > cveloc)
    {
        veloc -= cveloc;

        if(veloc > 200)
            player->takeDammage(veloc.getMagnitude() - 200);
    }

    ge->manager.script->processCollid(player, elem);
}

int MaterialManager::mPlayerOnDummyAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int)
{
    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1 || group0 == m_ghostGroupe || group1 == m_ghostGroupe)
        return 0;

    Player* player = NULL;
    MapElement* elem = NULL;

    if(group0 == m_playersGroupe)
    {
        player = getUserData<Player*>(body0);
        elem = getUserData<MapElement*>(body1);
    }

    else if(group1 == m_playersGroupe)
    {
        player = getUserData<Player*>(body1);
        elem = getUserData<MapElement*>(body0);
    }

    GameManager* ge = player->getGameManager();

    ge->manager.script->processCollid(player, elem);

    return 0;
}

void MaterialManager::mBulletOnMapContactsProcess(const NewtonJoint* contact, dFloat, int)
{
    NewtonBody* body0 = NewtonJointGetBody0(contact);
    NewtonBody* body1 = NewtonJointGetBody1(contact);

    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1 || group0 == m_ghostGroupe || group1 == m_ghostGroupe)
        return;

    Bullet* bullet = NULL;

    if(group0 == m_bulletGroupe)
        bullet = getUserData<Bullet*>(body0);

    else if(group1 == m_bulletGroupe)
        bullet = getUserData<Bullet*>(body1);

    bullet->setLife(0);
}

int MaterialManager::mBulletOnPlayerAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int)
{
    static tbe::ticks::Clock cheerClock;
    static int cheerCount = 0;

    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1 || group0 == m_ghostGroupe || group1 == m_ghostGroupe)
        return 0;

    Player* striker = NULL;
    Player* striked = NULL;
    Bullet* bullet = NULL;

    if(group0 == m_bulletGroupe)
    {
        striked = getUserData<Player*>(body1);
        bullet = getUserData<Bullet*>(body0);
    }

    else if(group1 == m_bulletGroupe)
    {
        striked = getUserData<Player*>(body0);
        bullet = getUserData<Bullet*>(body1);
    }

    striker = bullet->getWeapon()->getShooter();
    GameManager* playManager = striker->getGameManager();

    if(striked == striker)
        return 0;

    striked->takeDammage(bullet->getDammage(), striker);

    bullet->setLife(0);

    if(striker == playManager->getUserPlayer() && striked->isKilled())
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

    return 0;
}