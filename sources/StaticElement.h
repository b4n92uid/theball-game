/*
 * File:   StaticElement.h
 * Author: b4n92uid
 *
 * Created on 28 mai 2011, 11:43
 */

#ifndef STATICELEMENT_H
#define	STATICELEMENT_H

#include "MapElement.h"

class StaticElement : public MapElement
{
public:
    StaticElement(GameManager* gameManager, tbe::scene::Mesh* body);
    virtual ~StaticElement();

private:
    tbe::scene::Mesh* m_visualBody;
};

#endif	/* STATICELEMENT_H */

