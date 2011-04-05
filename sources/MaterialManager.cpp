/* 
 * File:   MaterialManager.cpp
 * Author: b4n92uid
 * 
 * Created on 10 novembre 2009, 15:04
 */

#include "MaterialManager.h"

#include "GameManager.h"
#include "PlayManager.h"

#include "Item.h"
#include "Weapon.h"
#include "Player.h"

#include "AppManager.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

MaterialManager::MaterialManager(GameManager * gameManager)
{
    m_gameManager = gameManager;

    m_world = m_gameManager->parallelscene.newton->GetNewtonWorld();

    Settings::World& worldSettings = m_gameManager->worldSettings;

    m_bulletGroupe = NewtonMaterialCreateGroupID(m_world);
    m_playersGroupe = NewtonMaterialCreateGroupID(m_world);
    m_itemGroupe = NewtonMaterialCreateGroupID(m_world);
    m_staticGroupe = NewtonMaterialCreateGroupID(m_world);
    m_dynamicGroupe = NewtonMaterialCreateGroupID(m_world);
    m_ghostGroupe = NewtonMaterialCreateGroupID(m_world);

    NewtonMaterialSetCollisionCallback(m_world, m_playersGroupe, m_dynamicGroupe, this, NULL, PlayerOnDynamicContactsProcess);
    // NewtonMaterialSetCollisionCallback(m_world, m_playersGroupe, m_staticGroupe, this, NULL, PlayerOnStaticContactsProcess);

    NewtonMaterialSetCollisionCallback(m_world, m_playersGroupe, m_itemGroupe, this, PlayerOnItemsAABBOverlape, NULL);

    NewtonMaterialSetCollisionCallback(m_world, m_bulletGroupe, m_dynamicGroupe, this, NULL, BulletOnMapContactsProcess);
    NewtonMaterialSetCollisionCallback(m_world, m_bulletGroupe, m_staticGroupe, this, NULL, BulletOnMapContactsProcess);
    NewtonMaterialSetCollisionCallback(m_world, m_bulletGroupe, m_itemGroupe, this, NULL, BulletOnMapContactsProcess);
    NewtonMaterialSetCollisionCallback(m_world, m_bulletGroupe, m_playersGroupe, this, BulletOnPlayerAABBOverlape, NULL);

    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_bulletGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_playersGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_itemGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_staticGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_dynamicGroupe, false);
    // NewtonMaterialSetDefaultCollidable(m_world, m_bulletGroupe, m_itemGroupe, false);


    NewtonMaterialSetDefaultFriction(m_world, m_playersGroupe, m_staticGroupe, worldSettings.staticFriction, worldSettings.keniticFriction);
}

MaterialManager::~MaterialManager()
{
    NewtonMaterialDestroyAllGroupID(m_world);
}

void MaterialManager::SetGhost(Object* body, bool state)
{
    NewtonNode* xbody = body->GetPhysicBody();

    if(state)
    {
        if(m_ghostState.count(xbody))
            return;

        int newtonMaterial = NewtonBodyGetMaterialGroupID(xbody->GetBody());
        m_ghostState[xbody] = newtonMaterial;
        NewtonBodySetMaterialGroupID(xbody->GetBody(), m_ghostGroupe);
    }

    else
    {
        if(!m_ghostState.count(xbody))
            return;

        NewtonBodySetMaterialGroupID(xbody->GetBody(), m_ghostState[xbody]);
        m_ghostState.erase(xbody);
    }
}

void MaterialManager::AddDynamic(DynamicObject* body)
{
    NewtonBodySetMaterialGroupID(body->GetPhysicBody()->GetBody(), m_dynamicGroupe);
}

void MaterialManager::AddStatic(StaticObject* body)
{
    NewtonBodySetMaterialGroupID(body->GetPhysicBody()->GetBody(), m_staticGroupe);
}

void MaterialManager::AddBullet(Bullet* body)
{
    NewtonBodySetMaterialGroupID(body->GetBody(), m_bulletGroupe);
}

