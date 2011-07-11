/*
 * File:   Settings.h
 * Author: b4n92uid
 *
 * Created on 16 avril 2010, 19:06
 */

#ifndef _SETTINGS_H
#define	_SETTINGS_H

#include <Tbe.h>

#include <boost/property_tree/ptree.hpp>

class Settings
{
public:
    Settings();
    ~Settings();

    void readSetting();
    void saveSetting();

    void readGui();
    void readAi();
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

    const char* operator()(std::string key) const;

    struct World
    {
        float gravity;

        float cameraBack;
        float cameraUp;

        float playerMoveSpeed;
        float playerJumpForce;
        float playerSize;
        float playerMasse;
        float playerLinearDamping;
        tbe::Vector3f playerAngularDamping;

        float weaponSize;
        float weaponMasse;

    } world;

    struct Video
    {
        tbe::Vector2i screenSize;
        int bits;

        bool fullScreen;
        int antialiasing;

        bool ppeUse;
        tbe::Vector2i ppeSize;

    } video;

    struct Gui
    {
        std::string button;
        std::string gauge;
        std::string editbox;
        std::string switchbox;
        std::string textbox;
        std::string udarrow;

        std::string maskH;
        std::string maskV;

        std::string fontpath;
        int fontSize;

        std::string mainmenu;
        std::string arrowleft;
        std::string arrowright;
        std::string playbutton;
        std::string vertline;
        std::string logo;
        std::string version;

    } gui;

    struct Control
    {
        typedef std::map<std::string, int> InputMap;

        InputMap mouse;
        InputMap keyboard;

    } control;

    struct MapInfo
    {
        MapInfo();
        MapInfo(std::string path);

        std::string name;
        std::string author;
        std::string comment;
        std::string script;
        std::string screen;

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

        unsigned curLevel;
    };

    struct Campaign
    {
        std::vector<PartySetting> maps;

    } campaign;

    bool noaudio;

    std::vector<MapInfo> availableMap;
    std::vector<PlayerInfo> availablePlayer;

    std::vector<std::string> botNames;

    boost::property_tree::ptree paths;
};



#endif	/* _SETTINGS_H */
