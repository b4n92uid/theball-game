#include "AppManager.h"

#include "GameManager.h"

#include <fmod_errors.h>

using namespace std;
using namespace tbe;

AppManager::AppManager()
{
    m_gameEngine = new SDLDevice;

    globalSettings.readSetting();

    setupVideoMode();
    setupSound();
}

AppManager::~AppManager()
{
    globalSettings.saveSetting();

    if(!globalSettings.noaudio)
        FMOD_System_Release(m_fmodsys);

    delete m_gameEngine;
}

void AppManager::setupVideoMode()
{
    m_gameEngine->window(CAPTION_TITLE,
                         globalSettings.video.screenSize,
                         globalSettings.video.bits,
                         globalSettings.video.fullScreen,
                         globalSettings.video.usePpe ? 0 : globalSettings.video.antialiasing);

    if(!Shader::checkHardware() && globalSettings.video.usePpe)
    {
        globalSettings.video.usePpe = false;

        m_gameEngine->window(CAPTION_TITLE,
                             globalSettings.video.screenSize,
                             globalSettings.video.bits,
                             globalSettings.video.fullScreen,
                             globalSettings.video.antialiasing);
    }

    m_eventMng = m_gameEngine->getEventManager();
    m_guiManager = m_gameEngine->getGuiManager();
    m_sceneManager = m_gameEngine->getSceneManager();
    m_fpsMng = m_gameEngine->getFpsManager();
    m_ppeManager = m_gameEngine->getPostProcessManager();


    if(globalSettings.video.usePpe
       && globalSettings.video.antialiasing
       && FrameBufferObject::checkHardware())
    {
        Rtt* ppeRtt = m_ppeManager->getRtt();
        ppeRtt->FBO_SetUseRenderBuffer(true);
        ppeRtt->FBO_SetMultisamplesCount(globalSettings.video.antialiasing);
    }

    m_fpsMng->setRunFps(60);

    Texture::resetCache();

    setupMenuGui();

    setupBackgroundScene();
}

void AppManager::setupSound()
{
    m_fmodsys = NULL;

    if(!globalSettings.noaudio)
    {
        FMOD_System_Create(&m_fmodsys);
        FMOD_System_SetOutput(m_fmodsys, FMOD_OUTPUTTYPE_DSOUND);
        FMOD_System_Init(m_fmodsys, 100, FMOD_INIT_NORMAL, 0);

        FMOD_RESULT res = FMOD_System_CreateStream(m_fmodsys, SOUND_GONG,
                                                   FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE,
                                                   0, &m_gongSound);

        if(res != FMOD_OK)
            throw tbe::Exception("AppManager::AppManager; %s (%s)",
                                 FMOD_ErrorString(res),
                                 SOUND_MAINTHEME);

        res = FMOD_System_CreateStream(m_fmodsys, SOUND_MAINTHEME,
                                       FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE,
                                       0, &m_mainMusic);

        if(res != FMOD_OK)
            throw tbe::Exception("AppManager::AppManager; %s (%s)",
                                 FMOD_ErrorString(res),
                                 SOUND_MAINTHEME);
    }
}

