/*
 * File:   UserEventManager.cpp
 * Author: b4n92uid
 *
 * Created on 16 décembre 2009, 21:43
 */

#include "UserControl.h"

#include "GameManager.h"
#include "Player.h"

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

void UserControl::process(Player* player)
{
    scene::Camera* camera = m_playManager->manager.scene->getCurCamera();

    // Move
    Vector3f force;

    if(isActionStateDown("forward"))
        force += camera->getTarget();
    if(isActionStateDown("backward"))
        force -= camera->getTarget();
    if(isActionStateDown("strafLeft"))
        force += camera->getLeft();
    if(isActionStateDown("strafRight"))
        force -= camera->getLeft();

    if(!math::isZero(force))
        player->move(force);

    // Jump
    if(isActionStateDown("jump"))
        player->jump();

    // Change weapons
    if(isActionStateDown("switchUpWeapon"))
    {
        if(m_eventManager->keyState[EventManager::KEY_LCTRL])
            player->switchUpPower();
        else
            player->switchUpWeapon();
    }

    if(isActionStateDown("switchDownWeapon"))
    {
        if(m_eventManager->keyState[EventManager::KEY_LCTRL])
            player->switchDownPower();
        else
            player->switchDownWeapon();
    }

    // Shoot
    if(isActionStateDown("shoot"))
        player->shoot(m_playManager->getShootTarget());

    // Power
    if(isActionJustDown("power"))
        player->power(true, m_playManager->getShootTarget());

    else if(isActionJustUp("power"))
        player->power(false, m_playManager->getShootTarget());
}
