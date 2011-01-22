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
    // Rendue
    Open(model);

    // Attributes
    m_name = name;
    m_playManager = playManager;
    m_curWeapon = m_weaponsPack.end();
    m_killed = false;
    m_boostAvalaible = true;
    m_score = 0;
    m_life = 100;
    m_attachedCotroller = NULL;
    m_soundManager = playManager->manager.sound;

    // Effet explosion
    m_deadExplode = new ParticlesEmiter(playManager->parallelscene.particles);
    m_deadExplode->SetTexture(PARTICLE_EXPLODE);
    m_deadExplode->SetLifeInit(1.000);
    m_deadExplode->SetLifeDown(0.025);
    m_deadExplode->SetFreeMove(0.025);
    m_deadExplode->SetNumber(64);
    m_deadExplode->SetAutoRebuild(false);
    m_deadExplode->SetParent(this);

    m_checkMe.push_back(new StartProtection(this));

    // Physique
    m_physicBody->BuildSphereNode(m_worldSettings.playerSize, m_worldSettings.playerMasse);

    SetRandomSpawnPos();

    NewtonBodySetLinearDamping(m_physicBody->GetBody(), m_worldSettings.playerLinearDamping);
    NewtonBodySetAutoSleep(m_physicBody->GetBody(), false);

    // Arme principale
    WeaponBlaster* blaster = new WeaponBlaster(m_playManager);

    AddWeapon(blaster);
}

Player::~Player()
{
    for(unsigned i = 0; i < m_checkMe.size(); i++)
        delete m_checkMe[i];

    delete m_attachedCotroller;
}

Object* Player::CloneToObject()
{
    return NULL;
}

void Player::SetRandomSpawnPos()
{
    Vector3f randPos;

    Vector3f::Array& spawns = m_playManager->map.spawnPoints;

    if(spawns.empty())
    {
        float factor = 5 * m_playManager->map.aabb.GetSize() / 100;

        do
        {
            randPos = tools::rand(m_playManager->map.aabb - Vector3f(factor));
            randPos.y = 1;
            randPos = m_physicBody->GetParallelScene()->FindFloor(randPos);
        }
        while(randPos.y < m_playManager->map.aabb.min.y);
    }

    else
    {
        randPos = spawns.back();

        spawns.pop_back();
        spawns.insert(spawns.begin(), randPos);
    }

    m_physicBody->SetVelocity(0);
    m_physicBody->SetMatrix(randPos);
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
    if(!m_enable)
        return;

    Object::Process();

    if(m_playManager->IsGameOver())
        return;

    if(!m_boostAvalaible)
    {
        if(clocks.boostDisableBlur.IsEsplanedTime(1000))
            m_playManager->HudBoost(false);

        if(clocks.boostAvailable.IsEsplanedTime(3000))
            m_boostAvalaible = true;
    }

    if(m_attachedCotroller && !m_killed)
        m_attachedCotroller->Process(this);

    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(m_checkMe[i]->Shutdown(this))
            delete m_checkMe[i], m_checkMe[i] = NULL;

    CheckMe::Array::iterator newEnd = remove(m_checkMe.begin(), m_checkMe.end(), (CheckMe*)NULL);
    m_checkMe.erase(newEnd, m_checkMe.end());
}

bool Player::Shoot(Vector3f targetpos)
{
    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(!m_checkMe[i]->OnShoot(this))
            return false;

    return (*m_curWeapon)->Shoot(m_matrix.GetPos(), targetpos);
}

void Player::Jump()
{
    NewtonBodyAddImpulse(m_physicBody->GetBody(), Vector3f(0, m_worldSettings.playerJumpForce, 0), m_matrix.GetPos());
}

void Player::Boost()
{
    if(m_physicBody->GetApplyForce() == 0.0f)
        return;

    if(m_boostAvalaible)
    {
        m_boostAvalaible = false;

        Vector3f impulseDeri = m_physicBody->GetApplyForce().Normalize()
                * m_worldSettings.playerBoostSpeed;

        impulseDeri.y = 0;

        NewtonBodyAddImpulse(m_physicBody->GetBody(), impulseDeri, m_matrix.GetPos());

        m_soundManager->Play("boost", this);

        clocks.boostAvailableSound.SnapShoot();
        clocks.boostAvailable.SnapShoot();

        clocks.boostDisableBlur.SnapShoot();
        m_playManager->HudBoost(true);
    }

    else if(clocks.boostAvailableSound.IsEsplanedTime(800))
    {
        m_soundManager->Play("noAvailable", this);
    }
}

inline bool IsWeaponSameName(Weapon* w1, Weapon* w2)
{
    return w1->GetWeaponName() == w2->GetWeaponName();
}