void AppManager::setupMenuGui()
{
    using namespace tbe::gui;

    m_guiManager->clearAll();

    const Vector2i& screenSize = globalSettings.video.screenSize;

    GuiSkin* guiskin = new GuiSkin;

    guiskin->button(globalSettings.gui.button);
    guiskin->buttonSize(globalSettings.gui.buttonSize);

    guiskin->gauge(globalSettings.gui.gauge);
    guiskin->gaugeSize(globalSettings.gui.gaugeSize);

    guiskin->editBox(globalSettings.gui.editBox);
    guiskin->editBoxSize(globalSettings.gui.editBoxSize);

    guiskin->switchBox(globalSettings.gui.switchBox);
    guiskin->switchBoxSize(globalSettings.gui.switchBoxSize);

    //    guiskin->vectorBox(globalSettings.gui.vectorBox);
    //    guiskin->vectorBoxSize(globalSettings.gui.vectorBoxSize);

    guiskin->stateShowSize(Vector2f(48, 48));

    guiskin->pencile(globalSettings.gui.font, globalSettings.gui.fontSize);

    m_guiManager->setSkin(guiskin);

    Pencil bigpen(globalSettings.gui.font, globalSettings.gui.fontSize * 1.5);

    // Construction ------------------------------------------------------------

    Texture background(globalSettings.gui.backgroundMainmenu);

    // Menu Principale

    m_guiManager->setSession(MENU_MAIN);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);

    m_guiManager->addLayout(Layout::Horizental);
    m_guiManager->addLayoutStretchSpace();

    m_guiManager->addLayoutSpace(256);

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->addLayout(Layout::Vertical, 10);
    m_guiManager->addLayoutStretchSpace();

    m_controls.quit = m_guiManager->addButton("quit", "Quitter");
    m_controls.about = m_guiManager->addButton("about", "A Propos");
    m_controls.settings = m_guiManager->addButton("settings", "Options");
    m_controls.quickplay = m_guiManager->addButton("quickplay", "Partie rapide");
    // m_controls.campaignmenu = m_guiManager->addButton("campaign", "Jouer");

    // m_guiManager->addImage("logo", globalSettings.gui.backgroundLogo);

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // Menu Campaign

    m_guiManager->setSession(MENU_CAMPAIGN);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);

    /*
    m_guiManager->addLayout(Layout::Horizental, 10, 10);
    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->addLayout(Layout::Vertical, 10);
    m_guiManager->addLayoutStretchSpace();

    m_controls.campaign.ret = m_guiManager->addButton("return", "Retour");

    m_controls.campaign.levelSelect = m_guiManager->addSwitchString("levelSelect");
    m_guiManager->addTextBox("")->write("Niveau");

    m_controls.campaign.playerName = m_guiManager->addEditBox("playerName", "Joueur");
    m_guiManager->addTextBox("")->write("Pseudo");

    m_controls.campaign.playerSelect = m_guiManager->addSwitchString("playerSelect");
    m_guiManager->addTextBox("")->write("Personnage");

    m_controls.campaign.play = m_guiManager->addButton("play", "Jouer");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // -------- Collone 1

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 2
    m_guiManager->addLayout(Layout::Vertical, 10);
    m_guiManager->addLayoutStretchSpace();

    m_controls.campaign.description = m_guiManager->addTextBox("description");
    m_controls.campaign.description->setBackground(globalSettings.gui.backgroundTextbox);
    m_controls.campaign.description->setBackgroundPadding(16);
    m_controls.campaign.description->setSize(Vector2f(384, 256));
    m_controls.campaign.description->setDefinedSize(true);

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // -------- Collone 2

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
     */

    // ******** Menu Jouer

    m_guiManager->setSession(MENU_QUICKPLAY);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);

    m_guiManager->addLayout(Layout::Horizental, 10, 10);

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->addLayout(Layout::Vertical, 5);
    m_guiManager->addLayoutStretchSpace();

    m_controls.playmenu.ret = m_guiManager->addButton("return", "Retour");

    m_controls.playmenu.playerSelect = m_guiManager->addSwitchString("playerSelect");
    m_guiManager->addTextBox("")->write("Personnage");

    m_controls.playmenu.mapSelect = m_guiManager->addSwitchString("levelSelect");
    m_guiManager->addTextBox("")->write("Carte à jouer");

    m_controls.playmenu.playerName = m_guiManager->addEditBox("nameSelect", "Joueur");
    m_guiManager->addTextBox("")->write("Pseudo");

    m_controls.playmenu.play = m_guiManager->addButton("play", "Jouer");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 2
    m_guiManager->addLayout(Layout::Vertical, 5);
    m_guiManager->addLayoutStretchSpace();

    m_controls.playmenu.description = m_guiManager->addTextBox("description");
    m_controls.playmenu.description->setBackground(globalSettings.gui.backgroundTextbox);
    m_controls.playmenu.description->setBackgroundPadding(16);
    m_controls.playmenu.description->setSize(Vector2f(384, 256));
    m_controls.playmenu.description->setDefinedSize(true);

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // ******** Ecran de chargement

    m_guiManager->setSession(MENU_LOAD);

    m_guiManager->addImage("", globalSettings.gui.backgroundMainmenu)
            ->setSize(screenSize);

    m_guiManager->addLayout(Layout::Horizental, 0, 10);
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->addLayout(Layout::Vertical, 10);
    m_guiManager->addLayoutStretchSpace();

    m_guiManager->addTextBox("load:stateText")
            ->setPencil(bigpen);

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // ******** Menu Option

    m_guiManager->setSession(MENU_SETTING);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);

    m_guiManager->addLayout(Layout::Horizental, 0, 10);
    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->addLayout(Layout::Vertical, 8);
    m_guiManager->addLayoutStretchSpace();

    m_guiManager->addButton("return", "Retour");

    m_guiManager->addButton("apply", "Appliquer");

    m_guiManager->addButton("keySetting", "Conf. Touches");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 2
    m_guiManager->addLayout(Layout::Vertical, 8);
    m_guiManager->addLayoutStretchSpace();

    m_controls.settingsmenu.screenSize = m_guiManager->addSwitchString("screenSize");
    m_guiManager->addTextBox("")->write("Résolution");

    m_controls.settingsmenu.antiAliasing = m_guiManager->addSwitchString("antiAliasing");
    m_guiManager->addTextBox("")->write("Anti-crélénage");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 3
    m_guiManager->addLayout(Layout::Vertical, 8);
    m_guiManager->addLayoutStretchSpace();

    m_controls.settingsmenu.fullscreen = m_guiManager->addSwitchString("fullScreen");
    m_guiManager->addTextBox("")->write("Mode d'affichage");

    m_controls.settingsmenu.usePpe = m_guiManager->addSwitchString("usePpe");
    m_guiManager->addTextBox("")->write("P.P Effects");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // ******** Menu Commandes

    m_guiManager->setSession(MENU_SETTING_KEYS);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);

    m_guiManager->addLayout(Layout::Horizental, 0, 10);
    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->addLayout(Layout::Vertical, 2);
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->addKeyConfig("forward");
    m_guiManager->addTextBox("")->write("Avancer");
    m_guiManager->addKeyConfig("backward");
    m_guiManager->addTextBox("")->write("Reculer");
    m_guiManager->addKeyConfig("strafRight");
    m_guiManager->addTextBox("")->write("A gauche");
    m_guiManager->addKeyConfig("strafLeft");
    m_guiManager->addTextBox("")->write("A droite");
    m_guiManager->addKeyConfig("jump");
    m_guiManager->addTextBox("")->write("Sauter");
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 2
    m_guiManager->addLayout(Layout::Vertical, 2);
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->addKeyConfig("boost");
    m_guiManager->addTextBox("")->write("Boost");
    m_guiManager->addKeyConfig("brake");
    m_guiManager->addTextBox("")->write("Brake");
    m_guiManager->addKeyConfig("shoot");
    m_guiManager->addTextBox("")->write("Tirer");
    m_guiManager->addKeyConfig("power");
    m_guiManager->addTextBox("")->write("Pouvoirs");
    m_guiManager->addKeyConfig("switchUpWeapon");
    m_guiManager->addTextBox("")->write("Arme suivante");
    m_guiManager->addKeyConfig("switchDownWeapon");
    m_guiManager->addTextBox("")->write("Arme précédente");
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 3
    m_guiManager->addLayout(Layout::Vertical, 10);
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->addButton("return", "Retour");
    m_guiManager->addButton("apply", "Appliquer");
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // ******** Menu A propos

    m_guiManager->setSession(MENU_ABOUT);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);


    m_guiManager->addLayout(Layout::Horizental);
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->addLayout(Layout::Vertical, 10);
    m_guiManager->addLayoutStretchSpace();

    m_guiManager->addButton("return", "Retour");

    m_controls.aboutmenu.aboutText = m_guiManager->addTextBox("aboutText");
    m_controls.aboutmenu.aboutText->setSize(Vector2f(screenSize) * Vector2f(0.75, 0.5));
    m_controls.aboutmenu.aboutText->setDefinedSize(true);
    m_controls.aboutmenu.aboutText->setBackgroundPadding(8);
    m_controls.aboutmenu.aboutText->setBackground(globalSettings.gui.backgroundTextbox);
    m_controls.aboutmenu.aboutText->setTextAlign(TextBox::LEFT);

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    m_guiManager->setSession(MENU_MAIN);

    // Remplisage --------------------------------------------------------------

    m_controls.playmenu.playerName->setLabel(globalSettings.profile.name);

    // A propos
    {
        stringstream ss;

        fstream file("README.txt");
        ss << file.rdbuf() << endl;
        file.close();

        ss << "Build in " << __DATE__;

        m_controls.aboutmenu.aboutText->write(ss.str());
    }

    // Menu de configuration des touches
    {
        for(map<string, int>::const_iterator itt = globalSettings.control.keyboard.begin();
            itt != globalSettings.control.keyboard.end(); itt++)
            m_guiManager->getControl<gui::KeyConfig > (itt->first, MENU_SETTING_KEYS)
            ->setKeyCode(itt->second, SDLDevice::getKeyName(itt->second));

        for(map<string, int>::const_iterator itt = globalSettings.control.mouse.begin();
            itt != globalSettings.control.mouse.end(); itt++)
            m_guiManager->getControl<gui::KeyConfig > (itt->first, MENU_SETTING_KEYS)
            ->setMouseCode(itt->second, SDLDevice::getMouseName(itt->second));
    }

    // Menu de configuration vidéo
    {
        m_controls.settingsmenu.usePpe->push("Désactiver", false).push("Activer", true);
        m_controls.settingsmenu.usePpe->setCurrent(globalSettings.video.usePpe);

        m_controls.settingsmenu.fullscreen->push("Fenêtrer", false).push("Plein-écran", true);
        m_controls.settingsmenu.fullscreen->setCurrent(globalSettings.video.fullScreen);
    }

    {
        int cutAA = globalSettings.video.antialiasing;

        map<int, int> AAAvailable;
        AAAvailable[0] = 0;
        AAAvailable[2] = 1;
        AAAvailable[4] = 2;
        AAAvailable[8] = 3;
        AAAvailable[16] = 4;

        m_controls.settingsmenu.antiAliasing->
                push("Désactiver", 0).
                push("2x", 2).
                push("4x", 4).
                push("8x", 8).
                push("16x", 16)
                ;

        m_controls.settingsmenu.antiAliasing->setCurrent(AAAvailable[cutAA]);
    }

    {
        vector<Vector2i> availableScreen = SDLDevice::getAvilableSceeenSize();
        unsigned curSizeIndex = 0;

        for(unsigned i = 0; i < availableScreen.size(); i++)
        {
            if(screenSize == availableScreen[i])
                curSizeIndex = i;

            stringstream ss;
            ss << availableScreen[i].x << "x" << availableScreen[i].y << "x32";

            m_controls.settingsmenu.screenSize->push(ss.str(), availableScreen[i]);
        }

        m_controls.settingsmenu.screenSize->setCurrent(curSizeIndex);
    }

    // Menu campaign
    /*
    m_controls.campaign.playerName->setLabel(globalSettings.profile.name);

    for(unsigned i = 0; i < globalSettings.availablePlayer.size(); i++)
        m_controls.campaign.playerSelect->push(globalSettings.availablePlayer[i].name, i);

    m_controls.campaign.playerSelect->setCurrent(math::rand(0, globalSettings.availablePlayer.size()));

     */

    updateGuiContent();
}

