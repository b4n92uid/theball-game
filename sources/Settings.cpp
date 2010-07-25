/* 
 * File:   Settings.cpp
 * Author: b4n92uid
 * 
 * Created on 16 avril 2010, 19:06
 */

#include "Settings.h"
#include "AppManager.h"
#include "Weapon.h"

using namespace std;
using namespace tbe;
using namespace tbe::gui;

Settings::Settings()
{
}

Settings::~Settings()
{
}

void Settings::ReadVideo()
{
    TiXmlDocument config("video.xml");

    if(!config.LoadFile())
        throw tbe::Exception("ReadVideo; Open file error");

    TiXmlNode* root = config.FirstChildElement();

    for(TiXmlElement* node2 = root->FirstChildElement(); node2; node2 = node2->NextSiblingElement())
    {
        string name = node2->Attribute("name");

        int value = 0;
        node2->Attribute("value", &value);

        if(name == "width") video.screenSize.x = value;
        if(name == "height") video.screenSize.y = value;
        if(name == "bits") video.bits = *reinterpret_cast<unsigned*>(&value);
        if(name == "antialiasing") video.antialiasing = *reinterpret_cast<unsigned*>(&value);
        if(name == "fullscreen") video.fullScreen = value;
        if(name == "useppe") video.usePpe = value;
    }
}

void Settings::ReadControl()
{
    control.keyboard.clear();
    control.mouse.clear();

    TiXmlDocument config("control.xml");

    if(!config.LoadFile())
        throw tbe::Exception("ReadControl; nOpen file error");

    TiXmlNode* root = config.FirstChildElement();

    for(TiXmlElement* node2 = root->FirstChildElement(); node2; node2 = node2->NextSiblingElement())
    {
        string name = node2->Attribute("name");

        int key = -1;
        node2->QueryIntAttribute("key", &key);

        if(key != -1)
            control.keyboard[name] = key;

        int mouse = -1;
        node2->QueryIntAttribute("mouse", &mouse);

        if(mouse != -1)
            control.mouse[name] = mouse;
    }
}

void Settings::ReadPhysics()
{
    map<string, float*> binder;

    binder["GRAVITY"] = &physics.gravity;

    binder["PLAYER_MOVESPEED"] = &physics.playerMoveSpeed;
    binder["PLAYER_BOOSTSPEED"] = &physics.playerBoostSpeed;
    binder["PLAYER_JUMPFORCE"] = &physics.playerJumpForce;
    binder["PLAYER_SIZE"] = &physics.playerSize;
    binder["PLAYER_MASS"] = &physics.playerMasse;
    binder["PLAYER_LDAMPING"] = &physics.playerLinearDamping;

    binder["WEAPON_SIZE"] = &physics.weaponSize;
    binder["WEAPON_MASS"] = &physics.weaponMasse;

    binder["STATIC_FRICTION"] = &physics.staticFriction;
    binder["KENITIC_FRICTION"] = &physics.keniticFriction;

    TiXmlDocument config("physics.xml");

    if(!config.LoadFile())
        throw tbe::Exception("ReadPhysic; Open file error");

    TiXmlNode* root = config.FirstChildElement();

    for(TiXmlElement* node2 = root->FirstChildElement(); node2; node2 = node2->NextSiblingElement())
    {
        const char* name = node2->Attribute("name");
        node2->QueryFloatAttribute("value", binder[name]);
    }
}

inline bool ScoreSort(const Settings::ScoreInfo& p1, const Settings::ScoreInfo& p2)
{
    if(p1.playerName == p2.playerName)
        return p1.score > p2.score;
    else
        return p1.playerName > p2.playerName;
}

inline bool ScoreUnique(const Settings::ScoreInfo& p1, const Settings::ScoreInfo& p2)
{
    return(p1.playerName == p2.playerName && p1.playMod == p2.playMod);
}

