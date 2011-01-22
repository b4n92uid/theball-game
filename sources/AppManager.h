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

    enum PlayMode
    {
        FRAG, TEAM, ALONE
    };

    static std::string UnsignedToPlayMod(unsigned pm)
    {
        switch(pm)
        {
            case AppManager::FRAG: return "FRAG";
            case AppManager::ALONE: return "ALONE";
            case AppManager::TEAM: return "TEAM";
            default: return "FRAG";
        }
    }

    static unsigned PlayModToUnsigned(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);

        if(str == "FRAG") return AppManager::FRAG;
        if(str == "ALONE") return AppManager::ALONE;
        if(str == "TEAM") return AppManager::TEAM;
        else return AppManager::FRAG;
    }

    Settings globalSettings;

    /// Constructeur
    AppManager();

    /// Destructeur
    ~AppManager();

    /// Exécute le menu du jeu
    void ExecuteMenu();

    /// Exécute l'éditeur de carte
    void ExecuteEditor(const Settings::EditSetting& editSetting);

    /// Exécute le jeu
    void ExecuteGame(const Settings::PartySetting& playSetting);

    /// *
    void ExecuteCampaign(const Settings::PartySetting& playSetting);

    tbe::EventManager* GetEventMng() const;
    tbe::ticks::FpsManager* GetFpsMng() const;
    tbe::scene::SceneManager* GetSceneMng() const;
    tbe::gui::GuiManager* GetGuiMng() const;
    tbe::SDLDevice* GetGameEngine() const;
    FMOD_SYSTEM* GetFmodSystem() const;

    void UpdateGuiContent();

protected:
    /// Configure les options vidéo
    void SetupVideoMode();

    /// Configure l'interface du menu
    void SetupMenuGui();

    /// Configure la scene de fond du menu
    void SetupBackgroundScene();

    /// Configure les option du son
    void SetupSound();

    void ProcessMainMenuEvent();
    void ProcessCampaignMenuEvent();
    void ProcessPlayMenuEvent();
    void ProcessEditMenuEvent();
    void ProcessSettingMenuEvent();
    void ProcessSettingKeyMenuEvent();
    void ProcessScoreMenuEvent();

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
            tbe::gui::Button* newMap;
            tbe::gui::SwitchString* mapSelect;
            tbe::gui::Button* editMap;
            tbe::gui::Button* ret;

        } edit;

        struct
        {
            tbe::gui::TextBox* scoreText;
            tbe::gui::SwitchString* sortType;
            tbe::gui::Button* ret;
        } score;

    } m_controls;
};

#endif // _APPMANAGER_H
