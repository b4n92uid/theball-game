#include "AppManager.h"

#include "GameManager.h"
#include "PlayManager.h"
#include "EditorManager.h"

#include "PlayFragManager.h"
#include "PlayAloneManager.h"
#include "PlayTeamManager.h"

#include <fmod_errors.h>

using namespace std;
using namespace tbe;

inline bool ScoreSortByDate(const Settings::ScoreInfo& p1, const Settings::ScoreInfo& p2)
{
    return p1.timestamp > p2.timestamp;
}

inline bool ScoreSortByValue(const Settings::ScoreInfo& p1, const Settings::ScoreInfo& p2)
{
    if(p1.playerName == p2.playerName)
        return p1.score > p2.score;
    else
        return p1.playerName > p2.playerName;
}

inline void writeCampaignDescription(gui::TextBox* text, Settings::PartySetting& party)
{
    text->write(gui::GuiString("Carte:       %s\n"
                               "Mode:        %s\n"
                               "Temps:       %d\n"
                               "Joueurs:     %d\n"
                               "Objectif:    %d point(s)",
                               party.playMap.name.c_str(),
                               AppManager::unsignedToPlayMod(party.playMod).c_str(),
                               party.playTime,
                               party.playerCount,
                               party.winCond));
}

inline void writeScore(gui::TextBox* textbox, vector<Settings::ScoreInfo>& scores)
{
    using namespace boost::posix_time;

    stringstream text;

    text << "Les Scores enregistrer" << endl;
    text << "Nom | Date | Type | Carte | Temps | Score" << endl;
    text << endl;

    for(unsigned i = 0; i < scores.size(); i++)
    {
        Settings::ScoreInfo& si = scores[i];

        text
                << si.playerName << " | "
                << from_time_t(si.timestamp) << " | "
                << AppManager::unsignedToPlayMod(si.playMod) << " | "
                << si.levelName << " | "
                << si.playTime << " sec | "
                << si.score << " point(s)" << endl;
    }

    textbox->write(text.str());
}

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

    const float& sizeFactor = globalSettings.video.guiSizeFactor = (globalSettings.video.screenSize.x / 800.0f);
    const Vector2i& screenSize = globalSettings.video.screenSize;

    GuiSkin* guiskin = new GuiSkin;

    guiskin->button(GUI_BUTTON);
    guiskin->buttonSize(Vector2f(192, 48) * sizeFactor);

    guiskin->gauge(GUI_GAUGE);
    guiskin->gaugeSize(Vector2f(192, 48) * sizeFactor);

    guiskin->editBox(GUI_EDIT);
    guiskin->editBoxSize(Vector2f(192, 48) * sizeFactor);

    guiskin->switchBox(GUI_SWITCH);
    guiskin->switchBoxSize(Vector2f(192, 48) * sizeFactor);

    guiskin->vectorBox(GUI_VECTOR);
    guiskin->vectorBoxSize(Vector2f(192, 48) * sizeFactor);

    guiskin->stateShowSize(Vector2f(48, 48) * sizeFactor);

    guiskin->pencile(GUI_FONT, int(sizeFactor * GUI_FONTSIZE));

    m_guiManager->setSkin(guiskin);

    Pencil bigpen(GUI_FONT, int(sizeFactor * GUI_FONTSIZE * 1.5));

    // Construction ------------------------------------------------------------

    Texture background(BACKGROUND_MAINMENU);

    // Menu Principale

    m_guiManager->setSession(MENU_MAIN);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);

    m_guiManager->addLayout(Layout::Horizental);
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->addLayout(Layout::Vertical, 10);
    m_guiManager->addLayoutStretchSpace();

    m_guiManager->addButton("quit", "Quitter");
    m_guiManager->addButton("about", "A Propos");
    m_guiManager->addButton("score", "Scores");
    m_guiManager->addButton("setting", "Options");
    m_guiManager->addButton("editor", "Editeur");
    m_guiManager->addButton("quickplay", "Partie rapide");
    m_guiManager->addButton("campaign", "Jouer");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // Menu Campaign

    m_guiManager->setSession(MENU_CAMPAIGN);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);

    m_guiManager->addLayout(Layout::Horizental, 10, 10);
    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->addLayout(Layout::Vertical, 10);
    //    m_guiManager->addLayoutSpace(screenSize.y / 4);
    m_guiManager->addLayoutStretchSpace();

    m_controls.campaign.ret = m_guiManager->addButton("ret", "Retour");

    m_controls.campaign.play = m_guiManager->addButton("play", "Jouer");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // -------- Collone 1

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 2
    m_guiManager->addLayout(Layout::Vertical, 10);
    //    m_guiManager->addLayoutSpace(screenSize.y / 4);
    m_guiManager->addLayoutStretchSpace();

    m_controls.campaign.playerSelect = m_guiManager->addSwitchString("playerSelect");
    m_guiManager->addTextBox("")->write("Personnage");

    m_controls.campaign.playerName = m_guiManager->addEditBox("nameSelect", "Joueur");
    m_guiManager->addTextBox("")->write("Pseudo");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // -------- Collone 2

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 3
    m_guiManager->addLayout(Layout::Vertical, 10);
    //    m_guiManager->addLayoutSpace(screenSize.y / 4);
    m_guiManager->addLayoutStretchSpace();

    m_controls.campaign.description = m_guiManager->addTextBox("description");
    m_controls.campaign.description->setBackground(GUI_TEXTBOX_H);
    m_controls.campaign.description->setBackgroundPadding(16);

    m_controls.campaign.levelSelect = m_guiManager->addSwitchString("level_select");
    m_guiManager->addTextBox("")->write("Niveau");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // -------- Collone 3

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // Menu Jouer

    m_guiManager->setSession(MENU_QUICKPLAY);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);

    m_guiManager->addLayout(Layout::Horizental, 10, 10);

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->addLayout(Layout::Vertical, 5);
    m_guiManager->addLayoutStretchSpace();

    m_controls.playmenu.playerSelect = m_guiManager->addSwitchString("playerSelect");
    m_guiManager->addTextBox("")->write("Personnage");

    m_controls.playmenu.modSelect = m_guiManager->addSwitchString("modSelect");
    m_guiManager->addTextBox("")->write("Mode");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 2
    m_guiManager->addLayout(Layout::Vertical, 5);
    m_guiManager->addLayoutStretchSpace();

    m_controls.playmenu.mapSelect = m_guiManager->addSwitchString("levelSelect");
    m_guiManager->addTextBox("")->write("Carte à jouer");

    m_controls.playmenu.timeSelect = m_guiManager->addSwitchString("timeSelect");
    m_guiManager->addTextBox("")->write("Temps");

    m_controls.playmenu.playerCount = m_guiManager->addSwitchNumeric<int>("playerCount");
    m_controls.playmenu.playerCount->setRange(new SwitchNumeric<int>::Range(0, 32));
    m_controls.playmenu.playerCount->setStep(2);
    m_guiManager->addTextBox("")->write("Nombres de joueurs");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 3
    m_guiManager->addLayout(Layout::Vertical, 5);
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->addButton("return", "Retour");
    m_guiManager->addButton("play", "Jouer");
    m_controls.playmenu.playerName = m_guiManager->addEditBox("nameSelect", "Joueur");
    m_guiManager->addTextBox("")->write("Pseudo");
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // Ecran de chargement

    m_guiManager->setSession(MENU_LOAD);

    m_guiManager->addImage("", BACKGROUND_LOAD)
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

    // Menu Edition

    m_guiManager->setSession(MENU_EDIT);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);

    m_guiManager->addLayout(Layout::Horizental, 0, 10);
    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->addLayout(Layout::Vertical, 10);
    m_guiManager->addLayoutStretchSpace();
    m_controls.edit.ret = m_guiManager->addButton("", "Retour");
    m_controls.edit.mapSelect = m_guiManager->addSwitchString("");
    m_controls.edit.editMap = m_guiManager->addButton("", "Editer");
    m_controls.edit.newMap = m_guiManager->addButton("", "Nouveau");
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // Menu Option

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

    m_controls.settings.screenSize = m_guiManager->addSwitchString("screenSize");
    m_guiManager->addTextBox("")->write("Résolution");

    m_controls.settings.antiAliasing = m_guiManager->addSwitchString("antiAliasing");
    m_guiManager->addTextBox("")->write("Anti-crélénage");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();

    // -------- Collone 3
    m_guiManager->addLayout(Layout::Vertical, 8);
    m_guiManager->addLayoutStretchSpace();

    m_controls.settings.fullscreen = m_guiManager->addSwitchString("fullScreen");
    m_guiManager->addTextBox("")->write("Mode d'affichage");

    m_controls.settings.usePpe = m_guiManager->addSwitchString("usePpe");
    m_guiManager->addTextBox("")->write("P.P Effects");

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    // --------

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // Menu Commandes

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
    m_guiManager->addKeyConfig("bullettime");
    m_guiManager->addTextBox("")->write("Bllettime");
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

    // Menu Score

    m_guiManager->setSession(MENU_SCORE);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);

    m_guiManager->addLayout(Layout::Horizental);
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->addLayout(Layout::Vertical, 10);
    m_guiManager->addLayoutStretchSpace();

    m_guiManager->addLayout(Layout::Horizental, 10);
    m_controls.score.ret = m_guiManager->addButton("score.ret", "Retour");
    m_controls.score.sortType = m_guiManager->addSwitchString("score.sortType");
    m_guiManager->endLayout();

    m_controls.score.scoreText = m_guiManager->addTextBox("");
    m_controls.score.scoreText->setSize(Vector2f(screenSize) * Vector2f(0.75, 0.5));
    m_controls.score.scoreText->setDefinedSize(true);
    m_controls.score.scoreText->setBackgroundPadding(8);
    m_controls.score.scoreText->setBackground(GUI_TEXTBOX_H);
    m_controls.score.scoreText->setTextAlign(TextBox::LEFT);

    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->endLayout();

    // Menu A propos

    m_guiManager->setSession(MENU_ABOUT);

    m_guiManager->addImage("", background)
            ->setSize(screenSize);


    m_guiManager->addLayout(Layout::Horizental);
    m_guiManager->addLayoutStretchSpace();
    m_guiManager->addLayout(Layout::Vertical, 10);
    m_guiManager->addLayoutStretchSpace();

    m_guiManager->addButton("return", "Retour");

    m_controls.aboutText = m_guiManager->addTextBox("aboutText");
    m_controls.aboutText->setSize(Vector2f(screenSize) * Vector2f(0.75, 0.5));
    m_controls.aboutText->setDefinedSize(true);
    m_controls.aboutText->setBackgroundPadding(8);
    m_controls.aboutText->setBackground(GUI_TEXTBOX_H);
    m_controls.aboutText->setTextAlign(TextBox::LEFT);

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

        m_controls.aboutText->write(ss.str());
    }

    // Menu des scores
    {
        sort(globalSettings.availableScore.begin(),
             globalSettings.availableScore.end(),
             ScoreSortByValue);

        writeScore(m_controls.score.scoreText, globalSettings.availableScore);

        m_controls.score.sortType
                ->push("Tri par date", 1)
                .push("Tri par points", 2);
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
        m_controls.settings.usePpe->push("Désactiver", false).push("Activer", true);
        m_controls.settings.usePpe->setCurrent(globalSettings.video.usePpe);

        m_controls.settings.fullscreen->push("Fenêtrer", false).push("Plein-écran", true);
        m_controls.settings.fullscreen->setCurrent(globalSettings.video.fullScreen);
    }

    {
        int cutAA = globalSettings.video.antialiasing;

        map<int, int> AAAvailable;
        AAAvailable[0] = 0;
        AAAvailable[2] = 1;
        AAAvailable[4] = 2;
        AAAvailable[8] = 3;
        AAAvailable[16] = 4;

        m_controls.settings.antiAliasing->
                push("Désactiver", 0).
                push("2x", 2).
                push("4x", 4).
                push("8x", 8).
                push("16x", 16)
                ;

        m_controls.settings.antiAliasing->setCurrent(AAAvailable[cutAA]);
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

            m_controls.settings.screenSize->push(ss.str(), availableScreen[i]);
        }

        m_controls.settings.screenSize->setCurrent(curSizeIndex);
    }

    // Menu campaign

    m_controls.campaign.playerName->setLabel(globalSettings.profile.name);

    for(unsigned i = 0; i < globalSettings.availablePlayer.size(); i++)
        m_controls.campaign.playerSelect->push(globalSettings.availablePlayer[i].name, i);

    m_controls.campaign.playerSelect->setCurrent(tools::rand(0, globalSettings.availablePlayer.size()));


    // Temps de la partie
    m_controls.playmenu.playerCount->setValue(4);

    m_controls.playmenu.timeSelect->
            push("60 sec", 60).
            push("2 min", 60 * 2).
            push("5 min", 60 * 5).
            push("20 min", 60 * 20).
            push("Infinie", 0);

    m_controls.playmenu.timeSelect->setCurrent(1);

    // Mod de la partie
    m_controls.playmenu.modSelect->
            push("Frag", FRAG).
            push("Alone", ALONE).
            push("Team", TEAM);

    m_controls.playmenu.modSelect->setCurrent(0);

    updateGuiContent();
}