void Settings::ReadScoreInfo()
{
    availableScore.clear();

    TiXmlDocument config("score.xml");

    if(!config.LoadFile())
        throw tbe::Exception("ReadScoreInfo; Open file error");

    TiXmlNode* root = config.FirstChildElement();

    for(TiXmlElement* node = root->FirstChildElement(); node; node = node->NextSiblingElement())
    {
        ScoreInfo entry;

        entry.playMod = 0;
        entry.playersCount = 0;
        entry.playTime = 0;
        entry.timestamp = 0;
        entry.score = 0;

        node->QueryValueAttribute<string > ("playerName", &entry.playerName);
        node->QueryValueAttribute<string > ("playerModel", &entry.playerModel);
        node->QueryValueAttribute<string > ("levelPath", &entry.levelPath);
        node->QueryValueAttribute<string > ("levelName", &entry.levelName);

        node->QueryValueAttribute<unsigned>("playMod", &entry.playMod);
        node->QueryValueAttribute<unsigned>("playersCount", &entry.playersCount);
        node->QueryValueAttribute<unsigned>("playTime", &entry.playTime);
        node->QueryValueAttribute<unsigned>("timestamp", &entry.timestamp);
        node->QueryValueAttribute<unsigned>("score", &entry.score);

        availableScore.push_back(entry);
    }

    sort(availableScore.begin(), availableScore.end(), ScoreSort);

    vector<Settings::ScoreInfo>::iterator
    newEnd = unique(availableScore.begin(), availableScore.end(), ScoreUnique);

    availableScore.erase(newEnd, availableScore.end());
}

void Settings::ReadPlayerInfo()
{
    availablePlayer.clear();

    string playersDir("data/players/");

    _finddata_t file;
    int handle = _findfirst((playersDir + "*.bpd").c_str(), &file);


    do
    {
        Settings::PlayerInfo pi(playersDir + file.name);

        cout << "Load player : " << pi.model << endl;

        availablePlayer.push_back(pi);

    }
    while(!_findnext(handle, &file));
}

void Settings::ReadMapInfo()
{
    availableMap.clear();

    string mapsDir("data/levels/");

    _finddata_t file;
    int handle = _findfirst((mapsDir + "*.bld").c_str(), &file);

    do
    {
        Settings::MapInfo mi(mapsDir + file.name);

        cout << "Load map : " << mi.file << endl;

        availableMap.push_back(mi);
    }
    while(!_findnext(handle, &file));
}

void Settings::ReadSetting()
{
    ReadVideo();
    ReadControl();
    ReadPhysics();

    ReadMapInfo();
    ReadPlayerInfo();
    ReadScoreInfo();
}

void Settings::SaveVideo()
{
    TiXmlDocument config("video.xml");

    if(!config.LoadFile())
        throw tbe::Exception("SaveVideo; Open file error");

    TiXmlNode * root = config.FirstChildElement();

    for(TiXmlElement* node2 = root->FirstChildElement(); node2; node2 = node2->NextSiblingElement())
    {
        string name = node2->Attribute("name");

        if(name == "width") node2->SetAttribute("value", video.screenSize.x);
        if(name == "height") node2->SetAttribute("value", video.screenSize.y);
        if(name == "bits") node2->SetAttribute("value", *reinterpret_cast<const int*>(&video.bits));
        if(name == "antialiasing") node2->SetAttribute("value", *reinterpret_cast<const int*>(&video.antialiasing));
        if(name == "fullscreen") node2->SetAttribute("value", video.fullScreen);

        if(name == "useppe") node2->SetAttribute("value", video.usePpe);
    }

    config.SaveFile();
}

