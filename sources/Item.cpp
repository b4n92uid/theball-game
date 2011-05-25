#include "Item.h"

#include "GameManager.h"
#include "PlayManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"
#include "Player.h"

#include "Define.h"
#include "UserControl.h"
#include "TeamModeAi.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// Item ------------------------------------------------------------------

Item::Item(GameManager* gameManager, tbe::Matrix4 pos)
: Object(gameManager), m_aiParams(gameManager->manager.app->globalSettings.ai)
{
    m_taked = false;
    m_initialMatrix = pos;
    m_matrix = m_initialMatrix;
    m_respawnTime = m_worldSettings.itemRespawnTime;
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

    setEnable(!taked);
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
    open(ITEM_AMMOPACK);

    m_physicBody->buildConvexNode(m_hardwareBuffer.getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

Object* ItemAddAmmo::cloneToObject()
{
    Item* it = new ItemAddAmmo(m_gameManager, m_matrix);
    m_gameManager->registerItem(it);
    return it;
}

void ItemAddAmmo::modifPlayer(Player* player)
{
    PlayManager* playManager = player->getPlayManager();

    if(playManager->getUserPlayer() == player)
        playManager->log("Munitions supplémentaire");

    player->getCurWeapon()->UpAmmoCount(50 * player->getCurWeapon()->getMaxAmmoCount() / 100);
    m_soundManager->play("takeammo", this);

    setTaked(true);
}

bool ItemAddAmmo::isNeeded(Player* player)
{
    unsigned percent = player->getCurWeapon()->getAmmoCount()*100
            / player->getCurWeapon()->getMaxAmmoCount();

    if(!isTaked() && percent < m_aiParams.criticalAmmoValue)
        return true;

    return false;
}

void ItemAddAmmo::outputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=AMMO" << endl;
    stream << endl;
}

// ItemAddLife -----------------------------------------------------------------

ItemAddLife::ItemAddLife(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    open(ITEM_MEDPACK);

    m_physicBody->buildConvexNode(m_hardwareBuffer.getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

Object* ItemAddLife::cloneToObject()
{
    Item* it = new ItemAddLife(m_gameManager, m_matrix);
    m_gameManager->registerItem(it);
    return it;
}

void ItemAddLife::modifPlayer(Player* player)
{
    PlayManager* playManager = player->getPlayManager();

    if(playManager->getUserPlayer() == player)
        playManager->log("Santé supplémentaire");

    player->upLife(50);

    m_soundManager->play("takelife", this);

    setTaked(true);
}

bool ItemAddLife::isNeeded(Player* player)
{
    if(!isTaked() && player->getLife() < (int)m_aiParams.criticalLifeValue)
        return true;

    return false;
}

void ItemAddLife::outputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=LIFE" << endl;
    stream << endl;
}


// ItemFatalShot ---------------------------------------------------------------

ItemFatalShot::ItemFatalShot(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    open(ITEM_FATALSHOT);

    m_physicBody->buildConvexNode(m_hardwareBuffer.getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

Object* ItemFatalShot::cloneToObject()
{
    Item* it = new ItemFatalShot(m_gameManager, m_matrix);
    m_gameManager->registerItem(it);
    return it;
}

class FatalShotEffect : public Player::CheckMe
{
public:

    bool shutdown(Player* player)
    {
        if(effectTime.isEsplanedTime(8000))
        {
            PlayManager* playManager = player->getPlayManager();

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

void ItemFatalShot::modifPlayer(Player* player)
{
    PlayManager* playManager = player->getPlayManager();

    if(playManager->getUserPlayer() == player)
    {
        playManager->log("Tire fatale");
        playManager->hudItem(true);
    }

    FatalShotEffect* effect = new FatalShotEffect;
    effect->initialDammage = player->getCurWeapon()->getMaxAmmoDammage();
    effect->effectTime.snapShoot();

    player->getCurWeapon()->setMaxAmmoDammage(100);
    player->addCheckMe(effect);

    m_soundManager->play("takefatalshot", this);

    setTaked(true);
}

bool ItemFatalShot::isNeeded(Player* player)
{
    if(!isTaked() && player->getPos() - m_matrix.getPos() < 4.0f)
        return true;

    return false;
}

void ItemFatalShot::outputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=FATALSHOT" << endl;
    stream << endl;
}

// ItemSuperLife ---------------------------------------------------------------

ItemSuperLife::ItemSuperLife(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    open(ITEM_SUPERLIFE);

    m_physicBody->buildConvexNode(m_hardwareBuffer.getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

Object* ItemSuperLife::cloneToObject()
{
    Item* it = new ItemSuperLife(m_gameManager, m_matrix);
    m_gameManager->registerItem(it);
    return it;
}

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
            PlayManager* playManager = player->getPlayManager();

            if(playManager->getUserPlayer() == player)
                playManager->hudItem(false);

            return true;
        }

        return false;
    }

    tbe::ticks::Clock effectTime;
};

void ItemSuperLife::modifPlayer(Player* player)
{
    PlayManager* playManager = player->getPlayManager();

    if(playManager->getUserPlayer() == player)
    {
        playManager->log("Super santé !");
        playManager->hudItem(true);
    }

    SuperLifeEffect* effect = new SuperLifeEffect;
    effect->effectTime.snapShoot();

    player->setLife(100);
    player->addCheckMe(effect);

    m_soundManager->play("takesuperlife", this);

    setTaked(true);
}

bool ItemSuperLife::isNeeded(Player* player)
{
    if(!isTaked() && player->getPos() - m_matrix.getPos() < 4.0f)
        return true;

    return false;
}

void ItemSuperLife::outputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=SUPERLIFE" << endl;
    stream << endl;
}

// ItemAddFinder ---------------------------------------------------------------

ItemAddFinder::ItemAddFinder(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    open(ITEM_ADDFINDER);

    m_physicBody->buildConvexNode(m_hardwareBuffer.getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

Object* ItemAddFinder::cloneToObject()
{
    Item* it = new ItemAddFinder(m_gameManager, m_matrix);
    m_gameManager->registerItem(it);
    return it;
}

void ItemAddFinder::modifPlayer(Player* player)
{
    PlayManager* playManager = player->getPlayManager();

    if(playManager->getUserPlayer() == player)
        playManager->log("Arme : Finder");

    WeaponFinder* finder = new WeaponFinder(playManager);
    player->addWeapon(finder);

    m_soundManager->play("takeammo", this);

    setTaked(true);
}

bool ItemAddFinder::isNeeded(Player* player)
{
    if(!isTaked() && player->getPos() - m_matrix.getPos() < 4.0f)
        return true;

    return false;
}

void ItemAddFinder::outputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=FINDER" << endl;
    stream << endl;
}

// ItemAddBomb -----------------------------------------------------------------

ItemAddBomb::ItemAddBomb(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    open(ITEM_ADDBOMB);

    m_physicBody->buildConvexNode(m_hardwareBuffer.getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

Object* ItemAddBomb::cloneToObject()
{
    Item* it = new ItemAddBomb(m_gameManager, m_matrix);
    m_gameManager->registerItem(it);
    return it;
}

void ItemAddBomb::modifPlayer(Player* player)
{
    PlayManager* playManager = player->getPlayManager();

    if(playManager->getUserPlayer() == player)
        playManager->log("Arme : Bomb");

    WeaponBomb* bomb = new WeaponBomb(playManager);
    player->addWeapon(bomb);

    m_soundManager->play("takeammo", this);

    setTaked(true);
}

bool ItemAddBomb::isNeeded(Player* player)
{
    if(!isTaked() && player->getPos() - m_matrix.getPos() < 4.0f)
        return true;

    return false;
}

void ItemAddBomb::outputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=BOMB" << endl;
    stream << endl;
}

// ItemAddShotgun --------------------------------------------------------------

ItemAddShotgun::ItemAddShotgun(GameManager* gameManager, tbe::Matrix4 pos) : Item(gameManager, pos)
{
    open(ITEM_ADDSHOTGUN);

    m_physicBody->buildConvexNode(m_hardwareBuffer.getAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
}

Object* ItemAddShotgun::cloneToObject()
{
    Item* it = new ItemAddShotgun(m_gameManager, m_matrix);
    m_gameManager->registerItem(it);
    return it;
}

void ItemAddShotgun::modifPlayer(Player* player)
{
    PlayManager* playManager = player->getPlayManager();

    if(playManager->getUserPlayer() == player)
        playManager->log("Arme : Shotgun");

    WeaponShotgun* shotgun = new WeaponShotgun(playManager);
    player->addWeapon(shotgun);

    m_soundManager->play("takeammo", this);

    setTaked(true);
}

bool ItemAddShotgun::isNeeded(Player* player)
{
    if(!isTaked() && player->getPos() - m_matrix.getPos() < 4.0f)
        return true;

    return false;
}

void ItemAddShotgun::outputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=SHOTGUN" << endl;
    stream << endl;
}
