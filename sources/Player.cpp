#include "Player.h"

#include <boost/foreach.hpp>

#include "AppManager.h"
#include "GameManager.h"
#include "MaterialManager.h"
#include "MapElement.h"
#include "SoundManager.h"
#include "BulletTime.h"
#include "Weapon.h"
#include "Bullet.h"

#include "Define.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// PlayerEngine ----------------------------------------------------------------

Player::Player(GameManager* playManager, std::string name, std::string model) : MapElement(playManager)
{
    // Attributes
    m_nickname = name;
    m_id = "player";
    m_playManager = playManager;
    m_curWeapon = m_weaponsPack.end();
    m_curPower = m_powerPack.end();
    m_killed = false;
    m_life = 100;
    m_energy = 100;
    m_attachedCotroller = NULL;
    m_soundManager = playManager->manager.sound;

    // Rendue
    m_visualBody = new OBJMesh(m_gameManager->parallelscene.meshs);
    m_visualBody->open(model);

    MapElement::m_visualBody = m_visualBody;

    // Effet explosion
    m_deadExplode = new ParticlesEmiter(playManager->parallelscene.particles);
    m_deadExplode->setTexture(PARTICLE_EXPLODE);
    m_deadExplode->setLifeInit(1.0);
    m_deadExplode->setLifeDown(1.0);
    m_deadExplode->setFreeMove(0.8);
    m_deadExplode->setNumber(64);
    m_deadExplode->setAutoRebuild(false);
    m_deadExplode->setParent(m_visualBody);

    // Physique
    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->setUpdatedMatrix(&m_visualBody->getMatrix());
    m_physicBody->buildSphereNode(m_worldSettings.playerSize, m_worldSettings.playerMasse);

    m_visualBody->addChild(m_physicBody);

    NewtonBodySetForceAndTorqueCallback(m_physicBody->getBody(), MapElement::applyForceAndTorqueCallback);

    NewtonBodySetLinearDamping(m_physicBody->getBody(), m_worldSettings.playerLinearDamping);
    NewtonBodySetAngularDamping(m_physicBody->getBody(), m_worldSettings.playerAngularDamping);

    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
    NewtonBodySetUserData(m_physicBody->getBody(), this);

    reBorn();
}

Player::~Player()
{
    delete m_attachedCotroller;

    free();
}

void Player::free()
{
    for(unsigned i = 0; i < m_weaponsPack.size(); i++)
        delete m_weaponsPack[i];

    m_weaponsPack.clear();

    for(unsigned i = 0; i < m_powerPack.size(); i++)
        delete m_powerPack[i];

    m_powerPack.clear();
}

void Player::setInLastSpawnPoint()
{
    m_physicBody->setVelocity(0);
    m_physicBody->setMatrix(m_lastSpawnPoint);
}

void Player::randomPosOnFloor()
{
    m_physicBody->setVelocity(0);
    m_physicBody->setMatrix(m_playManager->getRandomPosOnTheFloor());
}

void Player::attachItem(Item* item)
{
    item->modifPlayer(this);
}

void Player::attachController(Controller* controller)
{
    delete m_attachedCotroller, m_attachedCotroller = controller;
}

Controller* Player::getAttachedCotroller() const
{
    return m_attachedCotroller;
}

void Player::process()
{
    if(m_playManager->isGameOver())
        return;

    if(m_attachedCotroller && !m_killed)
        m_attachedCotroller->process(this);

    if(m_energy > 0 && !m_energyVoid)
    {
        (*m_curPower)->process();

        if(m_energy <= 0)
        {
            m_energyVoid = true;
            (*m_curPower)->diactivate();
        }
    }

    if(m_energy < 100)
    {
        m_energy++;

        if(m_energy > 50)
            m_energyVoid = false;
    }
}

bool Player::shoot(Vector3f targetpos)
{
    if(!onShoot.empty() && !onShoot(this, targetpos))
        return false;

    return (*m_curWeapon)->shoot(m_visualBody->getPos(), targetpos);
}

