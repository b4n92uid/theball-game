#include "Player.h"

#include "AppManager.h"
#include "PlayManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"

#include "Define.h"
#include "BulletTime.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// PlayerEngine ----------------------------------------------------------------

Player::Player(PlayManager* playManager, std::string name, std::string model) : Object(playManager)
{
    // Attributes
    m_name = name;
    m_playManager = playManager;
    m_curWeapon = m_weaponsPack.end();
    m_killed = false;
    m_boostAvalaible = true;
    m_score = 0;
    m_life = 100;
    m_attachedCotroller = NULL;

    // Effet explosion
    m_deadExplode = new ParticlesEmiter;
    m_deadExplode->SetTexture(PARTICLE_EXPLODE);
    m_deadExplode->SetLifeInit(0.5);
    m_deadExplode->SetLifeDown(0.02);
    m_deadExplode->SetFreeMove(0.05);
    m_deadExplode->SetNumber(500);
    m_deadExplode->SetAutoRebuild(false);

    m_playManager->parallelscene.particles->AddParticlesEmiter("", m_deadExplode);

    // Rendue
    Open(model);

    // Physique
    BuildSphereNode(m_worldSettings.playerSize, m_worldSettings.playerMasse);
    SetRandomPosInTheFloor();

    NewtonBodySetContinuousCollisionMode(m_body, true);
    NewtonBodySetLinearDamping(m_body, m_worldSettings.playerLinearDamping);
    NewtonBodySetAutoSleep(m_body, false);

    // Arme principale
    WeaponBlaster* blaster = new WeaponBlaster(m_playManager);
    AddWeapon("blaster", blaster);
}

Player::~Player()
{
    m_playManager->parallelscene.particles->DeleteParticlesEmiter(m_deadExplode);

    for(Weapon::Map::iterator i = m_weaponsPack.begin(); i != m_weaponsPack.end(); i++)
        delete i->second;

    for(unsigned i = 0; i < m_checkMe.size(); i++)
        delete m_checkMe[i];

    delete m_attachedCotroller;
}

Object* Player::Clone()
{
    return NULL;
}

void Player::ReBorn()
{
    m_life = 100;
    m_killed = false;

    m_boostAvalaible = true;

    m_playManager->manager.material->SetGhost(this, false);

    SetFreeze(false);
    SetEnable(true);

    for(Weapon::Map::iterator i = m_weaponsPack.begin(); i != m_weaponsPack.end(); i++)
        delete i->second;

    m_weaponsPack.clear();

    WeaponBlaster* blaster = new WeaponBlaster(m_playManager);
    AddWeapon("blaster", blaster);

    SetRandomPosInTheFloor();

    for(unsigned i = 0; i < m_checkMe.size(); i++)
        m_checkMe[i]->AfterReborn(this);
}

void Player::SetRandomPosInTheFloor()
{
    Vector3f randPos;

    do
    {
        randPos = tools::rand(m_playManager->map.aabb - Vector3f(4, 0, 4));
        randPos.y = 1;
        randPos = m_newtonScene->FindFloor(randPos);
    }
    while(randPos.y < m_playManager->map.aabb.min.y);

    NewtonBodySetVelocity(m_body, Vector3f(0));
    NewtonNode::SetMatrix(randPos);
}

void Player::AttachItem(Item* item)
{
    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(!m_checkMe[i]->OnTakeItems(this, item))
            return;

    item->ModifPlayer(this);
}

void Player::AttachController(Controller* controller)
{
    delete m_attachedCotroller, m_attachedCotroller = controller;
}

Controller* Player::GetAttachedCotroller() const
{
    return m_attachedCotroller;
}

void Player::Process()
{
    if(m_playManager->IsGameOver())
        return;

    if(!m_boostAvalaible)
    {
        if(m_playManager->manager.app->globalSettings.video.usePpe)
            if(clocks.boostDisableBlur.IsEsplanedTime(1000))
                m_playManager->ppe.boost->SetEnable(false);

        if(clocks.boostAvailable.IsEsplanedTime(3000))
            m_boostAvalaible = true;
    }

    if(m_attachedCotroller)
        m_attachedCotroller->Process(this);

    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(m_checkMe[i]->Shutdown(this))
            delete m_checkMe[i], m_checkMe[i] = NULL;

    CheckMe::Array::iterator newEnd = remove(m_checkMe.begin(), m_checkMe.end(), (CheckMe*)NULL);
    m_checkMe.erase(newEnd, m_checkMe.end());
}

void Player::Shoot(Vector3f targetpos)
{
    m_curWeapon->second->Shoot(m_matrix.GetPos(), targetpos);
}

