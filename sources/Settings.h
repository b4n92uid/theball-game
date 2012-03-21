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

class AppManager;

class Settings
{
public:

    Settings();
    ~Settings();

    void readSetting();
    void saveSetting();

    void readGui();
    void readAi();
    void readVideo();
    void readControl();
    void readWorld();

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
        std::string nopreview;

    } gui;

    struct Control
    {
        typedef std::map<std::string, int> InputMap;

        InputMap mouse;
        InputMap keyboard;

    } control;


    boost::property_tree::ptree paths;
    boost::property_tree::ptree weapons;

    bool noaudio;

};

class Content
{
public:

    Content(AppManager* appmng);
    ~Content();

    void readMapInfo(std::string dir);
    void readPlayerInfo(std::string dir);

    struct MapInfo
    {
        MapInfo();
        MapInfo(AppManager* appmng, std::string path);
        ~MapInfo();

        std::string name;
        std::string author;
        std::string comment;
        std::string script;
        std::string screen;
        
        std::string filepath;
    };

    struct PlayerInfo
    {
        PlayerInfo();
        PlayerInfo(AppManager* appmng, std::string path);
        ~PlayerInfo();

        std::string name;
        
        std::string filepath;
    };

    struct PartySetting
    {
        PartySetting();

        unsigned curLevel;

        std::string nickname;
        PlayerInfo* player;
        MapInfo* map;
    };

    PlayerInfo* mainPlayer;
    std::vector<PlayerInfo*> availablePlayer;

    std::vector<MapInfo*> availableMap;

    std::vector<std::string> botNames;

private:
    AppManager* m_appManager;
};


#endif	/* _SETTINGS_H */
