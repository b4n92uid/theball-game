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

DynamicObject::DynamicObject(GameManager* gameManager, tbe::Matrix4 matrix) : Object(gameManager)
{
    m_matrix = matrix;
}

DynamicObject::~DynamicObject()
{
}

DYJumper::DYJumper(GameManager* gameManager, tbe::Matrix4 matrix) : DynamicObject(gameManager, matrix)
{
    open(OBJ_JUMPER);

    m_physicBody->buildConvexNode(m_hardwareBuffer.getAllVertex(), 0);

    m_particles = new BurningEmitter(gameManager->parallelscene.particles);

    m_particles->setPos(m_aabb.min * 0.75f);
    m_particles->setBoxSize(m_aabb.max * 1.25f);
    m_particles->setTexture(PARTICLE_JUMPER);
    m_particles->setFreeMove(0);

    m_particles->setGravity(m_worldSettings.jumperGravity);
    m_particles->setLifeInit(m_worldSettings.jumperLifeInit);
    m_particles->setLifeDown(m_worldSettings.jumperLifeDown);
    m_particles->setNumber(m_worldSettings.jumperNumber);

    m_particles->setContinousMode(true);

    m_particles->build();

    m_particles->setParent(this);
}

DYJumper::~DYJumper()
{
}

Object* DYJumper::cloneToObject()
{
    DYJumper* dj = new DYJumper(m_gameManager, m_matrix);
    m_gameManager->registerDynamic(dj);
    return dj;
}

void DYJumper::outputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=DYNAMIC" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "action=JUMPER" << endl;
    stream << endl;
}

void DYJumper::interactWith(Player* player)
{
    Vector3f velocity = player->getPhysicBody()->getVelocity();

    velocity.y = 0;

    player->getPhysicBody()->setVelocity(velocity);

    NewtonBodyAddImpulse(player->getPhysicBody()->getBody(),
                         Vector3f(0, 32, 0), player->getPos());

    m_soundManager->play("jumper", player);
}

DYTeleporter::DYTeleporter(GameManager* gameManager, tbe::Matrix4 matrix) : DynamicObject(gameManager, matrix)
{
    open(OBJ_TELEPORTER);

    m_physicBody->buildConvexNode(m_hardwareBuffer.getAllVertex(), 0);

    m_particles = new BurningEmitter(gameManager->parallelscene.particles);
    m_particles->setPos(m_aabb.min * 0.75f);
    m_particles->setBoxSize(m_aabb.max * 1.25f);
    m_particles->setTexture(PARTICLE_TELEPORTER);
    m_particles->setFreeMove(0);

    m_particles->setLifeInit(m_worldSettings.teleporterLifeInit);
    m_particles->setLifeDown(m_worldSettings.teleporterLifeDown);
    m_particles->setNumber(m_worldSettings.teleporterNumber);
    m_particles->setGravity(m_worldSettings.teleporterGravity);

    m_particles->setContinousMode(true);

    m_particles->build();

    m_particles->setParent(this);
}

DYTeleporter::~DYTeleporter()
{
}

Object* DYTeleporter::cloneToObject()
{
    DYTeleporter* dt = new DYTeleporter(m_gameManager, m_matrix);
    m_gameManager->registerDynamic(dt);
    return dt;
}

void DYTeleporter::outputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=DYNAMIC" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "action=TELEPORTER" << endl;
    stream << endl;
}

void DYTeleporter::interactWith(Player* player)
{
    player->toNextSpawnPos();
    m_soundManager->play("teleport", player);
}
