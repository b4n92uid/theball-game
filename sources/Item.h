#ifndef _ITEMENGINE_H
#define _ITEMENGINE_H

#include <Tbe.h>
#include <Newton/Newton.h>

#include "Object.h"

class Player;
class GameManager;
class SoundManager;

class Item : public Object
{
public:

    Item(GameManager* gameManager, tbe::Matrix4f pos);
    ~Item();

    /// Réinitialise la matrice de l'item a la matrice initiale
    void ResetPosition();

    /*
     * Vérifier si l'item est pres pour une réinitialisation
     * dans le cas ou il a été utiliser
     */
    bool IsReadyToReborn();

    /// Réinitialise les attribute de l'item
    virtual void Reborn();

    /// Modifier le joueur
    virtual void ModifPlayer(Player* player) = 0;

    /**
     * Fonction virutal pure pour la notiifcation 
     * a un joueur de son besoin a cette item dans le cas
     * d'une inteligence artifiecielle
     */
    virtual bool IsNeeded(Player* player) = 0;

    /// Spécifier l'état de la disponibilité de l'item
    void SetTaked(bool taked);
    bool IsTaked() const;

    /// Spéicifier le temp de réaparition de l'item
    void SetRebornTime(unsigned rebornTime);
    unsigned GetRebornTime() const;

    typedef std::map<std::string, Item*> Map;
    typedef std::vector<Item*> Array;

protected:
    tbe::Matrix4f m_initialMatrix;
    unsigned m_rebornTime;

private:
    bool m_taked;
    tbe::ticks::Clock m_rebornClock;
};

// ItemAddAmmo -----------------------------------------------------------------

class ItemAddAmmo : public Item
{
public:
    ItemAddAmmo(GameManager* gameManager, tbe::Matrix4f pos);
    Object* Clone();
    void ModifPlayer(Player* player);
    bool IsNeeded(Player* player);
    void OutputConstruction(std::ofstream& file);
};

// ItemAddLife -----------------------------------------------------------------

class ItemAddLife : public Item
{
public:
    ItemAddLife(GameManager* gameManager, tbe::Matrix4f pos);
    Object* Clone();
    void ModifPlayer(Player* player);
    bool IsNeeded(Player* player);
    void OutputConstruction(std::ofstream& file);
};

// ItemFatalShot ----------------------------------------------------------------

class ItemFatalShot : public Item
{
public:
    ItemFatalShot(GameManager* gameManager, tbe::Matrix4f pos);
    Object* Clone();
    void ModifPlayer(Player* player);
    bool IsNeeded(Player* player);
    void OutputConstruction(std::ofstream& file);
};

// ItemSuperLife ---------------------------------------------------------------

class ItemSuperLife : public Item
{
public:
    ItemSuperLife(GameManager* gameManager, tbe::Matrix4f pos);
    Object* Clone();
    void ModifPlayer(Player* player);
    bool IsNeeded(Player* player);
    void OutputConstruction(std::ofstream& file);
};

// ItemAddFinder ---------------------------------------------------------------

class ItemAddFinder : public Item
{
public:
    ItemAddFinder(GameManager* gameManager, tbe::Matrix4f pos);
    Object* Clone();
    void ModifPlayer(Player* player);
    bool IsNeeded(Player* player);
    void OutputConstruction(std::ofstream& file);
};

// ItemAddBomb -----------------------------------------------------------------

class ItemAddBomb : public Item
{
public:
    ItemAddBomb(GameManager* gameManager, tbe::Matrix4f pos);
    Object* Clone();
    void ModifPlayer(Player* player);
    bool IsNeeded(Player* player);
    void OutputConstruction(std::ofstream& file);
};

// ItemAddShotgun --------------------------------------------------------------

class ItemAddShotgun : public Item
{
public:
    ItemAddShotgun(GameManager* gameManager, tbe::Matrix4f pos);
    Object* Clone();
    void ModifPlayer(Player* player);
    bool IsNeeded(Player* player);
    void OutputConstruction(std::ofstream& file);
};

#endif /* _ITEMENGINE_H */