void MaterialManager::AddPlayer(Player* body)
{
    NewtonBodySetMaterialGroupID(body->GetPhysicBody()->GetBody(), m_playersGroupe);
}

void MaterialManager::AddItem(Item* body)
{
    NewtonBodySetMaterialGroupID(body->GetPhysicBody()->GetBody(), m_itemGroupe);
}

int MaterialManager::GetPlayersGroupe() const
{
    return m_playersGroupe;
}

int MaterialManager::GetWeaponsGroupe() const
{
    return m_bulletGroupe;
}

int MaterialManager::GetItemGroupe() const
{
    return m_itemGroupe;
}

int MaterialManager::GetDynamicGroupe() const
{
    return m_dynamicGroupe;
}

int MaterialManager::GetStaticGroupe() const
{
    return m_staticGroupe;
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
    NewtonNode* obj = NULL;

    if(group0 == m_playersGroupe)
    {
        player = GetParentUserData<Player*>(body0);
        obj = GetParentUserData<NewtonNode*>(body1);
    }

    else if(group1 == m_playersGroupe)
    {
        player = GetParentUserData<Player*>(body1);
        obj = GetParentUserData<NewtonNode*>(body0);
    }
}

void MaterialManager::mPlayerOnDynamicContactsProcess(const NewtonJoint* contact, dFloat, int)
{
    NewtonBody* body0 = NewtonJointGetBody0(contact);
    NewtonBody* body1 = NewtonJointGetBody1(contact);

    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1 || group0 == m_ghostGroupe || group1 == m_ghostGroupe)
        return;

    Player* player = NULL;
    DynamicObject* obj = NULL;

    if(group0 == m_playersGroupe)
    {
        obj = GetParentUserData<DynamicObject*>(body1);
        player = GetParentUserData<Player*>(body0);
    }

    else if(group1 == m_playersGroupe)
    {
        obj = GetParentUserData<DynamicObject*>(body0);
        player = GetParentUserData<Player*>(body1);
    }

    obj->InteractWith(player);
}

int MaterialManager::mPlayerOnItemsAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int)
{
    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1 || group0 == m_ghostGroupe || group1 == m_ghostGroupe)
        return 0;

    Player* player = NULL;
    Item* item = NULL;

    if(group0 == m_itemGroupe)
    {
        item = GetParentUserData<Item*>(body0);
        player = GetParentUserData<Player*>(body1);
    }

    else if(group1 == m_itemGroupe)
    {
        item = GetParentUserData<Item*>(body1);
        player = GetParentUserData<Player*>(body0);
    }

    player->AttachItem(item);

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
        bullet = GetUserData<Bullet*>(body0);

    else if(group1 == m_bulletGroupe)
        bullet = GetUserData<Bullet*>(body1);

    bullet->SetLife(0);
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
        striked = GetParentUserData<Player*>(body1);
        bullet = GetUserData<Bullet*>(body0);
    }

    else if(group1 == m_bulletGroupe)
    {
        striked = GetParentUserData<Player*>(body0);
        bullet = GetUserData<Bullet*>(body1);
    }

    striker = bullet->GetWeapon()->GetShooter();
    PlayManager* playManager = striker->GetPlayManager();

    if(striked == striker)
        return 0;

    striked->TakeDammage(bullet);

    bullet->SetLife(0);

    if(striker == playManager->GetUserPlayer() && striked->IsKilled())
    {
        cheerCount++;

        if(cheerCount >= 2)
        {
            if(!cheerClock.IsEsplanedTime(4000))
                switch(cheerCount)
                {
                    case 2: playManager->Log("Double kill");
                        break;
                    case 3: playManager->Log("Triple kill");
                        break;
                    case 4: playManager->Log("Carnage");
                        break;
                    case 5: playManager->Log("The One");
                        break;
                    case 6: playManager->Log("War Warrior");
                        break;
                    case 7: playManager->Log("God Of War !!!");
                        break;
                    default: playManager->Log("...");
                        break;
                }
            else
                cheerCount = 0;

            cheerClock.SnapShoot();
        }
    }

    return 0;
}