void AppManager::updateQuickPlayMapInfo()
{
    unsigned selected = m_controls.playmenu.mapSelect->getCurrent();

    Settings::MapInfo& mapinfo = globalSettings.availableMap[selected];

    m_controls.playmenu.description
            ->write(gui::GuiString("Carte: %s\n"
                                   "Par: %s\n\n"
                                   "%s",
                                   mapinfo.name.c_str(),
                                   mapinfo.author.c_str(),
                                   mapinfo.comment.c_str()));
}

void AppManager::updateGuiContent()
{
    // Campaign

    /*
    m_controls.campaign.levelSelect->deleteAll();

    unsigned campMapSize = min(globalSettings.campaign.maps.size() - 1, globalSettings.profile.index);
    for(unsigned i = 0; i <= campMapSize; i++)
    {
        string label = "Niveau " + tools::numToStr(i + 1);
        m_controls.campaign.levelSelect->push(label, globalSettings.campaign.maps[i]);
    }

    {
        Settings::PartySetting& party = globalSettings.campaign.maps[campMapSize];

        m_controls.campaign.description
                ->write(gui::GuiString("Carte: %s\n"
                                       "Par: %s\n\n"
                                       "%s",
                                       party.map.name.c_str(),
                                       party.map.author.c_str(),
                                       party.map.comment.c_str()));
    }
     */

    // Ball a jouer

    m_controls.playmenu.playerSelect->deleteAll();

    for(unsigned i = 0; i < globalSettings.availablePlayer.size(); i++)
        m_controls.playmenu.playerSelect->push(globalSettings.availablePlayer[i].name, i);

    m_controls.playmenu.playerSelect->setCurrent(math::rand(0, globalSettings.availablePlayer.size()));

    // Carte a jouer

    m_controls.playmenu.mapSelect->deleteAll();

    for(unsigned i = 0; i < globalSettings.availableMap.size(); i++)
    {
        m_controls.playmenu.mapSelect->push(globalSettings.availableMap[i].name, i);
    }

    m_controls.playmenu.mapSelect->setCurrent(math::rand(0, globalSettings.availableMap.size()));

    updateQuickPlayMapInfo();
}

