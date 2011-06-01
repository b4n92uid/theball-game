#include "Weapon.h"

#include "GameManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"

#include "Player.h"
#include "Item.h"

#include "Define.h"

#include <fmod_errors.h>

typedef boost::filesystem::path fspath;

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// Weapon ----------------------------------------------------------------------

Weapon::Weapon(GameManager* playManager) : ParticlesEmiter(playManager->parallelscene.particles)
{
    m_playManager = playManager;
    m_worldSettings = m_playManager->manager.app->globalSettings.world;
    m_soundManager = m_playManager->manager.sound;

    m_shooter = NULL;

    m_maxAmmoDammage = 0;
    m_maxAmmoCount = 0;

    m_shootSize = 1;

    m_ammoCount = 0;

    m_shootSpeed = 0;
    m_shootCadency = 0;

    m_mapAABB = m_playManager->map.aabb;

    m_slot = 0;

    m_name = "Weapon";
}

void Weapon::setupBullet(Particle& p)
{
    p = Particle();
    p.life = 1;
    p.color = 1;
}

void Weapon::setShootSize(unsigned shootSize)
{
    this->m_shootSize = shootSize;
}

unsigned Weapon::getShootSize() const
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

void Weapon::process()
{
    unsigned requsetSize = m_ammoCount * m_shootSize + m_bulletArray.size();

    if(requsetSize > m_number)
    {
        setNumber(requsetSize);
        build();
    }

    Particle* particles = beginParticlesPosProcess();

    for(unsigned i = 0; i < m_bulletArray.size(); i++)
    {
        if(m_bulletArray[i]->isDeadAmmo())
        {
            delete m_bulletArray[i], m_bulletArray[i] = NULL;
        }

        else
        {
            m_bulletArray[i]->process();
            particles[i].pos = m_bulletArray[i]->getPos();
        }
    }

    endParticlesPosProcess();

    Bullet::Array::iterator newend = remove(m_bulletArray.begin(), m_bulletArray.end(), (Bullet*)NULL);
    m_bulletArray.erase(newend, m_bulletArray.end());

    setDrawNumber(m_bulletArray.size());
}

void Weapon::setFireSound(std::string fireSound)
{
    fspath path(fireSound);
    m_soundID = path.stem();

    m_soundManager->registerSound(m_soundID, fireSound);
}

bool Weapon::isEmpty()
{
    return (m_ammoCount == 0);
}

unsigned Weapon::getSlot() const
{
    return m_slot;
}

void Weapon::setShooter(Player* shooter)
{
    this->m_shooter = shooter;
}

Player* Weapon::getShooter() const
{
    return m_shooter;
}

std::string Weapon::getWeaponName() const
{
    return m_weaponName;
}

void Weapon::setWeaponName(std::string weaponName)
{
    this->m_weaponName = weaponName;
}

bool Weapon::shoot(Vector3f startpos, Vector3f targetpos)
{
    // Controle des munitions
    if(m_ammoCount <= 0)
    {
        if(m_shooter->clocks.shoot.isEsplanedTime(1000))
            m_soundManager->play("noAvailable", m_shooter);

        return false;
    }

    // Controle de la cadence de tire
    long shootCadency = m_playManager->getBullettime()->isActive() ? m_shootCadency * 4 : m_shootCadency;

    if(!m_shootCadencyClock.isEsplanedTime(shootCadency))
        return false;

    if(m_bulletArray.size() > m_maxAmmoCount)
        return false;

    m_soundManager->play(m_soundID, m_shooter);

    m_ammoCount--;

    processShoot(startpos, targetpos);

    return true;
}

void Weapon::setShootCadency(unsigned shootCadency)
{
    this->m_shootCadency = shootCadency;
}

unsigned Weapon::getShootCadency() const
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

void Weapon::setAmmoCount(unsigned ammoCount)
{

    if(ammoCount > m_maxAmmoCount)
        ammoCount = m_maxAmmoCount;

    this->m_ammoCount = ammoCount;
}

unsigned Weapon::getAmmoCount() const
{

    return m_ammoCount;
}

void Weapon::setMaxAmmoCount(unsigned maxAmmoCount)
{

    this->m_maxAmmoCount = maxAmmoCount;
}

unsigned Weapon::getMaxAmmoCount() const
{

    return m_maxAmmoCount;
}

