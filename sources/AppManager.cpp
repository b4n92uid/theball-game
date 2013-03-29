/*
 * File:   AppManager.cpp
 * Author: b4n92uid
 *
 * Created on 26 novemebre 2009, 18:07
 */

#include "AppManager.h"

#include "GameManager.h"
#include <fmod_errors.h>

#define guiRootPath(path) (string("../../") + path).c_str()

using namespace std;
using namespace tbe;

AppManager::AppManager()
{
    m_gameEngine = new SDLDevice;

    globalSettings.readSetting();

    setupVideoMode();
    setupInernalState();
    setupMenuGui();
    setupSound();
    setupBackgroundScene();
}

AppManager::~AppManager()
{
    globalSettings.saveSetting();

    if(!globalSettings.noaudio)
        FMOD_System_Release(m_fmodsys);

    delete globalContent;

    delete m_sceneParser;
    delete m_classParser;

    delete m_guiManager;
    delete m_gameEngine;
}

void AppManager::setupVideoMode()
{
    if(!globalSettings.video.shaderUse)
    {
        Shader::forceHardware(false);
        globalSettings.video.ppeUse = false;
    }

    Settings::Video apply = globalSettings.video;

    if(globalSettings.video.ppeUse)
        apply.antialiasing = 0;

    m_gameEngine->window(CAPTION_TITLE, apply.screenSize, apply.bits, apply.fullScreen, apply.antialiasing);
}

void AppManager::setupInernalState()
{
    m_eventMng = m_gameEngine->getEventManager();
    m_sceneManager = m_gameEngine->getSceneManager();
    m_fpsMng = m_gameEngine->getFpsManager();
    m_ppeManager = m_gameEngine->getPostProcessManager();

    m_guiManager = new gui::RocketGuiManager;
    m_guiManager->setup(globalSettings.video.screenSize);
    m_guiManager->loadFonts(globalSettings.paths.get<string > ("dirs.gui"));
    m_guiManager->addPath(globalSettings.paths.get<string > ("dirs.gui"));
    m_guiManager->addPath(globalSettings.paths.get<string > ("dirs.maps"));

    m_sceneParser = new scene::SceneParser(m_sceneManager);
    m_classParser = new scene::ClassParser(m_sceneManager);

    m_profile.mapSelection = 0;
    m_profile.nickname = "Joueur";

    #ifdef COMPILE_FOR_WINDOWS
    DWORD bufSize = 255;
    char userName[bufSize];
    GetUserName(userName, &bufSize);
    m_profile.nickname = userName;
    #endif

    globalContent = new Content(this);
    globalContent->readPlayerInfo(globalSettings.paths.get<string > ("dirs.players"));
    globalContent->readMapInfo(globalSettings.paths.get<string > ("dirs.maps"));

    if(globalSettings.video.ppeUse
       && globalSettings.video.antialiasing
       && FrameBufferObject::checkHardware())
    {
        Rtt* ppeRtt = m_ppeManager->getRtt();
        ppeRtt->FBO_SetUseRenderBuffer(true);
        ppeRtt->FBO_SetMultisamplesCount(globalSettings.video.antialiasing);
    }

    m_fpsMng->setRunFps(60);
}

void AppManager::setupSound()
{
    m_fmodsys = NULL;

    if(!globalSettings.noaudio)
    {
        FMOD_System_Create(&m_fmodsys);
        FMOD_System_SetOutput(m_fmodsys, FMOD_OUTPUTTYPE_DSOUND);
        FMOD_System_Init(m_fmodsys, 100, FMOD_INIT_NORMAL, 0);

        FMOD_RESULT res = FMOD_System_CreateStream(m_fmodsys, globalSettings("audio.maintheme"),
                                                   FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE,
                                                   0, &m_mainMusic);

        if(res != FMOD_OK)
            throw tbe::Exception("AppManager::AppManager; %s (%s)",
                                 FMOD_ErrorString(res),
                                 globalSettings("audio.maintheme"));
    }
}

typedef void (AppManager::*ActionMethod)(Rocket::Core::Event& e);

class EventListner :
public Rocket::Core::EventListener,
public std::map<Rocket::Core::String, ActionMethod>
{
public:

    EventListner(AppManager* appm) : m_gamem(appm)
    {
    }

    void ProcessEvent(Rocket::Core::Event& e)
    {
        if(!this->count(e.GetTargetElement()->GetId()))
            return;

        ActionMethod f = this->at(e.GetTargetElement()->GetId());
        (m_gamem->*f)(e);

        e.StopPropagation();
    }

private:
    AppManager* m_gamem;
};

