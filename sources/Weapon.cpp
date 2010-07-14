#include "Weapon.h"

#include "PlayManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"
#include "BldParser.h"

#include "Player.h"
#include "Item.h"

#include "Define.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// Weapon ----------------------------------------------------------------------

Weapon::Weapon(PlayManager* playManager)
{
    m_parent = NULL;

    m_playManager = playManager;
    m_worldSettings = m_playManager->manager.app->globalSettings.physics;

    m_maxAmmoDammage = 0;
    m_maxAmmoCount = 0;

    m_ammoCount = 0;

    m_shootSpeed = 0;
    m_shootCadency = 0;

    m_saveShootCadency = 0;

    m_fireSound = NULL;

    m_playManager->parallelscene.particles->AddParticlesEmiter("", this);

    m_mapAABB = m_playManager->map.aabb;
}

Weapon::Weapon(const Weapon& copy)
{
    *this = copy;
}

Weapon::~Weapon()
{
    m_playManager->parallelscene.particles->ReleaseParticlesEmiter(this);

    for(unsigned i = 0; i < m_ammosPack.size(); i++)
        delete m_ammosPack[i];

    if(m_fireSound)
        FSOUND_Sample_Free(m_fireSound);
}

void Weapon::Process()
{
    Particle* particles = BeginParticlesPosProcess();

    for(unsigned i = 0; i < m_ammosPack.size(); i++)
    {
        if(m_ammosPack[i]->IsDeadAmmo())
        {
            delete m_ammosPack[i];
            m_ammosPack.erase((vector<Ammo*>::iterator) & m_ammosPack[i]);
        }

        else
        {
            m_ammosPack[i]->UpdateMatrix();
            particles[i].pos = m_ammosPack[i]->NewtonNode::GetPos();
        }

    }

    EndParticlesPosProcess();

    SetDrawNumber(m_ammosPack.size());
}

void Weapon::SetFireSound(std::string fireSound)
{
    m_fireSound = FSOUND_Sample_Load(FSOUND_FREE, fireSound.c_str(), FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);

    if(!m_fireSound)
        throw tbe::Exception("WeaponEngine::SetFireSound\nFire sound load error (%s)\n\n", fireSound.c_str());
    else
        FSOUND_Sample_SetMinMaxDistance(m_fireSound, SOUND_MIN_DIST, SOUND_MAX_DIST);
}

FSOUND_SAMPLE* Weapon::GetFireSound() const
{
    return m_fireSound;
}

bool Weapon::IsEmpty()
{
    return(m_ammoCount == 0);
}

std::string Weapon::GetWeaponName() const
{
    return m_weaponName;
}

void Weapon::SetWeaponName(std::string weaponName)
{
    this->m_weaponName = weaponName;
}

void Weapon::Shoot(Vector3f startpos, Vector3f targetpos)
{
    // Controle des munitions
    if(m_ammoCount <= 0)
    {
        if(m_parent->clocks.shoot.IsEsplanedTime(1000))
            m_playManager->manager.sound->Play("noAvailable", m_parent);

        return;
    }

    // Controle de la cadence de tire
    if(!m_shootCadencyClock.IsEsplanedTime(m_shootCadency))
        return;

    if(m_ammosPack.size() > m_maxAmmoCount)
        return;

    #ifndef DISABLE_SOUND
    FSOUND_3D_SetAttributes(FSOUND_PlaySound(FSOUND_FREE, m_fireSound), m_parent->NewtonNode::GetPos(), 0);
    #endif

    m_ammoCount--;

    ProcessShoot(startpos, targetpos);
}

void Weapon::SetShooter(Player* shooter)
{

    m_parent = shooter;
}

Player* Weapon::GetShooter() const
{

    return m_parent;
}

void Weapon::RestoreShootCadency()
{
    m_shootCadency = m_saveShootCadency;
}

void Weapon::SetShootCadency(unsigned shootCadency)
{
    this->m_shootCadency = shootCadency;
    m_saveShootCadency = shootCadency;
}

unsigned Weapon::GetShootCadency() const
{
    return m_shootCadency;
}

bool Weapon::UpAmmoCount(int ammoCount)
{
    if(m_ammoCount >= m_maxAmmoCount)
        return false;

    if(m_ammoCount + ammoCount > m_maxAmmoCount)
        m_ammoCount = m_maxAmmoCount;

    else
        m_ammoCount += ammoCount;

    return true;
}