void AppManager::updateGuiContent()
{
    // Campaign

    m_controls.campaign.levelSelect->deleteAll();

    unsigned campMapSize = min(globalSettings.campaign.maps.size() - 1, globalSettings.profile.index);
    for(unsigned i = 0; i <= campMapSize; i++)
    {
        string label = "Niveau " + tools::numToStr(i + 1);
        m_controls.campaign.levelSelect->push(label, globalSettings.campaign.maps[i]);
    }

    {
        Settings::PartySetting& party = globalSettings.campaign.maps[campMapSize];

        writeCampaignDescription(m_controls.campaign.description, party);
    }

    // Ball a jouer

    m_controls.playmenu.playerSelect->deleteAll();

    for(unsigned i = 0; i < globalSettings.availablePlayer.size(); i++)
        m_controls.playmenu.playerSelect->push(globalSettings.availablePlayer[i].name, i);

    m_controls.playmenu.playerSelect->setCurrent(tools::rand(0, globalSettings.availablePlayer.size()));

    // Carte a jouer

    m_controls.playmenu.mapSelect->deleteAll();

    for(unsigned i = 0; i < globalSettings.availableMap.size(); i++)
    {
        m_controls.playmenu.mapSelect->push(globalSettings.availableMap[i].name, i);
        m_controls.edit.mapSelect->push(globalSettings.availableMap[i].name, i);
    }

    m_controls.playmenu.mapSelect->setCurrent(tools::rand(0, globalSettings.availableMap.size()));
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
        mblur->setRttFrameSize(tools::nextPow2(globalSettings.video.screenSize) / 2);
        m_ppeManager->addPostEffect("", mblur);
    }
}

