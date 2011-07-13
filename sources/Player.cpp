#include "Player.h"

#include <boost/foreach.hpp>

#include "AppManager.h"
#include "GameManager.h"
#include "MaterialManager.h"
#include "MapElement.h"
#include "StaticElement.h"
#include "SoundManager.h"
#include "BulletTime.h"
#include "Weapon.h"
#include "Bullet.h"

#include "Define.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// PlayerEngine ----------------------------------------------------------------

Player::Player(GameManager* gameManager, std::string name, std::string model)
: MapElement(gameManager), m_settings(gameManager->manager.app->globalSettings)
{
    // Attributes
    m_nickname = name;
    m_id = "player";
    m_playManager = gameManager;
    m_curWeapon = m_weaponsInventory.end();
    m_curPower = m_powersInventory.end();
    m_killed = false;
    m_life = 100;
    m_energy = 100;
    m_attachedCotroller = NULL;
    m_soundManager = gameManager->manager.sound;

    // Rendue
    m_visualBody = new OBJMesh(m_gameManager->parallelscene.meshs);
    m_visualBody->open(model);

    MapElement::m_visualBody = m_visualBody;

    // Effet explosion
    m_deadExplode = new ParticlesEmiter(gameManager->parallelscene.particles);
    m_deadExplode->setTexture(m_settings("particles.explosion"));
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
    for(unsigned i = 0; i < m_weaponsInventory.size(); i++)
        delete m_weaponsInventory[i];

    m_weaponsInventory.clear();
    m_curWeapon = m_weaponsInventory.end();

    for(unsigned i = 0; i < m_powersInventory.size(); i++)
        delete m_powersInventory[i];

    m_powersInventory.clear();
    m_curPower = m_powersInventory.end();
}

void Player::setInLastSpawnPoint()
{
    stopMotion();
    m_physicBody->setPos(m_lastSpawnPoint);
}

void Player::randomPosOnFloor()
{
    stopMotion();
    m_physicBody->setPos(m_playManager->getRandomPosOnTheFloor());
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

    if(m_curPower != m_powersInventory.end() && (*m_curPower)->isActive() && !m_energyVoid)
    {
        if(m_energy > 0)
        {
            (*m_curPower)->process();

            if(m_energy <= 0)
            {
                m_energyVoid = true;
                (*m_curPower)->diactivate();
            }
        }
    }
    else if(m_energy < 100)
    {
        m_energy++;

        if(m_energy > 50)
            m_energyVoid = false;
    }
}

bool Player::shoot(Vector3f targetpos)
{
    if(m_curWeapon == m_weaponsInventory.end())
        return false;

    if(!onShoot.empty() && !onShoot(this, targetpos))
        return false;

    return (*m_curWeapon)->shoot(m_visualBody->getPos(), targetpos);
}

inline bool isBodyContact(scene::NewtonNode* node1, scene::NewtonNode* node2)
{
    Vector3f contact, normal, penetration;

    int contactPoint = NewtonCollisionCollide(node1->getParallelScene()->getNewtonWorld(), 1,
                                              NewtonBodyGetCollision(node1->getBody()), node1->getMatrix(),
                                              NewtonBodyGetCollision(node2->getBody()), node2->getMatrix(),
                                              contact, normal, penetration, 0);

    float dot = Vector3f::dot(normal, Vector3f(0, 1, 0));

    return (contactPoint > 0 && dot > 0.25);
}

inline bool isPlayerCollidStaticElement(Player* player, const StaticElement::Array& elems)
{

    foreach(StaticElement* elem, elems)
    {
        NewtonNode* nnode = elem->getPhysicBody();

        if(nnode && isBodyContact(nnode, player->getPhysicBody()))
            return true;
    }

    return false;
}

void Player::move(tbe::Vector3f force)
{
    bool collideStatic = isPlayerCollidStaticElement(this, m_playManager->map.staticElements);

    /*
     * Enleve la force appliquer sur l'axe Y (Vertical)
     * pour eviter au joueur de voller
     */
    force.y = 0;

    /*
     * Normaliser le vecteur de force pour avoir une pouss� uniforme
     */
    force.normalize();

    /*
     * Diminue la force de mouvement dans les aires
     */
    if(!collideStatic)
        force /= 2.0f;

    if(onMove.empty() || onMove(this, force))
        m_physicBody->setApplyForce(force * m_worldSettings.playerMoveSpeed);
}

