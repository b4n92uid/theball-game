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
#include "MapElement.h"

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
    void addDummy(MapElement* body);

    int getPlayersGroupe() const;
    int getWeaponsGroupe() const;
    int getElementsGroupe() const;

protected:

    int mBulletOnPlayerAABBOverlape(const NewtonMaterial*, const NewtonBody*, const NewtonBody*, int);
    int mPlayerOnDummyAABBOverlape(const NewtonMaterial*, const NewtonBody*, const NewtonBody*, int);

    void mBulletOnMapContactsProcess(const NewtonJoint* contact, dFloat, int);
    void mPlayerOnStaticContactsProcess(const NewtonJoint* contact, dFloat, int);

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

        return(MaterialManager*)NewtonMaterialGetUserData(NewtonBodyGetWorld(body0), g0, g1);
    }

    static int BulletOnPlayerAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int i)
    {
        MaterialManager* matmanager = getMaterialManager(body0, body1);

        if(matmanager)
            return matmanager->mBulletOnPlayerAABBOverlape(material, body0, body1, i);
        else
            return 0;
    }

    static int PlayerOnDummyAABBOverlape(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int i)
    {
        MaterialManager* matmanager = getMaterialManager(body0, body1);

        if(matmanager)
            return matmanager->mPlayerOnDummyAABBOverlape(material, body0, body1, i);
        else
            return 0;
    }

    static void BulletOnMapContactsProcess(const NewtonJoint* contact, dFloat f, int i)
    {
        MaterialManager* matmanager = getMaterialManager(contact);

        if(matmanager)
            matmanager->mBulletOnMapContactsProcess(contact, f, i);
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
    int m_immunityGroupe;
    int m_ghostGroupe;
    int m_dummyGroupe;

    std::map<tbe::scene::NewtonNode*, int> m_ghostState;
    std::map<tbe::scene::NewtonNode*, int> m_immunityState;
};

#endif	/* _MATERIALMANAGER_H */

