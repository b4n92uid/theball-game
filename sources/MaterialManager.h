/* 
 * File:   MaterialManager.h
 * Author: b4n92uid
 *
 * Created on 10 novembre 2009, 15:04
 */

#ifndef _MATERIALMANAGER_H
#define	_MATERIALMANAGER_H

#include <Newton/Newton.h>

class GameManager;

class MaterialManager
{
public:
    MaterialManager(GameManager* playManager);
    virtual ~MaterialManager();

    void SetGhost(tbe::scene::NewtonNode* body, bool state);

    void AddWeapon(tbe::scene::NewtonNode* body);
    void AddPlayer(tbe::scene::NewtonNode* body);
    void AddStatic(tbe::scene::NewtonNode* body);
    void AddDynamic(tbe::scene::NewtonNode* body);
    void AddItem(tbe::scene::NewtonNode* body);

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
    int m_weaponsGroupe;
    int m_playersGroupe;
    int m_itemGroupe;
    int m_ghostGroupe;

    std::map<tbe::scene::NewtonNode*, int> m_ghostState;
};

#endif	/* _MATERIALMANAGER_H */

