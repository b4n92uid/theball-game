#include "Item.h"

#include "GameManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"
#include "Player.h"

#include "Define.h"
#include "UserControl.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// Item ------------------------------------------------------------------

Item::Item(GameManager* gameManager, tbe::Matrix4 pos) : MapElement(gameManager)
{
    m_taked = false;

    m_initialMatrix = pos;

    // m_respawnTime = m_worldSettings.itemRespawnTime;

    m_visualBody = new OBJMesh(gameManager->parallelscene.meshs);
    m_visualBody->setMatrix(m_initialMatrix);
}

Item::~Item()
{
}

void Item::setRespawnTime(unsigned respawnTime)
{
    this->m_respawnTime = respawnTime;
}

unsigned Item::getRespawnTime() const
{
    return m_respawnTime;
}

void Item::resetPosition()
{
    m_physicBody->setVelocity(0);

    m_physicBody->setMatrix(m_initialMatrix);
}

void Item::reborn()
{
    setTaked(false);

    resetPosition();
}

void Item::setTaked(bool taked)
{
    this->m_taked = taked;

    m_visualBody->setEnable(!taked);
    m_gameManager->manager.material->setGhost(this, taked);

    if(taked)
        m_rebornClock.snapShoot();
}

bool Item::isTaked() const
{
    return m_taked;
}

bool Item::isReadyToReborn()
{
    return m_rebornClock.isEsplanedTime(m_respawnTime);
}

// ItemAddAmmo -----------------------------------------------------------------

ItemAddAmmo::ItemAddAmmo(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    m_visualBody->open(ITEM_AMMOPACK);

    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->buildConvexNode(m_visualBody->getHardwareBuffer().getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);

    MapElement::m_visualBody = m_visualBody;
}

void ItemAddAmmo::modifPlayer(Player* player)
{
    if(!player->getCurWeapon())
        return;

    GameManager* playManager = player->getGameManager();

    if(playManager->getUserPlayer() == player)
        playManager->display("Munitions supplémentaire");

    player->getCurWeapon()->UpAmmoCount(50 * player->getCurWeapon()->getMaxAmmoCount() / 100);
    m_soundManager->playSound("takeammo", this);

    setTaked(true);
}

bool ItemAddAmmo::isNeeded(Player* player)
{
    //    unsigned percent = player->getCurWeapon()->getAmmoCount()*100
    //            / player->getCurWeapon()->getMaxAmmoCount();

    //    if(!isTaked() && percent < m_aiParams.criticalAmmoValue)
    //        return true;

    return false;
}

// ItemAddLife -----------------------------------------------------------------

