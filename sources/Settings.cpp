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

#include <boost/property_tree/ini_parser.hpp>

using namespace std;
using namespace tbe;
using namespace gui;
using namespace boost;

namespace pt = property_tree;

Settings::Settings()
{
    noaudio = false;
}

Settings::~Settings()
{
}

const char* Settings::operator()(std::string key) const
{
    return paths.get<std::string > (key).c_str();
}

void Settings::readGui()
{
    pt::ptree parser;

    pt::read_ini("gui.ini", parser);

    gui.button = parser.get<string > ("menu.button");
    gui.gauge = parser.get<string > ("menu.gauge");
    gui.editbox = parser.get<string > ("menu.editbox");
    gui.switchbox = parser.get<string > ("menu.switchbox");
    gui.textbox = parser.get<string > ("menu.textbox");
    gui.udarrow = parser.get<string > ("menu.udarrow");

    gui.maskH = parser.get<string > ("menu.mask_h");
    gui.maskV = parser.get<string > ("menu.mask_v");

    gui.fontpath = parser.get<string > ("menu.fontpath");
    gui.fontSize = parser.get<int>("menu.fontsize");

    gui.mainmenu = parser.get<string > ("menu.mainmenu");
    gui.arrowleft = parser.get<string > ("menu.arrowleft");
    gui.arrowright = parser.get<string > ("menu.arrowright");
    gui.playbutton = parser.get<string > ("menu.playbutton");
    gui.vertline = parser.get<string > ("menu.vertline");
    gui.logo = parser.get<string > ("menu.logo");
    gui.version = parser.get<string > ("menu.version");
}

void Settings::readVideo()
{
    pt::ptree parser;

    try
    {
        pt::read_ini("video.ini", parser);
    }
    catch(...)
    {
        parser.put("window.bits", 32);
        parser.put("window.antialiasing", 2);

        parser.put("window.fullscreen", true);
        parser.put("window.size", Vector2i(1024, 768));

        parser.put("ppe.use", true);
        parser.put("ppe.size", Vector2i(512));
    }

    video.bits = parser.get<int>("window.bits");
    video.antialiasing = parser.get<int>("window.antialiasing");

    video.fullScreen = parser.get<bool>("window.fullscreen");
    video.screenSize = parser.get<Vector2i > ("window.size");

    video.ppeUse = parser.get<bool>("ppe.use");
    video.ppeSize = parser.get<Vector2i > ("ppe.size");
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

void Settings::readWorld()
{
    pt::ptree parser;

    pt::read_ini("world.ini", parser);

    world.gravity = parser.get<float>("general.gravity");

    world.cameraBack = parser.get<float>("camera.back");
    world.cameraUp = parser.get<float>("camera.up");

    world.playerMoveSpeed = parser.get<float>("player.movespeed");
    world.playerJumpForce = parser.get<float>("player.jumpforce");
    world.playerSize = parser.get<float>("player.size");
    world.playerMasse = parser.get<float>("player.masse");
    world.playerLinearDamping = parser.get<float>("player.lineardamping");
    world.playerAngularDamping = parser.get<Vector3f > ("player.angulardamping");

    world.weaponSize = parser.get<float>("weapon.size");
    world.weaponMasse = parser.get<float>("weapon.masse");
}

void Settings::readPlayerInfo()
{
    using namespace boost::filesystem;

    ifstream namefile("NAMES.txt");

    do botNames.push_back(string());
    while(getline(namefile, botNames.back()));

    botNames.pop_back();

    namefile.close();

    availablePlayer.clear();

    directory_iterator end;
    for(directory_iterator it(paths.get<string > ("dirs.players")); it != end; it++)
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

        party.curLevel = campaign.maps.size();

        campaign.maps.push_back(party);
    }
}

void Settings::readMapInfo()
{
    using namespace boost::filesystem;

    availableMap.clear();

    directory_iterator end;
    for(directory_iterator it(paths.get<string > ("dirs.maps")); it != end; it++)
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
    boost::property_tree::read_ini("paths.ini", paths);

    readGui();
    // readCampaign();
    readProfiles();
    readVideo();
    readControl();
    readWorld();

    readMapInfo();
    readPlayerInfo();
}

void Settings::saveVideo()
{
    pt::ptree parser;

    parser.put("window.size", video.screenSize);
    parser.put("window.bits", video.bits);

    parser.put("window.fullscreen", video.fullScreen);
    parser.put("window.antialiasing", video.antialiasing);

    parser.put("ppe.use", video.ppeUse);
    parser.put("ppe.size", video.ppeSize);

    pt::write_ini("video.ini", parser);
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

    video.ppeUse = guiManager->getControl<SwitchString > ("usePpe")->getData().getValue<bool>();
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

    cout << "Load Map file: " << filename << endl;

    author = parser.getAuthorName();
    name = parser.getSceneName();

    script = parser.getAdditionalString("script");
    screen = parser.getAdditionalString("screenshot");
    comment = parser.getAdditionalString("comment");

    if(!script.empty())
        script = tools::pathScope(filename, script, true);

    if(!screen.empty())
        screen = tools::pathScope(filename, screen, true);

    if(author.empty())
        author = "<Inconnue>";

    if(name.empty())
        name = "<Inconnue>";

    if(comment.empty())
        comment = "<Pas de description>";
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
    curLevel = 0;
}

