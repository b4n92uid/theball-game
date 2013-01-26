/*
 * File:   GameManager.cpp
 * Author: b4n92uid
 *
 * Created on 15 juillet 2009, 22:23
 */

#ifndef _GAMEMANAGER_H
#define _GAMEMANAGER_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>
#include <SDLDevice/SDLDevice.h>

#include <fmod.h>

#include "AppManager.h"

#include "AIControl.h"
#include "Player.h"
#include "MapElement.h"
#include "StaticElement.h"

#include "ScriptManager.h"

#include "Define.h"

#include <boost/signals.hpp>
#include <boost/signals2.hpp>

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

    /// Construit la map et les entités
    void setupMap(const Content::PartySetting& playSetting);

    /// Construction de l'ETH
    void setupGui();

    /// Appler avant le lancement du jeu
    void startGameProcess();

    /// Traitement de l'entrer
    void eventProcess();

    /// Traitement ETH
    void hudProcess();

    /// Traitement Jeu
    void gameProcess();

    /// Rendue
    void render();

    /// Rotine d'affichage de notification
    void display(std::string msg, unsigned duration = 3000);

    /// Rotine d'affichage de l'état de la partie
    void status(std::string msg);

    MapElement* getInterface(tbe::scene::Node* node);

    tbe::Vector3f getRandomPosOnTheFloor(tbe::Vector3f pos, float radius);

    tbe::Vector3f getRandomPosOnTheFloor();

    void setGameOver(Player* winner, std::string finalmsg);
    bool isGameOver() const;
    bool isRunning() const;

    void backImpulse(float intensity, float push);
    void earthQuake(float intensity, bool physical);
    void whiteFlash(float initOpacity, float downOpacity);
    void dammageScreen();
    void glowEnable(bool stat);
    void glowSettings(float intensity, float thershold, float blurpass);

    virtual tbe::Vector3f getShootTarget() const;
    virtual tbe::Vector3f getViewDirection() const;

    virtual Player* getUserPlayer() const;
    Player* getWinnerPlayer() const;

    virtual const Player::Array& getPlayers() const;

    virtual const Player::Array getTargetsOf(Player* player) const;

    // Enregistrement des entités ----------------------------------------------

    void registerPlayer(Player* player);
    void registerArea(AreaElement* dummyObject);
    void registerElement(MapElement* staticObject);
    void registerElement(StaticElement* staticObject);

    void unregisterPlayer(Player* player, bool toDelete = false);
    void unregisterArea(AreaElement* dummyObject);
    void unregisterElement(MapElement* staticObject);
    void unregisterElement(StaticElement* staticObject);

    void onPauseMenuShow();
    void onPauseMenuQuit();
    void onPauseMenuReturn();

    struct
    {
        std::string name;

        StaticElement::Array staticElements;
        AreaElement::Array areaElements;
        MapElement::Array mapElements;

        tbe::AABB aabb;

        Content::PartySetting settings;

    } map;

    struct
    {
        AppManager* app;
        MaterialManager* material;
        SoundManager* sound;
        ScriptManager* script;

        tbe::SDLDevice* gameEngine;
        tbe::scene::SceneManager* scene;
        tbe::gui::RocketGuiManager* gui;
        tbe::ppe::PostProcessManager* ppe;
        tbe::ticks::FpsManager* fps;

        FMOD_SYSTEM* fmodsys;

    } manager;

    struct
    {
        tbe::scene::MeshParallelScene* meshs;
        tbe::scene::ParticlesParallelScene* particles;
        tbe::scene::LightParallelScene* light;
        tbe::scene::NewtonParallelScene* newton;
        tbe::scene::MapMarkParallelScene* marks;

    } parallelscene;

    Settings::World& worldSettings;

    boost::signals2::signal<void(Player*) > onEachFrame;
    boost::signals2::signal<void(Player*) > onPlayerInit;

    boost::signal<void(Player*) > onStartGame;
    boost::signal<bool(Player*), alltrue> onOutOfArena;

protected:

    void processDevelopperCodeEvent();


    bool m_cursorOnPlayer;
    bool m_gameOver;
    bool m_gameRunning;
    bool m_pauseRunning;

    unsigned m_logClockOff;

    Player* m_userPlayer;
    Player* m_winnerPlayer;

    Player::Array m_players;

    MapElement::Array m_shuduledDel;

    tbe::scene::Camera* m_camera;

    tbe::ticks::Clock m_logClock;
    tbe::ticks::Clock m_newtonClock;
    tbe::ticks::Clock m_validGameOver;
    tbe::ticks::Clock m_spawnPlayer;

    tbe::Vector3f m_shootTarget;

    std::list<tbe::Vector3f> m_playerPosRec;

    std::map<unsigned, unsigned> m_weaponSlot;
    std::map<unsigned, unsigned> m_powerSlot;

    struct
    {
        float intensity;
        bool physical;

    } m_earthquake;

    struct
    {
        float initOpacity;
        float downOpacity;

    } m_flasheffect;

    struct
    {
        float intensity;
        float push;

    } m_backImpulse;

    struct
    {
        float initOpacity;
        float downOpacity;

    } m_dammageEffect;

    struct
    {
        Rocket::Core::Element* objective;
        Rocket::Core::Element* message;

        //        tbe::gui::Gauge* ammoGauge;
        //        tbe::gui::Gauge* energyGauge;

        Rocket::Core::Element* life;
        Rocket::Core::Element* ammo;

        Rocket::Core::Element* powerIcon;
        Rocket::Core::Element* weaponIcon;

        Rocket::Core::Element* croshair;

        struct
        {
            tbe::ppe::BloomEffect* bloom;
            tbe::ppe::ColorEffect* dammage;
            tbe::ppe::ColorEffect* flash;
            tbe::ppe::BlurEffect* gameover;

        } background;

        Rocket::Core::ElementDocument* hud;
        Rocket::Core::ElementDocument* pause;
        Rocket::Core::ElementDocument* gameover;

    } m_gui;
};

#endif // _GAMEMANAGER_H