void Player::AddWeapon(Weapon* weapon)
{
    Weapon::Array::iterator select = find_if(m_weaponsPack.begin(), m_weaponsPack.end(),
                                             bind2nd(ptr_fun(IsWeaponSameName), weapon));

    if(select != m_weaponsPack.end())
    {
        (*select)->UpAmmoCount(weapon->GetAmmoCount());
        delete weapon;
    }

    else
    {
        m_weaponsPack.push_back(weapon);

        m_curWeapon = --m_weaponsPack.end();
        (*m_curWeapon)->SetShooter(this);

        m_playManager->manager.scene->GetRootNode()->AddChild(weapon);
    }
}

void Player::SlotWeapon(unsigned slot)
{
    for(unsigned i = 0; i < m_weaponsPack.size(); i++)
        if(m_weaponsPack[i]->GetSlot() == slot)
        {
            m_curWeapon = m_weaponsPack.begin() + i;
            break;
        }
}

void Player::SwitchUpWeapon()
{
    m_curWeapon++;

    if(m_curWeapon >= m_weaponsPack.end())
        m_curWeapon = m_weaponsPack.begin();
}

void Player::SwitchDownWeapon()
{
    m_curWeapon--;

    if(m_curWeapon < m_weaponsPack.begin())
        m_curWeapon = --m_weaponsPack.end();
}

void Player::SetCurWeapon(unsigned slot)
{
    if(slot >= m_weaponsPack.size())
        throw tbe::Exception("PlayerEngine::SetCurWeapon; Invalid weapon slot (%d)", slot);

    m_curWeapon = m_weaponsPack.begin() + slot;
}

Weapon* Player::GetCurWeapon() const
{
    return (*m_curWeapon);
}

void Player::ReBorn()
{
    m_life = 100;
    m_killed = false;

    m_boostAvalaible = true;

    m_playManager->manager.material->SetGhost(this, false);

    SetVisible(true);

    m_physicBody->SetFreeze(false);

    for(unsigned i = 0; i < m_weaponsPack.size(); i++)
        delete m_weaponsPack[i];

    m_weaponsPack.clear();

    WeaponBlaster* blaster = new WeaponBlaster(m_playManager);

    AddWeapon(blaster);

    SetRandomSpawnPos();

    for(unsigned i = 0; i < m_checkMe.size(); i++)
        m_checkMe[i]->AfterReborn(this);

    m_checkMe.push_back(new StartProtection(this));
}

void Player::Kill()
{
    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(!m_checkMe[i]->OnKilled(this))
            return;

    m_killed = true;
    m_life = 0;

    m_deadExplode->Build();

    m_playManager->manager.material->SetGhost(this, true);

    clocks.readyToDelete.SnapShoot();

    SetVisible(false);

    m_physicBody->SetFreeze(true);

    m_soundManager->Play("kill", this);

    if(this == m_playManager->GetUserPlayer())
    {
        m_playManager->GetBullettime()->SetActive(false);

        m_playManager->HudBoost(false);
        m_playManager->HudBullettime(false);
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
    m_score = max(m_score + m_playManager->ModulatScore(value), 0);
}

void Player::SetScore(int value)
{
    this->m_score = max(m_playManager->ModulatScore(value), 0);
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

    m_life = max(m_life - ammo->GetDammage(), 0);


    Weapon* wp = ammo->GetWeapon();

    if(wp)
    {
        Player* striker = wp->GetShooter();

        if(m_life <= 0)
        {
            striker->UpScore(10);
            UpScore(-10);
            Kill();
        }
        else
        {
            striker->UpScore(2);
            UpScore(-2);
        }
    }

    if(m_playManager->GetUserPlayer() == this)
        m_playManager->HudDammage(true);

    m_soundManager->Play("hit", this);
}

PlayManager* Player::GetPlayManager() const
{
    return m_playManager;
}

Player::StartProtection::StartProtection(Player* player)
{
    using namespace tbe;
    using namespace tbe::scene;

    HardwareBuffer& hb = player->GetHardwareBuffer();

    Vertex* vs = hb.Lock();
    for(unsigned i = 0; i < hb.GetVertexCount(); i++)
        vs[i].color.w = 0.25;
    hb.UnLock();

    Material::Array mats = player->GetAllMaterial();

    for(unsigned i = 0; i < mats.size(); i++)
        mats[i]->Enable(Material::BLEND_MOD);
}

bool Player::StartProtection::OnShoot(Player*)
{
    return false;
}

bool Player::StartProtection::OnTakeDammage(Player*, Bullet*)
{
    return false;
}

bool Player::StartProtection::Shutdown(Player* player)
{
    using namespace tbe;
    using namespace tbe::scene;

    if(m_clock.IsEsplanedTime(2000))
    {
        HardwareBuffer& hb = player->GetHardwareBuffer();

        Vertex* vs = hb.Lock();
        for(unsigned i = 0; i < hb.GetVertexCount(); i++)
            vs[i].color.w = 1;
        hb.UnLock();

        Material::Array mats = player->GetAllMaterial();

        for(unsigned i = 0; i < mats.size(); i++)
            mats[i]->Disable(Material::BLEND_MOD);

        return true;
    }

    return false;
}