void AppManager::setupBackgroundScene()
{
    using namespace tbe::scene;

    // SCENE -------------------------------------------------------------------

    m_sceneManager->clearAll();

    LightParallelScene* lightScene = new LightParallelScene;
    m_sceneManager->addParallelScene(lightScene);

    MeshParallelScene* meshScene = new MeshParallelScene;
    m_sceneManager->addParallelScene(meshScene);

    DiriLight* light0 = new DiriLight(lightScene);
    m_sceneManager->getRootNode()->addChild(light0);

    m_logo = new OBJMesh(meshScene, OBJ_LOGO);
    m_sceneManager->getRootNode()->addChild(m_logo);

    m_camera = new Camera(Camera::TARGET_RELATIVE);
    m_camera->setRotate(Vector2f(180, 5));
    m_sceneManager->addCamera(m_camera);

    m_sceneManager->setAmbientLight(0.1);

    // PPE ---------------------------------------------------------------------

    if(globalSettings.video.usePpe)
    {
        using namespace ppe;

        m_ppeManager->clearAll();

        // NOTE BlurEffect
        #if 0
        BlurEffect* blur = new BlurEffect;
        blur->setPasse(4);
        m_ppeManager->addPostEffect("", blur);
        #endif

        BloomEffect* bloom = new BloomEffect;
        bloom->setThreshold(0.1);
        bloom->setIntensity(1.0);
        bloom->setBlurPass(10);
        m_ppeManager->addPostEffect("", bloom);

        MotionBlurEffect* mblur = new MotionBlurEffect;
        mblur->setRttFrameSize(math::nextPow2(globalSettings.video.screenSize) / 2);
        m_ppeManager->addPostEffect("", mblur);
    }
}