void AppManager::setupMenuGui()
{
    using namespace tbe::gui;

    using namespace Rocket::Core;

    const Settings& gs = globalSettings;

    m_guiManager->clearAll();

    Rocket::Core::Context* context = m_guiManager->getContext();

    #define guidir(file) (gs.paths.get<string > ("dirs.gui") + "/" + file).c_str()

    m_menu.mainmenu = context->LoadDocument(guidir("mainmenu.rml"));
    m_menu.playmenu = context->LoadDocument(guidir("playmenu.rml"));
    m_menu.setsmenu = context->LoadDocument(guidir("settings.rml"));
    m_menu.keysmenu = context->LoadDocument(guidir("keybind.rml"));
    m_menu.aboutmenu = context->LoadDocument(guidir("about.rml"));
    m_menu.loadscreen = context->LoadDocument(guidir("loadscreen.rml"));

    #undef guidir

    // Remplisage --------------------------------------------------------------

    updateMapSelection();

    using namespace Rocket::Controls;

    #define getSetsSelect(id) dynamic_cast<ElementFormControlSelect*>(m_menu.setsmenu->GetElementById(id))
    #define getKeySelect(id) dynamic_cast<ElementFormControlSelect*>(m_menu.keysmenu->GetElementById(id))

    // Menu de configuration des touches

    #if 0
    vector<string> actions;

    actions.push_back("forward");
    actions.push_back("backward");
    actions.push_back("strafright");
    actions.push_back("strafleft");
    actions.push_back("jump");
    actions.push_back("shoot");
    actions.push_back("power");
    actions.push_back("next");
    actions.push_back("prev");

    BOOST_FOREACH(string act, actions)
    {
        ElementFormControlSelect* select = getKeySelect(act.c_str());

        int printchar[] = {13, 8, 32, 9, 273, 274, 275, 276, 277, 127, 278, 279,
            280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293,
            294, 295, 296, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313};

        vector<int> chars(printchar, printchar + sizeof(printchar));

        for(int c = 0x20; c <= 0x7e; c++)
            chars.push_back(c);

        BOOST_FOREACH(int c, chars)
        {
            String value;
            value.FormatString(16, "key:%d", c);

            select->Add(SDLDevice::getKeyName(c).c_str(), value);
        }

        for(int m = 0; m < 5; m++)
        {
            String value;
            value.FormatString(16, "mouse:%d", m);

            select->Add(SDLDevice::getMouseName(m).c_str(), value);
        }
    }
    #endif

    BOOST_FOREACH(Settings::Control::InputMap::value_type itt, globalSettings.control.keyboard)
    {
        String value;
        value.FormatString(8, "key:%d", itt.second);

        getKeySelect(itt.first.c_str())
                ->Add(SDLDevice::getKeyName(itt.second).c_str(), value, 0, true);
    }

    BOOST_FOREACH(Settings::Control::InputMap::value_type itt, globalSettings.control.mouse)
    {
        String value;
        value.FormatString(8, "mouse:%d", itt.second);

        getKeySelect(itt.first.c_str())
                ->Add(SDLDevice::getKeyName(itt.second).c_str(), value, 0, true);
    }

    // Menu de configuration vidéo

    getSetsSelect("usePpe")->SetSelection(globalSettings.video.ppeUse);
    getSetsSelect("fullScreen")->SetSelection(globalSettings.video.fullScreen);
    getSetsSelect("antiAliasing")->SetSelection(globalSettings.video.antialiasing / 2);

    // ----

    ElementFormControlSelect* screenSize = getSetsSelect("screenSize");

    vector<tbe::Vector2i> availableScreen = SDLDevice::getAvilableSceeenSize();
    unsigned curSizeIndex = 0;

    for(unsigned c = 0; c < availableScreen.size(); c++)
    {
        if(globalSettings.video.screenSize == availableScreen[c])
            curSizeIndex = c;

        stringstream ss;
        ss << availableScreen[c].x << "x" << availableScreen[c].y << "x32";

        screenSize->Add(ss.str().c_str(), availableScreen[c].toStr('x').c_str());
    }

    screenSize->SetSelection(curSizeIndex);

    #undef getSetsSelect
    #undef getKeySelect
}

