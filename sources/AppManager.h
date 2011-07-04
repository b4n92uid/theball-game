#ifndef _APPMANAGER_H
#define _APPMANAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <fmod.h>

#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

#include "Define.h"
#include "Platform.h"

#include "Settings.h"

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>

class AppManager
{
public:

    AppManager();
    ~AppManager();

    /// Ex�cute le menu du jeu
    void executeMenu();

    /// Ex�cute une partie rapide
    void executeGame(const Settings::PartySetting& playSetting);

    /// Ex�cute la campaigne du jeu
    void executeCampaign(const Settings::PartySetting& playSetting);

    tbe::EventManager* getEventMng() const;
    tbe::ticks::FpsManager* getFpsMng() const;
    tbe::scene::SceneManager* getSceneMng() const;
    tbe::gui::GuiManager* getGuiMng() const;
    tbe::SDLDevice* getGameEngine() const;
    FMOD_SYSTEM* getFmodSystem() const;

    Settings globalSettings;

protected:
    /// Initialise les options vid�o
    void setupVideoMode();

    /// Initialise l'interface du menu
    void setupMenuGui();

    /// Initialise les options audio
    void setupSound();

    ///
    void setupBackgroundScene();

    void processMainMenuEvent();
    void processCampaignMenuEvent();
    void processPlayMenuEvent();
    void processSettingMenuEvent();
    void processSettingKeyMenuEvent();

    /// Mise a jour du contenue de la GUI
    void updateGuiContent();

    void updateQuickPlayMapInfo();

protected:
    tbe::SDLDevice* m_gameEngine;
    tbe::EventManager* m_eventMng;
    tbe::gui::GuiManager* m_guiManager;
    tbe::scene::SceneManager* m_sceneManager;
    tbe::ppe::PostProcessManager* m_ppeManager;
    tbe::ticks::FpsManager* m_fpsMng;

    tbe::scene::Camera* m_camera;

    tbe::ticks::Clock m_screenWaitClock;

    FMOD_SYSTEM* m_fmodsys;
    FMOD_SOUND* m_mainMusic;
    FMOD_CHANNEL* m_mainMusicCh;
    FMOD_SOUND* m_gongSound;

    struct
    {
        tbe::gui::Button* campaign;
        tbe::gui::Button* quickplay;
        tbe::gui::Button* settings;
        tbe::gui::Button* about;
        tbe::gui::Button* quit;

        struct
        {
            tbe::gui::TextBox* aboutText;

        } aboutmenu;

        struct
        {
            tbe::gui::SwitchString* mapSelect;
            tbe::gui::SwitchString* playerSelect;
            tbe::gui::EditBox* playerName;
            tbe::gui::TextBox* description;

            tbe::gui::Image* preview;

            tbe::gui::Button* play;
            tbe::gui::Button* ret;

        } playmenu;

        struct
        {
            // tbe::gui::SwitchString* levelSelect;
            // tbe::gui::Button* play;
            // tbe::gui::Button* ret;
            // tbe::gui::SwitchString* playerSelect;
            // tbe::gui::EditBox* playerName;
            // tbe::gui::TextBox* description;

        } campaignmenu;

        struct
        {
            tbe::gui::SwitchString* screenSize;
            tbe::gui::SwitchString* antiAliasing;
            tbe::gui::SwitchString* fullscreen;
            tbe::gui::SwitchString* usePpe;

        } settingsmenu;

    } m_controls;
};

#endif // _APPMANAGER_H