inline bool playerCanJump(scene::NewtonNode* node1, scene::NewtonNode* node2)
{
    Vector3f contact, normal, penetration;

    int contactPoint = NewtonCollisionCollide(node1->getParallelScene()->getNewtonWorld(), 1,
                                              NewtonBodyGetCollision(node1->getBody()), node1->getMatrix(),
                                              NewtonBodyGetCollision(node2->getBody()), node2->getMatrix(),
                                              contact, normal, penetration, 0);

    float dot = Vector3f::dot(normal, Vector3f(0, 1, 0));

    return (contactPoint > 0 && dot > 0.25);
}

void Player::jump()
{

    foreach(MapElement* elem, m_playManager->map.mapElements)
    {
        NewtonNode* nnode = elem->getPhysicBody();

        if(nnode && playerCanJump(nnode, m_physicBody))
        {
            NewtonBodyAddImpulse(m_physicBody->getBody(),
                                 Vector3f(0, m_worldSettings.playerJumpForce, 0),
                                 m_visualBody->getMatrix().getPos());
            break;
        }
    }
}

void Player::brake()
{
    if(!clocks.brake.isEsplanedTime(500))
        return;

    m_physicBody->setOmega(0);
    m_physicBody->setVelocity(0);
    m_soundManager->playSound("stop", this);
}

bool Player::power(bool stat, tbe::Vector3f targetpos)
{
    if(!onPower.empty() && !onPower(this, stat, targetpos))
        return false;

    if(stat)
        (*m_curPower)->activate(targetpos);
    else
        (*m_curPower)->diactivate();

    return true;
}

inline bool isWeaponSameName(Weapon* w1, Weapon* w2)
{
    return w1->getWeaponName() == w2->getWeaponName();
}

void Player::addWeapon(Weapon* weapon)
{
    Weapon::Array::iterator select = find_if(m_weaponsPack.begin(), m_weaponsPack.end(),
                                             bind2nd(ptr_fun(isWeaponSameName), weapon));

    if(select != m_weaponsPack.end())
    {
        (*select)->UpAmmoCount(weapon->getAmmoCount());
        delete weapon;
    }

    else
    {
        m_weaponsPack.push_back(weapon);

        m_curWeapon = --m_weaponsPack.end();
        (*m_curWeapon)->setShooter(this);
    }
}

void Player::slotWeapon(unsigned slot)
{
    for(unsigned i = 0; i < m_weaponsPack.size(); i++)
        if(m_weaponsPack[i]->getSlot() == slot)
        {
            m_curWeapon = m_weaponsPack.begin() + i;
            break;
        }
}

void Player::switchUpWeapon()
{
    m_curWeapon++;

    if(m_curWeapon >= m_weaponsPack.end())
        m_curWeapon = m_weaponsPack.begin();
}

void Player::switchDownWeapon()
{
    m_curWeapon--;

    if(m_curWeapon < m_weaponsPack.begin())
        m_curWeapon = --m_weaponsPack.end();
}

void Player::setCurWeapon(unsigned slot)
{
    if(slot >= m_weaponsPack.size())
        throw tbe::Exception("PlayerEngine::setCurWeapon; Invalid weapon slot (%d)", slot);

    m_curWeapon = m_weaponsPack.begin() + slot;
}

Weapon* Player::getCurWeapon() const
{
    return (*m_curWeapon);
}

inline bool isPowerSameName(Power* w1, Power* w2)
{
    return w1->getName() == w2->getName();
}

void Player::addPower(Power* power)
{
    Power::Array::iterator select = find_if(m_powerPack.begin(), m_powerPack.end(),
                                            bind2nd(ptr_fun(isPowerSameName), power));

    if(select == m_powerPack.end())
    {
        m_powerPack.push_back(power);

        m_curPower = --m_powerPack.end();
        (*m_curPower)->setOwner(this);
    }
}

void Player::slotPower(unsigned slot)
{
}

void Player::switchUpPower()
{
}