void AppManager::processMainMenuEvent()
{
    if(m_guiManager->getControl("quickplay")->isActivate())
        m_guiManager->setSession(MENU_QUICKPLAY);

    else if(m_guiManager->getControl("campaign")->isActivate())
        m_guiManager->setSession(MENU_CAMPAIGN);

    else if(m_guiManager->getControl("setting")->isActivate())
        m_guiManager->setSession(MENU_SETTING);

    else if(m_guiManager->getControl("editor")->isActivate())
        m_guiManager->setSession(MENU_EDIT);

    else if(m_guiManager->getControl("about")->isActivate())
        m_guiManager->setSession(MENU_ABOUT);

    else if(m_guiManager->getControl("score")->isActivate())
        m_guiManager->setSession(MENU_SCORE);
}

void AppManager::processCampaignMenuEvent()
{
    if(m_controls.campaign.levelSelect->isActivate())
    {
        Settings::PartySetting party = m_controls.campaign.levelSelect->getData()
                .getValue<Settings::PartySetting > ();

        writeCampaignDescription(m_controls.campaign.description, party);
    }
    else if(m_controls.campaign.play->isActivate())
    {
        Settings::PartySetting party = m_controls.campaign.levelSelect->getData()
                .getValue<Settings::PartySetting > ();

        unsigned indexOfPlayer = m_controls.campaign.playerSelect->getData().getValue<unsigned>();

        party.playerName = globalSettings.availablePlayer[indexOfPlayer];
        party.playerName.nick = m_controls.campaign.playerName->getLabel();

        executeCampaign(party);

        setupBackgroundScene();
        m_guiManager->setSession(MENU_CAMPAIGN);
    }

    else if(m_controls.campaign.playerName->isActivate())
        globalSettings.profile.name = m_controls.campaign.playerName->getLabel();

    else if(m_controls.campaign.ret->isActivate())
        m_guiManager->setSession(MENU_MAIN);
}