void AppManager::processMainMenuEvent()
{
    if(m_controls.quickplay->isActivate())
        m_guiManager->setSession(MENU_QUICKPLAY);

        //    else if(m_controls.campaign->isActivate())
        //        m_guiManager->setSession(MENU_CAMPAIGN);

    else if(m_controls.settings->isActivate())
        m_guiManager->setSession(MENU_SETTING);

    else if(m_controls.about->isActivate())
        m_guiManager->setSession(MENU_ABOUT);
}

void AppManager::processCampaignMenuEvent()
{
    /*
    if(m_controls.campaign.levelSelect->isActivate())
    {
        Settings::PartySetting party = m_controls.campaign.levelSelect->getData()
                .getValue<Settings::PartySetting > ();

        m_controls.campaign.description
                ->write(gui::GuiString("Carte: %s\n"
                                       "Par: %s\n\n"
                                       "%s",
                                       party.map.name.c_str(),
                                       party.map.author.c_str(),
                                       party.map.comment.c_str()));
    }

    else if(m_controls.campaign.play->isActivate())
    {
        Settings::PartySetting party = m_controls.campaign.levelSelect->getData()
                .getValue<Settings::PartySetting > ();

        unsigned indexOfPlayer = m_controls.campaign.playerSelect->getData().getValue<unsigned>();

        party.player = globalSettings.availablePlayer[indexOfPlayer];
        party.player.nick = m_controls.campaign.playerName->getLabel();

        executeCampaign(party);

        setupBackgroundScene();
        m_guiManager->setSession(MENU_CAMPAIGN);
    }

    else if(m_controls.campaign.playerName->isActivate())
        globalSettings.profile.name = m_controls.campaign.playerName->getLabel();

    else if(m_controls.campaign.ret->isActivate())
        m_guiManager->setSession(MENU_MAIN);
     */
}

