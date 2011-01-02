#include "Weapon.h"

#include "PlayManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"
#include "BldParser.h"

#include "Player.h"
#include "Item.h"

#include "Define.h"

#include <fmod_errors.h>

typedef boost::filesystem::path fspath;

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// Weapon ----------------------------------------------------------------------

Weapon::Weapon(PlayManager* playManager) : ParticlesEmiter(playManager->parallelscene.particles)
{
    m_playManager = playManager;
    m_worldSettings = m_playManager->manager.app->globalSettings.physics;
    m_soundManager = m_playManager->manager.sound;

    m_shooter = NULL;

    m_maxAmmoDammage = 0;
    m_maxAmmoCount = 0;

    m_shootSize = 1;

    m_ammoCount = 0;

    m_shootSpeed = 0;
    m_shootCadency = 0;

    m_mapAABB = m_playManager->map.aabb;
}

void Weapon::SetShootSize(unsigned shootSize)
{
    this->m_shootSize = shootSize;
}

unsigned Weapon::GetShootSize() const
{
    return m_shootSize;
}

Weapon::Weapon(const Weapon& copy) : ParticlesEmiter(copy)
{
    *this = copy;
}

Weapon::~Weapon()
{
    for(unsigned i = 0; i < m_bulletArray.size(); i++)
        delete m_bulletArray[i];
}

void Weapon::Process()
{
    unsigned requsetSize = m_ammoCount * m_shootSize + m_bulletArray.size();

    if(requsetSize > m_number)
    {
        SetNumber(requsetSize);
        Build();
    }

    Particle* particles = BeginParticlesPosProcess();

    for(unsigned i = 0; i < m_bulletArray.size(); i++)
    {
        if(m_bulletArray[i]->IsDeadAmmo())
        {
            delete m_bulletArray[i], m_bulletArray[i] = NULL;
        }

        else
        {
            m_bulletArray[i]->Process();
            particles[i].pos = m_bulletArray[i]->GetPos();
        }
    }

    EndParticlesPosProcess();

    Bullet::Array::iterator newend = remove(m_bulletArray.begin(), m_bulletArray.end(), (Bullet*)NULL);
    m_bulletArray.erase(newend, m_bulletArray.end());

    SetDrawNumber(m_bulletArray.size());

    m_parallelScene->PushToDraw(this);
}

void Weapon::SetFireSound(std::string fireSound)
{
    fspath path(fireSound);
    m_soundID = path.stem();

    m_soundManager->RegisterSound(m_soundID, fireSound);
}

bool Weapon::IsEmpty()
{
    return (m_ammoCount == 0);
}

void Weapon::SetShooter(Player* shooter)
{
    this->m_shooter = shooter;
}

Player* Weapon::GetShooter() const
{
    return m_shooter;
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
        if(m_shooter->clocks.shoot.IsEsplanedTime(1000))
            m_soundManager->Play("noAvailable", m_shooter);

        return;
    }

    // Controle de la cadence de tire
    long shootCadency = m_playManager->GetBullettime()->IsActive() ? m_shootCadency * 4 : m_shootCadency;

    if(!m_shootCadencyClock.IsEsplanedTime(shootCadency))
        return;

    if(m_bulletArray.size() > m_maxAmmoCount)
        return;

    m_soundManager->Play(m_soundID, m_shooter);

    m_ammoCount--;

    ProcessShoot(startpos, targetpos);
}

void Weapon::SetShootCadency(unsigned shootCadency)
{
    this->m_shootCadency = shootCadency;
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

bool Weapon::operator==(const Weapon& copy)
{
    return m_name == copy.m_name;
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

    SetWeaponName("Blaster");
}

void WeaponBlaster::ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    startpos += tools::rand(Vector3f(-m_worldSettings.playerSize * 0.5),
                            Vector3f(m_worldSettings.playerSize * 0.5));

    // Creation du tire
    Bullet* fire = new Bullet(m_playManager);
    fire->SetWeapon(this);
    fire->SetDammage(tools::rand(1, m_maxAmmoDammage));
    fire->Shoot(startpos, targetpos, m_shootSpeed);

    m_bulletArray.push_back(fire);
}

// WeaponShotgun ---------------------------------------------------------------

WeaponShotgun::WeaponShotgun(PlayManager* playManager) : Weapon(playManager)
{
    SetMaxAmmoCount(50);
    SetAmmoCount(40);
    SetMaxAmmoDammage(75);
    SetShootCadency(512);
    SetShootSpeed(64);
    SetFireSound(SOUND_SHOTGUN);
    SetShootSize(7);

    SetTexture(WEAPON_SHOTGUN);
    SetNumber(50 * 7);
    Build();

    SetWeaponName("Shotgun");
}

void WeaponShotgun::ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    for(int i = 0; i < 7; i++)
    {
        // Creation du tire
        Bullet * fire = new Bullet(m_playManager);
        fire->SetWeapon(this);
        fire->SetDammage(tools::rand(1, m_maxAmmoDammage));
        fire->Shoot(startpos, targetpos, m_shootSpeed, 0.1);

        m_bulletArray.push_back(fire);
    }
}

// WeaponBomb ------------------------------------------------------------------

WeaponBomb::WeaponBomb(PlayManager* playManager) : Weapon(playManager)
{
    SetMaxAmmoCount(80);
    SetAmmoCount(60);
    SetMaxAmmoDammage(100);
    SetShootCadency(512);
    SetShootSpeed(8);
    SetFireSound(SOUND_BOMB);
    SetShootSize(8);

    SetTexture(WEAPON_BOMB);
    SetNumber(m_maxAmmoCount * m_shootSize);
    Build();

    SetWeaponName("Bomb");
}

