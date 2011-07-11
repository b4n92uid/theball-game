#ifndef _GAMEMANAGER_H
#define _GAMEMANAGER_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>
#include <SDLDevice/SDLDevice.h>

#include <fmod.h>

#include "AppManager.h"

#include "Player.h"
#include "MapElement.h"
#include "AIControl.h"

#include "ScriptManager.h"

#include <boost/signals.hpp>

class UserControl;
class MaterialManager;

class BulletTime;

class Player;
class SoundManager;

class GameManager
{
public:

    GameManager(AppManager* appManager);
    virtual ~GameManager();

    /// Construit la map et les entit�s
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
    void display(std::string msg);

    /// Rotine d'affichage de l'�tat de la partie
    void status(std::string msg);

    tbe::Vector3f getRandomPosOnTheFloor(tbe::Vector3f pos, float radius);

    tbe::Vector3f getRandomPosOnTheFloor();

    void setGameOver(Player* winner, std::string finalmsg);
    bool isGameOver() const;
    bool isRunning() const;

    void dammageEffect();
    void flashEffect();

    virtual tbe::Vector3f getShootTarget() const;

    virtual Player* getUserPlayer() const;
    Player* getWinnerPlayer() const;

    virtual const Player::Array& getPlayers() const;

    virtual const Player::Array getTargetsOf(Player* player) const;

    // Enregistrement des entit�s ----------------------------------------------

    void registerPlayer(Player* player);
    void registerElement(MapElement* staticObject);

    void unregisterPlayer(Player* player);
    void unregisterElement(MapElement* staticObject);

    struct
    {
        std::string name;

        MapElement::Array mapElements;

        tbe::AABB aabb;

    } map;

    struct
    {
        tbe::SDLDevice* gameEngine;
        tbe::scene::SceneManager* scene;
        tbe::gui::GuiManager* gui;
        tbe::ppe::PostProcessManager* ppe;
        tbe::ticks::FpsManager* fps;

        tbe::scene::SceneParser* parser;

        MaterialManager* material;
        SoundManager* sound;

        AppManager* app;

        FMOD_SYSTEM* fmodsys;

        ScriptManager* script;

    } manager;

    struct
    {
        tbe::scene::MeshParallelScene* meshs;
        tbe::scene::ParticlesParallelScene* particles;
        tbe::scene::LightParallelScene* light;
        tbe::scene::NewtonParallelScene* newton;
        tbe::scene::MapMarkParallelScene* marks;

    } parallelscene;

    Settings::World worldSettings;

    boost::signal<void(Player*) > onEachFrame;
    boost::signal<bool(Player*) > onOutOfArena;

protected:

    void processDevelopperCodeEvent();

    enum TimtTo
    {
        TIME_TO_PLAY,
        TIME_TO_PAUSE,
        TIME_TO_GAMEOVER,
    };

    Player* m_userPlayer;
    Player* m_winnerPlayer;

    Player::Array m_players;

    Settings::PartySetting m_playSetting;

    tbe::ticks::Clock m_logClock;
    tbe::ticks::Clock m_newtonClock;
    tbe::ticks::Clock m_validGameOver;
    tbe::ticks::Clock m_spawnPlayer;

    tbe::scene::Camera* m_camera;

    tbe::Vector3f m_shootTarget;
    bool m_cursorOnPlayer;

    std::list<tbe::Vector3f> m_playerPosRec;

    TimtTo m_timeTo;

    bool m_gameOver;
    bool m_running;

    std::map<unsigned, unsigned> m_numslot;

    struct
    {
        tbe::ppe::BloomEffect* bloom;
        tbe::ppe::BlurEffect* gameover;
        tbe::ppe::ColorEffect* dammage;

    } ppe;

    struct
    {

        struct
        {
            tbe::gui::Button* ret;
            tbe::gui::Button* quit;

        } playmenu;

        tbe::gui::TextBox* state;
        tbe::gui::TextBox* gameover;
        tbe::gui::TextBox* log;

        tbe::gui::Gauge* ammoGauge;
        tbe::gui::Gauge* energyGauge;

        tbe::gui::TextBox* life;
        tbe::gui::TextBox* ammo;

        tbe::gui::StateShow* powerIcon;
        tbe::gui::StateShow* weaponIcon;

        struct
        {
            tbe::gui::Image* dammage;
            tbe::gui::Image* flash;
            tbe::gui::Image* gameover;

        } background;

    } hud;
};

#endif // _GAMEMANAGER_H