void Weapon::SetAmmoCount(unsigned ammoCount)
{

    if(ammoCount > m_maxAmmoCount)
        ammoCount = m_maxAmmoCount;

    this->m_ammoCount = ammoCount;
}

unsigned Weapon::GetAmmoCount() const
{

    return m_ammoCount;
}

void Weapon::SetMaxAmmoCount(unsigned maxAmmoCount)
{

    this->m_maxAmmoCount = maxAmmoCount;
}

unsigned Weapon::GetMaxAmmoCount() const
{

    return m_maxAmmoCount;
}

void Weapon::SetMaxAmmoDammage(unsigned maxAmmoDammage)
{

    this->m_maxAmmoDammage = maxAmmoDammage;
}

unsigned Weapon::GetMaxAmmoDammage() const
{

    return m_maxAmmoDammage;
}

void Weapon::SetShootSpeed(float shootSpeed)
{
    this->m_shootSpeed = shootSpeed;
}

float Weapon::GetShootSpeed() const
{
    return m_shootSpeed;
}

void Weapon::SetBullttimeMotion(bool state)
{
    if(state)
    {
        m_shootCadency = m_saveShootCadency;
        m_saveShootCadency = m_shootCadency;
        m_shootCadency *= 4;
    }

    else
    {
        m_shootCadency = m_saveShootCadency;
    }
}

Weapon & Weapon::operator=(const Weapon& copy)
{
    m_maxAmmoDammage = copy.m_maxAmmoDammage;
    m_maxAmmoCount = copy.m_maxAmmoCount;

    m_ammoCount = copy.m_ammoCount;

    m_shootSpeed = copy.m_shootSpeed;
    m_shootCadency = copy.m_shootCadency;

    return *this;
}

// Ammo ------------------------------------------------------------------------

Ammo::Ammo(Weapon* weapon)
{
    m_weapon = weapon;
    m_playManager = m_weapon->GetShooter()->GetPlayManager();
    m_life = 300;
    m_dammage = tools::rand(1, m_weapon->GetMaxAmmoDammage());
}

void Ammo::SetShootSpeed(float shootSpeed)
{
    this->m_shootSpeed = shootSpeed;
}

float Ammo::GetShootSpeed() const
{
    return m_shootSpeed;
}

void Ammo::SetDammage(int dammage)
{
    this->m_dammage = dammage;
}

int Ammo::GetDammage() const
{
    return m_dammage;
}

void Ammo::SetLife(int life)
{

    this->m_life = life;
}

int Ammo::GetLife() const
{

    return m_life;
}

void Ammo::SetParent(Weapon* shooter)
{
    this->m_weapon = shooter;
}

Weapon* Ammo::GetParent() const
{
    return m_weapon;
}

bool Ammo::IsDeadAmmo()
{
    return(m_life <= 0 || !m_playManager->map.aabb.IsInner(m_matrix.GetPos()));
}

void Ammo::Shoot(Vector3f startpos, Vector3f shootdiri, float shootspeed)
{
    m_startPos = startpos;
    m_shootDiri = shootdiri;
    m_shootSpeed = shootspeed;

    m_matrix.SetPos(startpos);

    SetNewtonWorld(m_weapon->GetShooter()->GetNewtonWorld());
    SetUpdatedMatrix(&m_matrix);

    Settings::Physics& worldSettings = m_playManager->manager.app->globalSettings.physics;

    BuildSphereNode(worldSettings.weaponSize, worldSettings.weaponMasse);
    NewtonBodySetForceAndTorqueCallback(m_body, NULL);
    NewtonBodySetLinearDamping(m_body, 0);
    NewtonBodySetContinuousCollisionMode(m_body, true);

    m_playManager->manager.material->AddWeapon(this);

    NewtonBodyAddImpulse(m_body, m_shootDiri*m_shootSpeed, m_startPos);
}

// WeaponBlaster ---------------------------------------------------------------

WeaponBlaster::WeaponBlaster(PlayManager* playManager) : Weapon(playManager)
{
    SetMaxAmmoCount(200);
    SetAmmoCount(180);
    SetMaxAmmoDammage(10);
    SetShootCadency(64);
    SetShootSpeed(64);
    SetFireSound(SOUND_BLASTER);

    SetTexture(WEAPON_BLASTER);
    SetNumber(200);
    Build();

    m_weaponName = "Blaster";
}

