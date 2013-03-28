/*
 * File:   AppManager.h
 * Author: b4n92uid
 *
 * Created on 15 juillet 2009, 17:45
 */

#ifndef _APPMANAGER_H
#define _APPMANAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <fmod.h>

#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>
#include <RocketGuiManager/RocketGuiManager.h>

#include "Define.h"
#include "Platform.h"

#include "Settings.h"

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

class AppManager
{
public:

    AppManager();
    ~AppManager();

    /// Exécute le menu du jeu
    void executeMenu();

    /// Exécute une partie rapide
    void executeGame(const Content::PartySetting& playSetting);

    tbe::SDLDevice* getGameEngine() const;
    tbe::EventManager* getEventMng() const;
    tbe::ticks::FpsManager* getFpsMng() const;
    tbe::scene::SceneManager* getSceneMng() const;

    tbe::gui::RocketGuiManager* getGuiMng() const;

    FMOD_SYSTEM* getFmodSystem() const;

    tbe::scene::ClassParser* getClassParser() const;
    tbe::scene::SceneParser* getSceneParser() const;

    Settings globalSettings;
    Content* globalContent;

    void onMainMenuPlay(Rocket::Core::Event& e);
    void onMainMenuSettings(Rocket::Core::Event& e);
    void onMainMenuAbout(Rocket::Core::Event& e);
    void onMainMenuQuit(Rocket::Core::Event& e);

    void onPlayMenuStart(Rocket::Core::Event& e);
    void onPlayMenuReturn(Rocket::Core::Event& e);
    void onPlayMenuNext(Rocket::Core::Event& e);
    void onPlayMenuPrev(Rocket::Core::Event& e);

    void onSettingsMenuApply(Rocket::Core::Event& e);
    void onSettingsMenuReturn(Rocket::Core::Event& e);
    void onSettingsMenuKeyBind(Rocket::Core::Event& e);

    void onKeysMenuApply(Rocket::Core::Event& e);
    void onKeysMenuReturn(Rocket::Core::Event& e);

    void onAboutMenuReturn(Rocket::Core::Event& e);

protected:

    /// Initialise les options vidéo
    void setupVideoMode();

    /// Initialise les variables interne
    void setupInernalState();

    /// Initialise l'interface du menu
    void setupMenuGui();

    /// Initialise les options audio
    void setupSound();

    /// Initialise la scene 3D de fond
    void setupBackgroundScene();

    /// Met a jour le l'aperçu de la carte séléctioner
    void updateMapSelection();

protected:
    tbe::SDLDevice* m_gameEngine;
    tbe::EventManager* m_eventMng;
    tbe::gui::RocketGuiManager* m_guiManager;
    tbe::scene::SceneManager* m_sceneManager;
    tbe::ppe::PostProcessManager* m_ppeManager;
    tbe::ticks::FpsManager* m_fpsMng;

    tbe::scene::Camera* m_camera;

    tbe::ticks::Clock m_screenWaitClock;

    FMOD_SYSTEM* m_fmodsys;
    FMOD_SOUND* m_mainMusic;
    FMOD_CHANNEL* m_mainMusicCh;

    tbe::scene::SceneParser* m_sceneParser;
    tbe::scene::ClassParser* m_classParser;

    bool m_runingMenu;

    struct
    {
        std::string nickname;
        unsigned mapSelection;

    } m_profile;

    struct
    {
        Rocket::Core::ElementDocument* mainmenu;
        Rocket::Core::ElementDocument* playmenu;
        Rocket::Core::ElementDocument* aboutmenu;
        Rocket::Core::ElementDocument* setsmenu;
        Rocket::Core::ElementDocument* keysmenu;

        Rocket::Core::ElementDocument* loadscreen;

    } m_menu;
};

#endif // _APPMANAGER_H