void Weapon::setMaxAmmoDammage(unsigned maxAmmoDammage)
{

    this->m_maxAmmoDammage = maxAmmoDammage;
}

unsigned Weapon::getMaxAmmoDammage() const
{

    return m_maxAmmoDammage;
}

void Weapon::setShootSpeed(float shootSpeed)
{
    this->m_shootSpeed = shootSpeed;
}

float Weapon::getShootSpeed() const
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

Weapon* Weapon::clone()
{
    return NULL;
}

// WeaponBlaster ---------------------------------------------------------------

WeaponBlaster::WeaponBlaster(GameManager* playManager) : Weapon(playManager)
{
    setMaxAmmoCount(200);
    setAmmoCount(180);
    setMaxAmmoDammage(10);
    setShootCadency(64);
    setShootSpeed(64);
    setFireSound(SOUND_BLASTER);

    setTexture(WEAPON_BLASTER);
    setNumber(200);
    build();

    setWeaponName("Blaster");

    m_slot = 1;
}

void WeaponBlaster::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    startpos += tools::rand(Vector3f(-m_worldSettings.playerSize * 0.5),
                            Vector3f(m_worldSettings.playerSize * 0.5));

    // Creation du tire
    Bullet* fire = new Bullet(m_playManager);
    fire->setWeapon(this);
    fire->setDammage(tools::rand(1, m_maxAmmoDammage));
    fire->shoot(startpos, targetpos, m_shootSpeed);

    m_bulletArray.push_back(fire);
}

// WeaponShotgun ---------------------------------------------------------------

WeaponShotgun::WeaponShotgun(GameManager* playManager) : Weapon(playManager)
{
    setMaxAmmoCount(50);
    setAmmoCount(40);
    setMaxAmmoDammage(75);
    setShootCadency(512);
    setShootSpeed(64);
    setFireSound(SOUND_SHOTGUN);
    setShootSize(7);

    setTexture(WEAPON_SHOTGUN);
    setNumber(50 * 7);
    build();

    setWeaponName("Shotgun");

    m_slot = 2;
}

void WeaponShotgun::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    for(int i = 0; i < 7; i++)
    {
        // Creation du tire
        Bullet * fire = new Bullet(m_playManager);
        fire->setWeapon(this);
        fire->setDammage(tools::rand(1, m_maxAmmoDammage));
        fire->shoot(startpos, targetpos, m_shootSpeed, 0.1);

        m_bulletArray.push_back(fire);
    }
}

// WeaponBomb ------------------------------------------------------------------

WeaponBomb::WeaponBomb(GameManager* playManager) : Weapon(playManager)
{
    setMaxAmmoCount(80);
    setAmmoCount(60);
    setMaxAmmoDammage(100);
    setShootCadency(512);
    setShootSpeed(8);
    setFireSound(SOUND_BOMB);
    setShootSize(8);

    setTexture(WEAPON_BOMB);
    setNumber(m_maxAmmoCount * m_shootSize);
    build();

    setWeaponName("Bomb");

    m_slot = 4;
}

void WeaponBomb::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    Vector3f relrot(0, 0, 0.25);

    for(unsigned i = 0; i < m_shootSize; i++)
    {
        relrot.rotate(45 * i, 0);

        Bullet * fire = new Bullet(m_playManager);
        fire->setWeapon(this);
        fire->setDammage(100);
        fire->shoot(startpos + relrot, startpos + (relrot * 4.0f), m_shootSpeed);

        m_bulletArray.push_back(fire);
    }
}

// WeaponFinder ----------------------------------------------------------------

WeaponFinder::WeaponFinder(GameManager* playManager) : Weapon(playManager)
{
    setMaxAmmoCount(200);
    setAmmoCount(180);
    setMaxAmmoDammage(50);
    setShootCadency(64);
    setShootSpeed(64);
    setFireSound(SOUND_FINDER);

    setTexture(WEAPON_FINDER);
    setNumber(200);
    build();

    setWeaponName("Finder");

    m_slot = 3;
}

