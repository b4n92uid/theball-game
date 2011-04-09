/* 
 * File:   MaterialManager.h
 * Author: b4n92uid
 *
 * Created on 10 novembre 2009, 15:04
 */

#ifndef _MATERIALMANAGER_H
#define	_MATERIALMANAGER_H

#include <NewtonBall/NewtonBall.h>

#include "Weapon.h"
#include "Item.h"
#include "StaticObject.h"
#include "DynamicObject.h"

class GameManager;

class MaterialManager
{
public:
    MaterialManager(GameManager* playManager);
    virtual ~MaterialManager();

    void setGhost(Object* body, bool state);

    void addBullet(Bullet* body);
    void addPlayer(Player* body);
    void addStatic(StaticObject* body);
    void addDynamic(DynamicObject* body);
    void addItem(Item* body);

    int getPlayersGroupe() const;
    int getWeaponsGroupe() const;
    int getItemGroupe() const;
    int getDynamicGroupe() const;
    int getStaticGroupe() const;

protected:

    template<typename T> static T getUserData(const NewtonBody* body)
    {
        tbe::scene::NewtonNode* nn = static_cast<tbe::scene::NewtonNode*>(NewtonBodyGetUserData(body));
        return dynamic_cast<T>(nn);
    }

    template<typename T> static T getParentUserData(const NewtonBody* body)
    {
        tbe::scene::NewtonNode* nn = static_cast<tbe::scene::NewtonNode*>(NewtonBodyGetUserData(body));
        return dynamic_cast<T>(nn->getParent());
    }

    static MaterialManager* getMaterialManager(const NewtonJoint* contact)
    {
        return getMaterialManager(NewtonJointGetBody0(contact),
                                  NewtonJointGetBody1(contact));
    }

    static MaterialManager* getMaterialManager(const NewtonBody* body0, const NewtonBody* body1)
    {
        return (MaterialManager*)NewtonMaterialGetUserData(NewtonBodyGetWorld(body0),
                                                           NewtonBodyGetMaterialGroupID(body0),
                                                           NewtonBodyGetMaterialGroupID(body1));
    }

    void mBulletOnMapContactsProcess(const NewtonJoint* contact, dFloat, int);
    int mBulletOnPlayerAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int);
    int mPlayerOnItemsAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int);
    void mPlayerOnDynamicContactsProcess(const NewtonJoint* contact, dFloat, int);
    void mPlayerOnStaticContactsProcess(const NewtonJoint* contact, dFloat timestep, int threadIndex);

    static void BulletOnMapContactsProcess(const NewtonJoint* contact, dFloat f, int i)
    {
        MaterialManager* matmanager = getMaterialManager(contact);

        if(matmanager)
            matmanager->mBulletOnMapContactsProcess(contact, f, i);
    }

    static int BulletOnPlayerAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int i)
    {
        MaterialManager* matmanager = getMaterialManager(body0, body1);

        if(matmanager)
            return matmanager->mBulletOnPlayerAABBOverlape(material, body0, body1, i);
        else
            return 0;
    }

    static int PlayerOnItemsAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int i)
    {
        MaterialManager* matmanager = getMaterialManager(body0, body1);

        if(matmanager)
            return matmanager->mPlayerOnItemsAABBOverlape(material, body0, body1, i);
        else
            return 0;
    }

    static void PlayerOnDynamicContactsProcess(const NewtonJoint* contact, dFloat timestep, int threadIndex)
    {
        MaterialManager* matmanager = getMaterialManager(contact);

        if(matmanager)
            matmanager->mPlayerOnDynamicContactsProcess(contact, timestep, threadIndex);
    }

    static void PlayerOnStaticContactsProcess(const NewtonJoint* contact, dFloat timestep, int threadIndex)
    {
        MaterialManager* matmanager = getMaterialManager(contact);

        if(matmanager)
            matmanager->mPlayerOnStaticContactsProcess(contact, timestep, threadIndex);
    }

protected:
    NewtonWorld* m_world;
    GameManager* m_gameManager;
    int m_staticGroupe;
    int m_dynamicGroupe;
    int m_bulletGroupe;
    int m_playersGroupe;
    int m_itemGroupe;
    int m_ghostGroupe;

    std::map<tbe::scene::NewtonNode*, int> m_ghostState;
};

#endif	/* _MATERIALMANAGER_H */

