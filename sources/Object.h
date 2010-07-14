/* 
 * File:   Object.h
 * Author: b4n92uid
 *
 * Created on 7 juin 2010, 23:28
 */

#ifndef _OBJECT_H
#define	_OBJECT_H

#include <Tbe.h>
#include <Newton/Newton.h>

#include "AppManager.h"

class SoundManager;
class GameManager;

class Object :
public tbe::scene::OBJMesh,
public tbe::scene::NewtonNode
{
public:
    Object(GameManager* gameManager);
    ~Object();

    /// Clone l'objet dériver de cette class
    virtual Object* Clone() = 0;
    
    /**
     * Ecris sur le flux les instruction de construction
     * pour les besoin de la class BldDParser
     */
    virtual void OutputConstruction(std::ofstream& file) = 0;

    typedef std::map<std::string, Object*> Map;
    typedef std::vector<Object*> Array;

protected:
    GameManager* m_gameManager;
    SoundManager* m_soundManager;
    Settings::Physics m_worldSettings;
};

#endif	/* _OBJECT_H */
