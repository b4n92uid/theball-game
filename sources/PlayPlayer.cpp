/* 
 * File:   PlayPlayer.cpp
 * Author: b4n92uid
 * 
 * Created on 3 juin 2010, 18:25
 */

#include "PlayPlayer.h"
#include "Item.h"

#include "PlayModeManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

PlayPlayer::PlayPlayer(PlayModeManager* playManager, std::string name, std::string model) : Player(playManager, name, model)
{
    m_curWeapon = m_weaponsPack.end();
    m_killed = false;
    m_bullettimeActive = false;
    m_bullettime = 100;
    m_boostAvalaible = true;
    m_score = 0;
    m_life = 100;

    // Effet explosion
    m_deadExplode = new ParticlesEmiter;
    m_deadExplode->SetTexture(PARTICLE_EXPLODE);
    m_deadExplode->SetLifeInit(2);
    m_deadExplode->SetLifeDown(0.075);
    m_deadExplode->SetFreeMove(0.2);
    m_deadExplode->SetNumber(500);
    m_deadExplode->SetAutoRebuild(false);

    m_playManager->GetParticlesScene()->AddParticlesEmiter("", m_deadExplode);


    // Arme principale
    WeaponBlaster* blaster = new WeaponBlaster(m_playManager);
    AddWeapon("blaster", blaster);
}

PlayPlayer::~PlayPlayer()
{
    m_playManager->GetParticlesScene()->DeleteParticlesEmiter(m_deadExplode);

    for(Weapon::Map::iterator i = m_weaponsPack.begin(); i != m_weaponsPack.end(); i++)
        delete i->second;
}

void PlayPlayer::SetKilled()
{
    m_killed = true;
    m_life = 0;

    m_deadExplode->SetEnable(true);
    m_deadExplode->SetDeadEmiter(false);
    m_deadExplode->SetPos(m_matrix.GetPos());
    m_deadExplode->Build();

    m_playManager->GetMaterialManager()->SetGhost(this, true);

    clocks.readyToDelete.SnapShoot();

    SetFreeze(true);
    SetEnable(false);

    m_soundManager->Play("kill", this);
}

bool PlayPlayer::IsKilled() const
{

    return m_killed;
}

void PlayPlayer::ReBorn()
{
    m_life = 100;
    m_killed = false;

    m_bullettime = 100;
    m_bullettimeActive = false;

    m_boostAvalaible = true;

    m_playManager->GetMaterialManager()->SetGhost(this, false);

    SetFreeze(false);
    SetEnable(true);

    for(Weapon::Map::iterator i = m_weaponsPack.begin(); i != m_weaponsPack.end(); i++)
        delete i->second;

    m_weaponsPack.clear();

    WeaponBlaster* blaster = new WeaponBlaster(m_playManager);
    AddWeapon("blaster", blaster);

    SetRandomPosInTheFloor();
}

void PlayPlayer::Process()
{
    // Bullettime --------------------------------------------------------------

    if(m_bullettimeActive)
    {
        if(m_bullettime > 0)
            m_bullettime--;

        else if(m_bullettime == 0)
            SetBullettimeMotion(false);

    }

    else
    {
        if(m_bullettime < 100)
            m_bullettime++;
    }

    // Boost -------------------------------------------------------------------

    if(!m_boostAvalaible)
    {
        if(clocks.boostAvailable.IsEsplanedTime(3000))
        {
            m_boostAvalaible = true;

            if(m_playManager->GetAppManager()->globalSettings.video.usePpe)
                m_playManager->ppe.boost->SetEnable(false);
        }
    }
}

void PlayPlayer::Shoot(Vector3f targetpos)
{
    m_curWeapon->second->Shoot(m_matrix.GetPos() + (targetpos - m_matrix.GetPos()).Normalize() * 1.5f, targetpos);
}

void PlayPlayer::Jump()
{
    if(!m_body)
        return;

    NewtonBodyAddImpulse(m_body, Vector3f(0, 30, 0), m_matrix.GetPos());
}

