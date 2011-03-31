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

            tbe::Vector2f worldSize;
            float worldThershold;
            float worldIntensity;
            float worldBlurPass;

        } ppe;

    } video;

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
        std::string file;
    };

    struct ScoreInfo
    {
        ScoreInfo();

        std::string playerName;
        std::string playerModel;

        std::string levelName;
        std::string levelPath;

        unsigned playMod;
        unsigned playTime;

        unsigned playersCount;

        unsigned timestamp;

        unsigned score;
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

        PlayerInfo playerName;

        MapInfo playMap;

        unsigned playMod;
        unsigned playTime;

        unsigned playerCount;

        unsigned curLevel;
        unsigned winCond;
    };

    struct EditSetting
    {
        MapInfo editMap;
        bool createNew;
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
    std::vector<ScoreInfo> availableScore;
    std::vector<PlayerInfo> availablePlayer;

    void FillControlSettingsFromGui(tbe::gui::GuiManager* guiManager);
    void FillWindowSettingsFromGui(tbe::gui::GuiManager* guiManager);

    void ReadSetting();
    void SaveSetting();

    void ReadAi();
    void ReadProfiles();
    void ReadCampaign();
    void ReadVideo();
    void ReadControl();
    void ReadPhysics();
    void ReadMapInfo();
    void ReadScoreInfo();
    void ReadPlayerInfo();

    void SaveVideo();
    void SaveControl();
    void SaveProfiles();
};



#endif	/* _SETTINGS_H */
