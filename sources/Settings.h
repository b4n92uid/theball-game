/* 
 * File:   Settings.h
 * Author: b4n92uid
 *
 * Created on 16 avril 2010, 19:06
 */

#ifndef _SETTINGS_H
#define	_SETTINGS_H

#include <Tbe.h>

#include <tinyxml.h>
#include <fstream>

class Settings
{
public:
    Settings();
    ~Settings();

    struct Physics
    {
        float gravity;

        float playerMoveSpeed;
        float playerBoostSpeed;
        float playerJumpForce;
        float playerSize;
        float playerMasse;
        float playerLinearDamping;

        float weaponSize;
        float weaponMasse;

        float staticFriction;
        float keniticFriction;

    } physics;

    struct Video
    {
        tbe::Vector2i screenSize;
        bool fullScreen;
        bool usePpe;
        unsigned bits;
        unsigned antialiasing;
        unsigned lod;

        float guiSizeFactor;
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
        std::string playerName;
        std::string playerModel;
        std::string levelPath;
        std::string levelName;
        
        unsigned playMod;
        unsigned playersCount;
        unsigned playTime;
        unsigned timestamp;
        unsigned score;
    };

    struct PlayerInfo
    {
        PlayerInfo();
        PlayerInfo(std::string path);

        std::string name;
        std::string model;
        std::string file;
    };

    std::vector<MapInfo> availableMap;
    std::vector<ScoreInfo> availableScore;
    std::vector<PlayerInfo> availablePlayer;

    void FillControlSettingsFromGui(tbe::gui::GuiManager* guiManager);
    void FillWindowSettingsFromGui(tbe::gui::GuiManager* guiManager);

    void ReadSetting();
    void SaveSetting();

    void ReadVideo();
    void ReadControl();
    void ReadPhysics();
    void ReadMapInfo();
    void ReadScoreInfo();
    void ReadPlayerInfo();

    void SaveVideo();
    void SaveControl();
};



#endif	/* _SETTINGS_H */