void WeaponFinder::process()
{
    Particle* particles = beginParticlesPosProcess();

    for(unsigned i = 0; i < m_bulletArray.size(); i++)
    {
        if(m_bulletArray[i]->isDeadAmmo())
        {
            delete m_bulletArray[i], m_bulletArray[i] = NULL;
        }

        else
        {
            m_bulletArray[i]->process();
            particles[i].pos = m_bulletArray[i]->getPos();

            bool targetLocked = false;

            Vector3f minDist = m_mapAABB.max - m_mapAABB.min;

            const Player::Array& targets = m_playManager->getTargetsOf(m_shooter);

            for(unsigned j = 0; j < targets.size(); j++)
            {
                Vector3f ammodiri = m_bulletArray[i]->getVelocity().normalize();
                Vector3f targetdiri = (targets[j]->getVisualBody()->getPos() - m_bulletArray[i]->getPos()).normalize();

                if(Vector3f::dot(targetdiri, ammodiri) > 0.5f)
                {
                    minDist = min(targets[j]->getVisualBody()->getPos() - m_bulletArray[i]->getPos(), minDist);
                    targetLocked = true;
                }
            }

            if(targetLocked)
            {
                m_bulletArray[i]->setApplyForce(minDist.normalize() * m_shootSpeed / 2.0f);
            }
        }
    }

    endParticlesPosProcess();

    Bullet::Array::iterator newend = remove(m_bulletArray.begin(), m_bulletArray.end(), (Bullet*)NULL);
    m_bulletArray.erase(newend, m_bulletArray.end());

    setDrawNumber(m_bulletArray.size());
}

void WeaponFinder::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    // Creation du tire
    Bullet * fire = new Bullet(m_playManager);
    fire->setWeapon(this);
    fire->setDammage(tools::rand(1, m_maxAmmoDammage));
    fire->shoot(startpos, targetpos, m_shootSpeed);

    m_bulletArray.push_back(fire);
}

// Bullet ----------------------------------------------------------------------

Bullet::Bullet(GameManager* playManager) : NewtonNode(playManager->parallelscene.newton)
{
    m_playManager = playManager;
    m_life = 300;
    m_dammage = 0;
    m_weapon = NULL;
}

void Bullet::setShootSpeed(float shootSpeed)
{
    this->m_shootSpeed = shootSpeed;
}

float Bullet::getShootSpeed() const
{
    return m_shootSpeed;
}

void Bullet::setDammage(int dammage)
{
    this->m_dammage = dammage;
}

int Bullet::getDammage() const
{
    return m_dammage;
}

void Bullet::setShootDiri(tbe::Vector3f shootDiri)
{
    this->m_shootDiri = shootDiri;
}

tbe::Vector3f Bullet::getShootDiri() const
{
    return m_shootDiri;
}

void Bullet::setTargetPos(tbe::Vector3f targetPos)
{
    this->m_targetPos = targetPos;
}

tbe::Vector3f Bullet::getTargetPos() const
{
    return m_targetPos;
}

void Bullet::setStartPos(tbe::Vector3f startPos)
{
    this->m_startPos = startPos;
}

tbe::Vector3f Bullet::getStartPos() const
{
    return m_startPos;
}

void Bullet::setWeapon(Weapon* weapon)
{
    this->m_weapon = weapon;
}

Weapon* Bullet::getWeapon() const
{
    return m_weapon;
}

void Bullet::setLife(int life)
{
    this->m_life = life;
}

int Bullet::getLife() const
{
    return m_life;
}

bool Bullet::isDeadAmmo()
{
    return (m_life <= 0 || !m_playManager->map.aabb.isInner(m_matrix.getPos()));
}

void Bullet::shoot(tbe::Vector3f startpos, tbe::Vector3f targetpos, float shootspeed, float accuracy)
{
    m_startPos = startpos;
    m_targetPos = targetpos;
    m_shootDiri = (targetpos - startpos).normalize();
    m_shootSpeed = shootspeed;

    m_shootDiri += tools::rand(AABB(-accuracy, accuracy));

    m_matrix.setPos(startpos);

    setUpdatedMatrix(&m_matrix);

    Settings::World& worldSettings = m_playManager->manager.app->globalSettings.world;

    buildSphereNode(worldSettings.weaponSize, worldSettings.weaponMasse);
    NewtonBodySetContinuousCollisionMode(m_body, true);

    m_applyGravity = false;

    m_playManager->manager.material->addBullet(this);

    NewtonBodyAddImpulse(m_body, m_shootDiri * m_shootSpeed, m_startPos);
}
