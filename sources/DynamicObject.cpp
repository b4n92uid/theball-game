/*
 * File:   DynamicObject.cpp
 * Author: b4n92uid
 *
 * Created on 7 juin 2010, 23:59
 */

#include "DynamicObject.h"

#include "GameManager.h"
#include "SoundManager.h"

#include "Player.h"
#include "Item.h"
#include "Define.h"
#include "PlayManager.h"

using namespace tbe;
using namespace tbe::scene;

DynamicObject::DynamicObject(GameManager* gameManager, tbe::Matrix4f matrix) : Object(gameManager)
{
    m_matrix = matrix;
}

DynamicObject::~DynamicObject()
{
}

DYJumper::DYJumper(GameManager* gameManager, tbe::Matrix4f matrix) : DynamicObject(gameManager, matrix)
{
    Open(OBJ_JUMPER);

    m_physicBody->BuildConvexNode(m_hardwareBuffer.GetAllVertex(), 0);

    m_particles = new ParticlesEmiter;
    m_particles->SetPos(m_aabb.min * 0.75f);
    m_particles->SetEndPos(m_aabb.max * 1.25f);
    m_particles->SetGravity(Vector3f(0, 0.001, 0));
    m_particles->SetFreeMove(0);
    m_particles->SetTexture(PARTICLE_JUMPER);
    m_particles->SetLifeInit(2);
    m_particles->SetLifeDown(0.05);
    m_particles->SetNumber(64);
    m_particles->SetContinousMode(true);
    m_particles->Build();

    m_particles->SetParent(this);

    m_gameManager->parallelscene.particles->AddChild(m_particles);
}

DYJumper::~DYJumper()
{
}

Object* DYJumper::CloneToObject()
{
    DYJumper* dj = new DYJumper(m_gameManager, m_matrix);
    m_gameManager->RegisterDynamic(dj);
    return dj;
}

void DYJumper::OutputConstruction(std::ofstream& file)
{
    using namespace std;

    file << "+node" << endl;
    file << "type=DYNAMIC" << endl;
    file << "matrix=" << m_matrix << endl;
    file << "action=JUMPER" << endl;
    file << endl;
}

void DYJumper::InteractWith(Player* player)
{
    Vector3f velocity = player->GetPhysicBody()->GetVelocity();

    velocity.y = 0;

    player->GetPhysicBody()->SetVelocity(velocity);

    NewtonBodyAddImpulse(player->GetPhysicBody()->GetBody(),
                         Vector3f(0, 32, 0), player->GetPos());

    m_soundManager->Play("jumper", player);
}

DYTeleporter::DYTeleporter(GameManager* gameManager, tbe::Matrix4f matrix) : DynamicObject(gameManager, matrix)
{
    Open(OBJ_TELEPORTER);

    m_physicBody->BuildConvexNode(m_hardwareBuffer.GetAllVertex(), 0);

    m_particles = new ParticlesEmiter;
    m_particles->SetPos(m_aabb.min * 0.75f);
    m_particles->SetEndPos(m_aabb.max * 1.25f);
    m_particles->SetFreeMove(0);
    m_particles->SetTexture(PARTICLE_TELEPORTER);
    m_particles->SetLifeInit(2);
    m_particles->SetLifeDown(0.05);
    m_particles->SetNumber(64);
    m_particles->SetContinousMode(true);
    m_particles->Build();

    m_particles->SetParent(this);

    m_gameManager->parallelscene.particles->AddChild(m_particles);
}

DYTeleporter::~DYTeleporter()
{
}

Object* DYTeleporter::CloneToObject()
{
    DYTeleporter* dt = new DYTeleporter(m_gameManager, m_matrix);
    m_gameManager->RegisterDynamic(dt);
    return dt;
}

void DYTeleporter::OutputConstruction(std::ofstream& file)
{
    using namespace std;

    file << "+node" << endl;
    file << "type=DYNAMIC" << endl;
    file << "matrix=" << m_matrix << endl;
    file << "action=TELEPORTER" << endl;
    file << endl;
}

void DYTeleporter::InteractWith(Player* player)
{
    player->SetRandomPosInTheFloor();
    m_soundManager->Play("teleport", player);
}