void AppManager::setupBackgroundScene()
{
    using namespace tbe::scene;

    // SCENE -------------------------------------------------------------------

    m_sceneManager->clearAll();

    m_camera = new Camera(Camera::TARGET_RELATIVE);
    m_camera->setRotate(Vector2f(180, 5));
    m_sceneManager->addCamera(m_camera);

    SceneParser parser(m_sceneManager);
    parser.load(globalSettings("scenes.mainmenu"));
    parser.build();

    // PPE ---------------------------------------------------------------------

    if(globalSettings.video.ppeUse)
    {
        using namespace ppe;

        m_ppeManager->clearAll();

        #if 0
        BlurEffect* blur = new BlurEffect;
        blur->setPasse(4);
        m_ppeManager->addPostEffect("", blur);

        BloomEffect* bloom = new BloomEffect;
        bloom->setThreshold(0.1);
        bloom->setIntensity(1.0);
        bloom->setBlurPass(10);
        m_ppeManager->addPostEffect("", bloom);

        MotionBlurEffect* mblur = new MotionBlurEffect;
        mblur->setRttFrameSize(math::nextPow2(globalSettings.video.screenSize) / 2);
        m_ppeManager->addPostEffect("", mblur);
        #endif
    }
}

void AppManager::executeMenu()
{
    using namespace tbe::gui;
    using namespace tbe::scene;

    if(!globalSettings.noaudio)
        FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_mainMusic, false, &m_mainMusicCh);

    m_runingMenu = true;

    EventListner mainmenu_elister(this);
    mainmenu_elister["play"] = &AppManager::onMainMenuPlay;
    mainmenu_elister["settings"] = &AppManager::onMainMenuSettings;
    mainmenu_elister["about"] = &AppManager::onMainMenuAbout;
    mainmenu_elister["quit"] = &AppManager::onMainMenuQuit;
    m_menu.mainmenu->AddEventListener("click", &mainmenu_elister);

    EventListner play_elister(this);
    play_elister["start"] = &AppManager::onPlayMenuStart;
    play_elister["return"] = &AppManager::onPlayMenuReturn;
    play_elister["prevmap"] = &AppManager::onPlayMenuPrev;
    play_elister["nextmap"] = &AppManager::onPlayMenuNext;
    m_menu.playmenu->AddEventListener("click", &play_elister);

    EventListner about_elister(this);
    about_elister["return"] = &AppManager::onAboutMenuReturn;
    m_menu.aboutmenu->AddEventListener("click", &about_elister);

    EventListner settings_elister(this);
    settings_elister["keybind"] = &AppManager::onSettingsMenuKeyBind;
    settings_elister["apply"] = &AppManager::onSettingsMenuApply;
    settings_elister["return"] = &AppManager::onSettingsMenuReturn;
    m_menu.setsmenu->AddEventListener("click", &settings_elister);
    m_menu.setsmenu->AddEventListener("submit", &settings_elister);

    EventListner keybind_elister(this);
    keybind_elister["return"] = &AppManager::onKeysMenuReturn;
    keybind_elister["apply"] = &AppManager::onKeysMenuApply;
    m_menu.keysmenu->AddEventListener("click", &keybind_elister);
    m_menu.keysmenu->AddEventListener("submit", &keybind_elister);

    m_menu.mainmenu->Show();

    while(m_runingMenu)
    {
        if(m_fpsMng->doRender())
        {
            m_gameEngine->pollEvent();
            m_guiManager->trasmitEvent(*m_eventMng);

            if(m_eventMng->notify == EventManager::EVENT_WIN_QUIT)
                m_runingMenu = false;

            m_camera->setPos(-m_camera->getTarget() * 8.0f);
            m_camera->rotate(Vector2f(1, 0));

            m_gameEngine->beginScene();

            if(globalSettings.video.ppeUse)
            {
                Rtt* ppeRtt = m_ppeManager->getRtt();
                ppeRtt->use(true);
                ppeRtt->clear();
                m_sceneManager->render();
                ppeRtt->use(false);
                m_ppeManager->render();
            }

            else
            {
                m_sceneManager->render();
            }

            m_guiManager->render();
            m_gameEngine->endScene();
        }

        m_fpsMng->update();
    }

    m_menu.mainmenu->RemoveEventListener("click", &mainmenu_elister);
    m_menu.playmenu->RemoveEventListener("click", &play_elister);
    m_menu.aboutmenu->RemoveEventListener("click", &about_elister);
    m_menu.setsmenu->RemoveEventListener("click", &settings_elister);
    m_menu.setsmenu->RemoveEventListener("submit", &settings_elister);
    m_menu.keysmenu->RemoveEventListener("click", &keybind_elister);
    m_menu.keysmenu->RemoveEventListener("submit", &keybind_elister);
}

