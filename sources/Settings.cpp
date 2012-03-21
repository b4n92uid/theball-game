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

#include <boost/property_tree/ini_parser.hpp>

using namespace std;
using namespace tbe;
using namespace gui;
using namespace boost;

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
    property_tree::ptree parser;

    property_tree::read_ini("gui.ini", parser);

    property_tree::read_ini("weapons.ini", weapons);

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
    gui.nopreview = parser.get<string > ("menu.nopreview");
}

void Settings::readVideo()
{
    property_tree::ptree parser;

    try
    {
        property_tree::read_ini("video.ini", parser);
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

    property_tree::ptree parser;

    property_tree::read_ini("control.ini", parser);

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
    property_tree::ptree parser;

    property_tree::read_ini("world.ini", parser);

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
    boost::property_tree::read_ini("paths.ini", paths);

    readGui();
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

    parser.put("ppe.use", video.ppeUse);
    parser.put("ppe.size", video.ppeSize);

    property_tree::write_ini("video.ini", parser);
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

    property_tree::write_ini("control.ini", parser);
}

void Settings::saveSetting()
{
    saveVideo();
    saveControl();
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

    ifstream namefile("NAMES.txt");

    do botNames.push_back(string());
    while(getline(namefile, botNames.back()));

    botNames.pop_back();

    namefile.close();

    availablePlayer.clear();

    directory_iterator end;
    for(directory_iterator it(dir + "/pnj"); it != end; it++)
    {
        const path& filename = it->path();

        if(filename.extension() == ".class")
        {
            PlayerInfo* pi = new PlayerInfo(m_appManager, filename.file_string());
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
            MapInfo* mi = new MapInfo(m_appManager, filename.file_string());
            availableMap.push_back(mi);
        }
    }
}

Content::MapInfo::MapInfo()
{
}

Content::MapInfo::MapInfo(AppManager* appmng, std::string path)
{
    cout << "Load Map file: " << path << endl;

    scene::SceneParser* loader = appmng->getSceneParser();
    loader->load(path);

    this->author = loader->getAuthorName();
    this->name = loader->getSceneName();

    this->script = loader->getAdditionalString("script");
    this->screen = loader->getAdditionalString("screenshot");
    this->comment = loader->getAdditionalString("comment");

    if(!script.empty())
        script = tools::pathScope(path, script, true);

    if(!screen.empty())
        screen = tools::pathScope(path, screen, true);

    if(author.empty())
        author = "<Inconnue>";

    if(name.empty())
        name = "<Inconnue>";

    if(comment.empty())
        comment = "<Pas de description>";

    this->filepath = path;
}

Content::MapInfo::~MapInfo()
{
}

Content::PlayerInfo::PlayerInfo()
{
}

Content::PlayerInfo::PlayerInfo(AppManager* appmng, std::string path)
{
    scene::ClassParser* loader = appmng->getClassParser();
    loader->load(path);

    this->name = loader->getClassname();

    this->filepath = path;
}

Content::PlayerInfo::~PlayerInfo()
{
}

Content::PartySetting::PartySetting()
{
    curLevel = 0;
    nickname = "Player";
}
