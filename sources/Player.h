#ifndef _PLAYERENGINE_H
#define _PLAYERENGINE_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>

#include "MapElement.h"
#include "Weapon.h"
#include "Item.h"
#include "Settings.h"
#include "Controller.h"
#include "BulletTime.h"

class GameManager;

class SoundManager;
class MaterialManager;

class Player : public MapElement
{
public:

    /// Constructeur
    Player(GameManager* playManager, std::string name, std::string model);

    /// Destructeur
    ~Player();

    /**
     * Ajoute une arme au joueur
     * si l'arme existe déja on ajouter les munitions puis on la supprime
     */
    void addWeapon(Weapon* weapon);

    /// Tire sur le vecteur targetpos
    bool shoot(tbe::Vector3f targetpos);

    /**
     * Réunitilase les attribut de santé
     * l'état physique, les armes et la position
     */
    void reBorn();

    /// Saut
    void jump();

    /// Accéleration
    void boost();

    /// Freinage
    void brake();

    /// Assigne les dommages et augemente le score
    void takeDammage(Bullet* ammo);

    /**
     * Routine de traitement pour
     * le boost et les items
     */
    void process();

    /// Mort :)
    void kill();
    bool isKilled() const;

    void makeTransparent(bool enable, float alpha = 0.25);

    /// Attache un élément au joueur
    void attachItem(Item* item);

    /// Attache une source de controlle au joueurs
    void attachController(Controller* controller);
    Controller* getAttachedCotroller() const;

    /// Assigne une position alétoire au sol
    void toNextSpawnPos();

    /// Change l'arme courante
    void slotWeapon(unsigned slot);
    void switchUpWeapon();
    void switchDownWeapon();

    /// Armes
    void setCurWeapon(unsigned slot);
    Weapon* getCurWeapon() const;

    /// Boost
    void setBoostAvalaible(bool boost);
    bool isBoostAvalaible() const;

    /// Score
    void upScore(int value = 1);
    void setScore(int value);
    int getScore() const;

    void setName(std::string name);
    std::string getName() const;

    /// Santé
    void upLife(int life);
    void setLife(int life);
    int getLife() const;

    GameManager* getGameManager() const;

    void outputConstruction(std::iostream& stream)
    {
    }

    struct
    {
        tbe::ticks::Clock boostDisableBlur;
        tbe::ticks::Clock boostAvailable;
        tbe::ticks::Clock boostBrake;
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

        virtual void afterReborn(Player* player)
        {
        }

        virtual bool shutdown(Player* player)
        {
            return false;
        }

        virtual bool onTakeDammage(Player* player, Bullet* ammo)
        {
            return true;
        }

        virtual bool onShoot(Player* player)
        {
            return true;
        }

        virtual bool onKilled(Player* player)
        {
            return true;
        }

        virtual bool onTakeItems(Player* player, Item* item)
        {
            return true;
        }

        virtual bool isInvisible(Player* player)
        {
            return false;
        }

        typedef std::vector<CheckMe*> Array;
    };


    void addCheckMe(CheckMe* cm);

    void setVisibleFromIA(bool visibleFromIA);
    bool isVisibleFromIA() const;

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

    GameManager* m_playManager;
    SoundManager* m_soundManager;

    tbe::scene::OBJMesh* m_visualBody;

    std::string m_name;

    class StartProtection : public CheckMe
    {
    public:
        StartProtection(Player* player);
        bool onTakeDammage(Player* player, Bullet* ammo);
        bool shutdown(Player* player);
        bool onShoot(Player* player);

    private:
        tbe::ticks::Clock m_clock;
    };
};

#endif /* _PLAYERENGINE_H */