void AppManager::executeGame(const Content::PartySetting& playSetting)
{
    cout << "> Execute game " << playSetting.map->filepath << ", " << playSetting.player->filepath << endl;

    m_sceneManager->clearAll();
    m_ppeManager->clearAll();

    if(!globalSettings.noaudio)
        FMOD_Channel_Stop(m_mainMusicCh);

    // Affichage de l'ecran de chargement --------------------------------------

    m_menu.loadscreen->GetElementById("title")->SetInnerRML(playSetting.map->name.c_str());

    if(!playSetting.map->preview.empty())
        m_menu.loadscreen->GetElementById("preview")->SetAttribute("src", guiRootPath(playSetting.map->preview));
    else
        m_menu.loadscreen->GetElementById("preview")->SetAttribute("src", guiRootPath("data/menu/nopreview.png"));

    m_menu.loadscreen->Show();

    m_gameEngine->beginScene();
    m_guiManager->render();
    m_gameEngine->endScene();

    // Chargement de la carte --------------------------------------------------

    GameManager* gameManager = NULL;

    try
    {
        gameManager = new GameManager(this);

        m_sceneParser->setMeshScene(gameManager->parallelscene.meshs);
        m_sceneParser->setParticlesScene(gameManager->parallelscene.particles);
        m_sceneParser->setLightScene(gameManager->parallelscene.light);
        m_sceneParser->setMarkScene(gameManager->parallelscene.marks);
        m_classParser->setMeshScene(gameManager->parallelscene.meshs);
        m_classParser->setParticlesScene(gameManager->parallelscene.particles);
        m_classParser->setLightScene(gameManager->parallelscene.light);
        m_classParser->setMarkScene(gameManager->parallelscene.marks);

        gameManager->setupGui();
        gameManager->setupMap(playSetting);
    }

    catch(std::exception& e)
    {
        cout << e.what() << endl;

        m_menu.loadscreen->Hide();
        delete gameManager;
        return;
    }

    // Attente de réponse ------------------------------------------------------

    m_menu.loadscreen->GetElementById("loading-text")
            ->SetInnerRML("Appuyer sur \"Espace\" pour continuer...");

    m_eventMng->keyState[EventManager::KEY_SPACE] = false;

    while(!m_eventMng->keyState[EventManager::KEY_SPACE])
    {
        m_gameEngine->pollEvent();
        m_gameEngine->beginScene();
        m_guiManager->render();
        m_gameEngine->endScene();
    }

    m_menu.loadscreen->Hide();

    // Début du jeu ------------------------------------------------------------

    cout << "Start game" << endl;

    gameManager->startGameProcess();

    while(gameManager->isRunning())
    {
        gameManager->eventProcess();
        gameManager->gameProcess();
        gameManager->hudProcess();

        gameManager->render();

        m_fpsMng->update();
    }

    cout << "End game" << endl;

    delete gameManager;

    if(!globalSettings.noaudio)
        FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_mainMusic, false, &m_mainMusicCh);
}

void AppManager::onMainMenuPlay(Rocket::Core::Event& e)
{
    m_menu.mainmenu->Hide();
    m_menu.playmenu->Show();
}

void AppManager::onMainMenuSettings(Rocket::Core::Event& e)
{
    m_menu.mainmenu->Hide();
    m_menu.setsmenu->Show();
}

void AppManager::onMainMenuAbout(Rocket::Core::Event& e)
{
    m_menu.mainmenu->Hide();
    m_menu.aboutmenu->Show();
}

void AppManager::onMainMenuQuit(Rocket::Core::Event& e)
{
    m_runingMenu = false;
}

void AppManager::onPlayMenuStart(Rocket::Core::Event& e)
{
    Content::PartySetting playSetting;
    playSetting.map = globalContent->availableMap[m_profile.mapSelection];
    playSetting.player = globalContent->mainPlayer;
    playSetting.nickname = m_profile.nickname;

    m_menu.playmenu->Hide();

    executeGame(playSetting);

    m_menu.playmenu->Show();

    setupBackgroundScene();
}