void AppManager::processPlayMenuEvent()
{
    if(m_controls.playmenu.mapSelect->isActivate())
        updateQuickPlayMapInfo();

    else if(m_controls.playmenu.playerSelect->isActivate());

    else if(m_controls.playmenu.play->isActivate())
    {
        unsigned indexOfLevel = m_controls.playmenu.mapSelect->getData().getValue<unsigned>();
        unsigned indexOfPlayer = m_controls.playmenu.playerSelect->getData().getValue<unsigned>();

        Settings::PartySetting playSetting;

        playSetting.map = globalSettings.availableMap[indexOfLevel];

        playSetting.player = globalSettings.availablePlayer[indexOfPlayer];
        playSetting.player.nick = m_controls.playmenu.playerName->getLabel();

        executeGame(playSetting);

        setupBackgroundScene();
        m_guiManager->setSession(MENU_QUICKPLAY);
    }

    else if(m_controls.playmenu.playerName->isActivate())
        globalSettings.profile.name = m_controls.playmenu.playerName->getLabel();

    else if(m_controls.playmenu.ret->isActivate())
        m_guiManager->setSession(MENU_MAIN);
}

void AppManager::processSettingMenuEvent()
{
    if(m_guiManager->getControl("keySetting")->isActivate())
        m_guiManager->setSession(MENU_SETTING_KEYS);

    if(m_guiManager->getControl("return")->isActivate())
        m_guiManager->setSession(MENU_MAIN);

    else if(m_guiManager->getControl("apply")->isActivate())
    {
        globalSettings.fillWindowSettingsFromGui(m_guiManager);

        globalSettings.saveSetting();

        setupVideoMode();

        m_guiManager->setSession(MENU_MAIN);
    }
}

void AppManager::processSettingKeyMenuEvent()
{
    if(m_guiManager->getControl("return")->isActivate())
        m_guiManager->setSession(MENU_SETTING);

    else if(m_guiManager->getControl("apply")->isActivate())
    {
        globalSettings.fillControlSettingsFromGui(m_guiManager);

        m_guiManager->setSession(MENU_SETTING);
    }
}

