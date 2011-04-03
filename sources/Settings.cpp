/*
 * File:   Settings.cpp
 * Author: b4n92uid
 *
 * Created on 16 avril 2010, 19:06
 */

#include "Settings.h"
#include "AppManager.h"
#include "Weapon.h"

#include <tinyxml.h>
#include <fstream>

using namespace std;
using namespace tbe;
using namespace tbe::gui;

Settings::Settings()
{
    noaudio = false;
    nomusic = false;
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

void Settings::ReadAi()
{
    map<string, float*> fltbinder;
    map<string, unsigned*> unsbinder;

    unsbinder["SWITCH_TARGET_TIME"] = &ai.switchTargetTime;
    unsbinder["CRITICAL_LIFE_VALUE"] = &ai.criticalLifeValue;
    unsbinder["CRITICAL_AMMO_VALUE"] = &ai.criticalAmmoValue;
    unsbinder["SHOOT_GUST_COUNT"] = &ai.shootGustCount;
    unsbinder["SHOOT_GUST_TIME"] = &ai.shootGustTime;

    fltbinder["DYNAMIC_INTERACTION"] = &ai.dynamicInteraction;
    fltbinder["SHOOT_ACCURACY"] = &ai.shootAccuracy;

    TiXmlDocument config("ai.xml");

    if(!config.LoadFile())
        throw tbe::Exception("ReadAi; Open file error");

    TiXmlNode* root = config.FirstChildElement();

    for(TiXmlElement* node2 = root->FirstChildElement(); node2; node2 = node2->NextSiblingElement())
    {
        const char* name = node2->Attribute("name");

        if(unsbinder.count(name))
            node2->QueryValueAttribute<unsigned>("value", unsbinder[name]);
        else if(fltbinder.count(name))
            node2->QueryFloatAttribute("value", fltbinder[name]);
    }
}

void Settings::ReadWorld()
{
    map<string, float*> floatbinder;
    map<string, Vector2f*> vec2fbinder;
    map<string, Vector3f*> vec3fbinder;
    map<string, Vector4f*> vec4fbinder;
    map<string, long*> longbinder;
    map<string, int*> intbinder;

    floatbinder["World.Gravity"] = &world.gravity;

    floatbinder["Friction.Static"] = &world.staticFriction;
    floatbinder["Friction.Kenitic"] = &world.keniticFriction;

    floatbinder["Player.MoveSpeed"] = &world.playerMoveSpeed;
    floatbinder["Player.BoostSpeed"] = &world.playerBoostSpeed;
    floatbinder["Player.JumpForce"] = &world.playerJumpForce;
    floatbinder["Player.Size"] = &world.playerSize;
    floatbinder["Player.Masse"] = &world.playerMasse;
    floatbinder["Player.LinearDamping"] = &world.playerLinearDamping;
    longbinder["Player.BoostReload"] = &world.playerBoostReload;
    longbinder["Player.StartImmunity"] = &world.playerStartImmunity;

    floatbinder["Player.Explode.LifeInit"] = &world.playerExplodeLifeInit;
    floatbinder["Player.Explode.LifeDown"] = &world.playerExplodeLifeDown;
    floatbinder["Player.Explode.FreeMove"] = &world.playerExplodeFreeMove;
    longbinder["Player.Explode.Number"] = &world.playerExplodeNumber;

    floatbinder["Bullettime.Down"] = &world.bullettimeDown;
    floatbinder["Bullettime.Up"] = &world.bullettimeUp;
    floatbinder["Bullettime.Factor"] = &world.bullettimeFactor;

    floatbinder["Weapon.Size"] = &world.weaponSize;
    floatbinder["Weapon.Masse"] = &world.weaponMasse;

    floatbinder["Jumper.LifeInit"] = &world.jumperLifeInit;
    floatbinder["Jumper.LifeDown"] = &world.jumperLifeDown;
    longbinder["Jumper.Number"] = &world.jumperNumber;
    vec3fbinder["Jumper.Gravity"] = &world.jumperGravity;

    floatbinder["Teleporter.LifeInit"] = &world.teleporterLifeInit;
    floatbinder["Teleporter.LifeDown"] = &world.teleporterLifeDown;
    longbinder["Teleporter.Number"] = &world.teleporterNumber;
    vec3fbinder["Teleporter.Gravity"] = &world.teleporterGravity;

    longbinder["Item.RespawnTime"] = &world.itemRespawnTime;

    floatbinder["PPE.BoostIntensity"] = &video.ppe.boostIntensity;

    vec4fbinder["PPE.DammageColor"] = &video.ppe.dammageColor;

    intbinder["PPE.GameoverPass"] = &video.ppe.gameoverPass;

    vec4fbinder["PPE.BullettimeColor"] = &video.ppe.bullettimeColor;
    vec2fbinder["PPE.BullettimeSize"] = &video.ppe.bullettimeSize;

    vec2fbinder["PPE.WorldSize"] = &video.ppe.worldSize;
    floatbinder["PPE.WorldThershold"] = &video.ppe.worldThershold;
    floatbinder["PPE.WorldIntensity"] = &video.ppe.worldIntensity;
    floatbinder["PPE.WorldBlurPass"] = &video.ppe.worldBlurPass;

    TiXmlDocument config("world.xml");

    if(!config.LoadFile())
        throw tbe::Exception("ReadPhysic; Open file error");

    TiXmlNode* root = config.FirstChildElement();

    for(TiXmlElement* node2 = root->FirstChildElement(); node2; node2 = node2->NextSiblingElement())
    {
        const char* name = node2->Attribute("name");

        if(floatbinder.count(name))
            node2->QueryFloatAttribute("value", floatbinder[name]);

        else if(vec2fbinder.count(name))
            node2->QueryValueAttribute<Vector2f > ("value", vec2fbinder[name]);

        else if(vec3fbinder.count(name))
            node2->QueryValueAttribute<Vector3f > ("value", vec3fbinder[name]);

        else if(vec4fbinder.count(name))
            node2->QueryValueAttribute<Vector4f > ("value", vec4fbinder[name]);

        else if(longbinder.count(name))
            node2->QueryValueAttribute<long>("value", longbinder[name]);

        else if(intbinder.count(name))
            node2->QueryValueAttribute<int>("value", intbinder[name]);
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
    return (p1.playerName == p2.playerName && p1.playMod == p2.playMod);
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

        node->QueryValueAttribute<string > ("levelName", &entry.levelName);
        node->QueryValueAttribute<string > ("levelPath", &entry.levelPath);

        node->QueryValueAttribute<unsigned>("playMod", &entry.playMod);
        node->QueryValueAttribute<unsigned>("playTime", &entry.playTime);

        node->QueryValueAttribute<unsigned>("playersCount", &entry.playersCount);

        node->QueryValueAttribute<unsigned>("timestamp", &entry.timestamp);

        node->QueryValueAttribute<unsigned>("score", &entry.score);

        availableScore.push_back(entry);
    }

    sort(availableScore.begin(), availableScore.end(), ScoreSort);
}

void Settings::ReadPlayerInfo()
{
    using namespace boost::filesystem;

    availablePlayer.clear();

    directory_iterator end;
    for(directory_iterator it(PLAYERS_DIR); it != end; it++)
    {
        const path& filename = it->path();

        if(filename.extension() == ".bpd")
        {
            Settings::PlayerInfo pi(filename.file_string());

            availablePlayer.push_back(pi);
        }
    }
}

void Settings::ReadProfiles()
{
    TiXmlDocument profilesDoc("profiles.xml");

    if(profilesDoc.LoadFile())
    {
        TiXmlElement* root = profilesDoc.FirstChildElement();

        TiXmlElement* defprofil = root->FirstChildElement();
        profile.name = defprofil->Attribute("name");
        defprofil->QueryValueAttribute<unsigned>("index", &profile.index);
    }
    else
    {
        TiXmlElement root("root");

        TiXmlElement defprofile("profile");

        string name = "Joueur";

        #ifdef COMPILE_FOR_WINDOWS
        {
            DWORD bufSize = 255;
            char userName[bufSize];
            GetUserName(userName, &bufSize);
            name = userName;
        }
        #endif

        defprofile.SetAttribute("name", name);
        defprofile.SetAttribute("index", 0);

        profile.name = name;
        profile.index = 0;

        root.InsertEndChild(defprofile);

        TiXmlDeclaration dec("1.0", "ISO-8859-1", "");
        profilesDoc.InsertEndChild(dec);
        profilesDoc.InsertEndChild(root);
        profilesDoc.SaveFile();
    }
}

void Settings::ReadCampaign()
{
    TiXmlDocument config("campaign.xml");

    if(!config.LoadFile())
        throw tbe::Exception("ReadCampaign; Open file error");

    campaign.maps.clear();

    TiXmlNode* root = config.FirstChildElement();

    for(TiXmlElement* node = root->FirstChildElement(); node; node = node->NextSiblingElement())
    {
        PartySetting party;

        party.playMap = MapInfo(node->Attribute("map"));

        node->QueryValueAttribute<unsigned>("playMode", &party.playMod);
        node->QueryValueAttribute<unsigned>("playTime", &party.playTime);

        node->QueryValueAttribute<unsigned>("playerCount", &party.playerCount);

        node->QueryValueAttribute<unsigned>("winCond", &party.winCond);

        party.curLevel = campaign.maps.size();

        campaign.maps.push_back(party);
    }
}

void Settings::ReadMapInfo()
{
    using namespace boost::filesystem;

    availableMap.clear();

    directory_iterator end;
    for(directory_iterator it(MAPS_DIR); it != end; it++)
    {
        const path& filename = it->path();

        if(filename.extension() == ".bld")
        {
            Settings::MapInfo mi(filename.file_string());

            availableMap.push_back(mi);
        }
    }
}

void Settings::ReadSetting()
{
    ReadAi();
    ReadCampaign();
    ReadProfiles();
    ReadVideo();
    ReadControl();
    ReadWorld();

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

void Settings::SaveProfiles()
{
    TiXmlDocument profilesDoc("profiles.xml");

    if(!profilesDoc.LoadFile())
        throw tbe::Exception("Settings::SaveProfiles; Open file error");

    TiXmlElement* root = profilesDoc.FirstChildElement();
    TiXmlElement* defprofil = root->FirstChildElement();

    defprofil->SetAttribute("name", profile.name);
    defprofil->SetAttribute("index", profile.index);

    profilesDoc.SaveFile();
}

void Settings::SaveSetting()
{
    SaveVideo();
    SaveControl();
    SaveProfiles();
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

    while(tools::getline(file, buffer))
    {
        if(buffer == ".map")
        {
            if(buffer.empty() || buffer[0] == '#')
                continue;

            while(tools::getline(file, buffer))
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

    while(tools::getline(file, buffer))
    {
        if(buffer.empty() || buffer[0] == '#')
            continue;

        int eqPos = buffer.find_first_of('=');
        string key(buffer, 0, eqPos), value(buffer, eqPos + 1);
        attMap[key] = value;
    }

    this->name = attMap["name"];
    this->nick = attMap["name"];
    this->model = attMap["model"];
}

Settings::PartySetting::PartySetting()
{
    playMod = 0;
    playTime = 0;

    playerCount = 0;

    curLevel = 0;
    winCond = 0;
}

Settings::ScoreInfo::ScoreInfo()
{
    playMod = 0;
    playersCount = 0;
    playTime = 0;
    timestamp = 0;
    score = 0;
}
