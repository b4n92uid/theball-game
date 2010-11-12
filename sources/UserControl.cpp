/* 
 * File:   UserEventManager.cpp
 * Author: b4n92uid
 * 
 * Created on 16 décembre 2009, 21:43
 */

#include "UserControl.h"

#include "PlayManager.h"

#include "Player.h"
#include "Item.h"

#include "Define.h"
#include "EditorManager.h"

using namespace tbe;

typedef std::map<std::string, int> StateMap;

UserControl::UserControl(PlayManager* playManager) : Controller(playManager)
{
    m_control = m_playManager->manager.app->globalSettings.control;
    m_eventManager = m_playManager->manager.app->GetEventMng();
}

UserControl::~UserControl()
{
}

void UserControl::SetActionStateDown(std::string name, bool state)
{
    StateMap::iterator kit = m_control.keyboard.find(name);
    StateMap::iterator mit = m_control.mouse.find(name);

    if(kit != m_control.keyboard.end())
        m_eventManager->keyState[kit->second] = state;

    else if(mit != m_control.mouse.end())
        m_eventManager->mouseState[mit->second] = state;
}

bool UserControl::IsActionJustUp(std::string name)
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

bool UserControl::IsActionJustDown(std::string name)
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

bool UserControl::IsActionStateDown(std::string name)
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

void UserControl::Process(Player* player)
{
    scene::Camera* playerCam = m_playManager->manager.scene->GetCurCamera();

    bool collideWithStatic = false;

    for(unsigned i = 0; i < m_playManager->map.staticObjects.size(); i++)
        if(player->GetPhysicBody()->IsCollidWith(m_playManager->map.staticObjects[i]->GetPhysicBody()))
        {
            collideWithStatic = true;
            break;
        }

    // Move
    Vector3f addForce;

    if(IsActionStateDown("forward"))
        addForce += playerCam->GetTarget() * m_worldSettings.playerMoveSpeed;
    if(IsActionStateDown("backward"))
        addForce -= playerCam->GetTarget() * m_worldSettings.playerMoveSpeed;
    if(IsActionStateDown("strafLeft"))
        addForce += playerCam->GetLeft() * m_worldSettings.playerMoveSpeed;
    if(IsActionStateDown("strafRight"))
        addForce -= playerCam->GetLeft() * m_worldSettings.playerMoveSpeed;

    addForce.y = 0;

    player->GetPhysicBody()->SetApplyForce(addForce);

    // Jump
    if(collideWithStatic)
        if(IsActionStateDown("jump"))
            player->Jump();

    // Boost
    if(IsActionStateDown("boost"))
        player->Boost();

    // Change weapons
    if(IsActionStateDown("switchUpWeapon"))
        player->SwitchUpWeapon();

    if(IsActionStateDown("switchDownWeapon"))
        player->SwitchDownWeapon();

    // Shoot
    if(IsActionStateDown("shoot"))
        player->Shoot(m_playManager->GetShootTarget());

    BulletTime* btim = m_playManager->GetBullettime();

    // Bullettime
    if(IsActionJustDown("bullettime"))
        btim->SetActive(true);

    else if(IsActionJustUp("bullettime"))
        btim->SetActive(false);
}