void Player::Jump()
{
    if(!m_body)
        return;

    NewtonBodyAddImpulse(m_body, Vector3f(0, m_worldSettings.playerJumpForce, 0), m_matrix.GetPos());
}

void Player::Boost()
{
    if(!m_body || m_applyForce == 0.0f)
        return;

    if(m_boostAvalaible)
    {
        m_boostAvalaible = false;

        Vector3f impulseDeri = Vector3f::Normalize(m_applyForce) * m_worldSettings.playerBoostSpeed;
        impulseDeri.y = 0;

        NewtonBodyAddImpulse(m_body, impulseDeri, m_matrix.GetPos());

        m_playManager->manager.sound->Play("boost", this);

        clocks.boostAvailableSound.SnapShoot();
        clocks.boostAvailable.SnapShoot();

        if(m_playManager->manager.app->globalSettings.video.usePpe)
        {
            clocks.boostDisableBlur.SnapShoot();
            m_playManager->ppe.boost->SetEnable(true);
        }
    }

    else if(clocks.boostAvailableSound.IsEsplanedTime(800))
    {
        m_playManager->manager.sound->Play("noAvailable", this);
    }
}

void Player::AddWeapon(std::string name, Weapon* weapon)
{
    if(m_weaponsPack.find(name) != m_weaponsPack.end())
    {
        m_weaponsPack[name]->UpAmmoCount(weapon->GetAmmoCount());
        delete weapon;
    }

    else
    {
        m_weaponsPack[name] = weapon;
    }

    m_curWeapon = m_weaponsPack.find(name);
    m_curWeapon->second->SetShooter(this);
}

void Player::SwitchUpWeapon()
{
    m_curWeapon++;

    if(m_curWeapon == m_weaponsPack.end())
        m_curWeapon = m_weaponsPack.begin();
}

void Player::SwitchDownWeapon()
{
    if(m_curWeapon == m_weaponsPack.begin())
        m_curWeapon = m_weaponsPack.end();

    m_curWeapon--;
}

void Player::SetCurWeapon(std::string weaponName)
{
    m_curWeapon = m_weaponsPack.find(weaponName);

    if(m_curWeapon == m_weaponsPack.end())
        throw tbe::Exception("PlayerEngine::SetCurWeapon; Weapon not found (%s)", weaponName.c_str());
}

Weapon* Player::GetCurWeapon() const
{
    return m_curWeapon->second;
}

void Player::Kill()
{
    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(!m_checkMe[i]->OnKilled(this))
            return;

    m_killed = true;
    m_life = 0;

    m_deadExplode->SetEnable(true);
    m_deadExplode->SetDeadEmiter(false);
    m_deadExplode->SetPos(m_matrix.GetPos());
    m_deadExplode->Build();

    m_playManager->manager.material->SetGhost(this, true);

    clocks.readyToDelete.SnapShoot();

    SetFreeze(true);
    SetEnable(false);

    m_playManager->manager.sound->Play("kill", this);

    if(m_playManager->GetUserPlayer() == this)
    {
        m_playManager->GetBullettime()->SetActive(false);

        if(m_playManager->manager.app->globalSettings.video.usePpe)
            m_playManager->ppe.boost->SetEnable(false);
    }
}

bool Player::IsKilled() const
{
    return m_killed;
}

void Player::SetBoostAvalaible(bool boost)
{
    this->m_boostAvalaible = boost;
}

bool Player::IsBoostAvalaible() const
{
    return m_boostAvalaible;
}

void Player::UpScore(int value)
{
    m_score += max(value, 0);
}

void Player::SetScore(int frag)
{
    this->m_score = frag;
}

int Player::GetScore() const
{
    return m_score;
}

void Player::UpLife(int life)
{
    m_life = max(min(m_life + life, 100), 0);
}

void Player::SetLife(int life)
{
    m_life = max(min(life, 100), 0);
}

int Player::GetLife() const
{
    return m_life;
}

void Player::AddCheckPoint(CheckMe* cm)
{
    m_checkMe.push_back(cm);
}

void Player::TakeDammage(Bullet* ammo)
{
    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(!m_checkMe[i]->OnTakeDammage(this, ammo))
            return;

    Player* striker = ammo->GetParent()->GetShooter();

    m_life = max(m_life - ammo->GetDammage(), 0);

    striker->m_score++;

    if(m_life <= 0)
    {
        striker->m_score += 10;
        Kill();
    }

    if(this == m_playManager->GetUserPlayer())
    {
        m_playManager->hud.background.dammage->SetOpacity(0.75);
        m_playManager->hud.background.dammage->SetEnable(true);
    }

    m_playManager->manager.sound->Play("hit", this);
}

PlayManager* Player::GetPlayManager() const
{
    return m_playManager;
}
