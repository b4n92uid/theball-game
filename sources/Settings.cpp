/*
 * File:   Settings.cpp
 * Author: b4n92uid
 *
 * Created on 16 avril 2010, 19:06
 */

#include "Settings.h"
#include "AppManager.h"
#include "Weapon.h"

#include <fstream>
#include <iostream>

#include <boost/property_tree/ini_parser.hpp>

using namespace std;
using namespace tbe;
using namespace boost;

Settings::Settings()
{
    noaudio = false;
}

Settings::~Settings() { }

const char* Settings::operator()(std::string key) const
{
    return paths.get<std::string > (key).c_str();
}

void Settings::readVideo()
{
    property_tree::ptree parser;

    try
    {
        property_tree::read_ini("config/video.ini", parser);
    }
    catch(...)
    {
        parser.put("window.size", Vector2i(1024, 768));
        parser.put("window.bits", 32);
        parser.put("window.fullscreen", true);
        parser.put("window.antialiasing", 2);

        parser.put("shader.enable", true);
        parser.put("shader.ppeUse", true);
        parser.put("shader.ppeSize", Vector2i(512));
    }

    video.bits = parser.get<int>("window.bits");
    video.antialiasing = parser.get<int>("window.antialiasing");

    video.fullScreen = parser.get<bool>("window.fullscreen");
    video.screenSize = parser.get<Vector2i > ("window.size");

    video.shaderUse = parser.get<bool>("shader.enable");
    video.ppeUse = parser.get<bool>("shader.ppeUse");
    video.ppeSize = parser.get<Vector2i > ("shader.ppeSize");
}

void Settings::readControl()
{
    control.keyboard.clear();
    control.mouse.clear();

    property_tree::ptree parser;

    property_tree::read_ini("config/control.ini", parser);

    foreach(property_tree::ptree::value_type &i, parser.get_child("keyboard"))
    {
        control.keyboard[i.first] = i.second.get_value<int>();
    }

    foreach(property_tree::ptree::value_type &i, parser.get_child("mouse"))
    {
        control.mouse[i.first] = i.second.get_value<int>();
    }
}

void Settings::readWorld()
{
    property_tree::read_ini("config/weapons.ini", weapons);

    property_tree::ptree parser;
    property_tree::read_ini("config/world.ini", parser);

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

void Settings::readSetting()
{
    boost::property_tree::read_ini("config/paths.ini", paths);
        
    readVideo();
    readControl();
    readWorld();
}

void Settings::saveVideo()
{
    property_tree::ptree parser;

    parser.put("window.size", video.screenSize);
    parser.put("window.bits", video.bits);
    parser.put("window.fullscreen", video.fullScreen);
    parser.put("window.antialiasing", video.antialiasing);

    parser.put("shader.enable", video.shaderUse);
    parser.put("shader.ppeUse", video.ppeUse);
    parser.put("shader.ppeSize", video.ppeSize);

    property_tree::write_ini("config/video.ini", parser);
}

void Settings::saveControl()
{
    property_tree::ptree parser;

    foreach(Control::InputMap::value_type &i, control.keyboard)
    {
        parser.put("keyboard." + i.first, i.second);
    }

    foreach(Control::InputMap::value_type &i, control.mouse)
    {
        parser.put("mouse." + i.first, i.second);
    }

    property_tree::write_ini("config/control.ini", parser);
}

void Settings::saveSetting()
{
    saveVideo();
    saveControl();
}

Content::Content(AppManager* appmng)
{
    m_appManager = appmng;
}

Content::~Content()
{
    BOOST_FOREACH(PlayerInfo* pi, availablePlayer) delete pi;
    BOOST_FOREACH(MapInfo* mi, availableMap) delete mi;

    delete mainPlayer;
}

void Content::readPlayerInfo(std::string dir)
{
    using namespace boost::filesystem;

    availablePlayer.clear();

    directory_iterator end;
    for(directory_iterator it(dir); it != end; it++)
    {
        const path& filename = it->path();

        if(filename.extension() == ".class")
        {
            PlayerInfo* pi = new PlayerInfo(m_appManager, filename.string());
            availablePlayer.push_back(pi);
        }
    }

    mainPlayer = new PlayerInfo(m_appManager, dir + "/eve.class");
}

void Content::readMapInfo(std::string dir)
{
    using namespace boost::filesystem;

    availableMap.clear();

    directory_iterator end;
    for(directory_iterator it(dir); it != end; it++)
    {
        const path& filename = it->path();

        if(filename.extension() == ".map")
        {
            MapInfo* mi = new MapInfo(m_appManager, filename.string());
            availableMap.push_back(mi);
        }
    }
}

Content::MapInfo::MapInfo() { }

Content::MapInfo::MapInfo(AppManager* appmng, std::string path)
{
    cout << "Load Map file: " << path << endl;

    scene::SceneParser* loader = appmng->getSceneParser();
    loader->load(path);

    this->author = loader->getAuthorName();
    this->name = loader->getSceneName();

    const rtree& attributes = loader->attributes();
    this->script = attributes.get<string>("script", "");
    this->preview = attributes.get<string>("preview", "");
    this->comment = attributes.get<string>("comment", "");

    /*
    if(!script.empty())
        script = tools::pathScope(path, script, true);

    if(!preview.empty())
        preview = tools::pathScope(path, preview, true);
     */

    if(author.empty())
        author = "<Inconnue>";

    if(name.empty())
        name = "<Inconnue>";

    if(comment.empty())
        comment = "<Pas de description>";

    this->filepath = path;
}

Content::MapInfo::~MapInfo() { }

Content::PlayerInfo::PlayerInfo() { }

Content::PlayerInfo::PlayerInfo(AppManager* appmng, std::string path)
{
    scene::ClassParser* loader = appmng->getClassParser();
    loader->load(path);

    this->name = loader->getClassname();

    this->filepath = path;
}

Content::PlayerInfo::~PlayerInfo() { }

Content::PartySetting::PartySetting()
{
    curLevel = 0;
    nickname = "Player";
}
