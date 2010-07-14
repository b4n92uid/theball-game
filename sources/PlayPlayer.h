/* 
 * File:   PlayPlayer.h
 * Author: b4n92uid
 *
 * Created on 3 juin 2010, 18:25
 */

#ifndef _PLAYPLAYER_H
#define	_PLAYPLAYER_H

#include <Tbe.h>

#include "Player.h"
#include "Weapon.h"

class PlayModeManager;
class MaterialManager;

class PlayPlayer : public Player
{
public:
    PlayPlayer(PlayModeManager* playManager, std::string name, std::string model);
    ~PlayPlayer();

    /// *
    void ReBorn();

    /// *
    void Process();

    /// Tire sur le vecteur targetpos
    void Shoot(tbe::Vector3f targetpos);

    /// Saut
    void Jump();

    /// Accéleration
    void Boost();

    /// Assignation dommages
    void TakeDammage(Ammo* ammo);

    /// Ajoute d'une arme
    void AddWeapon(std::string name, Weapon* weapon);

    /// Change l'arme courante
    void SwitchUpWeapon();
    void SwitchDownWeapon();

    /// Armes
    void SetCurWeapon(std::string weaponName);
    Weapon* GetCurWeapon() const;

    /// Boost
    void SetBoostAvalaible(bool boost);
    bool IsBoostAvalaible() const;

    /// Score
    void UpScore(int value = 1);
    void SetScore(int frag);
    int GetScore() const;

    /// Santé
    void UpLife(int life);
    void SetLife(int life);
    int GetLife() const;

    /// Bullettime (Valeur)
    void SetBullettime(int bullettime);
    int GetBullettime() const;

    /// Bullettime
    void SetBullettimeMotion(bool bullettimeActive);
    bool IsBullettimeMotion() const;

    /// Mort :)
    void SetKilled();
    bool IsKilled() const;

    PlayModeManager* GetPlayManager() const;

    struct
    {
        tbe::ticks::Clock boostDisableBlur;
        tbe::ticks::Clock boostAvailable;
        tbe::ticks::Clock boostAvailableSound;
        tbe::ticks::Clock shoot;
        tbe::ticks::Clock readyToDelete;
    } clocks;

    typedef std::map<std::string, PlayPlayer*> Map;

protected:
    int m_life;
    bool m_killed;

    bool m_bullettimeActive;
    int m_bullettime;
    bool m_boostAvalaible;

    int m_score;

    Weapon::Map::iterator m_curWeapon;
    Weapon::Map m_weaponsPack;

    tbe::scene::ParticlesEmiter* m_deadExplode;

    PlayModeManager* m_playManager;
};

#endif	/* _PLAYPLAYER_H */