void AppManager::executeMenu()
{
    using namespace tbe::gui;
    using namespace tbe::scene;

    if(!globalSettings.noaudio && !globalSettings.nomusic)
        FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_mainMusic, false, &m_mainMusicCh);

    bool done = false;
    while(!done)
    {
        if(m_fpsMng->doRender())
        {
            m_gameEngine->pollEvent();

            if(m_eventMng->notify == EventManager::EVENT_WIN_QUIT)
                done = true;

            switch(m_guiManager->getSession())
            {
                case MENU_MAIN:
                    if(m_controls.quit->isActivate())
                        done = true;
                    else
                        processMainMenuEvent();
                    break;

                case MENU_CAMPAIGN:
                    processCampaignMenuEvent();
                    break;

                case MENU_SETTING_KEYS:
                    processSettingKeyMenuEvent();
                    break;

                case MENU_SETTING:
                    processSettingMenuEvent();
                    break;

                case MENU_ABOUT:
                    if(m_guiManager->getControl("return")->isActivate())
                        m_guiManager->setSession(MENU_MAIN);
                    break;

                case MENU_QUICKPLAY:
                    processPlayMenuEvent();
                    break;
            }

            m_camera->setPos(-m_camera->getTarget() * 8.0f);

            m_logo->getMatrix().rotate(0.01, Vector3f::Y(1));

            m_gameEngine->beginScene();

            if(globalSettings.video.usePpe)
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
}

void AppManager::executeGame(const Settings::PartySetting& playSetting)
{
    cout << "--- ExecuteGame :" << endl
            << "  playLevel = " << playSetting.map.name << endl
            << "  playerModel = " << playSetting.player.file << endl
            << "  playerName = " << playSetting.player.name << endl;

    m_sceneManager->clearAll();
    m_ppeManager->clearAll();

    if(!globalSettings.noaudio && !globalSettings.nomusic)
        FMOD_Channel_Stop(m_mainMusicCh);

    // Affichage de l'ecran de chargement --------------------------------------

    m_guiManager->setSession(MENU_LOAD);

    m_guiManager->getControl<gui::TextBox > ("load:stateText")
            ->write("Chargement en cours...");

    m_guiManager->updateLayout();

    m_gameEngine->beginScene();
    m_guiManager->render();
    m_gameEngine->endScene();

    // Chargement de la carte --------------------------------------------------

    GameManager* gameManager = new GameManager(this);

    gameManager->setupGui();
    gameManager->setupMap(playSetting);

    // Attente de réponse ------------------------------------------------------

    m_guiManager->setSession(MENU_LOAD);

    m_guiManager->getControl<gui::TextBox > ("load:stateText")
            ->write("Appuyer sur \"Espace\" pour continuer...");

    m_eventMng->keyState[EventManager::KEY_SPACE] = false;

    while(!m_eventMng->keyState[EventManager::KEY_SPACE])
    {
        m_gameEngine->pollEvent();
        m_gameEngine->beginScene();
        m_guiManager->render();
        m_gameEngine->endScene();
    }

    // Début du jeu ------------------------------------------------------------

    cout << "Start game" << endl;

    gameManager->onStartGame();

    while(gameManager->isRunning())
    {
        if(m_fpsMng->doRender())
        {
            gameManager->eventProcess();
            gameManager->gameProcess();
            gameManager->hudProcess();

            gameManager->render();
        }

        m_fpsMng->update();
    }

    cout << "End game" << endl;

    delete gameManager;

    if(!globalSettings.noaudio && !globalSettings.nomusic)
        FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_mainMusic, false, &m_mainMusicCh);
}

