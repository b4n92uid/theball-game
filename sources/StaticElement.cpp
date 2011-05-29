/*
 * File:   StaticElement.cpp
 * Author: b4n92uid
 *
 * Created on 28 mai 2011, 11:43
 */

#include "StaticElement.h"

#include "GameManager.h"
#include "SoundManager.h"

using namespace std;
using namespace tbe;

StaticElement::StaticElement(GameManager* gameManager, tbe::scene::Mesh* body)
: MapElement(gameManager)
{
    MapElement::m_visualBody = m_visualBody = body;

    m_id = body->getName();

    m_physicBody = new scene::NewtonNode(gameManager->parallelscene.newton);
    m_physicBody->setUpdatedMatrix(&body->getMatrix());
    m_physicBody->buildTreeNode(body->getHardwareBuffer().getAllFace());
    body->addChild(m_physicBody);

    NewtonBodySetUserData(m_physicBody->getBody(), this);
}

StaticElement::~StaticElement()
{

}

