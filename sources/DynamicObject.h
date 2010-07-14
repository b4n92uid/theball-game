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
    DynamicObject(GameManager* gameManager, tbe::Matrix4f matrix);
    virtual ~DynamicObject();

    /**
     * Fonction virutal pure pour routine d'intercation
     * de l'objet dynamique avec a un joueur
     */
    virtual void InteractWith(Player* player) = 0;

    typedef std::map<std::string, DynamicObject*> Map;
    typedef std::vector<DynamicObject*> Array;

protected:
    tbe::ticks::Clock clock;
};

class DYJumper : public DynamicObject
{
public:
    DYJumper(GameManager* gameManager, tbe::Matrix4f pos);
    ~DYJumper();
    Object* Clone();
    void OutputConstruction(std::ofstream& file);
    void InteractWith(Player* player);
    void Process();


protected:
    tbe::scene::ParticlesEmiter* m_particles;
};

class DYTeleporter : public DynamicObject
{
public:
    DYTeleporter(GameManager* gameManager, tbe::Matrix4f pos);
    ~DYTeleporter();
    Object* Clone();
    void OutputConstruction(std::ofstream& file);
    void InteractWith(Player* player);
    void Process();

protected:
    tbe::scene::ParticlesEmiter* m_particles;
};

#endif	/* _DYNAMICOBJECT_H */