void AppManager::onPlayMenuReturn(Rocket::Core::Event& e)
{
    m_menu.playmenu->Hide();
    m_menu.mainmenu->Show();
}

void AppManager::updateMapSelection()
{
    Content::MapInfo* map = globalContent->availableMap[m_profile.mapSelection];

    Rocket::Core::Element* mapTitle = m_menu.playmenu->GetElementById("title");
    mapTitle->SetInnerRML(map->name.c_str());

    Rocket::Core::Element* mapPreview = m_menu.playmenu->GetElementById("preview");

    if(!map->preview.empty())
        mapPreview->SetAttribute("src", guiRootPath(map->preview));
    else
        mapPreview->SetAttribute("src", guiRootPath("data/menu/nopreview.png"));
}

void AppManager::onPlayMenuNext(Rocket::Core::Event& e)
{
    if(m_profile.mapSelection == globalContent->availableMap.size() - 1)
        return;

    m_profile.mapSelection++;

    updateMapSelection();
}

void AppManager::onPlayMenuPrev(Rocket::Core::Event& e)
{
    if(m_profile.mapSelection == 0)
        return;

    m_profile.mapSelection--;

    updateMapSelection();
}

void AppManager::onSettingsMenuApply(Rocket::Core::Event& e)
{
    globalSettings.video.ppeUse = e.GetParameter<bool>("usePpe", false);
    globalSettings.video.fullScreen = e.GetParameter<bool>("fullScreen", false);
    globalSettings.video.antialiasing = e.GetParameter<unsigned >("antiAliasing", 0);
    globalSettings.video.screenSize.fromStr(e.GetParameter<string > ("screenSize", "800x600"));

    globalSettings.saveVideo();

    m_menu.setsmenu->Hide();
    m_menu.mainmenu->Show();
}

void AppManager::onSettingsMenuReturn(Rocket::Core::Event& e)
{
    m_menu.setsmenu->Hide();
    m_menu.mainmenu->Show();
}

void AppManager::onSettingsMenuKeyBind(Rocket::Core::Event& e)
{
    m_menu.setsmenu->Hide();
    m_menu.keysmenu->Show();
}

void AppManager::onKeysMenuApply(Rocket::Core::Event& e)
{
    globalSettings.control.keyboard.clear();
    globalSettings.control.mouse.clear();

    vector<string> actions;

    actions.push_back("forward");
    actions.push_back("backward");
    actions.push_back("strafright");
    actions.push_back("strafleft");
    actions.push_back("jump");
    actions.push_back("shoot");
    actions.push_back("power");
    actions.push_back("next");
    actions.push_back("prev");

    for(unsigned i = 0; i < actions.size(); i++)
    {
        string code = e.GetParameter<string > (actions[i].c_str(), "");

        vector<string> param = tools::tokenize(code, ':');

        if(param[0] == "key")
            globalSettings.control.keyboard[actions[i]] = tools::strToNum<int>(param[1]);

        if(param[0] == "mouse")
            globalSettings.control.mouse[actions[i]] = tools::strToNum<int>(param[1]);
    }

    globalSettings.saveControl();

    m_menu.keysmenu->Hide();
    m_menu.setsmenu->Show();
}

void AppManager::onKeysMenuReturn(Rocket::Core::Event& e)
{
    m_menu.keysmenu->Hide();
    m_menu.setsmenu->Show();
}

void AppManager::onAboutMenuReturn(Rocket::Core::Event& e)
{
    m_menu.aboutmenu->Hide();
    m_menu.mainmenu->Show();
}

tbe::EventManager* AppManager::getEventMng() const
{
    return m_eventMng;
}

tbe::ticks::FpsManager* AppManager::getFpsMng() const
{
    return m_fpsMng;
}

tbe::scene::SceneManager* AppManager::getSceneMng() const
{
    return m_sceneManager;
}

tbe::gui::RocketGuiManager* AppManager::getGuiMng() const
{
    return m_guiManager;
}

tbe::SDLDevice* AppManager::getGameEngine() const
{
    return m_gameEngine;
}

FMOD_SYSTEM* AppManager::getFmodSystem() const
{
    return m_fmodsys;
}

tbe::scene::ClassParser* AppManager::getClassParser() const
{
    return m_classParser;
}

tbe::scene::SceneParser* AppManager::getSceneParser() const
{
    return m_sceneParser;
}
