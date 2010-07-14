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

void PlayerOnStaticContactsProcess(const NewtonJoint* contact, dFloat timestep, int threadIndex)
{
    NewtonBody* body0 = NewtonJointGetBody0(contact);
    NewtonBody* body1 = NewtonJointGetBody1(contact);

    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1)
        return;

    NewtonWorld* nWorld = NewtonBodyGetWorld(body0);
    PlayManager* playManager = static_cast<PlayManager*>(NewtonMaterialGetUserData(nWorld, group0, group1));
    int playerGroup = playManager->manager.material->GetPlayersGroupe();

    Player* player = NULL;
    NewtonNode* obj = NULL;

    try
    {
        if(group0 == playerGroup)
        {
            player = dynamic_cast<Player*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body0));
            obj = dynamic_cast<NewtonNode*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body1));
        }

        else if(group1 == playerGroup)
        {
            player = dynamic_cast<Player*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body1));
            obj = dynamic_cast<NewtonNode*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body0));
        }

        else
            return;
    }

    catch(...)
    {

    }
}

void PlayerOnDynamicContactsProcess(const NewtonJoint* contact, dFloat timestep, int threadIndex)
{
    NewtonBody* body0 = NewtonJointGetBody0(contact);
    NewtonBody* body1 = NewtonJointGetBody1(contact);

    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1)
        return;

    NewtonWorld* nWorld = NewtonBodyGetWorld(body0);
    PlayManager* playManager = static_cast<PlayManager*>(NewtonMaterialGetUserData(nWorld, group0, group1));
    int playerGroup = playManager->manager.material->GetPlayersGroupe();

    Player* player = NULL;
    DynamicObject* obj = NULL;

    try
    {
        if(group0 == playerGroup)
        {
            obj = dynamic_cast<DynamicObject*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body1));
            player = dynamic_cast<Player*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body0));
        }

        else if(group1 == playerGroup)
        {
            obj = dynamic_cast<DynamicObject*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body0));
            player = dynamic_cast<Player*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body1));
        }

        else
            return;

        obj->InteractWith(player);
    }

    catch(...)
    {

    }
}

int PlayerOnItemsAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int threadIndex)
{
    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1)
        return 0;

    Player* player = NULL;
    Item* item = NULL;

    NewtonWorld* nWorld = NewtonBodyGetWorld(body0);
    PlayManager* playManager = static_cast<PlayManager*>(NewtonMaterialGetUserData(nWorld, group0, group1));
    int itemGroup = playManager->manager.material->GetItemGroupe();

    try
    {
        if(group0 == itemGroup)
        {
            item = dynamic_cast<Item*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body0));
            player = dynamic_cast<Player*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body1));
        }

        else if(group1 == itemGroup)
        {
            item = dynamic_cast<Item*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body1));
            player = dynamic_cast<Player*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body0));
        }

        else
            return 1;

        player->AttachItem(item);
    }

    catch(...)
    {

    }

    return 0;
}

void WeaponsOnMapContactsProcess(const NewtonJoint* contact, dFloat timestep, int threadIndex)
{
    NewtonBody* body0 = NewtonJointGetBody0(contact);
    NewtonBody* body1 = NewtonJointGetBody1(contact);

    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1)
        return;

    NewtonWorld* nWorld = NewtonBodyGetWorld(body0);
    PlayManager* playManager = static_cast<PlayManager*>(NewtonMaterialGetUserData(nWorld, group0, group1));
    int weaponGroup = playManager->manager.material->GetWeaponsGroupe();

    Ammo* ammo = NULL;

    try
    {
        if(group0 == weaponGroup)
            ammo = dynamic_cast<Ammo*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body0));

        else if(group1 == weaponGroup)
            ammo = dynamic_cast<Ammo*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body1));

        else
            return;

        ammo->SetLife(0);
    }

    catch(...)
    {

    }
}

int WeaponsOnPlayerAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int threadIndex)
{
    static tbe::ticks::Clock cheerClock;
    static int cheerCount = 0;

    int group0 = NewtonBodyGetMaterialGroupID(body0);
    int group1 = NewtonBodyGetMaterialGroupID(body1);

    if(group0 == group1)
        return 0;

    Player* striker = NULL;
    Player* striked = NULL;
    Ammo* ammo = NULL;

    NewtonWorld* nWorld = NewtonBodyGetWorld(body0);
    PlayManager* playManager = static_cast<PlayManager*>(NewtonMaterialGetUserData(nWorld, group0, group1));
    int weaponGroup = playManager->manager.material->GetWeaponsGroupe();

    try
    {
        if(group0 == weaponGroup)
        {
            striked = dynamic_cast<Player*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body1));
            ammo = dynamic_cast<Ammo*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body0));
        }

        else if(group1 == weaponGroup)
        {
            striked = dynamic_cast<Player*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body0));
            ammo = dynamic_cast<Ammo*>((tbe::scene::NewtonNode*)NewtonBodyGetUserData(body1));
        }

        else
            return 1;

        striker = ammo->GetParent()->GetShooter();

        if(striked != striker)
        {
            striked->TakeDammage(ammo);
            ammo->SetLife(0);

            if(playManager->GetUserPlayer() == striked)
            {
                playManager->hud.background.dammage->SetOpacity(0.75);
                playManager->hud.background.dammage->SetEnable(true);
            }

            else if(playManager->GetUserPlayer() == striker)
            {
                if(cheerClock.IsEsplanedTime(3000))
                    cheerCount = 0;

                if(!striked->IsKilled() && striked->GetLife() <= 0)
                {
                    cheerClock.SnapShoot();
                    cheerCount++;

                    if(cheerCount > 1)
                        switch(cheerCount)
                        {
                            case 2: playManager->Log("Double Frag");
                                break;
                            case 3: playManager->Log("Triple Frag");
                                break;
                            case 4: playManager->Log("A Killer");
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
                }
            }
        }
    }

    catch(...)
    {

    }

    return 0;
}

