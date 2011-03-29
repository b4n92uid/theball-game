#ifndef _PLAYERENGINE_H
#define _PLAYERENGINE_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>

#include "Object.h"
#include "Weapon.h"
#include "Item.h"
#include "Settings.h"
#include "Controller.h"
#include "BulletTime.h"

class PlayManager;

class SoundManager;
class MaterialManager;

class Player : public Object
{
public:

    /// Constructeur
    Player(PlayManager* playManager, std::string name, std::string model);

    /// Destructeur
    ~Player();

    Object* CloneToObject();

    /** 
     * Ajoute une arme au joueur
     * si l'arme existe déja on ajouter les munitions puis on la supprime
     */
    void AddWeapon(Weapon* weapon);

    /// Tire sur le vecteur targetpos
    bool Shoot(tbe::Vector3f targetpos);

    /**
     * Réunitilase les attribut de santé
     * l'état physique, les armes et la position
     */
    void ReBorn();

    /// Saut
    void Jump();

    /// Accéleration
    void Boost();

    /// Assigne les dommages et augemente le score
    void TakeDammage(Bullet* ammo);

    /** 
     * Routine de traitement pour
     * le boost et les items
     */
    void Process();

    /// Mort :)
    void Kill();
    bool IsKilled() const;

    /// Attache un élément au joueur
    void AttachItem(Item* item);

    /// Attache une source de controlle au joueurs
    void AttachController(Controller* controller);
    Controller* GetAttachedCotroller() const;

    /// Assigne une position alétoire au sol
    void SetRandomSpawnPos();

    /// Change l'arme courante
    void SlotWeapon(unsigned slot);
    void SwitchUpWeapon();
    void SwitchDownWeapon();

    /// Armes
    void SetCurWeapon(unsigned slot);
    Weapon* GetCurWeapon() const;

    /// Boost
    void SetBoostAvalaible(bool boost);
    bool IsBoostAvalaible() const;

    /// Score
    void UpScore(int value = 1);
    void SetScore(int value);
    int GetScore() const;

    /// Santé
    void UpLife(int life);
    void SetLife(int life);
    int GetLife() const;

    PlayManager* GetPlayManager() const;

    void OutputConstruction(std::iostream& stream)
    {
    }

    struct
    {
        tbe::ticks::Clock boostDisableBlur;
        tbe::ticks::Clock boostAvailable;
        tbe::ticks::Clock boostAvailableSound;
        tbe::ticks::Clock shoot;
        tbe::ticks::Clock readyToDelete;

    } clocks;

    class CheckMe
    {
    public:

        CheckMe()
        {
        }

        virtual ~CheckMe()
        {
        }

        virtual bool Shutdown(Player* player)
        {
            return false;
        }

        virtual bool OnTakeDammage(Player* player, Bullet* ammo)
        {
            return true;
        }

        virtual bool OnShoot(Player* player)
        {
            return true;
        }

        virtual bool OnKilled(Player* player)
        {
            return true;
        }

        virtual void AfterReborn(Player* player)
        {
        }

        virtual bool OnTakeItems(Player* player, Item* item)
        {
            return true;
        }

        virtual bool IsInvisible(Player* player)
        {
            return false;
        }

        typedef std::vector<CheckMe*> Array;
    };


    void AddCheckPoint(CheckMe* cm);
    void SetVisibleFromIA(bool visibleFromIA);
    bool IsVisibleFromIA() const;

    typedef std::map<std::string, Player*> Map;
    typedef std::vector<Player*> Array;

protected:
    int m_life;
    int m_score;
    bool m_killed;
    bool m_boostAvalaible;
    bool m_visibleFromIA;

    CheckMe::Array m_checkMe;

    Weapon::Array::iterator m_curWeapon;
    Weapon::Array m_weaponsPack;

    Controller* m_attachedCotroller;

    tbe::scene::BurningEmitter* m_deadExplode;

    PlayManager* m_playManager;
    SoundManager* m_soundManager;

    class StartProtection : public CheckMe
    {
    public:

        StartProtection(Player* player);
        bool OnTakeDammage(Player* player, Bullet* ammo);
        bool Shutdown(Player* player);
        bool OnShoot(Player* player);

    private:
        tbe::ticks::Clock m_clock;
    };
};

#endif /* _PLAYERENGINE_H */