void WeaponBlaster::ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    startpos += tools::rand(Vector3f(-m_worldSettings.playerSize * 0.5),
                            Vector3f(m_worldSettings.playerSize * 0.5));

    Vector3f shootdiri = (targetpos - startpos).Normalize();

    // Creation du tire
    Ammo * fire = new Ammo(this);
    fire->Shoot(startpos, shootdiri, m_shootSpeed);

    m_playManager->manager.material->AddWeapon(fire);

    m_ammosPack.push_back(fire);
}

// WeaponShotgun ---------------------------------------------------------------

WeaponShotgun::WeaponShotgun(PlayManager* playManager) : Weapon(playManager)
{
    SetMaxAmmoCount(50);
    SetAmmoCount(40);
    SetMaxAmmoDammage(25);
    SetShootCadency(512);
    SetShootSpeed(64);
    SetFireSound(SOUND_SHOTGUN);

    SetTexture(WEAPON_SHOTGUN);
    SetNumber(50);
    Build();

    m_weaponName = "Shotgun";
}

void WeaponShotgun::ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    for(int i = 0; i < 7; i++)
    {
        Vector3f shootdiri = (targetpos - startpos).Normalize();

        shootdiri += tools::rand(Vector3f(-m_worldSettings.playerSize * 0.5),
                                 Vector3f(m_worldSettings.playerSize * 0.5));

        // Creation du tire
        Ammo * fire = new Ammo(this);
        fire->Shoot(startpos, shootdiri, m_shootSpeed);

        m_playManager->manager.material->AddWeapon(fire);

        m_ammosPack.push_back(fire);
    }

    m_ammoCount--;
}

// WeaponBomb ------------------------------------------------------------------

WeaponBomb::WeaponBomb(PlayManager* playManager) : Weapon(playManager)
{
    SetMaxAmmoCount(20);
    SetAmmoCount(18);
    SetMaxAmmoDammage(100);
    SetShootCadency(512);
    SetShootSpeed(0);
    SetFireSound(SOUND_BOMB);

    SetTexture(WEAPON_BOMB);
    SetNumber(20);
    Build();

    m_weaponName = "Bomb";
}

void WeaponBomb::ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    Vector3f shootdiri = (targetpos - startpos).Normalize();

    // Creation du tire
    Ammo * fire = new Ammo(this);
    fire->Shoot(startpos, shootdiri, m_shootSpeed);

    m_playManager->manager.material->AddWeapon(fire);

    m_ammosPack.push_back(fire);
}

// WeaponFinder ----------------------------------------------------------------

WeaponFinder::WeaponFinder(PlayManager* playManager) : Weapon(playManager)
{
    SetMaxAmmoCount(200);
    SetAmmoCount(180);
    SetMaxAmmoDammage(50);
    SetShootCadency(256);
    SetShootSpeed(16);
    SetFireSound(SOUND_FINDER);

    SetTexture(WEAPON_FINDER);
    SetNumber(200);
    Build();

    m_weaponName = "Finder";
}

void WeaponFinder::Process()
{
    Particle* particles = BeginParticlesPosProcess();

    for(unsigned i = 0; i < m_ammosPack.size(); i++)
    {
        if(m_ammosPack[i]->IsDeadAmmo())
        {
            delete m_ammosPack[i];
            m_ammosPack.erase((vector<Ammo*>::iterator) & m_ammosPack[i]);
        }

        else
        {
            m_ammosPack[i]->UpdateMatrix();
            particles[i].pos = m_ammosPack[i]->NewtonNode::GetPos();

            Vector3f minDist = m_mapAABB.max - m_mapAABB.min;

            Player::Array& players = m_playManager->players;
            for(unsigned j = 0; j < players.size(); j++)
                if(players[j] != m_parent)
                    minDist = min(players[j]->NewtonNode::GetPos() - m_ammosPack[i]->NewtonNode::GetPos(), minDist);

            m_ammosPack[i]->SetApplyForce(minDist.Normalize() * m_shootSpeed);
        }
    }

    EndParticlesPosProcess();

    SetDrawNumber(m_ammosPack.size());
}

void WeaponFinder::ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    Vector3f shootdiri = (targetpos - startpos).Normalize();
    shootdiri.y = 0.8f;

    // Creation du tire
    Ammo * fire = new Ammo(this);
    fire->Shoot(startpos, shootdiri, m_shootSpeed);

    NewtonBodySetForceAndTorqueCallback(fire->GetBody(), NewtonParallelScene::ApplyForceAndTorque);

    m_playManager->manager.material->AddWeapon(fire);

    m_ammosPack.push_back(fire);
}