void AppManager::processPlayMenuEvent()
{
    if(m_controls.playmenu.mapSelect->isActivate());

    else if(m_controls.playmenu.playerSelect->isActivate());

    else if(m_guiManager->getControl("play")->isActivate())
    {
        unsigned indexOfLevel = m_controls.playmenu.mapSelect->getData().getValue<unsigned>();
        unsigned indexOfPlayer = m_controls.playmenu.playerSelect->getData().getValue<unsigned>();

        Settings::PartySetting playSetting;

        playSetting.playMap = globalSettings.availableMap[indexOfLevel];

        playSetting.playerName = globalSettings.availablePlayer[indexOfPlayer];
        playSetting.playerName.nick = m_controls.playmenu.playerName->getLabel();

        playSetting.playMod = m_controls.playmenu.modSelect->getData().getValue<unsigned>();
        playSetting.playTime = m_controls.playmenu.timeSelect->getData().getValue<unsigned>();

        playSetting.playerCount = m_controls.playmenu.playerCount->getValue();

        executeGame(playSetting);

        setupBackgroundScene();
        m_guiManager->setSession(MENU_QUICKPLAY);
    }

    else if(m_controls.playmenu.playerName->isActivate())
        globalSettings.profile.name = m_controls.playmenu.playerName->getLabel();

    else if(m_guiManager->getControl("return")->isActivate())
        m_guiManager->setSession(MENU_MAIN);
}

