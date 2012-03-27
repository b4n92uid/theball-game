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

    string physic = "convex";

    if(body->hasUserData("physic"))
        physic = body->getUserData("physic").getValue<string > ();

    Vector3f size = body->getAabb().getSize() / 2.0f;

    if(body->hasUserData("size"))
        size.fromStr(body->getUserData("size").getValue<string > ());

    // Pour les corp convex a un seul coté
    if(physic == "convex" and body->getHardwareBuffer()->getVertexCount() < 18)
    {
        physic = "box";

        AABB aabb = body->getAabb();

        if(math::isZero(aabb.min.x) and math::isZero(aabb.max.x))
        {
            aabb.max.x = 0.001;
            aabb.min.x = -0.01;
        }
        if(math::isZero(aabb.min.y) and math::isZero(aabb.max.y))
        {
            aabb.max.y = 0.001;
            aabb.min.y = -0.001;
        }
        if(math::isZero(aabb.min.z) and math::isZero(aabb.max.z))
        {
            aabb.max.z = 0.001;
            aabb.min.z = -0.001;
        }

        size = aabb.getSize() / 2.0f;
    }

    float masse = 0;

    if(body->hasUserData("masse"))
        masse = tools::strToNum<float>(body->getUserData("masse").getValue<string > ());

    if(physic != "ghost")
    {
        m_physicBody = new scene::NewtonNode(gameManager->parallelscene.newton);
        m_physicBody->setUpdatedMatrix(&body->getMatrix());

        if(physic == "box")
            m_physicBody->buildBoxNode(size, masse);

        else if(physic == "sphere")
            m_physicBody->buildSphereNode(size, masse);

        else if(physic == "convex")
            m_physicBody->buildConvexNode(body, masse);

        else if(physic == "tree")
            m_physicBody->buildTreeNode(body);

        else
            throw Exception("StaticElement::StaticElement; unable to build (%s) physics body", physic.c_str());

        body->addChild(m_physicBody);

        NewtonBodySetForceAndTorqueCallback(m_physicBody->getBody(), MapElement::applyForceAndTorqueCallback);
        NewtonBodySetUserData(m_physicBody->getBody(), this);
        NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
    }
}

StaticElement::~StaticElement()
{

}

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

    Vertex* vs = hardbuf->lock();

    if(enable)
        for(unsigned i = 0; i < hardbuf->getVertexCount(); i++)
            vs[i].color.w = alpha;
    else
        for(unsigned i = 0; i < hardbuf->getVertexCount(); i++)
            vs[i].color.w = 1;

    hardbuf->unlock();

    Material::Array mats = m_visualBody->getAllMaterial();

    if(enable)
        for(unsigned i = 0; i < mats.size(); i++)
            mats[i]->enable(Material::BLEND_ADD);
    else
        for(unsigned i = 0; i < mats.size(); i++)
            mats[i]->disable(Material::BLEND_ADD);
}

void StaticElement::makeLighted(bool enable)
{
    using namespace tbe;
    using namespace scene;

    if(enable)
        foreach(Material * mat, m_visualBody->getAllMaterial())
        mat->enable(Material::LIGHTED);

    else
        foreach(Material* mat, m_visualBody->getAllMaterial())
        mat->disable(Material::LIGHTED);
}

tbe::scene::Mesh* StaticElement::getVisualBody()
{
    return m_visualBody;
}
