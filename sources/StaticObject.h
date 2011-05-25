/* 
 * File:   StaticObject.h
 * Author: b4n92uid
 *
 * Created on 7 juin 2010, 23:58
 */

#ifndef _STATICOBJECT_H
#define	_STATICOBJECT_H

#include "Object.h"

class StaticObject : public Object
{
public:
    StaticObject(GameManager* gameManager, std::string path, tbe::Matrix4 pos);
    ~StaticObject();

    Object* cloneToObject();

    void outputConstruction(std::iostream& stream);

    typedef std::map<std::string, StaticObject*> Map;
    typedef std::vector<StaticObject*> Array;

    static bool isCollidWithStaticWorld(tbe::scene::NewtonNode* body, Array staticObjects)
    {
        for(unsigned i = 0; i < staticObjects.size(); i++)
            if(body->isCollidWith(staticObjects[i]->getPhysicBody()))
                return true;

        return false;
    }
};

#endif	/* _STATICOBJECT_H */