void PlayPlayer::Boost()
{
    if(!m_body || m_applyForce == 0.0f)
        return;

    if(m_boostAvalaible)
    {
        m_boostAvalaible = false;

        Vector3f impulseDeri = Vector3f::Normalize(m_applyForce) * PLAYER_BOOSTSPEED;
        impulseDeri.y = 0;

        NewtonBodyAddImpulse(m_body, impulseDeri, m_matrix.GetPos());

        m_soundManager->Play("boost", this);

        clocks.boostAvailableSound.SnapShoot();
        clocks.boostAvailable.SnapShoot();

        if(m_playManager->GetAppManager()->globalSettings.video.usePpe)
        {
            clocks.boostDisableBlur.SnapShoot();
            m_playManager->ppe.boost->SetEnable(true);
        }
    }

    else if(clocks.boostAvailableSound.IsEsplanedTime(800))
    {
        m_playManager->GetSoundManager()->Play("noAvailable", this);
    }
}

void PlayPlayer::AddWeapon(std::string name, Weapon* weapon)
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

void PlayPlayer::SwitchUpWeapon()
{
    m_curWeapon->second->RestoreShootCadency();

    m_curWeapon++;

    if(m_curWeapon == m_weaponsPack.end())
        m_curWeapon = m_weaponsPack.begin();

    m_curWeapon->second->SetBullttimeMotion(m_bullettimeActive);
}

void PlayPlayer::SwitchDownWeapon()
{
    m_curWeapon->second->RestoreShootCadency();

    if(m_curWeapon == m_weaponsPack.begin())
        m_curWeapon = m_weaponsPack.end();

    m_curWeapon--;

    m_curWeapon->second->SetBullttimeMotion(m_bullettimeActive);
}

void PlayPlayer::SetCurWeapon(std::string weaponName)
{
    m_curWeapon = m_weaponsPack.find(weaponName);

    if(m_curWeapon == m_weaponsPack.end())
        throw tbe::Exception("PlayPlayer::SetCurWeapon\nWeapon not found (%s)\n\n", weaponName.c_str());
}

Weapon* PlayPlayer::GetCurWeapon() const
{

    return m_curWeapon->second;
}

void PlayPlayer::SetBoostAvalaible(bool boost)
{

    this->m_boostAvalaible = boost;
}

bool PlayPlayer::IsBoostAvalaible() const
{

    return m_boostAvalaible;
}

void PlayPlayer::UpScore(int value)
{

    m_score += value;
}

void PlayPlayer::SetScore(int frag)
{

    this->m_score = frag;
}

int PlayPlayer::GetScore() const
{

    return m_score;
}

void PlayPlayer::SetBullettime(int bullettime)
{
    if(bullettime > 100)
        bullettime = 100;

    this->m_bullettime = bullettime;
}

int PlayPlayer::GetBullettime() const
{

    return m_bullettime;
}

void PlayPlayer::SetBullettimeMotion(bool bullettimeActive)
{
    m_curWeapon->second->SetBullttimeMotion(bullettimeActive);

    this->m_bullettimeActive = bullettimeActive;
}

bool PlayPlayer::IsBullettimeMotion() const
{

    return m_bullettimeActive;
}

void PlayPlayer::UpLife(int life)
{
    m_life += life;

    m_life = max(min(m_life, 100), 0);
}

void PlayPlayer::SetLife(int life)
{
    this->m_life = life;

    m_life = max(min(m_life, 100), 0);
}

int PlayPlayer::GetLife() const
{
    return m_life;
}

void PlayPlayer::TakeDammage(Ammo* ammo)
{
    Weapon* strikerWeapon = ammo->GetParent();
    PlayPlayer* striker = strikerWeapon->GetShooter();

    // Assignation dommage
    m_life -= strikerWeapon->GetMaxAmmoDammage();

    striker->UpScore();

    // Mise a jour score
    if(m_life <= 0)
    {
        m_life = 0;

        striker->UpScore(10);
    }

    // Son
    m_soundManager->Play("hit", this);
}
