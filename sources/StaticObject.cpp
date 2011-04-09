/*
 * File:   StaticObject.cpp
 * Author: b4n92uid
 *
 * Created on 7 juin 2010, 23:58
 */

#include "StaticObject.h"

#include "GameManager.h"
#include "SoundManager.h"

#include "Player.h"
#include "Item.h"
#include "Define.h"

using namespace tbe;
using namespace tbe::scene;

StaticObject::StaticObject(GameManager* gameManager, std::string path,
                           tbe::Matrix4f matrix = tbe::Matrix4f()) : Object(gameManager)
{
    m_matrix = matrix;

    open(path);

    Material::Array materials = getAllMaterial();

    for(unsigned i = 0; i < materials.size(); i++)
        materials[i]->enable(Material::BACKFACE_CULL);

    m_physicBody->buildTreeNode(m_hardwareBuffer.getAllFace());
}

Object* StaticObject::cloneToObject()
{
    StaticObject* so = new StaticObject(m_gameManager, m_filename, m_matrix);
    m_gameManager->registerStatic(so);
    return so;
}

void StaticObject::outputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=STATIC" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "modelPath=" << m_filename << endl;
    stream << endl;
}

StaticObject::~StaticObject()
{
}
