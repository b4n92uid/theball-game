#ifndef _APPMANAGER_H
#define _APPMANAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <dirent.h>
#include <io.h>

#include <fmod.h>

#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

#include "Define.h"
#include "Platform.h"

#include "Settings.h"

class AppManager
{
public:

    enum PlayMode
    {
        FRAG, TEAM, ALONE
    };

    Settings globalSettings;

    struct PlaySetting
    {
        std::string playerName;
        std::string playerModel;
        std::string playMap;
        unsigned playMod;
        unsigned playerCount;
        unsigned playTime;
    };

    struct EditSetting
    {
        std::string editMap;
        bool createNew;
    };

    /// Constructeur
    AppManager();

    /// Destructeur
    ~AppManager();

    /// Exécute le menu du jeu
    void ExecuteMenu();

    /// Exécute l'éditeur de carte
    void ExecuteEditor(const EditSetting& editSetting);

    /// Exécute le jeu
    void ExecuteGame(const PlaySetting& playSetting);

    tbe::EventManager* GetEventMng() const;
    tbe::ticks::FpsManager* GetFpsMng() const;
    tbe::scene::SceneManager* GetSceneMng() const;
    tbe::gui::GuiManager* GetGuiMng() const;
    tbe::SDLDevice* GetGameEngine() const;

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

    FSOUND_STREAM* m_mainMusic;

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