ItemAddLife::ItemAddLife(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    m_visualBody->open(ITEM_MEDPACK);

    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->buildConvexNode(m_visualBody->getHardwareBuffer().getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

void ItemAddLife::modifPlayer(Player* player)
{
    GameManager* playManager = player->getGameManager();

    if(playManager->getUserPlayer() == player)
        playManager->display("Santé supplémentaire");

    player->upLife(50);

    m_soundManager->playSound("takelife", this);

    setTaked(true);
}

bool ItemAddLife::isNeeded(Player* player)
{
    //    if(!isTaked() && player->getLife() < (int)m_aiParams.criticalLifeValue)
    //        return true;

    return false;
}

// ItemFatalShot ---------------------------------------------------------------

ItemFatalShot::ItemFatalShot(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    m_visualBody->open(ITEM_FATALSHOT);

    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->buildConvexNode(m_visualBody->getHardwareBuffer().getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

/*
class FatalShotEffect : public Player::CheckMe
{
public:

    bool shutdown(Player* player)
    {
        if(effectTime.isEsplanedTime(8000))
        {
            GameManager* playManager = player->getGameManager();

            player->getCurWeapon()->setMaxAmmoDammage(initialDammage);

            if(playManager->getUserPlayer() == player)
                playManager->hudItem(false);

            return true;
        }

        else
            return false;
    }

    unsigned initialDammage;
    tbe::ticks::Clock effectTime;
};
 */

void ItemFatalShot::modifPlayer(Player* player)
{
    if(!player->getCurWeapon())
        return;

    GameManager* playManager = player->getGameManager();

    if(playManager->getUserPlayer() == player)
    {
        playManager->display("Tire fatale");
        // playManager->hudItem(true);
    }

    //    FatalShotEffect* effect = new FatalShotEffect;
    //    effect->initialDammage = player->getCurWeapon()->getMaxAmmoDammage();
    //    effect->effectTime.snapShoot();
    //     player->addCheckMe(effect);

    player->getCurWeapon()->setMaxAmmoDammage(100);

    m_soundManager->playSound("takefatalshot", this);

    setTaked(true);
}

bool ItemFatalShot::isNeeded(Player* player)
{
    if(!isTaked() && player->getVisualBody()->getPos() - m_visualBody->getPos() < 4.0f)
        return true;

    return false;
}

// ItemSuperLife ---------------------------------------------------------------

ItemSuperLife::ItemSuperLife(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    m_visualBody->open(ITEM_SUPERLIFE);

    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->buildConvexNode(m_visualBody->getHardwareBuffer().getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

/*
class SuperLifeEffect : public Player::CheckMe
{
public:

    bool onTakeDammage(Player* player, Bullet* ammo)
    {
        return false;
    }

    bool shutdown(Player* player)
    {
        if(effectTime.isEsplanedTime(8000))
        {
            GameManager* playManager = player->getGameManager();

            if(playManager->getUserPlayer() == player)
                playManager->hudItem(false);

            return true;
        }

        return false;
    }

    tbe::ticks::Clock effectTime;
};
 */

void ItemSuperLife::modifPlayer(Player* player)
{
    GameManager* playManager = player->getGameManager();

    if(playManager->getUserPlayer() == player)
    {
        playManager->display("Super santé !");
        // playManager->hudItem(true);
    }

    //    SuperLifeEffect* effect = new SuperLifeEffect;
    //    effect->effectTime.snapShoot();
    //    player->addCheckMe(effect);

    player->setLife(100);

    m_soundManager->playSound("takesuperlife", this);

    setTaked(true);
}

bool ItemSuperLife::isNeeded(Player* player)
{
    if(!isTaked() && player->getVisualBody()->getPos() - m_visualBody->getPos() < 4.0f)
        return true;

    return false;
}

// ItemAddFinder ---------------------------------------------------------------

ItemAddFinder::ItemAddFinder(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    m_visualBody->open(ITEM_ADDFINDER);

    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->buildConvexNode(m_visualBody->getHardwareBuffer().getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

void ItemAddFinder::modifPlayer(Player* player)
{
    GameManager* playManager = player->getGameManager();

    if(playManager->getUserPlayer() == player)
        playManager->display("Arme : Finder");

    WeaponFinder* finder = new WeaponFinder(playManager);
    player->addWeapon(finder);

    m_soundManager->playSound("takeammo", this);

    setTaked(true);
}

bool ItemAddFinder::isNeeded(Player* player)
{
    if(!isTaked() && player->getVisualBody()->getPos() - m_visualBody->getPos() < 4.0f)
        return true;

    return false;
}

// ItemAddBomb -----------------------------------------------------------------

ItemAddBomb::ItemAddBomb(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    m_visualBody->open(ITEM_ADDBOMB);

    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->buildConvexNode(m_visualBody->getHardwareBuffer().getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

void ItemAddBomb::modifPlayer(Player* player)
{
    GameManager* playManager = player->getGameManager();

    if(playManager->getUserPlayer() == player)
        playManager->display("Arme : Bomb");

    WeaponBomb* bomb = new WeaponBomb(playManager);
    player->addWeapon(bomb);

    m_soundManager->playSound("takeammo", this);

    setTaked(true);
}

bool ItemAddBomb::isNeeded(Player* player)
{
    if(!isTaked() && player->getVisualBody()->getPos() - m_visualBody->getPos() < 4.0f)
        return true;

    return false;
}

// ItemAddShotgun --------------------------------------------------------------

ItemAddShotgun::ItemAddShotgun(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    m_visualBody->open(ITEM_ADDSHOTGUN);

    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->buildConvexNode(m_visualBody->getHardwareBuffer().getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

void ItemAddShotgun::modifPlayer(Player* player)
{
    GameManager* playManager = player->getGameManager();

    if(playManager->getUserPlayer() == player)
        playManager->display("Arme : Shotgun");

    WeaponShotgun* shotgun = new WeaponShotgun(playManager);
    player->addWeapon(shotgun);

    m_soundManager->playSound("takeammo", this);

    setTaked(true);
}

bool ItemAddShotgun::isNeeded(Player* player)
{
    if(!isTaked() && player->getVisualBody()->getPos() - m_visualBody->getPos() < 4.0f)
        return true;

    return false;
}
