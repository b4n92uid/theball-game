/* 
 * File:   Object.h
 * Author: b4n92uid
 *
 * Created on 7 juin 2010, 23:28
 */

#ifndef _OBJECT_H
#define	_OBJECT_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>

#include "AppManager.h"

class SoundManager;
class GameManager;

class Object : public tbe::scene::OBJMesh
{
public:
    Object(GameManager* gameManager);
    ~Object();

    /// Clone l'objet dériver de cette class
    virtual Object* CloneToObject() = 0;

    /**
     * Ecris sur le flux les instruction de construction
     * pour les besoin de la class BldDParser
     */
    virtual void OutputConstruction(std::iostream& stream) = 0;

    void SetPhysicBody(tbe::scene::NewtonNode* physicBody);

    tbe::scene::NewtonNode* GetPhysicBody() const;

    typedef std::map<std::string, Object*> Map;
    typedef std::vector<Object*> Array;

protected:
    GameManager* m_gameManager;
    SoundManager* m_soundManager;
    Settings::Physics m_worldSettings;
    tbe::scene::NewtonNode* m_physicBody;
};

#endif	/* _OBJECT_H */