MaterialManager::MaterialManager(GameManager * playManager)
{
    m_gameManager = playManager;

    m_world = m_gameManager->parallelscene.newton->GetNewtonWorld();

    Settings::Physics& worldSettings = m_gameManager->worldSettings;

    m_weaponsGroupe = NewtonMaterialCreateGroupID(m_world);
    m_playersGroupe = NewtonMaterialCreateGroupID(m_world);
    m_itemGroupe = NewtonMaterialCreateGroupID(m_world);
    m_staticGroupe = NewtonMaterialCreateGroupID(m_world);
    m_dynamicGroupe = NewtonMaterialCreateGroupID(m_world);
    m_ghostGroupe = NewtonMaterialCreateGroupID(m_world);

    NewtonMaterialSetCollisionCallback(m_world, m_playersGroupe, m_dynamicGroupe, m_gameManager, 0, PlayerOnDynamicContactsProcess);
    //    NewtonMaterialSetCollisionCallback(m_world, m_playersGroupe, m_staticGroupe, 0, 0, PlayerOnStaticContactsProcess);

    NewtonMaterialSetCollisionCallback(m_world, m_playersGroupe, m_itemGroupe, m_gameManager, PlayerOnItemsAABBOverlape, 0);

    NewtonMaterialSetCollisionCallback(m_world, m_weaponsGroupe, m_dynamicGroupe, m_gameManager, 0, WeaponsOnMapContactsProcess);
    NewtonMaterialSetCollisionCallback(m_world, m_weaponsGroupe, m_staticGroupe, m_gameManager, 0, WeaponsOnMapContactsProcess);
    NewtonMaterialSetCollisionCallback(m_world, m_weaponsGroupe, m_weaponsGroupe, m_gameManager, WeaponsOnPlayerAABBOverlape, 0);
    NewtonMaterialSetCollisionCallback(m_world, m_weaponsGroupe, m_playersGroupe, m_gameManager, WeaponsOnPlayerAABBOverlape, 0);

    NewtonMaterialSetDefaultFriction(m_world, m_playersGroupe, m_staticGroupe, worldSettings.staticFriction, worldSettings.keniticFriction);

    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_weaponsGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_playersGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_itemGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_staticGroupe, false);
    NewtonMaterialSetDefaultCollidable(m_world, m_ghostGroupe, m_dynamicGroupe, false);

    NewtonMaterialSetDefaultCollidable(m_world, m_weaponsGroupe, m_itemGroupe, false);
}

MaterialManager::~MaterialManager()
{
    NewtonMaterialDestroyAllGroupID(m_world);
}

void MaterialManager::SetGhost(tbe::scene::NewtonNode* body, bool state)
{
    if(state)
    {
        if(m_ghostState.count(body))
            return;

        int newtonMaterial = NewtonBodyGetMaterialGroupID(body->GetBody());
        m_ghostState[body] = newtonMaterial;
        NewtonBodySetMaterialGroupID(body->GetBody(), m_ghostGroupe);
    }

    else
    {
        if(!m_ghostState.count(body))
            return;

        NewtonBodySetMaterialGroupID(body->GetBody(), m_ghostState[body]);
        m_ghostState.erase(body);
    }
}

void MaterialManager::AddDynamic(tbe::scene::NewtonNode * body)
{
    NewtonBodySetMaterialGroupID(body->GetBody(), m_dynamicGroupe);
}

void MaterialManager::AddStatic(tbe::scene::NewtonNode * body)
{
    NewtonBodySetMaterialGroupID(body->GetBody(), m_staticGroupe);
}

void MaterialManager::AddWeapon(tbe::scene::NewtonNode * body)
{
    NewtonBodySetMaterialGroupID(body->GetBody(), m_weaponsGroupe);
}

void MaterialManager::AddPlayer(tbe::scene::NewtonNode * body)
{
    NewtonBodySetMaterialGroupID(body->GetBody(), m_playersGroupe);
}

void MaterialManager::AddItem(tbe::scene::NewtonNode * body)
{
    NewtonBodySetMaterialGroupID(body->GetBody(), m_itemGroupe);
}

int MaterialManager::GetPlayersGroupe() const
{
    return m_playersGroupe;
}

int MaterialManager::GetWeaponsGroupe() const
{
    return m_weaponsGroupe;
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
