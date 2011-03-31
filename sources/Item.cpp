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

Item::Item(GameManager* gameManager, tbe::Matrix4f pos)
: Object(gameManager), m_aiParams(gameManager->manager.app->globalSettings.ai)
{
    m_taked = false;
    m_initialMatrix = pos;
    m_matrix = m_initialMatrix;
    m_respawnTime = m_worldSettings.itemRespawnTime;
    m_name = "Item";
}

Item::~Item()
{
}

void Item::SetRespawnTime(unsigned respawnTime)
{
    this->m_respawnTime = respawnTime;
}

unsigned Item::GetRespawnTime() const
{
    return m_respawnTime;
}

void Item::ResetPosition()
{
    m_physicBody->SetVelocity(0);

    m_physicBody->SetMatrix(m_initialMatrix);
}

void Item::Reborn()
{
    SetTaked(false);

    ResetPosition();
}

void Item::SetTaked(bool taked)
{
    this->m_taked = taked;

    SetEnable(!taked);
    m_gameManager->manager.material->SetGhost(this, taked);

    if(taked)
        m_rebornClock.SnapShoot();
}

bool Item::IsTaked() const
{
    return m_taked;
}

bool Item::IsReadyToReborn()
{
    return m_rebornClock.IsEsplanedTime(m_respawnTime);
}

// ItemAddAmmo -----------------------------------------------------------------

ItemAddAmmo::ItemAddAmmo(GameManager* gameManager, tbe::Matrix4f pos) : Item(gameManager, pos)
{
    Open(ITEM_AMMOPACK);

    m_physicBody->BuildConvexNode(m_hardwareBuffer.GetAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->GetBody(), false);
}

Object* ItemAddAmmo::CloneToObject()
{
    Item* it = new ItemAddAmmo(m_gameManager, m_matrix);
    m_gameManager->RegisterItem(it);
    return it;
}

void ItemAddAmmo::ModifPlayer(Player* player)
{
    PlayManager* playManager = player->GetPlayManager();

    if(playManager->GetUserPlayer() == player)
        playManager->Log("Munitions supplémentaire");

    player->GetCurWeapon()->UpAmmoCount(50 * player->GetCurWeapon()->GetMaxAmmoCount() / 100);
    m_soundManager->Play("takeammo", this);

    SetTaked(true);
}

bool ItemAddAmmo::IsNeeded(Player* player)
{
    unsigned percent = player->GetCurWeapon()->GetAmmoCount()*100
            / player->GetCurWeapon()->GetMaxAmmoCount();

    if(!IsTaked() && percent < m_aiParams.criticalAmmoValue)
        return true;

    return false;
}

void ItemAddAmmo::OutputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=AMMO" << endl;
    stream << endl;
}

// ItemAddLife -----------------------------------------------------------------

ItemAddLife::ItemAddLife(GameManager* gameManager, tbe::Matrix4f pos) : Item(gameManager, pos)
{
    Open(ITEM_MEDPACK);

    m_physicBody->BuildConvexNode(m_hardwareBuffer.GetAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->GetBody(), false);
}

Object* ItemAddLife::CloneToObject()
{
    Item* it = new ItemAddLife(m_gameManager, m_matrix);
    m_gameManager->RegisterItem(it);
    return it;
}

void ItemAddLife::ModifPlayer(Player* player)
{
    PlayManager* playManager = player->GetPlayManager();

    if(playManager->GetUserPlayer() == player)
        playManager->Log("Santé supplémentaire");

    player->UpLife(50);

    m_soundManager->Play("takelife", this);

    SetTaked(true);
}

bool ItemAddLife::IsNeeded(Player* player)
{
    if(!IsTaked() && player->GetLife() < m_aiParams.criticalLifeValue)
        return true;

    return false;
}

void ItemAddLife::OutputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=LIFE" << endl;
    stream << endl;
}


// ItemFatalShot ---------------------------------------------------------------

