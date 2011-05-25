#ifndef _ITEMENGINE_H
#define _ITEMENGINE_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>

#include "Object.h"

class Player;
class GameManager;
class SoundManager;

class Item : public Object
{
public:

    Item(GameManager* gameManager, tbe::Matrix4 pos);
    ~Item();

    /// Réinitialise la matrice de l'item a la matrice initiale
    void resetPosition();

    /*
     * Vérifier si l'item est pres pour une réinitialisation
     * dans le cas ou il a été utiliser
     */
    bool isReadyToReborn();

    /// Réinitialise les attribute de l'item
    virtual void reborn();

    /// Modifier le joueur
    virtual void modifPlayer(Player* player) = 0;

    /**
     * Fonction virutal pure pour la notiifcation 
     * a un joueur de son besoin a cette item dans le cas
     * d'une inteligence artifiecielle
     */
    virtual bool isNeeded(Player* player) = 0;

    /// Spécifier l'état de la disponibilité de l'item
    void setTaked(bool taked);
    bool isTaked() const;

    /// Spéicifier le temp de réaparition de l'item
    void setRespawnTime(unsigned respawnTime);
    unsigned getRespawnTime() const;

    typedef std::map<std::string, Item*> Map;
    typedef std::vector<Item*> Array;

protected:
    tbe::Matrix4 m_initialMatrix;
    unsigned m_respawnTime;
    Settings::Ai& m_aiParams;

private:
    bool m_taked;
    tbe::ticks::Clock m_rebornClock;
};

// ItemAddAmmo -----------------------------------------------------------------

class ItemAddAmmo : public Item
{
public:
    ItemAddAmmo(GameManager* gameManager, tbe::Matrix4 pos);
    Object* cloneToObject();
    void modifPlayer(Player* player);
    bool isNeeded(Player* player);
    void outputConstruction(std::iostream& stream);
};

// ItemAddLife -----------------------------------------------------------------

class ItemAddLife : public Item
{
public:
    ItemAddLife(GameManager* gameManager, tbe::Matrix4 pos);
    Object* cloneToObject();
    void modifPlayer(Player* player);
    bool isNeeded(Player* player);
    void outputConstruction(std::iostream& stream);
};

// ItemFatalShot ----------------------------------------------------------------

class ItemFatalShot : public Item
{
public:
    ItemFatalShot(GameManager* gameManager, tbe::Matrix4 pos);
    Object* cloneToObject();
    void modifPlayer(Player* player);
    bool isNeeded(Player* player);
    void outputConstruction(std::iostream& stream);
};

// ItemSuperLife ---------------------------------------------------------------

class ItemSuperLife : public Item
{
public:
    ItemSuperLife(GameManager* gameManager, tbe::Matrix4 pos);
    Object* cloneToObject();
    void modifPlayer(Player* player);
    bool isNeeded(Player* player);
    void outputConstruction(std::iostream& stream);
};

// ItemAddFinder ---------------------------------------------------------------

class ItemAddFinder : public Item
{
public:
    ItemAddFinder(GameManager* gameManager, tbe::Matrix4 pos);
    Object* cloneToObject();
    void modifPlayer(Player* player);
    bool isNeeded(Player* player);
    void outputConstruction(std::iostream& stream);
};

// ItemAddBomb -----------------------------------------------------------------

class ItemAddBomb : public Item
{
public:
    ItemAddBomb(GameManager* gameManager, tbe::Matrix4 pos);
    Object* cloneToObject();
    void modifPlayer(Player* player);
    bool isNeeded(Player* player);
    void outputConstruction(std::iostream& stream);
};

// ItemAddShotgun --------------------------------------------------------------

class ItemAddShotgun : public Item
{
public:
    ItemAddShotgun(GameManager* gameManager, tbe::Matrix4 pos);
    Object* cloneToObject();
    void modifPlayer(Player* player);
    bool isNeeded(Player* player);
    void outputConstruction(std::iostream& stream);
};

#endif /* _ITEMENGINE_H */
