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

void UserControl::SetGameActiveAction(std::string name, bool state)
{
    StateMap::iterator kit = m_control.keyboard.find(name);
    StateMap::iterator mit = m_control.mouse.find(name);

    if(kit != m_control.keyboard.end())
        m_eventManager->keyState[kit->second] = state;

    else if(mit != m_control.mouse.end())
        m_eventManager->mouseState[mit->second] = state;
}

bool UserControl::IsGameActiveAction(std::string name)
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
        if(player->IsCollidWith(m_playManager->map.staticObjects[i]))
        {
            collideWithStatic = true;
            break;
        }

    // Move
    Vector3f addForce;

    if(IsGameActiveAction("forward"))
        addForce += playerCam->GetTarget() * m_worldSettings.playerMoveSpeed;
    if(IsGameActiveAction("backward"))
        addForce -= playerCam->GetTarget() * m_worldSettings.playerMoveSpeed;
    if(IsGameActiveAction("strafLeft"))
        addForce += playerCam->GetLeft() * m_worldSettings.playerMoveSpeed;
    if(IsGameActiveAction("strafRight"))
        addForce -= playerCam->GetLeft() * m_worldSettings.playerMoveSpeed;

    addForce.y = 0;

    player->SetApplyForce(addForce);

    // Jump
    if(collideWithStatic)
        if(IsGameActiveAction("jump"))
            player->Jump();

    // Boost
    if(IsGameActiveAction("boost"))
        player->Boost();

    // Change weapons
    if(IsGameActiveAction("switchUpWeapon"))
        player->SwitchUpWeapon();

    if(IsGameActiveAction("switchDownWeapon"))
        player->SwitchDownWeapon();

    // Shoot
    if(IsGameActiveAction("shoot"))
        player->Shoot(m_playManager->GetShootTarget());

    BulletTime* btim = m_playManager->GetBullettime();

    // Bullettime
    if(IsGameActiveAction("bullettime"))
    {
        if(btim->GetValue() > 0.0)
            btim->SetActive(true);

        else
            SetGameActiveAction("bullettime", false);
    }

    else
    {
        btim->SetActive(false);
    }
}