ItemFatalShot::ItemFatalShot(GameManager* gameManager, tbe::Matrix4f pos) : Item(gameManager, pos)
{
    Open(ITEM_FATALSHOT);

    m_physicBody->BuildConvexNode(m_hardwareBuffer.GetAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->GetBody(), false);
}

Object* ItemFatalShot::CloneToObject()
{
    Item* it = new ItemFatalShot(m_gameManager, m_matrix);
    m_gameManager->RegisterItem(it);
    return it;
}

class FatalShotEffect : public Player::CheckMe
{
public:

    bool Shutdown(Player* player)
    {
        if(effectTime.IsEsplanedTime(8000))
        {
            PlayManager* playManager = player->GetPlayManager();

            player->GetCurWeapon()->SetMaxAmmoDammage(initialDammage);

            if(playManager->GetUserPlayer() == player)
                playManager->HudItem(false);

            return true;
        }

        else
            return false;
    }

    unsigned initialDammage;
    tbe::ticks::Clock effectTime;
};

void ItemFatalShot::ModifPlayer(Player* player)
{
    PlayManager* playManager = player->GetPlayManager();

    if(playManager->GetUserPlayer() == player)
    {
        playManager->Log("Tire fatale");
        playManager->HudItem(true);
    }

    FatalShotEffect* effect = new FatalShotEffect;
    effect->initialDammage = player->GetCurWeapon()->GetMaxAmmoDammage();
    effect->effectTime.SnapShoot();

    player->GetCurWeapon()->SetMaxAmmoDammage(100);
    player->AddCheckPoint(effect);

    m_soundManager->Play("takefatalshot", this);

    SetTaked(true);
}

bool ItemFatalShot::IsNeeded(Player* player)
{
    if(!IsTaked() && player->GetPos() - m_matrix.GetPos() < 4.0f)
        return true;

    return false;
}

void ItemFatalShot::OutputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=FATALSHOT" << endl;
    stream << endl;
}

// ItemSuperLife ---------------------------------------------------------------

ItemSuperLife::ItemSuperLife(GameManager* gameManager, tbe::Matrix4f pos) : Item(gameManager, pos)
{
    Open(ITEM_SUPERLIFE);

    m_physicBody->BuildConvexNode(m_hardwareBuffer.GetAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->GetBody(), false);
}

Object* ItemSuperLife::CloneToObject()
{
    Item* it = new ItemSuperLife(m_gameManager, m_matrix);
    m_gameManager->RegisterItem(it);
    return it;
}

class SuperLifeEffect : public Player::CheckMe
{
public:

    bool OnTakeDammage(Player* player, Bullet* ammo)
    {
        return false;
    }

    bool Shutdown(Player* player)
    {
        if(effectTime.IsEsplanedTime(8000))
        {
            PlayManager* playManager = player->GetPlayManager();

            if(playManager->GetUserPlayer() == player)
                playManager->HudItem(false);

            return true;
        }

        return false;
    }

    tbe::ticks::Clock effectTime;
};

void ItemSuperLife::ModifPlayer(Player* player)
{
    PlayManager* playManager = player->GetPlayManager();

    if(playManager->GetUserPlayer() == player)
    {
        playManager->Log("Super santé !");
        playManager->HudItem(true);
    }

    SuperLifeEffect* effect = new SuperLifeEffect;
    effect->effectTime.SnapShoot();

    player->SetLife(100);
    player->AddCheckPoint(effect);

    m_soundManager->Play("takesuperlife", this);

    SetTaked(true);
}

bool ItemSuperLife::IsNeeded(Player* player)
{
    if(!IsTaked() && player->GetPos() - m_matrix.GetPos() < 4.0f)
        return true;

    return false;
}

void ItemSuperLife::OutputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=SUPERLIFE" << endl;
    stream << endl;
}

// ItemAddFinder ---------------------------------------------------------------

