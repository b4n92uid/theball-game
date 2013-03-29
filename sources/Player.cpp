#include "Player.h"

#include <boost/foreach.hpp>

#include "Controller.h"
#include "AppManager.h"
#include "GameManager.h"
#include "MaterialManager.h"
#include "StaticElement.h"
#include "SoundManager.h"
#include "BulletTime.h"
#include "Power.h"
#include "Weapon.h"
#include "Bullet.h"

#include "Define.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// PlayerEngine ----------------------------------------------------------------

Player::Player(GameManager* gameManager, std::string nickname, Content::PlayerInfo* pinfo)
: MapElement(gameManager), m_settings(gameManager->manager.app->globalSettings)
{
    // Attributes
    m_nickname = nickname;
    m_id = "player";
    m_curWeapon = m_weaponsInventory.end();
    m_curPower = m_powersInventory.end();
    m_killed = false;
    m_life = 100;
    m_energy = 100;
    m_attachedCotroller = NULL;
    m_soundManager = gameManager->manager.sound;
    m_immunity = false;

    m_jumpStatu = 0;

    // Rendue
    ClassParser* loader = gameManager->manager.app->getClassParser();
    loader->load(pinfo->filepath);
    loader->build();

    MapElement::m_visualBody = m_visualBody = loader->getBuildedNode<Mesh*>()->clone();

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
    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton, m_visualBody);
    m_physicBody->buildSphereNode(m_worldSettings.playerSize, m_worldSettings.playerMasse);

    NewtonBodySetForceAndTorqueCallback(m_physicBody->getBody(), MapElement::applyForceAndTorqueCallback);
    NewtonBodySetTransformCallback(m_physicBody->getBody(), MapElement::applyTransformCallback);

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
    m_physicBody->setPos(m_gameManager->getRandomPosOnTheFloor());
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
    if(m_gameManager->isGameOver())
        return;

    if(m_physicBody->getVelocity() > 0.0)
        onVelocity(this, m_physicBody->getVelocity());

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

    return(*m_curWeapon)->shoot(m_visualBody->getPos(), targetpos);
}

inline bool isBodyContactOnFloor(scene::NewtonNode* node1, scene::NewtonNode* node2)
{
    const int pointCount = 2;

    Vector3f contact[pointCount], normal[pointCount];
    float penetration[pointCount];

    int contactPoint = NewtonCollisionCollide(node1->getParallelScene()->getNewtonWorld(), pointCount,
                                              NewtonBodyGetCollision(node1->getBody()), node1->getMatrix(),
                                              NewtonBodyGetCollision(node2->getBody()), node2->getMatrix(),
                                              &contact[0].x, &normal[0].x, penetration, 0);

    for(int i = 0; i < contactPoint; i++)
    {
        float dot = Vector3f::dot(normal[i], Vector3f(0, 1, 0));

        if(dot > 0.25)
            return true;
    }

    return false;
}

bool Player::isStayDown()
{

    foreach(StaticElement* elem, m_gameManager->map.staticElements)
    {
        scene::NewtonNode* nnode = elem->getPhysicBody();

        if(nnode && isBodyContactOnFloor(m_physicBody, nnode))
            return true;
    }

    return false;
}

void Player::move(tbe::Vector3f force)
{
    bool collideStatic = isStayDown();

    /*
     * Enleve la force appliquer sur l'axe Y (Vertical)
     * pour eviter au joueur de voller
     */
    force.y = 0;

    /*
     * Normaliser le vecteur de force pour avoir une poussé uniforme
     */
    force.normalize();

    /*
     * Diminue la force de mouvement dans les aires
     */
    if(!collideStatic)
    {
        force /= 2.0f;

        Vector3f vel = m_physicBody->getVelocity();
        vel.x *= 0.98;
        vel.z *= 0.98;

        m_physicBody->setVelocity(vel);
    }

    if(onMove.empty() || onMove(this, force))
        m_physicBody->setApplyForce(force * m_worldSettings.playerMoveSpeed);
}

void Player::jump()
{
    /*
     * Le joueur peut sauter seulement quand il est en contact avec
     * un élement statique et que le produit scalair entre le vecteur (0,1,0)
     * et la normal de contact soit inferieur a 0.25
     */

    bool stayDown = isStayDown();

    if(stayDown && m_jumpStatu == 1)
        m_jumpStatu = 0;

    if(!onJump.empty())
        stayDown = onJump(this, stayDown);

    if(stayDown || !stayDown && m_jumpStatu == 1)
    {
        Vector3f deltaVeloc(0, 1, 0);

        if(m_jumpStatu == 1) // Double saut
            m_jumpStatu = 0;
        else
            m_jumpStatu = 1;

        deltaVeloc *= m_worldSettings.playerJumpForce;

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
    WeaponArray::iterator select = find_if(m_weaponsInventory.begin(), m_weaponsInventory.end(),
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
        return(*m_curWeapon);
}

inline bool isPowerSameName(Power* w1, Power* w2)
{
    return w1->getName() == w2->getName();
}

void Player::addPower(Power* power)
{
    PowerArray::iterator select = find_if(m_powersInventory.begin(), m_powersInventory.end(),
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

    m_gameManager->manager.material->setGhost(this, false);

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

    m_gameManager->manager.material->setGhost(this, true);

    clocks.toRespawn.snapShoot();

    m_visualBody->setVisible(false);

    m_physicBody->setApplyGravity(false);

    if(m_gameManager->getUserPlayer() == killer)
    {
        m_gameManager->earthQuake(0.2, false);
    }

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

    if(!onDammage.empty() && !onDammage(this, killer, dammage))
        return;

    m_life = max(m_life - dammage, 0);

    if(m_life <= 0)
        kill(killer);

    if(m_gameManager->getUserPlayer() == this)
    {
        m_gameManager->earthQuake(0.1, false);
        m_gameManager->dammageScreen();
    }

    m_soundManager->playSound("hit", this);
}

GameManager* Player::getGameManager() const
{
    return m_gameManager;
}

void Player::setVisualBody(tbe::scene::Mesh* visualBody)
{
    this->m_visualBody = visualBody;
    MapElement::m_visualBody = visualBody;
}

tbe::scene::Mesh* Player::getVisualBody() const
{
    return m_visualBody;
}

void Player::setImmunity(bool enable)
{
    makeTransparent(enable, 0.5);
    m_gameManager->manager.material->setImmunity(this, enable);

    m_immunity = enable;
}

void Player::makeTransparent(bool enable, float alpha)
{
    if(m_immunity)
        return;

    HardwareBuffer* hardbuf = m_visualBody->getHardwareBuffer();

    Vertex* vs = hardbuf->bindBuffer(true).lock();

    if(enable)
        for(unsigned i = 0; i < hardbuf->getVertexCount(); i++)
            vs[i].color.w = alpha;
    else
        for(unsigned i = 0; i < hardbuf->getVertexCount(); i++)
            vs[i].color.w = 1;

    hardbuf->unlock().bindBuffer(false);

    Material::Array mats = m_visualBody->getAllMaterial();

    if(enable)
        for(unsigned i = 0; i < mats.size(); i++)
            mats[i]->enable(Material::BLEND_MOD | Material::VERTEX_SORT_CULL_TRICK);
    else
        for(unsigned i = 0; i < mats.size(); i++)
            mats[i]->disable(Material::BLEND_MOD | Material::VERTEX_SORT_CULL_TRICK);
}
