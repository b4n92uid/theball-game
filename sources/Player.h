#ifndef _PLAYERENGINE_H
#define _PLAYERENGINE_H

#include <Tbe.h>
#include <Newton/Newton.h>

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
     * si l'arme existe d�ja on ajouter les munitions puis on la supprime
     */
    void AddWeapon(Weapon* weapon);

    /// Tire sur le vecteur targetpos
    void Shoot(tbe::Vector3f targetpos);

    /**
     * R�unitilase les attribut de sant�
     * l'�tat physique, les armes et la position
     */
    void ReBorn();

    /// Saut
    void Jump();

    /// Acc�leration
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

    /// Attache un �l�ment au joueur
    void AttachItem(Item* item);

    /// Attache une source de controlle au joueurs
    void AttachController(Controller* controller);
    Controller* GetAttachedCotroller() const;

    /// Assigne une position al�toire au sol
    void SetRandomPosInTheFloor();

    /// Change l'arme courante
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
    void SetScore(int frag);
    int GetScore() const;

    /// Sant�
    void UpLife(int life);
    void SetLife(int life);
    int GetLife() const;

    PlayManager* GetPlayManager() const;

    void OutputConstruction(std::ofstream& file)
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

        typedef std::vector<CheckMe*> Array;
    };


    void AddCheckPoint(CheckMe* cm);

    typedef std::map<std::string, Player*> Map;
    typedef std::vector<Player*> Array;

protected:
    int m_life;
    int m_score;
    bool m_killed;
    bool m_boostAvalaible;

    CheckMe::Array m_checkMe;

    Weapon* m_curWeapon;
    Weapon::Array m_weaponsPack;

    Controller* m_attachedCotroller;

    tbe::scene::ParticlesEmiter* m_deadExplode;

    PlayManager* m_playManager;
    SoundManager* m_soundManager;
};

#endif /* _PLAYERENGINE_H */