ItemAddFinder::ItemAddFinder(GameManager* gameManager, tbe::Matrix4f pos) : Item(gameManager, pos)
{
    Open(ITEM_ADDFINDER);

    m_physicBody->BuildConvexNode(m_hardwareBuffer.GetAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->GetBody(), false);
}

Object* ItemAddFinder::CloneToObject()
{
    Item* it = new ItemAddFinder(m_gameManager, m_matrix);
    m_gameManager->RegisterItem(it);
    return it;
}

void ItemAddFinder::ModifPlayer(Player* player)
{
    PlayManager* playManager = player->GetPlayManager();

    if(playManager->GetUserPlayer() == player)
        playManager->Log("Arme : Finder");

    WeaponFinder* finder = new WeaponFinder(playManager);
    player->AddWeapon(finder);

    m_soundManager->Play("takeammo", this);

    SetTaked(true);
}

bool ItemAddFinder::IsNeeded(Player* player)
{
    if(!IsTaked() && player->GetPos() - m_matrix.GetPos() < 4.0f)
        return true;

    return false;
}

void ItemAddFinder::OutputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=FINDER" << endl;
    stream << endl;
}

// ItemAddBomb -----------------------------------------------------------------

ItemAddBomb::ItemAddBomb(GameManager* gameManager, tbe::Matrix4f pos) : Item(gameManager, pos)
{
    Open(ITEM_ADDBOMB);

    m_physicBody->BuildConvexNode(m_hardwareBuffer.GetAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->GetBody(), false);
}

Object* ItemAddBomb::CloneToObject()
{
    Item* it = new ItemAddBomb(m_gameManager, m_matrix);
    m_gameManager->RegisterItem(it);
    return it;
}

void ItemAddBomb::ModifPlayer(Player* player)
{
    PlayManager* playManager = player->GetPlayManager();

    if(playManager->GetUserPlayer() == player)
        playManager->Log("Arme : Bomb");

    WeaponBomb* bomb = new WeaponBomb(playManager);
    player->AddWeapon(bomb);

    m_soundManager->Play("takeammo", this);

    SetTaked(true);
}

bool ItemAddBomb::IsNeeded(Player* player)
{
    if(!IsTaked() && player->GetPos() - m_matrix.GetPos() < 4.0f)
        return true;

    return false;
}

void ItemAddBomb::OutputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=BOMB" << endl;
    stream << endl;
}

// ItemAddShotgun --------------------------------------------------------------

ItemAddShotgun::ItemAddShotgun(GameManager* gameManager, tbe::Matrix4f pos) : Item(gameManager, pos)
{
    Open(ITEM_ADDSHOTGUN);

    m_physicBody->BuildConvexNode(m_hardwareBuffer.GetAllVertex(), 1.0);
    NewtonBodySetAutoSleep(m_physicBody->GetBody(), false);
}

Object* ItemAddShotgun::CloneToObject()
{
    Item* it = new ItemAddShotgun(m_gameManager, m_matrix);
    m_gameManager->RegisterItem(it);
    return it;
}

void ItemAddShotgun::ModifPlayer(Player* player)
{
    PlayManager* playManager = player->GetPlayManager();

    if(playManager->GetUserPlayer() == player)
        playManager->Log("Arme : Shotgun");

    WeaponShotgun* shotgun = new WeaponShotgun(playManager);
    player->AddWeapon(shotgun);

    m_soundManager->Play("takeammo", this);

    SetTaked(true);
}

bool ItemAddShotgun::IsNeeded(Player* player)
{
    if(!IsTaked() && player->GetPos() - m_matrix.GetPos() < 4.0f)
        return true;

    return false;
}

void ItemAddShotgun::OutputConstruction(std::iostream& stream)
{
    using namespace std;

    stream << "+node" << endl;
    stream << "type=ITEM" << endl;
    stream << "matrix=" << m_matrix << endl;
    stream << "add=SHOTGUN" << endl;
    stream << endl;
}