void WeaponBomb::ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    Vector3f relrot(0, 0, 0.25);

    for(unsigned i = 0; i < m_shootSize; i++)
    {
        relrot.Rotate(45 * i, 0);

        Bullet * fire = new Bullet(m_playManager);
        fire->SetWeapon(this);
        fire->SetDammage(100);
        fire->Shoot(startpos + relrot, startpos + (relrot * 4.0f), m_shootSpeed);

        m_bulletArray.push_back(fire);
    }
}

// WeaponFinder ----------------------------------------------------------------

WeaponFinder::WeaponFinder(PlayManager* playManager) : Weapon(playManager)
{
    SetMaxAmmoCount(200);
    SetAmmoCount(180);
    SetMaxAmmoDammage(50);
    SetShootCadency(64);
    SetShootSpeed(64);
    SetFireSound(SOUND_FINDER);

    SetTexture(WEAPON_FINDER);
    SetNumber(200);
    Build();

    SetWeaponName("Finder");
}

void WeaponFinder::Process()
{
    Particle* particles = BeginParticlesPosProcess();

    for(unsigned i = 0; i < m_bulletArray.size(); i++)
    {
        if(m_bulletArray[i]->IsDeadAmmo())
        {
            delete m_bulletArray[i], m_bulletArray[i] = NULL;
        }

        else
        {
            m_bulletArray[i]->Process();
            particles[i].pos = m_bulletArray[i]->GetPos();

            bool targetLocked = false;

            Vector3f minDist = m_mapAABB.max - m_mapAABB.min;

            const Player::Array& players = m_playManager->GetTargetsOf(m_shooter);

            for(unsigned j = 0; j < players.size(); j++)
            {
                Vector3f ammodiri = m_bulletArray[i]->GetVelocity().Normalize();
                Vector3f playerdiri = (players[j]->GetPos() - m_bulletArray[i]->GetPos()).Normalize();

                if(Vector3f::Dot(playerdiri, ammodiri) > 0.9f)
                {
                    minDist = min(players[j]->GetPos() - m_bulletArray[i]->GetPos(), minDist);
                    targetLocked = true;
                }
            }

            if(targetLocked)
            {
                m_bulletArray[i]->SetApplyForce(minDist.Normalize() * m_shootSpeed);
            }
        }
    }

    EndParticlesPosProcess();

    Bullet::Array::iterator newend = remove(m_bulletArray.begin(), m_bulletArray.end(), (Bullet*)NULL);
    m_bulletArray.erase(newend, m_bulletArray.end());

    SetDrawNumber(m_bulletArray.size());

    m_parallelScene->PushToDraw(this);
}

void WeaponFinder::ProcessShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    // Creation du tire
    Bullet * fire = new Bullet(m_playManager);
    fire->SetWeapon(this);
    fire->SetDammage(tools::rand(1, m_maxAmmoDammage));
    fire->Shoot(startpos, targetpos, m_shootSpeed);

    m_bulletArray.push_back(fire);
}

// Bullet ----------------------------------------------------------------------

Bullet::Bullet(PlayManager* playManager) : NewtonNode(playManager->parallelscene.newton)
{
    m_playManager = playManager;
    m_life = 300;
    m_dammage = 0;
    m_weapon = NULL;
}

void Bullet::SetShootSpeed(float shootSpeed)
{
    this->m_shootSpeed = shootSpeed;
}

float Bullet::GetShootSpeed() const
{
    return m_shootSpeed;
}

void Bullet::SetDammage(int dammage)
{
    this->m_dammage = dammage;
}

int Bullet::GetDammage() const
{
    return m_dammage;
}

void Bullet::SetShootDiri(tbe::Vector3f shootDiri)
{
    this->m_shootDiri = shootDiri;
}

tbe::Vector3f Bullet::GetShootDiri() const
{
    return m_shootDiri;
}

void Bullet::SetTargetPos(tbe::Vector3f targetPos)
{
    this->m_targetPos = targetPos;
}

tbe::Vector3f Bullet::GetTargetPos() const
{
    return m_targetPos;
}

void Bullet::SetStartPos(tbe::Vector3f startPos)
{
    this->m_startPos = startPos;
}

tbe::Vector3f Bullet::GetStartPos() const
{
    return m_startPos;
}

void Bullet::SetWeapon(Weapon* weapon)
{
    this->m_weapon = weapon;
}

Weapon* Bullet::GetWeapon() const
{
    return m_weapon;
}

void Bullet::SetLife(int life)
{
    this->m_life = life;
}

int Bullet::GetLife() const
{
    return m_life;
}

bool Bullet::IsDeadAmmo()
{
    return (m_life <= 0 || !m_playManager->map.aabb.IsInner(m_matrix.GetPos()));
}

void Bullet::Shoot(tbe::Vector3f startpos, tbe::Vector3f targetpos, float shootspeed, float accuracy)
{
    m_startPos = startpos;
    m_targetPos = targetpos;
    m_shootDiri = (targetpos - startpos).Normalize();
    m_shootSpeed = shootspeed;

    m_shootDiri += tools::rand(AABB(-accuracy, accuracy));

    m_matrix.SetPos(startpos);

    SetUpdatedMatrix(&m_matrix);

    Settings::Physics& worldSettings = m_playManager->manager.app->globalSettings.physics;

    BuildSphereNode(worldSettings.weaponSize, worldSettings.weaponMasse);
    NewtonBodySetForceAndTorqueCallback(m_body, NULL);
    NewtonBodySetLinearDamping(m_body, 0);
    NewtonBodySetContinuousCollisionMode(m_body, true);

    m_playManager->manager.material->AddBullet(this);

    NewtonBodyAddImpulse(m_body, m_shootDiri*m_shootSpeed, m_startPos);
}
