/* 
 * File:   MaterialManager.h
 * Author: b4n92uid
 *
 * Created on 10 novembre 2009, 15:04
 */

#ifndef _MATERIALMANAGER_H
#define	_MATERIALMANAGER_H

#include <Newton/Newton.h>

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

    void SetGhost(Object* body, bool state);

    void AddBullet(Bullet* body);
    void AddPlayer(Player* body);
    void AddStatic(StaticObject* body);
    void AddDynamic(DynamicObject* body);
    void AddItem(Item* body);

    int GetPlayersGroupe() const;
    int GetWeaponsGroupe() const;
    int GetItemGroupe() const;
    int GetDynamicGroupe() const;
    int GetStaticGroupe() const;

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