void AppManager::processEditMenuEvent()
{
    if(m_controls.edit.editMap->isActivate())
    {
        unsigned index = m_controls.edit.mapSelect->getData().getValue<unsigned>();
        Settings::MapInfo& mi = globalSettings.availableMap[index];

        Settings::EditSetting es;
        es.createNew = false;
        es.editMap = mi.file;

        executeEditor(es);

        setupBackgroundScene();
        m_guiManager->setSession(MENU_EDIT);
    }
    else if(m_controls.edit.newMap->isActivate())
    {
        Settings::EditSetting es;
        es.createNew = true;

        executeEditor(es);

        setupBackgroundScene();
        m_guiManager->setSession(MENU_EDIT);
    }
    else if(m_controls.edit.ret->isActivate())
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

void AppManager::processScoreMenuEvent()
{
    if(m_controls.score.ret->isActivate())
        m_guiManager->setSession(MENU_MAIN);

    else if(m_controls.score.sortType->isActivate())
    {
        switch(m_controls.score.sortType->getData().getValue<int>())
        {
            case 1:
                std::sort(globalSettings.availableScore.begin(),
                          globalSettings.availableScore.end(),
                          ScoreSortByDate);
                break;
            case 2:
                std::sort(globalSettings.availableScore.begin(),
                          globalSettings.availableScore.end(),
                          ScoreSortByValue);
                break;
        }

        writeScore(m_controls.score.scoreText, globalSettings.availableScore);
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
                    if(m_guiManager->getControl("quit")->isActivate())
                        done = true;
                    else
                        processMainMenuEvent();
                    break;

                case MENU_CAMPAIGN:
                    processCampaignMenuEvent();
                    break;

                case MENU_SCORE:
                    processScoreMenuEvent();
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

                case MENU_EDIT:
                    processEditMenuEvent();
                    break;
            }

            m_camera->setPos(-m_camera->getTarget() * 8.0f);

            m_logo->getMatrix().setRotateY(0.01);

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
            << "  playLevel = " << playSetting.playMap.name << endl
            << "  playMod = " << playSetting.playMod << endl
            << "  playTime = " << playSetting.playTime << endl
            << "  playerCount = " << playSetting.playerCount << endl
            << "  playerModel = " << playSetting.playerName.file << endl
            << "  playerName = " << playSetting.playerName.name << endl;

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

    PlayManager* gameManager;

    switch(playSetting.playMod)
    {
        case FRAG: gameManager = new PlayFragManager(this);
            break;
        case ALONE: gameManager = new PlayAloneManager(this);
            break;
        case TEAM: gameManager = new PlayTeamManager(this);
            break;
        default:
            throw Exception("AppManager::ExecuteGame; Unknown mode type (%d)", playSetting.playMod);
    }

    gameManager->setupMap(playSetting);
    gameManager->setupGui();

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

    while(gameManager->running)
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
                << "playLevel = " << curPlaySetting.playMap.name << endl
                << "playMod = " << curPlaySetting.playMod << endl
                << "playTime = " << curPlaySetting.playTime << endl
                << "playerCount = " << curPlaySetting.playerCount << endl
                << "playerModel = " << curPlaySetting.playerName.file << endl
                << "playerName = " << curPlaySetting.playerName.name << endl
                << "winCond = " << curPlaySetting.winCond << endl;

        m_sceneManager->clearAll();
        m_ppeManager->clearAll();

        if(!globalSettings.noaudio && !globalSettings.nomusic)
            FMOD_Channel_Stop(m_mainMusicCh);

        // Affichage de l'ecran de chargement --------------------------------------

        m_guiManager->setSession(MENU_LOAD);

        m_guiManager->getControl<gui::TextBox > ("load:stateText")
                ->write(gui::GuiString("%s en mode %s\n"
                                       "Marquer %d points en %d sec pour passer au niveau suivant !\n"
                                       "Chargement en cours...",
                                       curPlaySetting.playMap.name.c_str(),
                                       unsignedToPlayMod(curPlaySetting.playMod).c_str(),
                                       curPlaySetting.winCond, curPlaySetting.playTime));

        m_guiManager->updateLayout();

        m_gameEngine->beginScene();
        m_guiManager->render();
        m_gameEngine->endScene();

        // Chargement de la carte --------------------------------------------------

        PlayManager* gameManager;

        switch(curPlaySetting.playMod)
        {
            case FRAG: gameManager = new PlayFragManager(this);
                break;
            case ALONE: gameManager = new PlayAloneManager(this);
                break;
            case TEAM: gameManager = new PlayTeamManager(this);
                break;
            default:
                throw Exception("AppManager::ExecuteGame; Unknown mode type (%d)", curPlaySetting.playMod);
        }

        gameManager->setupMap(curPlaySetting);
        gameManager->setupGui();

        // Attente de réponse ------------------------------------------------------

        m_guiManager->setSession(MENU_LOAD);

        m_guiManager->getControl<gui::TextBox > ("load:stateText")
                ->write(gui::GuiString("%s en mode %s\n"
                                       "Marquer %d points en %d sec pour passer au niveau suivant !\n"
                                       "Appuyer sur \"Espace\" pour continuer...",
                                       curPlaySetting.playMap.name.c_str(),
                                       unsignedToPlayMod(curPlaySetting.playMod).c_str(),
                                       curPlaySetting.winCond, curPlaySetting.playTime));

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

        while(gameManager->running)
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

        int score = gameManager->getUserPlayer()->getScore();
        done = !gameManager->isGameOver();

        delete gameManager;

        if(done)
            continue;

        if(score >= (int)curPlaySetting.winCond)
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

                curPlaySetting.playMap = nextParty.playMap;
                curPlaySetting.playMod = nextParty.playMod;
                curPlaySetting.playTime = nextParty.playTime;
                curPlaySetting.playerCount = nextParty.playerCount;
                curPlaySetting.winCond = nextParty.winCond;

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

void AppManager::executeEditor(const Settings::EditSetting& editSetting)
{
    cout << "ExecuteEditor" << endl;

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

    EditorManager* editorManager = new EditorManager(this);

    editorManager->setupMap(editSetting);
    editorManager->setupGui();

    // Attente de réponse ------------------------------------------------------

    m_guiManager->setSession(MENU_LOAD);

    m_guiManager->getControl<gui::TextBox > ("load:stateText")
            ->write("Appuyer sur \"Espace\" pour continuer...");

    while(!m_eventMng->keyState[EventManager::KEY_SPACE])
    {
        m_gameEngine->pollEvent();
        m_gameEngine->beginScene();
        m_guiManager->render();
        m_gameEngine->endScene();
    }

    // Début du jeu ------------------------------------------------------------

    while(editorManager->running)
    {
        if(m_fpsMng->doRender())
        {
            editorManager->eventProcess();
            editorManager->hudProcess();

            editorManager->render();
        }

        m_fpsMng->update();
    }

    delete editorManager;

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
