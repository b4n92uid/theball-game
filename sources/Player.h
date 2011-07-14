#ifndef _PLAYERENGINE_H
#define _PLAYERENGINE_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>

#include "MapElement.h"
#include "Weapon.h"
#include "Settings.h"
#include "Controller.h"
#include "Power.h"

#include "Define.h"

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

    void free();

    /// Tire sur le vecteur targetpos
    bool shoot(tbe::Vector3f targetpos);

    /**
     * R�unitilase les attribut de sant�
     * l'�tat physique, les armes et la position
     */
    void reBorn();

    /// Mouvements
    void move(tbe::Vector3f force);

    /// Saut
    void jump();

    /// Acc�leration
    bool power(bool stat, tbe::Vector3f targetpos = 0);

    /// Assigne les dommages et augemente le score
    void takeDammage(int dammage, Player* killer = NULL);

    /**
     * Routine de traitement
     */
    void process();

    /// Mort :)
    void kill(Player* killer);
    bool isKilled() const;

    void makeTransparent(bool enable, float alpha = 0.25);

    /// Attache une source de controlle au joueurs
    void attachController(Controller* controller);
    Controller* getAttachedCotroller() const;

    /// Assigne une position al�toire au sol
    void randomPosOnFloor();

    /// Assigne la dernier position d'apparition
    void setInLastSpawnPoint();

    /**
     * Ajoute une arme au joueur
     * si l'arme existe d�ja on ajouter les munitions puis on la supprime
     */
    void addWeapon(Weapon* weapon);

    /// Change l'arme courante
    void slotWeapon(unsigned slot);
    void switchUpWeapon();
    void switchDownWeapon();

    /// Armes
    Weapon* getCurWeapon() const;

    /**
     * Ajoute un pouvoir au joueur
     */
    void addPower(Power* power);

    /// Change le pouvoir courante
    void slotPower(unsigned slot);
    void switchUpPower();
    void switchDownPower();

    /// Pouvoires
    void setCurPower(unsigned slot);
    Power* getCurPower() const;

    /// Energie
    void upEnergy(int value = 1);
    void setEnergy(int energy);
    int getEnergy() const;

    void setName(std::string name);
    std::string getName() const;

    /// Sant�
    void upLife(int life);
    void setLife(int life);
    int getLife() const;

    bool isStayDown();

    GameManager* getGameManager() const;

    struct
    {
        tbe::ticks::Clock brake;
        tbe::ticks::Clock shoot;
        tbe::ticks::Clock readyToDelete;

    } clocks;

    boost::signal<void(Player*) > onRespawn;
    boost::signal<bool(Player*, tbe::Vector3f), alltrue> onShoot;
    boost::signal<bool(Player*, bool, tbe::Vector3f), alltrue> onPower;
    boost::signal<bool(Player*, Player*), alltrue> onDammage;
    boost::signal<bool(Player*, Player*), alltrue> onKilled;
    boost::signal<bool(Player*, bool), alltrue> onJump;
    boost::signal<bool(Player*, tbe::Vector3f), alltrue> onMove;

    typedef std::map<std::string, Player*> Map;
    typedef std::vector<Player*> Array;

protected:
    int m_life;
    bool m_killed;
    int m_energy;

    Weapon::Array::iterator m_curWeapon;
    Weapon::Array m_weaponsInventory;

    Power::Array::iterator m_curPower;
    Power::Array m_powersInventory;

    Controller* m_attachedCotroller;

    tbe::scene::ParticlesEmiter* m_deadExplode;

    SoundManager* m_soundManager;

    tbe::scene::OBJMesh* m_visualBody;

    std::string m_nickname;

    tbe::Vector3f m_lastSpawnPoint;

    const Settings& m_settings;

private:
    bool m_energyVoid;
};

#endif /* _PLAYERENGINE_H */