void Player::switchDownPower()
{
}

void Player::setCurPower(unsigned slot)
{
}

Power* Player::getCurPower() const
{
    return *m_curPower;
}

void Player::reBorn()
{
    m_life = 100;
    m_killed = false;

    m_playManager->manager.material->setGhost(this, false);

    m_visualBody->setVisible(true);

    m_physicBody->setApplyGravity(true);

    free();

    // Arme principale
    WeaponBlaster* blaster = new WeaponBlaster(m_playManager);
    addWeapon(blaster);

    // Pouvoir principale
    BulletTime* btime = new BulletTime(m_playManager);
    addPower(btime);

    onRespawn(this);

    m_lastSpawnPoint = m_physicBody->getPos();
}

void Player::kill(Player* killer)
{
    if(!onKilled.empty() && !onKilled(this, killer))
        return;

    m_killed = true;
    m_life = 0;

    m_deadExplode->build();

    m_playManager->manager.material->setGhost(this, true);

    clocks.readyToDelete.snapShoot();

    m_visualBody->setVisible(false);

    m_physicBody->setVelocity(0);
    m_physicBody->setOmega(0);
    m_physicBody->setApplyForce(0);
    m_physicBody->setApplyTorque(0);
    m_physicBody->setApplyGravity(false);

    m_soundManager->playSound("kill", this);
}

bool Player::isKilled() const
{
    return m_killed;
}

void Player::upEnergy(int value)
{
    this->m_energy += value;
}

void Player::setEnergy(int energy)
{
    this->m_energy = energy;
}

int Player::getEnergy() const
{
    return m_energy;
}

void Player::setName(std::string name)
{
    this->m_nickname = name;
}

std::string Player::getName() const
{
    return m_nickname;
}

void Player::upLife(int life)
{
    m_life = max(min(m_life + life, 100), 0);
}

void Player::setLife(int life)
{
    m_life = max(min(life, 100), 0);
}

int Player::getLife() const
{
    return m_life;
}

void Player::takeDammage(int dammage, Player* killer)
{
    if(m_killed)
        return;

    if(!onDammage.empty() && !onDammage(this, killer))
        return;

    m_life = max(m_life - dammage, 0);

    if(m_life <= 0)
        kill(killer);

    if(m_playManager->getUserPlayer() == this)
        m_playManager->hudDammage(true);

    m_soundManager->playSound("hit", this);
}

GameManager* Player::getGameManager() const
{
    return m_playManager;
}

void Player::makeTransparent(bool enable, float alpha)
{
    HardwareBuffer& hardbuf = m_visualBody->getHardwareBuffer();

    Vertex* vs = hardbuf.lock();

    if(enable)
        for(unsigned i = 0; i < hardbuf.getVertexCount(); i++)
            vs[i].color.w = alpha;
    else
        for(unsigned i = 0; i < hardbuf.getVertexCount(); i++)
            vs[i].color.w = 1;

    hardbuf.unlock();

    Material::Array mats = m_visualBody->getAllMaterial();

    if(enable)
        for(unsigned i = 0; i < mats.size(); i++)
            mats[i]->enable(Material::BLEND_MOD);
    else
        for(unsigned i = 0; i < mats.size(); i++)
            mats[i]->disable(Material::BLEND_MOD);
}

/*
Player::StartProtection::StartProtection(Player* player)
{
    using namespace tbe;
    using namespace tbe::scene;

    player->makeTransparent(true);

    // player->setVisibleFromIA(false);
}

bool Player::StartProtection::onShoot(Player*)
{
    return false;
}

bool Player::StartProtection::onTakeDammage(Player*, Bullet*)
{
    return false;
}

bool Player::StartProtection::shutdown(Player* player)
{
    using namespace tbe;
    using namespace tbe::scene;

    if(m_clock.isEsplanedTime(player->m_playManager->worldSettings.playerStartImmunity))
    {
        player->makeTransparent(false);

        // player->setVisibleFromIA(true);

        return true;
    }

    return false;
}
 */
