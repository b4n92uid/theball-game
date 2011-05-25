/* 
 * File:   DynamicObject.h
 * Author: b4n92uid
 *
 * Created on 7 juin 2010, 23:59
 */

#ifndef _DYNAMICOBJECT_H
#define	_DYNAMICOBJECT_H

#include "Object.h"
#include "Player.h"
#include "Item.h"

class DynamicObject : public Object
{
public:
    DynamicObject(GameManager* gameManager, tbe::Matrix4 matrix);
    virtual ~DynamicObject();

    /**
     * Fonction virutal pure pour routine d'intercation
     * de l'objet dynamique avec a un joueur
     */
    virtual void interactWith(Player* player) = 0;

    typedef std::map<std::string, DynamicObject*> Map;
    typedef std::vector<DynamicObject*> Array;

protected:
    tbe::ticks::Clock clock;
};

class DYJumper : public DynamicObject
{
public:
    DYJumper(GameManager* gameManager, tbe::Matrix4 pos);
    ~DYJumper();
    Object* cloneToObject();
    void outputConstruction(std::iostream& stream);
    void interactWith(Player* player);


protected:
    tbe::scene::BurningEmitter* m_particles;
};

class DYTeleporter : public DynamicObject
{
public:
    DYTeleporter(GameManager* gameManager, tbe::Matrix4 pos);
    ~DYTeleporter();
    Object* cloneToObject();
    void outputConstruction(std::iostream& stream);
    void interactWith(Player* player);

protected:
    tbe::scene::BurningEmitter* m_particles;
};

#endif	/* _DYNAMICOBJECT_H */
