#include "Player.h"

#include "AppManager.h"
#include "GameManager.h"
#include "MaterialManager.h"
#include "MapElement.h"
#include "SoundManager.h"

#include "Define.h"
#include "BulletTime.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// PlayerEngine ----------------------------------------------------------------

Player::Player(GameManager* playManager, std::string name, std::string model) : MapElement(playManager)
{
    // Attributes
    m_name = name;
    m_id = "player";
    m_playManager = playManager;
    m_curWeapon = m_weaponsPack.end();
    m_killed = false;
    m_boostAvalaible = true;
    m_visibleFromIA = true;
    m_score = 0;
    m_life = 100;
    m_attachedCotroller = NULL;
    m_soundManager = playManager->manager.sound;

    // Rendue
    m_visualBody = new OBJMesh(m_gameManager->parallelscene.meshs);
    m_visualBody->open(model);

    MapElement::m_visualBody = m_visualBody;

    // Effet explosion
    m_deadExplode = new BurningEmitter(playManager->parallelscene.particles);
    m_deadExplode->setTexture(PARTICLE_EXPLODE);
    m_deadExplode->setLifeInit(m_worldSettings.playerExplodeLifeInit);
    m_deadExplode->setLifeDown(m_worldSettings.playerExplodeLifeDown);
    m_deadExplode->setFreeMove(m_worldSettings.playerExplodeFreeMove);
    m_deadExplode->setNumber(m_worldSettings.playerExplodeNumber);
    m_deadExplode->setAutoRebuild(false);
    m_deadExplode->setParent(m_visualBody);

    m_checkMe.push_back(new StartProtection(this));

    // Physique
    m_physicBody = new tbe::scene::NewtonNode(m_gameManager->parallelscene.newton);
    m_physicBody->setUpdatedMatrix(&m_visualBody->getMatrix());
    m_physicBody->buildSphereNode(m_worldSettings.playerSize, m_worldSettings.playerMasse);

    m_visualBody->addChild(m_physicBody);

    NewtonBodySetForceAndTorqueCallback(m_physicBody->getBody(), MapElement::applyForceAndTorqueCallback);
    NewtonBodySetLinearDamping(m_physicBody->getBody(), m_worldSettings.playerLinearDamping);
    NewtonBodySetAutoSleep(m_physicBody->getBody(), false);
    NewtonBodySetUserData(m_physicBody->getBody(), this);

    toNextSpawnPos();

    // Arme principale
    WeaponBlaster* blaster = new WeaponBlaster(m_playManager);

    addWeapon(blaster);
}

Player::~Player()
{
    for(unsigned i = 0; i < m_checkMe.size(); i++)
        delete m_checkMe[i];

    delete m_attachedCotroller;
}

void Player::toNextSpawnPos()
{
    Vector3f::Array& spawns = m_playManager->map.spawnPoints;

    Vector3f newpos = 0;

    if(spawns.empty())
    {
        newpos = m_playManager->getRandomPosOnTheFloor();
    }

    else
    {
        newpos = spawns.back();
        spawns.pop_back();
        spawns.insert(spawns.begin(), newpos);
    }

    m_physicBody->setVelocity(0);
    m_physicBody->setMatrix(newpos);
}

void Player::attachItem(Item* item)
{
    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(!m_checkMe[i]->onTakeItems(this, item))
            return;

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

    if(!m_boostAvalaible)
    {
        if(clocks.boostDisableBlur.isEsplanedTime(1000))
            m_playManager->hudBoost(false);

        if(clocks.boostAvailable.isEsplanedTime(m_worldSettings.playerBoostReload))
            m_boostAvalaible = true;
    }

    if(m_attachedCotroller && !m_killed)
        m_attachedCotroller->process(this);

    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(m_checkMe[i]->shutdown(this))
            delete m_checkMe[i], m_checkMe[i] = NULL;

    CheckMe::Array::iterator newEnd = remove(m_checkMe.begin(), m_checkMe.end(), (CheckMe*)NULL);
    m_checkMe.erase(newEnd, m_checkMe.end());
}

bool Player::shoot(Vector3f targetpos)
{
    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(!m_checkMe[i]->onShoot(this))
            return false;

    return (*m_curWeapon)->shoot(m_visualBody->getPos(), targetpos);
}

void Player::jump()
{
    NewtonBodyAddImpulse(m_physicBody->getBody(),
                         Vector3f(0, m_worldSettings.playerJumpForce, 0),
                         m_visualBody->getMatrix().getPos());
}

