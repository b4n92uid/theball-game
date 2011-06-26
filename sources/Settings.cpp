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

void Settings::readGui()
{
    map<string, string*> binder;

    binder["BACKGROUND_MAINMENU"] = &gui.backgroundMainmenu;
    binder["BACKGROUND_DAMMAGE"] = &gui.backgroundDammage;
    binder["BACKGROUND_HUD"] = &gui.backgroundHud;
    binder["BACKGROUND_PAUSE"] = &gui.backgroundPause;
    binder["BACKGROUND_TEXTBOX"] = &gui.backgroundTextbox;
    binder["BACKGROUND_LISTBOX"] = &gui.backgroundListbox;
    binder["BACKGROUND_LOGO"] = &gui.backgroundLogo;
    binder["MASK_H"] = &gui.maskH;
    binder["MASK_V"] = &gui.maskV;
    binder["GUI_SCORE"] = &gui.score;
    binder["GUI_NOTIFY"] = &gui.notify;
    binder["GUI_PREVIEW"] = &gui.preview;
    binder["GUI_BUTTON"] = &gui.button;
    binder["GUI_GAUGE"] = &gui.gauge;
    binder["GUI_EDIT"] = &gui.editBox;
    binder["GUI_SWITCH"] = &gui.switchBox;
    binder["GUI_VECTOR"] = &gui.vectorBox;
    binder["GUI_FONT"] = &gui.font;

    map<string, tbe::Vector2f*> binderSize;

    binderSize["GUI_BUTTON"] = &gui.buttonSize;
    binderSize["GUI_GAUGE"] = &gui.gaugeSize;
    binderSize["GUI_EDIT"] = &gui.editBoxSize;
    binderSize["GUI_SWITCH"] = &gui.switchBoxSize;
    binderSize["GUI_VECTOR"] = &gui.vectorBoxSize;

    TiXmlDocument config("gui.xml");

    if(!config.LoadFile())
        throw tbe::Exception("readGui; Open file error");

    TiXmlNode* root = config.FirstChildElement();

    for(TiXmlElement* node2 = root->FirstChildElement(); node2; node2 = node2->NextSiblingElement())
    {
        string name = node2->Attribute("name");

        if(name == "GUI_FONTSIZE")
            node2->Attribute("value", &gui.fontSize);
        else
        {
            *binder[name] = node2->Attribute("value");

            if(binderSize.count(name))
            {
                const char* data = node2->Attribute("size");
                binderSize[name]->fromStr(data, true);
            }
        }
    }
}

void Settings::readVideo()
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

void Settings::readControl()
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

void Settings::readAi()
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

void Settings::readWorld()
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

    floatbinder["Weapon.Size"] = &world.weaponSize;
    floatbinder["Weapon.Masse"] = &world.weaponMasse;

    longbinder["Item.RespawnTime"] = &world.itemRespawnTime;

    floatbinder["PPE.BoostIntensity"] = &video.ppe.boostIntensity;

    vec4fbinder["PPE.DammageColor"] = &video.ppe.dammageColor;

    intbinder["PPE.GameoverPass"] = &video.ppe.gameoverPass;

    vec2fbinder["PPE.BloomSize"] = &video.ppe.bloomSize;
    floatbinder["PPE.BloomThershold"] = &video.ppe.bloomThershold;
    floatbinder["PPE.BloomIntensity"] = &video.ppe.bloomIntensity;
    floatbinder["PPE.BloomBlurPass"] = &video.ppe.bloomBlurPass;

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

void Settings::readPlayerInfo()
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

void Settings::readProfiles()
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

void Settings::readCampaign()
{
    TiXmlDocument config("campaign.xml");

    if(!config.LoadFile())
        throw tbe::Exception("ReadCampaign; Open file error");

    campaign.maps.clear();

    TiXmlNode* root = config.FirstChildElement();

    for(TiXmlElement* node = root->FirstChildElement(); node; node = node->NextSiblingElement())
    {
        PartySetting party;

        party.map = MapInfo(node->Attribute("map"));

        node->QueryValueAttribute<unsigned>("playerCount", &party.playerCount);

        party.curLevel = campaign.maps.size();

        campaign.maps.push_back(party);
    }
}

void Settings::readMapInfo()
{
    using namespace boost::filesystem;

    availableMap.clear();

    directory_iterator end;
    for(directory_iterator it(MAPS_DIR); it != end; it++)
    {
        const path& filename = it->path();

        if(filename.extension() == ".map")
        {
            Settings::MapInfo mi(filename.file_string());

            availableMap.push_back(mi);
        }
    }
}

void Settings::readSetting()
{
    readAi();
    readGui();
    readCampaign();
    readProfiles();
    readVideo();
    readControl();
    readWorld();

    readMapInfo();
    readPlayerInfo();
}

void Settings::saveVideo()
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

void Settings::saveControl()
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
    actions.push_back("brake");
    actions.push_back("shoot");
    actions.push_back("power");
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

void Settings::saveProfiles()
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

void Settings::saveSetting()
{
    saveVideo();
    saveControl();
    saveProfiles();
}

void Settings::fillWindowSettingsFromGui(tbe::gui::GuiManager* guiManager)
{
    char sep;

    istringstream exp(guiManager->getControl("screenSize")->getLabel());
    exp >> video.screenSize.x >> sep >> video.screenSize.y >> sep >> video.bits;

    video.usePpe = guiManager->getControl<SwitchString > ("usePpe")->getData().getValue<bool>();
    video.fullScreen = guiManager->getControl<SwitchString > ("fullScreen")->getData().getValue<bool>();
    video.antialiasing = guiManager->getControl<SwitchString > ("antiAliasing")->getData().getValue<unsigned >();
}

void Settings::fillControlSettingsFromGui(tbe::gui::GuiManager* guiManager)
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
    actions.push_back("brake");
    actions.push_back("shoot");
    actions.push_back("power");
    actions.push_back("switchUpWeapon");
    actions.push_back("switchDownWeapon");

    for(unsigned i = 0; i < actions.size(); i++)
    {
        int key = guiManager->getControl<KeyConfig > (actions[i])->getKeyCode(),
                mouse = guiManager->getControl<KeyConfig > (actions[i])->getMouseCode();

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
    this->filename = path;

    scene::SceneParser parser;
    parser.loadScene(path);

    author = parser.getAuthorName();
    name = parser.getSceneName();

    script = parser.getAdditionalString("script");
    comment = parser.getAdditionalString("comment");
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
    playerCount = 0;
    curLevel = 0;
}
