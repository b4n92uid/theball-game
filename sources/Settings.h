/*
 * File:   Settings.h
 * Author: b4n92uid
 *
 * Created on 16 avril 2010, 19:06
 */

#ifndef _SETTINGS_H
#define	_SETTINGS_H

#include <Tbe.h>

class Settings
{
public:
    Settings();
    ~Settings();

    void readSetting();
    void saveSetting();

    void readGui();
    void readAi();
    void readProfiles();
    void readCampaign();
    void readVideo();
    void readControl();
    void readWorld();
    void readMapInfo();
    void readPlayerInfo();

    void saveVideo();
    void saveControl();
    void saveProfiles();

    void fillControlSettingsFromGui(tbe::gui::GuiManager* guiManager);
    void fillWindowSettingsFromGui(tbe::gui::GuiManager* guiManager);

    struct Ai
    {
        unsigned switchTargetTime;
        unsigned criticalLifeValue;
        unsigned criticalAmmoValue;
        unsigned shootGustCount;
        unsigned shootGustTime;
        float dynamicInteraction;
        float shootAccuracy;

    } ai;

    struct World
    {
        float gravity;

        float staticFriction;
        float keniticFriction;

        float playerMoveSpeed;
        float playerBoostSpeed;
        float playerJumpForce;
        float playerSize;
        float playerMasse;
        float playerLinearDamping;
        long playerStartImmunity;
        long playerBoostReload;

        float playerExplodeLifeInit;
        float playerExplodeLifeDown;
        float playerExplodeFreeMove;
        long playerExplodeNumber;

        float bullettimeDown;
        float bullettimeUp;
        float bullettimeFactor;

        float weaponSize;
        float weaponMasse;

        float jumperLifeInit;
        float jumperLifeDown;
        long jumperNumber;
        tbe::Vector3f jumperGravity;

        float teleporterLifeInit;
        float teleporterLifeDown;
        long teleporterNumber;
        tbe::Vector3f teleporterGravity;

        long itemRespawnTime;

    } world;

    struct Video
    {
        tbe::Vector2i screenSize;
        bool fullScreen;
        bool usePpe;
        unsigned bits;
        unsigned antialiasing;
        unsigned lod;

        float guiSizeFactor;

        struct Ppe
        {
            float boostIntensity;

            tbe::Vector2f bullettimeSize;
            tbe::Vector4f bullettimeColor;

            tbe::Vector4f dammageColor;

            int gameoverPass;

            tbe::Vector2f bloomSize;
            float bloomThershold;
            float bloomIntensity;
            float bloomBlurPass;

        } ppe;

    } video;

    struct Gui
    {
        std::string backgroundMainmenu;
        std::string backgroundBullettime;
        std::string backgroundDammage;
        std::string backgroundHud;
        std::string backgroundPause;
        std::string backgroundTextboxV;
        std::string backgroundTextboxH;
        std::string backgroundListboxV;
        std::string backgroundListboxH;

        std::string score;
        std::string notify;
        std::string preview;
        std::string button;
        std::string gauge;
        std::string editBox;
        std::string switchBox;
        std::string vectorBox;

        tbe::Vector2f buttonSize;
        tbe::Vector2f gaugeSize;
        tbe::Vector2f editBoxSize;
        tbe::Vector2f switchBoxSize;
        tbe::Vector2f vectorBoxSize;

        std::string font;
        int fontSize;

    } gui;

    struct Control
    {
        std::map<std::string, int> mouse;
        std::map<std::string, int> keyboard;

    } control;

    struct MapInfo
    {
        MapInfo();
        MapInfo(std::string path);

        std::string name;
        std::string author;
        std::string comment;
        std::string script;

        std::string filename;
    };

    struct PlayerInfo
    {
        PlayerInfo();
        PlayerInfo(std::string path);

        std::string nick;
        std::string name;
        std::string model;
        std::string file;
    };

    struct PartySetting
    {
        PartySetting();

        PlayerInfo player;

        MapInfo map;

        unsigned playerCount;
        unsigned curLevel;
    };

    struct Profile
    {
        std::string name;
        unsigned index;

    } profile;

    struct Campaign
    {
        std::vector<PartySetting> maps;

    } campaign;

    bool noaudio;
    bool nomusic;

    std::vector<MapInfo> availableMap;
    std::vector<PlayerInfo> availablePlayer;
};



#endif	/* _SETTINGS_H */
