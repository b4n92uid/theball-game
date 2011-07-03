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
#include "BulletWeapon.h"
#include "Item.h"
#include "MapElement.h"

class GameManager;

class MaterialManager
{
public:
    MaterialManager(GameManager* playManager);
    virtual ~MaterialManager();

    void setGhost(MapElement* body, bool state);

    void addBullet(Bullet* body);
    void addPlayer(Player* body);
    void addElement(MapElement* body);
    void addItem(Item* body);

    int getPlayersGroupe() const;
    int getWeaponsGroupe() const;
    int getItemGroupe() const;
    int getElementsGroupe() const;

protected:

    void mBulletOnMapContactsProcess(const NewtonJoint* contact, dFloat, int);
    int mBulletOnPlayerAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int);
    int mPlayerOnItemsAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int);
    void mPlayerOnStaticContactsProcess(const NewtonJoint* contact, dFloat timestep, int threadIndex);

    static MaterialManager* getMaterialManager(const NewtonJoint* contact)
    {
        NewtonBody* b0 = NewtonJointGetBody0(contact);
        NewtonBody* b1 = NewtonJointGetBody1(contact);

        if(b0 && b1)
            return getMaterialManager(b0, b1);
        else
            return NULL;
    }

    static MaterialManager* getMaterialManager(const NewtonBody* body0, const NewtonBody* body1)
    {
        int g0 = NewtonBodyGetMaterialGroupID(body0);
        int g1 = NewtonBodyGetMaterialGroupID(body1);

        return (MaterialManager*)NewtonMaterialGetUserData(NewtonBodyGetWorld(body0), g0, g1);
    }

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

    static void PlayerOnStaticContactsProcess(const NewtonJoint* contact, dFloat timestep, int threadIndex)
    {
        MaterialManager* matmanager = getMaterialManager(contact);

        if(matmanager)
            matmanager->mPlayerOnStaticContactsProcess(contact, timestep, threadIndex);
    }

protected:
    NewtonWorld* m_world;
    GameManager* m_gameManager;
    int m_elementsGroupe;
    int m_bulletGroupe;
    int m_playersGroupe;
    int m_itemGroupe;
    int m_ghostGroupe;

    std::map<tbe::scene::NewtonNode*, int> m_ghostState;
};

#endif	/* _MATERIALMANAGER_H */