void AppManager::executeCampaign(const Settings::PartySetting& playSetting)
{
    Settings::PartySetting curPlaySetting = playSetting;

    bool done = false;

    do
    {
        cout << "ExecuteCampaign :" << endl
                << "playLevel = " << curPlaySetting.map.name << endl
                << "playerModel = " << curPlaySetting.player.file << endl
                << "playerName = " << curPlaySetting.player.name << endl;

        m_sceneManager->clearAll();
        m_ppeManager->clearAll();

        if(!globalSettings.noaudio && !globalSettings.nomusic)
            FMOD_Channel_Stop(m_mainMusicCh);

        // Affichage de l'ecran de chargement --------------------------------------

        m_guiManager->setSession(MENU_LOAD);

        m_guiManager->getControl<gui::TextBox > ("load:stateText")
                ->write(gui::GuiString("Chargement en cours..."));

        m_guiManager->updateLayout();

        m_gameEngine->beginScene();
        m_guiManager->render();
        m_gameEngine->endScene();

        // Chargement de la carte --------------------------------------------------

        GameManager* gameManager = new GameManager(this);

        gameManager->setupMap(curPlaySetting);
        gameManager->setupGui();

        // Attente de réponse ------------------------------------------------------

        m_guiManager->setSession(MENU_LOAD);

        m_guiManager->getControl<gui::TextBox > ("load:stateText")
                ->write(gui::GuiString("Appuyer sur \"Espace\" pour continuer..."));

        m_eventMng->keyState[EventManager::KEY_SPACE] = false;

        while(!m_eventMng->keyState[EventManager::KEY_SPACE])
        {
            m_gameEngine->pollEvent();
            m_gameEngine->beginScene();
            m_guiManager->render();
            m_gameEngine->endScene();
        }

        // Début du jeu ------------------------------------------------------------

        cout << "Start game" << endl;

        gameManager->onStartGame();

        while(gameManager->isRunning())
        {
            if(m_fpsMng->doRender())
            {
                gameManager->eventProcess();
                gameManager->gameProcess();
                gameManager->hudProcess();

                gameManager->render();
            }

            m_fpsMng->update();
        }

        cout << "End game" << endl;

        done = !gameManager->isGameOver();

        bool haswin = gameManager->getWinnerPlayer() == gameManager->getUserPlayer();

        delete gameManager;

        if(!done && haswin)
        {
            if(curPlaySetting.curLevel + 1 < globalSettings.campaign.maps.size())
            {
                // *** Pre-Load next level

                cout << "Next level available" << endl;

                if(curPlaySetting.curLevel >= globalSettings.profile.index)
                {
                    globalSettings.profile.index++;
                    curPlaySetting.curLevel = globalSettings.profile.index;
                }
                else
                    curPlaySetting.curLevel++;

                Settings::PartySetting nextParty = globalSettings.campaign.maps[curPlaySetting.curLevel];

                curPlaySetting.map = nextParty.map;

                m_guiManager->setSession(MENU_LOAD);

                m_guiManager->getControl<gui::TextBox > ("load:stateText")
                        ->write(gui::GuiString("Nouveau niveau déploqué !\n"
                                               "Appuyer sur \"Espace\" pour continuer..."));

                if(!globalSettings.noaudio)
                    FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_gongSound, false, 0);

                m_screenWaitClock.snapShoot();

                m_eventMng->keyState[EventManager::KEY_SPACE] = false;
                while(!m_eventMng->keyState[EventManager::KEY_SPACE]
                      || !m_screenWaitClock.isEsplanedTime(1000))
                {
                    m_gameEngine->pollEvent();
                    m_gameEngine->beginScene();
                    m_guiManager->render();
                    m_gameEngine->endScene();
                }
            }

            else
            {
                // *** Game complete !!!

                cout << "Game complete !!!" << endl;

                m_guiManager->setSession(MENU_LOAD);

                m_guiManager->getControl<gui::TextBox > ("load:stateText")
                        ->write(gui::GuiString("FELECITATION JEU TERMINER !!!\n"
                                               "Appuyer sur \"Espace\" pour continuer..."));

                if(!globalSettings.noaudio)
                    FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_gongSound, false, 0);

                m_screenWaitClock.snapShoot();

                m_eventMng->keyState[EventManager::KEY_SPACE] = false;
                while(!m_eventMng->keyState[EventManager::KEY_SPACE]
                      || !m_screenWaitClock.isEsplanedTime(1000))
                {
                    m_gameEngine->pollEvent();
                    m_gameEngine->beginScene();
                    m_guiManager->render();
                    m_gameEngine->endScene();
                }

                done = true;
            }
        }

        globalSettings.saveProfiles();

        updateGuiContent();
    }
    while(!done);

    if(!globalSettings.noaudio && !globalSettings.nomusic)
        FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_mainMusic, false, &m_mainMusicCh);
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

tbe::gui::GuiManager* AppManager::getGuiMng() const
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
