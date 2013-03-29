/*
 * File:   Sniper.cpp
 * Author: b4n92uid
 *
 * Created on 18 janvier 2013, 20:21
 */

#include "WeaponSniper.h"

#include "GameManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"

#include "MapElement.h"
#include "Player.h"

#include "Define.h"

#include <fmod_errors.h>

typedef boost::filesystem::path fspath;

using namespace std;
using namespace tbe;
using namespace tbe::scene;

WeaponSniper::WeaponSniper(GameManager* gameManager) :
Weapon(gameManager),
LaserBeam(gameManager->parallelscene.meshs)
{
    m_slot = m_settings.weapons.get<int>("sniper.slot");

    setWeaponName(m_settings.weapons.get<string > ("sniper.name"));
    setMaxAmmoCount(m_settings.weapons.get<int>("sniper.maxAmmoCount"));
    setAmmoCount(m_settings.weapons.get<int>("sniper.ammoCount"));
    setMaxAmmoDammage(m_settings.weapons.get<int>("sniper.maxAmmoDammage"));
    setShootCadency(m_settings.weapons.get<int>("sniper.shootCadency"));
    setFireSound(m_settings.weapons.get<string > ("sniper.fireSound"));

    LaserBeam::setTexture(m_settings.weapons.get<string > ("sniper.texture"));

    gameManager->manager.scene->getRootNode()->addChild(this);

    m_life = 0;
}

WeaponSniper::~WeaponSniper()
{
}

void WeaponSniper::process()
{
    EventManager* eventmng = m_playManager->manager.gameEngine->getEventManager();

    if(m_life > 0)
    {
        m_life -= eventmng->lastPollTimestamp * 0.001f;

        LaserBeam::setOpacity(m_life);

        if(m_life < 0.1)
        {
            m_life = 0;
            LaserBeam::clear();
        }
    }
}

void WeaponSniper::processShoot(tbe::Vector3f startpos, tbe::Vector3f targetpos)
{
    m_life = 1;
    LaserBeam::shoot(startpos, targetpos);

    NewtonParallelScene* newton = m_playManager->parallelscene.newton;

    if(m_shooter == m_playManager->getUserPlayer())
        m_playManager->backImpulse(m_settings.weapons.get<float>("sniper.backIntensity"),
                                   m_settings.weapons.get<float>("sniper.backPush"));

    Vector3f::Array hits = newton->rayCast(startpos, targetpos);

    if(hits.empty())
        return;

    const Player::Array players = m_playManager->getTargetsOf(m_shooter);

    foreach(Player* p, players)
    {
        if(p->getVisualBody()->getAbsolutAabb().isInner(hits.front()))
        {
            p->takeDammage(m_maxAmmoDammage, m_shooter);
            break;
        }
    }
}
