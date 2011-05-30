#ifndef _GAMEMANAGER_H
#define _GAMEMANAGER_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>
#include <SDLDevice/SDLDevice.h>

#include <fmod.h>

#include "AppManager.h"

#include "Player.h"
#include "Item.h"
#include "MapElement.h"
#include "AIControl.h"

#include "ScriptActions.h"

class UserControl;
class MaterialManager;

class BulletTime;

class Player;
class Item;
class SoundManager;

class GameManager
{
public:

    GameManager(AppManager* appManager);
    virtual ~GameManager();

    /// Construit la map et les entités
    void setupMap(const Settings::PartySetting& playSetting);

    /// Construction de l'ETH
    void setupGui();

    /// Appler avant le lancement du jeu
    void onStartGame();

    /// Traitement de l'entrer
    void eventProcess();

    /// Traitement ETH
    void hudProcess();

    /// Traitement Jeu
    void gameProcess();

    /// Rendue
    void render();

    /// Rotine d'affichage de notification
    void log(std::string msg);

    tbe::Vector3f getRandomPosOnTheFloor();

    void setGameOver();
    bool isGameOver() const;
    bool isRunning() const;

    void hudItem(bool status);
    void hudBullettime(bool status);
    void hudBoost(bool status);
    void hudDammage(bool status);

    virtual BulletTime* getBullettime() const;

    virtual tbe::Vector3f getShootTarget() const;

    virtual Player* getUserPlayer() const;

    virtual const Player::Array& getPlayers() const;

    virtual const Player::Array getTargetsOf(Player* player) const;

    // Enregistrement des entités ----------------------------------------------

    void registerPlayer(Player* player);
    void registerItem(Item* item);
    void registerElement(MapElement* staticObject);

    void unregisterPlayer(Player* player);
    void unregisterItem(Item* item);
    void unregisterElement(MapElement* staticObject);

    struct
    {
        std::string name;

        FMOD_SOUND* musicStream;
        FMOD_CHANNEL* musicChannel;
        std::string musicPath;

        Item::Array items;
        MapElement::Array mapElements;
        tbe::scene::Light::Array lights;

        tbe::AABB aabb;

        tbe::Vector3f::Array spawnPoints;

    } map;

    struct
    {
        tbe::SDLDevice* gameEngine;
        tbe::scene::SceneManager* scene;
        tbe::gui::GuiManager* gui;
        tbe::ppe::PostProcessManager* ppe;
        tbe::ticks::FpsManager* fps;

        MaterialManager* material;
        SoundManager* sound;

        AppManager* app;

        FMOD_SYSTEM* fmodsys;

        ScriptActions* script;

    } manager;

    struct
    {
        tbe::scene::MeshParallelScene* meshs;
        tbe::scene::ParticlesParallelScene* particles;
        tbe::scene::LightParallelScene* light;
        tbe::scene::NewtonParallelScene* newton;

    } parallelscene;

    Settings::World worldSettings;

protected:

    void processDevelopperCodeEvent();

    static bool playerScoreSortProcess(Player* p1, Player* p2)
    {
        if(p1->getScore() == p2->getScore())
            return (p1->getName() > p2->getName());
        else
            return (p1->getScore() > p2->getScore());
    }

    enum TimtTo
    {
        TIME_TO_VIEWSCORE,
        TIME_TO_PLAY,
        TIME_TO_PAUSE,
        TIME_TO_GAMEOVER,
    };

    Player* m_userPlayer;
    Player* m_winnerPlayer;

    Player::Array m_players;

    BulletTime* m_bullettime;

    Settings::PartySetting m_playSetting;

    tbe::ticks::Clock m_logClock;
    tbe::ticks::Clock m_newtonClock;
    tbe::ticks::Clock m_validGameOver;
    tbe::ticks::Clock m_spawnPlayer;

    tbe::scene::Camera* m_camera;

    tbe::Vector3f m_shootTarget;
    bool m_cursorOnPlayer;

    std::list<tbe::Vector3f> m_playerPosRec;

    std::vector<std::string> m_botNames;

    TimtTo m_timeTo;

    bool m_gameOver;
    bool m_running;

    std::map<unsigned, unsigned> m_numslot;

    struct
    {
        tbe::ppe::BloomEffect* bloom;
        tbe::ppe::BlurEffect* gameover;
        tbe::ppe::ColorEffect* bullettime;
        tbe::ppe::ColorEffect* dammage;
        tbe::ppe::MotionBlurEffect* boost;

    } ppe;

    struct
    {

        struct
        {
            tbe::gui::Button* ret;
            tbe::gui::Button* quit;

        } playmenu;

        tbe::gui::TextBox* scorelist;
        tbe::gui::TextBox* state;
        tbe::gui::TextBox* gameover;
        tbe::gui::TextBox* log;

        tbe::gui::Gauge* ammo;
        tbe::gui::Gauge* life;
        tbe::gui::Gauge* bullettime;

        tbe::gui::StateShow* boost;
        tbe::gui::StateShow* item;

        struct
        {
            tbe::gui::Image* background;
            tbe::gui::Image* dammage;
            tbe::gui::Image* bullettime;
            tbe::gui::Image* gameover;

        } background;

    } hud;
};

#endif // _GAMEMANAGER_H
