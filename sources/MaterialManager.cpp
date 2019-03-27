/*
 * File:   BodyMaterialManager.cpp
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

#include <boost/format.hpp>

using namespace std;
using namespace tbe;
using namespace tbe::scene;

template<typename T> static T getUserData(const NewtonBody* body)
{
    return static_cast<T> (NewtonBodyGetUserData(body));
}

BodyMaterialManager* getBodyMaterialManager(const NewtonBody* body0, const NewtonBody* body1)
{
    int g0 = NewtonBodyGetMaterialGroupID(body0);
    int g1 = NewtonBodyGetMaterialGroupID(body1);

    return (BodyMaterialManager*) NewtonMaterialGetUserData(NewtonBodyGetWorld(body0), g0, g1);
}

BodyMaterialManager* getBodyMaterialManager(const NewtonJoint* contact)
{
    NewtonBody* b0 = NewtonJointGetBody0(contact);
    NewtonBody* b1 = NewtonJointGetBody1(contact);

    if(b0 && b1)
        return getBodyMaterialManager(b0, b1);
    else
        return NULL;
}

int BulletOnPlayerAABBOverlape(const NewtonJoint* const contact, dFloat timestep, int threadIndex)
{
    const NewtonBody *body0, *body1;

    body0 = NewtonJointGetBody0(contact);
    body1 = NewtonJointGetBody1(contact);

    BodyMaterialManager* matmanager = getBodyMaterialManager(body0, body1);

    if(matmanager)
        return matmanager->callbackBulletOnPlayerAABBOverlape(body0, body1);
    else
        return 0;
}

void BulletOnMapContactsProcess(const NewtonJoint* contact, dFloat f, int i)
{
    BodyMaterialManager* matmanager = getBodyMaterialManager(contact);

    if(matmanager)
        matmanager->callbackBulletOnMapContactsProcess(contact, f, i);
}

void PlayerOnStaticContactsProcess(const NewtonJoint* contact, dFloat timestep, int threadIndex)
{
    BodyMaterialManager* matmanager = getBodyMaterialManager(contact);

    if(matmanager)
        matmanager->callbackPlayerOnStaticContactsProcess(contact, timestep, threadIndex);
}

BodyMaterialManager::BodyMaterialManager(GameManager* gameManager)
{
    m_gameManager = gameManager;

    m_world = m_gameManager->parallelscene.newton->getNewtonWorld();

    m_bulletGroupe = NewtonMaterialCreateGroupID(m_world);
    m_playersGroupe = NewtonMaterialCreateGroupID(m_world);
    m_immunityGroupe = NewtonMaterialCreateGroupID(m_world);
    m_elementsGroupe = NewtonMaterialCreateGroupID(m_world);
    m_ghostGroupe = NewtonMaterialCreateGroupID(m_world);

    NewtonMaterialSetCollisionCallback(m_world, m_playersGroupe, m_elementsGroupe, NULL, PlayerOnStaticContactsProcess);
    NewtonMaterialSetCollisionCallback(m_world, m_bulletGroupe, m_playersGroupe, BulletOnPlayerAABBOverlape, NULL);
    NewtonMaterialSetCollisionCallback(m_world, m_bulletGroupe, m_elementsGroupe, NULL, BulletOnMapContactsProcess);

    NewtonMaterialSetCallbackUserData(m_world, m_playersGroupe, m_elementsGroupe, this);
    NewtonMaterialSetCallbackUserData(m_world, m_bulletGroupe, m_playersGroupe, this);
    NewtonMaterialSetCallbackUserData(m_world, m_bulletGroupe, m_elementsGroupe, this);

    NewtonMaterialSetDefaultCollidable(m_world, m_bulletGroupe, m_bulletGroupe, false);

    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_ghostGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_bulletGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_playersGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_elementsGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_immunityGroupe, false);

    NewtonMaterialSetDefaultCollidable(m_world, m_immunityGroupe, m_bulletGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_immunityGroupe, m_immunityGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_immunityGroupe, m_playersGroupe, false);

    NewtonMaterialSetDefaultFriction(m_world, m_playersGroupe, m_elementsGroupe, 256, 512);
    NewtonMaterialSetDefaultFriction(m_world, m_immunityGroupe, m_elementsGroupe, 256, 512);
}

BodyMaterialManager::~BodyMaterialManager()
{
    NewtonMaterialDestroyAllGroupID(m_world);
}

void BodyMaterialManager::setGhost(MapElement* body, bool state)
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

void BodyMaterialManager::setImmunity(Player* body, bool state)
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

void BodyMaterialManager::addElement(MapElement* body)
{
    NewtonBodySetMaterialGroupID(body->getPhysicBody()->getBody(), m_elementsGroupe);
}

void BodyMaterialManager::addBullet(Bullet* body)
{
    NewtonBodySetMaterialGroupID(body->getPhysicBody()->getBody(), m_bulletGroupe);
}

void BodyMaterialManager::addPlayer(Player* body)
{
    NewtonBodySetMaterialGroupID(body->getPhysicBody()->getBody(), m_playersGroupe);
}

void BodyMaterialManager::process()
{
    const Player::Array& pls = m_gameManager->getPlayers();

    for(unsigned i = 0; i < pls.size(); i++)
    {
        Player* player = pls[i];

        BOOST_FOREACH(AreaElement* area, m_gameManager->map.areaElements)
        {
            Matrix4 pmatrix = player->getVisualBody()->getAbsoluteMatrix();
            Matrix4 amatrix = area->getVisualBody()->getAbsoluteMatrix();

            if(amatrix.getPos() - pmatrix.getPos() < area->getRadius())
            {
                if(!m_triggerCollidState.count(player))
                {
                    m_triggerCollidState[player] = area;
                    m_gameManager->manager.script->processCollid(player, area);
                }
            }
        }

        if(m_triggerCollidState.count(player))
        {
            AreaElement* area = m_triggerCollidState[player];

            Matrix4 pmatrix = player->getVisualBody()->getAbsoluteMatrix();
            Matrix4 amatrix = area->getVisualBody()->getAbsoluteMatrix();

            if(amatrix.getPos() - pmatrix.getPos() > area->getRadius())
                m_triggerCollidState.erase(player);
        }
    }
}

void BodyMaterialManager::callbackPlayerOnStaticContactsProcess(const NewtonJoint* contact, dFloat, int)
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

    void* thiscontact = NewtonContactJointGetFirstContact(contact);

    NewtonMaterial* nmat = NewtonContactGetMaterial(thiscontact);

    Vector3f force;
    NewtonMaterialGetContactForce(nmat, player->getPhysicBody()->getBody(), force);

    float normalSpeed = NewtonMaterialGetContactNormalSpeed(nmat);

    ge->manager.script->processCollid(player, elem, force.getMagnitude(), normalSpeed);
}

void BodyMaterialManager::callbackBulletOnMapContactsProcess(const NewtonJoint* contact, dFloat, int)
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

int BodyMaterialManager::callbackBulletOnPlayerAABBOverlape(const NewtonBody* body0, const NewtonBody* body1)
{
    using boost::format;

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
            if(!cheerClock.isEsplanedTime(3000))
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
                    default: playManager->display((format("%1% Kills...") % cheerCount).str());
                        break;
                }
            else
                cheerCount = 0;

            cheerClock.snapShoot();
        }
    }

    return 0;
}
