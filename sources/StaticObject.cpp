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

StaticObject::StaticObject(GameManager* gameManager, std::string path, tbe::Matrix4f matrix) : Object(gameManager)
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
    StaticObject* so = new StaticObject(m_gameManager, m_filepath, m_matrix);
    m_gameManager->RegisterStatic(so);
    return so;
}

void StaticObject::OutputConstruction(std::ofstream& file)
{
    using namespace std;

    file << "+node" << endl;
    file << "type=STATIC" << endl;
    file << "matrix=" << m_matrix << endl;
    file << "modelPath=" << m_filepath << endl;
    file << endl;
}

StaticObject::~StaticObject()
{
}
