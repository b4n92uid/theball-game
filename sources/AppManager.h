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

    /// Exécute le menu du jeu
    void executeMenu();

    /// Exécute une partie rapide
    void executeGame(const Settings::PartySetting& playSetting);

    /// Exécute la campaigne du jeu
    void executeCampaign(const Settings::PartySetting& playSetting);

    tbe::EventManager* getEventMng() const;
    tbe::ticks::FpsManager* getFpsMng() const;
    tbe::scene::SceneManager* getSceneMng() const;
    tbe::gui::GuiManager* getGuiMng() const;
    tbe::SDLDevice* getGameEngine() const;
    FMOD_SYSTEM* getFmodSystem() const;

    /// Mise a jour du contenue de la GUI
    void updateGuiContent();

    Settings globalSettings;

    enum PlayMode
    {
        FRAG, TEAM, ALONE
    };

    static std::string unsignedToPlayMod(unsigned pm)
    {
        switch(pm)
        {
            case AppManager::FRAG: return "FRAG";
            case AppManager::ALONE: return "ALONE";
            case AppManager::TEAM: return "TEAM";
            default: return "FRAG";
        }
    }

    static unsigned playModToUnsigned(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);

        if(str == "FRAG") return AppManager::FRAG;
        if(str == "ALONE") return AppManager::ALONE;
        if(str == "TEAM") return AppManager::TEAM;
        else return AppManager::FRAG;
    }

protected:
    /// Initialise les options vidéo
    void setupVideoMode();

    /// Initialise l'interface du menu
    void setupMenuGui();

    /// Initialise la scene de fond du menu
    void setupBackgroundScene();

    /// Initialise les options audio
    void setupSound();

    void processMainMenuEvent();
    void processCampaignMenuEvent();
    void processPlayMenuEvent();
    void processSettingMenuEvent();
    void processSettingKeyMenuEvent();
    void processScoreMenuEvent();

protected:
    tbe::SDLDevice* m_gameEngine;
    tbe::EventManager* m_eventMng;
    tbe::gui::GuiManager* m_guiManager;
    tbe::scene::SceneManager* m_sceneManager;
    tbe::ppe::PostProcessManager* m_ppeManager;
    tbe::ticks::FpsManager* m_fpsMng;

    tbe::scene::Mesh* m_logo;
    tbe::scene::Camera* m_camera;

    tbe::ticks::Clock m_screenWaitClock;

    FMOD_SYSTEM* m_fmodsys;
    FMOD_SOUND* m_mainMusic;
    FMOD_CHANNEL* m_mainMusicCh;
    FMOD_SOUND* m_gongSound;

    struct
    {
        tbe::gui::TextBox* aboutText;

        struct
        {
            tbe::gui::SwitchString* mapSelect;
            tbe::gui::SwitchString* playerSelect;
            tbe::gui::SwitchString* modSelect;
            tbe::gui::SwitchString* timeSelect;
            tbe::gui::SwitchNumeric<int>* playerCount;
            tbe::gui::EditBox* playerName;

        } playmenu;

        struct
        {
            tbe::gui::SwitchString* levelSelect;
            tbe::gui::Button* play;
            tbe::gui::Button* ret;
            tbe::gui::SwitchString* playerSelect;
            tbe::gui::EditBox* playerName;
            tbe::gui::TextBox* description;

        } campaign;

        struct
        {
            tbe::gui::SwitchString* screenSize;
            tbe::gui::SwitchString* antiAliasing;
            tbe::gui::SwitchString* fullscreen;
            tbe::gui::SwitchString* usePpe;

        } settings;

        struct
        {
            tbe::gui::TextBox* scoreText;
            tbe::gui::SwitchString* sortType;
            tbe::gui::Button* ret;
        } score;

    } m_controls;
};

#endif // _APPMANAGER_H