void Player::jump()
{
    bool allowed = isPlayerCollidStaticElement(this, m_playManager->map.staticElements);

    if(!onJump.empty())
        allowed = onJump(this, allowed);

    /*
     * Le joueur peut sauter seulement quand il est en contact avec
     * un �lement statique et que le produit scalair entre le vecteur (0,1,0)
     * et la normal de contact soit inferieur a 0.25
     */

    if(allowed)
    {
        Vector3f deltaVeloc(0, m_worldSettings.playerJumpForce, 0);
        deltaVeloc += m_physicBody->getApplyForce().normalize() * m_worldSettings.playerJumpForce / 2;

        Vector3f pointPos = m_physicBody->getPos();

        NewtonBodyAddImpulse(m_physicBody->getBody(), deltaVeloc, pointPos);
    }
}

bool Player::power(bool stat, tbe::Vector3f targetpos)
{
    if(m_curPower == m_powersInventory.end())
        return false;

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
    Weapon::Array::iterator select = find_if(m_weaponsInventory.begin(), m_weaponsInventory.end(),
                                             bind2nd(ptr_fun(isWeaponSameName), weapon));

    if(select != m_weaponsInventory.end())
    {
        (*select)->UpAmmoCount(weapon->getAmmoCount());
        delete weapon;
    }

    else
    {
        m_weaponsInventory.push_back(weapon);

        m_curWeapon = --m_weaponsInventory.end();
        (*m_curWeapon)->setShooter(this);
    }
}

void Player::slotWeapon(unsigned slot)
{
    for(unsigned i = 0; i < m_weaponsInventory.size(); i++)
        if(m_weaponsInventory[i]->getSlot() == slot)
        {
            m_curWeapon = m_weaponsInventory.begin() + i;
            break;
        }
}

void Player::switchUpWeapon()
{
    m_curWeapon++;

    if(m_curWeapon >= m_weaponsInventory.end())
        m_curWeapon = m_weaponsInventory.begin();
}

void Player::switchDownWeapon()
{
    m_curWeapon--;

    if(m_curWeapon < m_weaponsInventory.begin())
        m_curWeapon = --m_weaponsInventory.end();
}

Weapon* Player::getCurWeapon() const
{
    if(m_curWeapon == m_weaponsInventory.end())
        return NULL;
    else
        return (*m_curWeapon);
}

inline bool isPowerSameName(Power* w1, Power* w2)
{
    return w1->getName() == w2->getName();
}

void Player::addPower(Power* power)
{
    Power::Array::iterator select = find_if(m_powersInventory.begin(), m_powersInventory.end(),
                                            bind2nd(ptr_fun(isPowerSameName), power));

    if(select == m_powersInventory.end())
    {
        m_powersInventory.push_back(power);

        m_curPower = --m_powersInventory.end();
        (*m_curPower)->setOwner(this);
    }
}

void Player::slotPower(unsigned slot)
{
    for(unsigned i = 0; i < m_powersInventory.size(); i++)
        if(m_powersInventory[i]->getSlot() == slot)
        {
            (*m_curPower)->diactivate();
            m_curPower = m_powersInventory.begin() + i;
            break;
        }
}

void Player::switchUpPower()
{
    (*m_curPower)->diactivate();

    m_curPower++;

    if(m_curPower >= m_powersInventory.end())
        m_curPower = m_powersInventory.begin();
}

void Player::switchDownPower()
{
    (*m_curPower)->diactivate();

    m_curPower--;

    if(m_curPower < m_powersInventory.begin())
        m_curPower = --m_powersInventory.end();
}

Power* Player::getCurPower() const
{
    if(m_curPower != m_powersInventory.end())
        return *m_curPower;
    else
        return NULL;
}

void Player::reBorn()
{
    m_life = 100;
    m_killed = false;

    m_playManager->manager.material->setGhost(this, false);

    m_visualBody->setVisible(true);

    m_physicBody->setApplyGravity(true);

    free();

    onRespawn(this);

    m_lastSpawnPoint = m_physicBody->getPos();
}

void Player::kill(Player* killer)
{
    if(!onKilled.empty() && !onKilled(this, killer))
        return;

    m_killed = true;
    m_life = 0;

    stopMotion();

    m_deadExplode->build();

    m_playManager->manager.material->setGhost(this, true);

    clocks.readyToDelete.snapShoot();

    m_visualBody->setVisible(false);

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
        m_playManager->dammageEffect();

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
            mats[i]->enable(Material::BLEND_ADD);
    else
        for(unsigned i = 0; i < mats.size(); i++)
            mats[i]->disable(Material::BLEND_ADD);
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
