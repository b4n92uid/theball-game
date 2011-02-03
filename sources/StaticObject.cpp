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

    Open(path);

    Material::Array materials = GetAllMaterial();

    for(unsigned i = 0; i < materials.size(); i++)
        materials[i]->Enable(Material::BACKFACE_CULL);

    m_physicBody->BuildTreeNode(m_hardwareBuffer.GetAllFace());
}

Object* StaticObject::CloneToObject()
{
    StaticObject* so = new StaticObject(m_gameManager, m_filename, m_matrix);
    m_gameManager->RegisterStatic(so);
    return so;
}

void StaticObject::OutputConstruction(std::iostream& stream)
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
