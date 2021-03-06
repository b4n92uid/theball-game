/*
 * File:   StaticElement.cpp
 * Author: b4n92uid
 *
 * Created on 28 mai 2011, 11:43
 */

#include "StaticElement.h"

#include "GameManager.h"
#include "SoundManager.h"
#include "MaterialManager.h"

using namespace std;
using namespace tbe;

StaticElement::StaticElement(GameManager* gameManager, tbe::scene::Mesh* body)
: MapElement(gameManager)
{
    MapElement::m_visualBody = m_visualBody = body;

    m_initialMatrix = body->getMatrix();

    m_id = body->getName();

    string physic = "tree";
    float masse = 0;

    if(body->hasUserData("masse"))
        masse = tools::strToNum<float>(body->getUserData("masse").getValue<string > ());

    if(body->hasUserData("physic"))
        physic = body->getUserData("physic").getValue<string > ();

    // Calculer la boite englobante en prenant compte le scale
    Vector3f size = body->getAabb().getSize() / 2.0f;

    if(body->hasUserData("size"))
        size.fromStr(body->getUserData("size").getValue<string > ());

    if(physic != "ghost" && body->getHardwareBuffer()->getVertexCount() >= 24)
    {
        m_physicBody = new scene::NewtonNode(gameManager->parallelscene.newton);
        m_physicBody->setMatrix(body->getMatrix());
        
        if(physic == "box")
            m_physicBody->buildBoxNode(size, masse);

        else if(physic == "sphere")
            m_physicBody->buildSphereNode(size, masse);

        else if(physic == "convex")
            m_physicBody->buildConvexNode(body, masse);

        else if(physic == "tree")
            m_physicBody->buildTreeNode(body);

        else
            throw Exception("StaticElement::StaticElement; unable to build (%1%) physics body") % physic;

        if(physic != "tree")
        {
            NewtonBodySetForceAndTorqueCallback(m_physicBody->getBody(), MapElement::applyForceAndTorqueCallback);
            NewtonBodySetTransformCallback(m_physicBody->getBody(), MapElement::applyTransformCallback);
        }

        NewtonBodySetUserData(m_physicBody->getBody(), this);
        NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
    }
}

StaticElement::~StaticElement() { }

void StaticElement::setGhost(bool enable)
{
    makeTransparent(enable, 0.25);
    m_gameManager->manager.material->setGhost(this, enable);
}

void StaticElement::makeTransparent(bool enable, float alpha)
{
    using namespace tbe;
    using namespace scene;

    HardwareBuffer* hardbuf = m_visualBody->getHardwareBuffer();

    Vertex* vs = hardbuf->bindBuffer().lock();

    if(enable)
        for(unsigned i = 0; i < hardbuf->getVertexCount(); i++)
            vs[i].color.w = alpha;
    else
        for(unsigned i = 0; i < hardbuf->getVertexCount(); i++)
            vs[i].color.w = 1;

    hardbuf->unlock().unbindBuffer();

    if(enable)
    {
        foreach(SubMesh* m, m_visualBody->getAllSubMesh())
            m->getMaterial()->enable(Material::ADDITIVE);
    }

    else
    {
        foreach(SubMesh* m, m_visualBody->getAllSubMesh())
            m->getMaterial()->disable(Material::ADDITIVE);
    }
}

void StaticElement::makeLighted(bool enable)
{
    using namespace tbe;
    using namespace scene;

    if(enable)
    {
        foreach(SubMesh* m, m_visualBody->getAllSubMesh())
            m->getMaterial()->enable(Material::LIGHTED);
    }

    else
    {
        foreach(SubMesh* m, m_visualBody->getAllSubMesh())
            m->getMaterial()->disable(Material::LIGHTED);
    }
}

tbe::scene::Mesh* StaticElement::getVisualBody()
{
    return m_visualBody;
}