void Player::brake()
{
    if(!clocks.boostBrake.isEsplanedTime(500))
        return;

    m_physicBody->setVelocity(0);
    m_soundManager->play("stop", this);
}

void Player::boost()
{
    if(!m_boostAvalaible || !m_physicBody->getApplyForce())
        return;

    m_boostAvalaible = false;

    Vector3f impulseDeri = m_physicBody->getApplyForce().normalize()
            * m_worldSettings.playerBoostSpeed;

    impulseDeri.y = 0;

    NewtonBodyAddImpulse(m_physicBody->getBody(), impulseDeri, m_visualBody->getPos());

    m_soundManager->play("boost", this);

    clocks.boostAvailable.snapShoot();

    clocks.boostDisableBlur.snapShoot();
    m_playManager->hudBoost(true);
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

        m_playManager->manager.scene->getRootNode()->addChild(weapon);
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

void Player::reBorn()
{
    m_life = 100;
    m_killed = false;

    m_boostAvalaible = true;

    m_playManager->manager.material->setGhost(this, false);

    m_visualBody->setVisible(true);

    m_physicBody->setFreeze(false);

    for(unsigned i = 0; i < m_weaponsPack.size(); i++)
        delete m_weaponsPack[i];

    m_weaponsPack.clear();

    WeaponBlaster* blaster = new WeaponBlaster(m_playManager);

    addWeapon(blaster);

    toNextSpawnPos();

    for(unsigned i = 0; i < m_checkMe.size(); i++)
        m_checkMe[i]->afterReborn(this);

    m_checkMe.push_back(new StartProtection(this));
}

void Player::kill()
{
    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(!m_checkMe[i]->onKilled(this))
            return;

    m_killed = true;
    m_life = 0;

    m_deadExplode->build();

    m_playManager->manager.material->setGhost(this, true);

    clocks.readyToDelete.snapShoot();

    m_visualBody->setVisible(false);

    m_physicBody->setOmega(0);
    m_physicBody->setFreeze(true);

    m_soundManager->play("kill", this);

    if(this == m_playManager->getUserPlayer())
    {
        m_playManager->getBullettime()->setActive(false);

        m_playManager->hudBoost(false);
        m_playManager->hudBullettime(false);
    }
}

bool Player::isKilled() const
{
    return m_killed;
}

void Player::setBoostAvalaible(bool boost)
{
    this->m_boostAvalaible = boost;
}

bool Player::isBoostAvalaible() const
{
    return m_boostAvalaible;
}

void Player::upScore(int value)
{
    m_score = max(m_score + m_playManager->modulatScore(value), 0);
}

void Player::setScore(int value)
{
    this->m_score = max(m_playManager->modulatScore(value), 0);
}

int Player::getScore() const
{
    return m_score;
}

void Player::setName(std::string name)
{
    this->m_name = name;
}

std::string Player::getName() const
{
    return m_name;
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

void Player::addCheckMe(CheckMe* cm)
{
    m_checkMe.push_back(cm);
}

void Player::setVisibleFromIA(bool visibleFromIA)
{
    this->m_visibleFromIA = visibleFromIA;
}

bool Player::isVisibleFromIA() const
{
    return m_visibleFromIA;
}

void Player::takeDammage(Bullet* ammo)
{
    if(m_killed)
        return;

    for(unsigned i = 0; i < m_checkMe.size(); i++)
        if(!m_checkMe[i]->onTakeDammage(this, ammo))
            return;

    int dammage = ammo->getDammage();

    int scoremod = dammage > m_life ? m_life : dammage;

    m_life = max(m_life - dammage, 0);

    Weapon* wp = ammo->getWeapon();

    if(wp)
    {
        Player* striker = wp->getShooter();

        if(m_life <= 0)
        {
            striker->upScore(500);
            upScore(-500);

            kill();
        }
        else
        {
            striker->upScore(scoremod);
            upScore(-scoremod);
        }
    }

    if(m_playManager->getUserPlayer() == this)
        m_playManager->hudDammage(true);

    m_soundManager->play("hit", this);
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

Player::StartProtection::StartProtection(Player* player)
{
    using namespace tbe;
    using namespace tbe::scene;

    player->makeTransparent(true);

    player->setVisibleFromIA(false);
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

        player->setVisibleFromIA(true);

        return true;
    }

    return false;
}