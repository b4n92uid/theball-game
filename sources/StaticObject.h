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
    StaticObject(GameManager* gameManager, std::string path, tbe::Matrix4f pos);
    ~StaticObject();

    Object* CloneToObject();
    
    void OutputConstruction(std::iostream& stream);

    typedef std::map<std::string, StaticObject*> Map;
    typedef std::vector<StaticObject*> Array;
};

#endif	/* _STATICOBJECT_H */