void Settings::SaveControl()
{
    TiXmlDocument config("control.xml");

    if(!config.LoadFile())
        throw tbe::Exception("SaveControl; Open file error");

    TiXmlNode * root = config.FirstChildElement();

    vector<string> actions;

    actions.push_back("forward");
    actions.push_back("backward");
    actions.push_back("strafRight");
    actions.push_back("strafLeft");
    actions.push_back("jump");
    actions.push_back("boost");
    actions.push_back("shoot");
    actions.push_back("bullettime");
    actions.push_back("switchUpWeapon");
    actions.push_back("switchDownWeapon");

    for(TiXmlElement* node2 = root->FirstChildElement(); node2; node2 = node2->NextSiblingElement())
    {
        string name = node2->Attribute("name");

        for(unsigned i = 0; i < actions.size(); i++)
        {
            if(name == actions[i])
            {
                map<string, int>::const_iterator keyItt = control.keyboard.find(actions[i]);
                map<string, int>::const_iterator mouseItt = control.mouse.find(actions[i]);

                node2->RemoveAttribute("key");
                node2->RemoveAttribute("mouse");

                if(keyItt != control.keyboard.end())
                    node2->SetAttribute("key", keyItt->second);

                else

                    if(mouseItt != control.mouse.end())
                    node2->SetAttribute("mouse", mouseItt->second);

                break;
            }
        }
    }

    config.SaveFile();
}

void Settings::SaveSetting()
{
    SaveVideo();
    SaveControl();
}

void Settings::FillWindowSettingsFromGui(tbe::gui::GuiManager* guiManager)
{
    char sep;

    istringstream exp(guiManager->GetControl("screenSize")->GetLabel());
    exp >> video.screenSize.x >> sep >> video.screenSize.y >> sep >> video.bits;

    video.usePpe = guiManager->GetControl<SwitchString > ("usePpe")->GetData().GetValue<bool>();
    video.fullScreen = guiManager->GetControl<SwitchString > ("fullScreen")->GetData().GetValue<bool>();
    video.antialiasing = guiManager->GetControl<SwitchString > ("antiAliasing")->GetData().GetValue<unsigned >();
}

void Settings::FillControlSettingsFromGui(tbe::gui::GuiManager* guiManager)
{
    control.keyboard.clear();
    control.mouse.clear();

    vector<string> actions;

    actions.push_back("forward");
    actions.push_back("backward");
    actions.push_back("strafRight");
    actions.push_back("strafLeft");
    actions.push_back("jump");
    actions.push_back("boost");
    actions.push_back("shoot");
    actions.push_back("bullettime");
    actions.push_back("switchUpWeapon");
    actions.push_back("switchDownWeapon");

    for(unsigned i = 0; i < actions.size(); i++)
    {
        int key = guiManager->GetControl<KeyConfig > (actions[i])->GetKeyCode(),
                mouse = guiManager->GetControl<KeyConfig > (actions[i])->GetMouseCode();

        if(key != -1)
            control.keyboard[actions[i]] = key;

        else if(mouse != -1)
            control.mouse[actions[i]] = mouse;
    }

}

Settings::MapInfo::MapInfo()
{

}

Settings::MapInfo::MapInfo(std::string path)
{
    this->file = path;
    this->name = "Unknown";

    ifstream file(path.c_str());

    if(!file)
        throw Exception("Settings::MapInfo::MapInfo; Open file error (%s)", path.c_str());

    string buffer;

    while(getline(file, buffer))
        if(buffer == ".map")
        {
            if(buffer.empty() || buffer[0] == '#')
                continue;

            while(getline(file, buffer))
            {
                if(buffer.empty() || buffer[0] == '#')
                    continue;

                if(buffer.find("name") != string::npos)
                    name = buffer.substr(buffer.find_first_of('=') + 1);

                break;
            }

            break;
        }
}

Settings::PlayerInfo::PlayerInfo()
{

}

Settings::PlayerInfo::PlayerInfo(std::string path)
{
    this->file = path;

    map<string, string> attMap;

    ifstream file(path.c_str());

    if(!file)
        throw Exception("Settings::PlayerInfo::PlayerInfo; Open file error (%s)", path.c_str());

    string buffer;

    while(getline(file, buffer))
    {
        if(buffer.empty() || buffer[0] == '#')
            continue;

        int eqPos = buffer.find_first_of('=');
        string key(buffer, 0, eqPos), value(buffer, eqPos + 1);
        attMap[key] = value;
    }

    this->name = attMap["name"];
    this->model = attMap["model"];
}
