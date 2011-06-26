/*
 * File:   UserEventManager.cpp
 * Author: b4n92uid
 *
 * Created on 16 décembre 2009, 21:43
 */

#include "UserControl.h"

#include "GameManager.h"

#include "Player.h"
#include "Item.h"

#include "Define.h"

using namespace tbe;

typedef std::map<std::string, int> StateMap;

UserControl::UserControl(GameManager* playManager) : Controller(playManager)
{
    m_control = m_playManager->manager.app->globalSettings.control;
    m_eventManager = m_playManager->manager.app->getEventMng();
}

UserControl::~UserControl()
{
}

void UserControl::setActionStateDown(std::string name, bool state)
{
    StateMap::iterator kit = m_control.keyboard.find(name);
    StateMap::iterator mit = m_control.mouse.find(name);

    if(kit != m_control.keyboard.end())
        m_eventManager->keyState[kit->second] = state;

    else if(mit != m_control.mouse.end())
        m_eventManager->mouseState[mit->second] = state;
}

bool UserControl::isActionJustUp(std::string name)
{
    StateMap::iterator kit = m_control.keyboard.find(name);
    StateMap::iterator mit = m_control.mouse.find(name);

    if(kit != m_control.keyboard.end())
        return m_eventManager->lastActiveKey.first == kit->second
            && m_eventManager->notify == EventManager::EVENT_KEY_UP;

    else if(mit != m_control.mouse.end())
        return m_eventManager->lastActiveMouse.first == mit->second
            && m_eventManager->notify == EventManager::EVENT_MOUSE_UP;

    else
        return false;
}

bool UserControl::isActionJustDown(std::string name)
{
    StateMap::iterator kit = m_control.keyboard.find(name);
    StateMap::iterator mit = m_control.mouse.find(name);

    if(kit != m_control.keyboard.end())
        return m_eventManager->lastActiveKey.first == kit->second
            && m_eventManager->notify == EventManager::EVENT_KEY_DOWN;

    else if(mit != m_control.mouse.end())
        return m_eventManager->lastActiveMouse.first == mit->second
            && m_eventManager->notify == EventManager::EVENT_MOUSE_DOWN;

    else
        return false;
}

bool UserControl::isActionStateDown(std::string name)
{
    StateMap::iterator kit = m_control.keyboard.find(name);
    StateMap::iterator mit = m_control.mouse.find(name);

    if(kit != m_control.keyboard.end())
        return m_eventManager->keyState[kit->second];

    else if(mit != m_control.mouse.end())
        return m_eventManager->mouseState[mit->second];

    else
        return false;
}

inline bool playerCanJump(scene::NewtonNode* node1, scene::NewtonNode* node2)
{
    Vector3f contact, normal, penetration;

    int contactPoint = NewtonCollisionCollide(node1->getParallelScene()->getNewtonWorld(), 1,
                                              NewtonBodyGetCollision(node1->getBody()), node1->getMatrix(),
                                              NewtonBodyGetCollision(node2->getBody()), node2->getMatrix(),
                                              contact, normal, penetration, 0);

    return (contactPoint > 0 && Vector3f::dot(normal, Vector3f(0, 1, 0)) > 0.25);
}

void UserControl::process(Player* player)
{
    scene::Camera* playerCam = m_playManager->manager.scene->getCurCamera();

    bool collideWithStatic = false;

    for(unsigned i = 0; i < m_playManager->map.mapElements.size(); i++)
        if(m_playManager->map.mapElements[i]->getPhysicBody())
            if(playerCanJump(player->getPhysicBody(), m_playManager->map.mapElements[i]->getPhysicBody()))
            {
                collideWithStatic = true;
                break;
            }

    // Move
    Vector3f addForce;

    if(isActionStateDown("forward"))
        addForce += playerCam->getTarget() * m_worldSettings.playerMoveSpeed;
    if(isActionStateDown("backward"))
        addForce -= playerCam->getTarget() * m_worldSettings.playerMoveSpeed;
    if(isActionStateDown("strafLeft"))
        addForce += playerCam->getLeft() * m_worldSettings.playerMoveSpeed;
    if(isActionStateDown("strafRight"))
        addForce -= playerCam->getLeft() * m_worldSettings.playerMoveSpeed;

    addForce.y = 0;

    player->getPhysicBody()->setApplyForce(addForce);

    // Jump
    if(collideWithStatic)
        if(isActionStateDown("jump"))
            player->jump();

    if(isActionStateDown("brake"))
        player->brake();

    // Change weapons
    if(isActionStateDown("switchUpWeapon"))
        player->switchUpWeapon();

    if(isActionStateDown("switchDownWeapon"))
        player->switchDownWeapon();

    // Shoot
    if(isActionStateDown("shoot"))
        player->shoot(m_playManager->getShootTarget());

    // Power
    if(isActionJustDown("power"))
        player->power(true, m_playManager->getShootTarget());

    else if(isActionJustUp("power"))
        player->power(false, m_playManager->getShootTarget());
}
