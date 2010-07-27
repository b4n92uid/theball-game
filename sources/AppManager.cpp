#include "AppManager.h"

#include "GameManager.h"
#include "PlayManager.h"
#include "EditorManager.h"

#include "PlayFragManager.h"
#include "PlayAloneManager.h"
#include "PlayTeamManager.h"

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

inline string UnsignedToPlayMod(unsigned pm)
{
    switch(pm)
    {
        case AppManager::FRAG: return "Frag";
        case AppManager::ALONE: return "Alone";
        case AppManager::TEAM: return "Team";
        default: return "Inconnu";
    }
}

inline string WriteScore(vector<Settings::ScoreInfo>& scores)
{
    stringstream text;

    text << "Les Scores enregistrer" << endl;
    text << "Nom : Date : Type : Carte : Temps : Score" << endl;
    text << endl;

    for(unsigned i = 0; i < scores.size(); i++)
    {
        Settings::ScoreInfo& si = scores[i];

        text << si.playerName << " : "
                << ticks::Clock::Date("%x", si.timestamp) << " : "
                << UnsignedToPlayMod(si.playMod) << " : "
                << si.levelName << " : "
                << si.playTime << " sec : "
                << si.score << " point(s)" << endl;
    }

    return text.str();
}

AppManager::AppManager()
{
    m_gameEngine = new SDLDevice;

    globalSettings.ReadSetting();

    SetupVideoMode();
    SetupSound();
}

AppManager::~AppManager()
{
    FSOUND_Close();

    delete m_gameEngine;
}

void AppManager::SetupVideoMode()
{
    m_gameEngine->Window(CAPTION_TITLE,
                         globalSettings.video.screenSize,
                         globalSettings.video.bits,
                         globalSettings.video.fullScreen,
                         globalSettings.video.usePpe ? 0 : globalSettings.video.antialiasing);

    if(!Shader::CheckHardware() && globalSettings.video.usePpe)
    {
        globalSettings.video.usePpe = false;

        m_gameEngine->Window(CAPTION_TITLE,
                             globalSettings.video.screenSize,
                             globalSettings.video.bits,
                             globalSettings.video.fullScreen,
                             globalSettings.video.antialiasing);
    }

    m_eventMng = m_gameEngine->GetEventManager();
    m_guiManager = m_gameEngine->GetGuiManager();
    m_sceneManager = m_gameEngine->GetSceneManager();
    m_fpsMng = m_gameEngine->GetFpsManager();
    m_ppeManager = m_gameEngine->GetPostProcessManager();


    if(globalSettings.video.usePpe
       && globalSettings.video.antialiasing
       && FrameBufferObject::CheckHardware())
    {
        Rtt* ppeRtt = m_ppeManager->GetRtt();
        ppeRtt->FBO_SetUseRenderBuffer(true);
        ppeRtt->FBO_SetMultisamplesCount(globalSettings.video.antialiasing);
    }

    m_fpsMng->SetRunFps(60);

    Texture::ResetCache();

    SetupMenuGui();

    SetupBackgroundScene();
}

void AppManager::SetupSound()
{
    FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
    FSOUND_SetDriver(1);
    FSOUND_Init(44100, 32, 0);

    m_mainMusic = FSOUND_Stream_Open(SOUND_MAINTHEME, FSOUND_LOOP_NORMAL, 0, 0);

    if(!m_mainMusic)
        throw tbe::Exception("AppManager::AppManager; Main music load error (%s)", SOUND_MAINTHEME);
}

void AppManager::SetupMenuGui()
{
    using namespace tbe::gui;

    m_guiManager->ClearAll();

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

    m_guiManager->SetSkin(guiskin);

    // Construction ------------------------------------------------------------

    Texture background(BACKGROUND_MAINMENU);

    // Menu Principale

    m_guiManager->SetSession(MENU_MAIN);

    m_guiManager->AddImage("", background)
            ->SetSize(screenSize);

    m_guiManager->AddLayout(Layout::Horizental);
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->AddLayout(Layout::Vertical, 10);
    m_guiManager->AddLayoutStretchSpace();

    m_guiManager->AddButton("quit", "Quitter");
    m_guiManager->AddButton("about", "A Propos");
    m_guiManager->AddButton("score", "Scores");
    m_guiManager->AddButton("setting", "Options");
    m_guiManager->AddButton("editor", "Editeur");
    m_guiManager->AddButton("newgame", "Jouer");

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();

    // Menu Jouer

    m_guiManager->SetSession(MENU_PLAY);

    m_guiManager->AddImage("", background)
            ->SetSize(screenSize);

    m_guiManager->AddLayout(Layout::Horizental, 10, 10);

    m_guiManager->AddLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->AddLayout(Layout::Vertical, 5);
    m_guiManager->AddLayoutStretchSpace();

    m_controls.playmenu.playerSelect = m_guiManager->AddSwitchString("playerSelect");
    m_guiManager->AddTextBox("")->Write("Personnage");

    m_controls.playmenu.modSelect = m_guiManager->AddSwitchString("modSelect");
    m_guiManager->AddTextBox("")->Write("Mode");

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    // --------

    m_guiManager->AddLayoutStretchSpace();

    // -------- Collone 2
    m_guiManager->AddLayout(Layout::Vertical, 5);
    m_guiManager->AddLayoutStretchSpace();

    m_controls.playmenu.mapSelect = m_guiManager->AddSwitchString("levelSelect");
    m_guiManager->AddTextBox("")->Write("Carte à jouer");

    m_controls.playmenu.timeSelect = m_guiManager->AddSwitchString("timeSelect");
    m_guiManager->AddTextBox("")->Write("Temps");

    m_controls.playmenu.playerCount = m_guiManager->AddSwitchNumeric<int>("playerCount");
    m_controls.playmenu.playerCount->SetRange(new SwitchNumeric<int>::Range(0, 32));
    m_controls.playmenu.playerCount->SetStep(2);
    m_guiManager->AddTextBox("")->Write("Nombres de joueurs");

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    // --------

    m_guiManager->AddLayoutStretchSpace();

    // -------- Collone 3
    m_guiManager->AddLayout(Layout::Vertical, 5);
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->AddButton("return", "Retour");
    m_guiManager->AddButton("play", "Jouer");
    m_controls.playmenu.playerName = m_guiManager->AddEditBox("nameSelect", "Joueur");
    m_guiManager->AddTextBox("")->Write("Pseudo");
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    // --------

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();

    // Ecran de chargement

    m_guiManager->SetSession(MENU_LOAD);

    m_guiManager->AddImage("", background)
            ->SetSize(screenSize);

    m_guiManager->AddTextBox("load:stateText")->SetPos(10);

    // Menu Edition

    m_guiManager->SetSession(MENU_EDIT);

    m_guiManager->AddImage("", background)
            ->SetSize(screenSize);

    m_guiManager->AddLayout(Layout::Horizental, 0, 10);
    m_guiManager->AddLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->AddLayout(Layout::Vertical, 10);
    m_guiManager->AddLayoutStretchSpace();
    m_controls.edit.ret = m_guiManager->AddButton("", "Retour");
    m_controls.edit.mapSelect = m_guiManager->AddSwitchString("");
    m_controls.edit.editMap = m_guiManager->AddButton("", "Editer");
    m_controls.edit.newMap = m_guiManager->AddButton("", "Nouveau");
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    // --------

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();

    // Menu Option

    m_guiManager->SetSession(MENU_SETTING);

    m_guiManager->AddImage("", background)
            ->SetSize(screenSize);

    m_guiManager->AddLayout(Layout::Horizental, 0, 10);
    m_guiManager->AddLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->AddLayout(Layout::Vertical, 8);
    m_guiManager->AddLayoutStretchSpace();

    m_guiManager->AddButton("return", "Retour");

    m_guiManager->AddButton("apply", "Appliquer");

    m_guiManager->AddButton("keySetting", "Conf. Touches");

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    // --------

    m_guiManager->AddLayoutStretchSpace();

    // -------- Collone 2
    m_guiManager->AddLayout(Layout::Vertical, 8);
    m_guiManager->AddLayoutStretchSpace();

    m_controls.settings.screenSize = m_guiManager->AddSwitchString("screenSize");
    m_guiManager->AddTextBox("")->Write("Résolution");

    m_controls.settings.antiAliasing = m_guiManager->AddSwitchString("antiAliasing");
    m_guiManager->AddTextBox("")->Write("Anti-crélénage");

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    // --------

    m_guiManager->AddLayoutStretchSpace();

    // -------- Collone 3
    m_guiManager->AddLayout(Layout::Vertical, 8);
    m_guiManager->AddLayoutStretchSpace();

    m_controls.settings.fullscreen = m_guiManager->AddSwitchString("fullScreen");
    m_guiManager->AddTextBox("")->Write("Mode d'affichage");

    m_controls.settings.usePpe = m_guiManager->AddSwitchString("usePpe");
    m_guiManager->AddTextBox("")->Write("P.P Effects");

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    // --------

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();

    // Menu Commandes

    m_guiManager->SetSession(MENU_SETTING_KEYS);

    m_guiManager->AddImage("", background)
            ->SetSize(screenSize);

    m_guiManager->AddLayout(Layout::Horizental, 0, 10);
    m_guiManager->AddLayoutStretchSpace();

    // -------- Collone 1
    m_guiManager->AddLayout(Layout::Vertical, 2);
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->AddKeyConfig("forward");
    m_guiManager->AddTextBox("")->Write("Avancer");
    m_guiManager->AddKeyConfig("backward");
    m_guiManager->AddTextBox("")->Write("Reculer");
    m_guiManager->AddKeyConfig("strafRight");
    m_guiManager->AddTextBox("")->Write("A gauche");
    m_guiManager->AddKeyConfig("strafLeft");
    m_guiManager->AddTextBox("")->Write("A droite");
    m_guiManager->AddKeyConfig("jump");
    m_guiManager->AddTextBox("")->Write("Sauter");
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    // --------

    m_guiManager->AddLayoutStretchSpace();

    // -------- Collone 2
    m_guiManager->AddLayout(Layout::Vertical, 2);
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->AddKeyConfig("boost");
    m_guiManager->AddTextBox("")->Write("Boost");
    m_guiManager->AddKeyConfig("shoot");
    m_guiManager->AddTextBox("")->Write("Tirer");
    m_guiManager->AddKeyConfig("bullettime");
    m_guiManager->AddTextBox("")->Write("Bllettime");
    m_guiManager->AddKeyConfig("switchUpWeapon");
    m_guiManager->AddTextBox("")->Write("Arme suivante");
    m_guiManager->AddKeyConfig("switchDownWeapon");
    m_guiManager->AddTextBox("")->Write("Arme précédente");
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    // --------

    m_guiManager->AddLayoutStretchSpace();

    // -------- Collone 3
    m_guiManager->AddLayout(Layout::Vertical, 10);
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->AddButton("return", "Retour");
    m_guiManager->AddButton("apply", "Appliquer");
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    // --------

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();

    // Menu Score

    m_guiManager->SetSession(MENU_SCORE);

    m_guiManager->AddImage("", background)
            ->SetSize(screenSize);

    m_guiManager->AddLayout(Layout::Horizental);
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->AddLayout(Layout::Vertical, 10);
    m_guiManager->AddLayoutStretchSpace();

    m_guiManager->AddLayout(Layout::Horizental, 10);
    m_controls.score.ret = m_guiManager->AddButton("score.ret", "Retour");
    m_controls.score.sortType = m_guiManager->AddSwitchString("score.sortType");
    m_guiManager->EndLayout();

    m_controls.score.scoreText = m_guiManager->AddTextBox("");
    m_controls.score.scoreText->SetSize(Vector2f(screenSize) * Vector2f(0.75, 0.5));
    m_controls.score.scoreText->SetDefinedSize(true);
    m_controls.score.scoreText->SetBackgroundPadding(8);
    m_controls.score.scoreText->SetBackground(GUI_TEXTBOX_H);
    m_controls.score.scoreText->SetTextAlign(TextBox::LEFT);

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();

    // Menu A propos

    m_guiManager->SetSession(MENU_ABOUT);

    m_guiManager->AddImage("", background)
            ->SetSize(screenSize);


    m_guiManager->AddLayout(Layout::Horizental);
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->AddLayout(Layout::Vertical, 10);
    m_guiManager->AddLayoutStretchSpace();

    m_guiManager->AddButton("return", "Retour");

    m_controls.aboutText = m_guiManager->AddTextBox("aboutText");
    m_controls.aboutText->SetSize(Vector2f(screenSize) * Vector2f(0.75, 0.5));
    m_controls.aboutText->SetDefinedSize(true);
    m_controls.aboutText->SetBackgroundPadding(8);
    m_controls.aboutText->SetBackground(GUI_TEXTBOX_H);
    m_controls.aboutText->SetTextAlign(TextBox::LEFT);

    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();
    m_guiManager->AddLayoutStretchSpace();
    m_guiManager->EndLayout();

    m_guiManager->SetSession(MENU_MAIN);

    // Remplisage --------------------------------------------------------------

    #ifdef COMPILE_FOR_WINDOWS
    DWORD bufSize = 255;
    char userName[bufSize];
    GetUserName(userName, &bufSize);
    m_controls.playmenu.playerName->SetLabel(userName);
    #endif

    // A propos
    {
        stringstream ss;

        fstream file("README.txt");
        ss << file.rdbuf() << endl;
        file.close();

        ss << "Build in " << __DATE__;

        m_controls.aboutText->Write(ss.str());
    }

    // Menu des scores
    {
        sort(globalSettings.availableScore.begin(),
             globalSettings.availableScore.end(),
             ScoreSortByValue);

        string txt = WriteScore(globalSettings.availableScore);

        m_controls.score.scoreText->Write(txt);

        m_controls.score.sortType
                ->Push("Tri par date", 1)
                .Push("Tri par points", 2);
    }

    // Menu de configuration des touches
    {
        for(map<string, int>::const_iterator itt = globalSettings.control.keyboard.begin();
            itt != globalSettings.control.keyboard.end(); itt++)
            m_guiManager->GetControl<gui::KeyConfig > (itt->first, MENU_SETTING_KEYS)
            ->SetKeyCode(itt->second, SDLDevice::GetKeyName(itt->second));

        for(map<string, int>::const_iterator itt = globalSettings.control.mouse.begin();
            itt != globalSettings.control.mouse.end(); itt++)
            m_guiManager->GetControl<gui::KeyConfig > (itt->first, MENU_SETTING_KEYS)
            ->SetMouseCode(itt->second, SDLDevice::GetMouseName(itt->second));
    }

    // Menu de configuration vidéo
    {
        m_controls.settings.usePpe->Push("Désactiver", false).Push("Activer", true);
        m_controls.settings.usePpe->SetCurrent(globalSettings.video.usePpe);

        m_controls.settings.fullscreen->Push("Fenêtrer", false).Push("Plein-écran", true);
        m_controls.settings.fullscreen->SetCurrent(globalSettings.video.fullScreen);
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
                Push("Désactiver", 0).
                Push("2x", 2).
                Push("4x", 4).
                Push("8x", 8).
                Push("16x", 16)
                ;

        m_controls.settings.antiAliasing->SetCurrent(AAAvailable[cutAA]);
    }

    {
        vector<Vector2i> availableScreen = SDLDevice::GetAvilableSceeenSize();
        unsigned curSizeIndex = 0;

        for(unsigned i = 0; i < availableScreen.size(); i++)
        {
            if(screenSize == availableScreen[i])
                curSizeIndex = i;

            stringstream ss;
            ss << availableScreen[i].x << "x" << availableScreen[i].y << "x32";

            m_controls.settings.screenSize->Push(ss.str(), availableScreen[i]);
        }

        m_controls.settings.screenSize->SetCurrent(curSizeIndex);
    }

    // Menu de partie

    // Temps de la partie
    m_controls.playmenu.playerCount->SetValue(4);

    m_controls.playmenu.timeSelect->
            Push("60 sec", 60).
            Push("5 min", 60 * 5).
            Push("20 min", 60 * 20).
            Push("Infinie", 0);

    m_controls.playmenu.timeSelect->SetCurrent(1);

    // Mod de la partie
    m_controls.playmenu.modSelect->
            Push("Frag", FRAG).
            Push("Alone", ALONE).
            Push("Team", TEAM);

    m_controls.playmenu.modSelect->SetCurrent(0);

    UpdateGuiContent();
}

void AppManager::UpdateGuiContent()
{
    // Ball a jouer

    m_controls.playmenu.playerSelect->DeleteAll();

    for(unsigned i = 0; i < globalSettings.availablePlayer.size(); i++)
        m_controls.playmenu.playerSelect->Push(globalSettings.availablePlayer[i].name, i);

    m_controls.playmenu.playerSelect->SetCurrent(tools::rand(0, globalSettings.availablePlayer.size()));

    // Carte a jouer

    m_controls.playmenu.mapSelect->DeleteAll();

    for(unsigned i = 0; i < globalSettings.availableMap.size(); i++)
    {
        m_controls.playmenu.mapSelect->Push(globalSettings.availableMap[i].name, i);
        m_controls.edit.mapSelect->Push(globalSettings.availableMap[i].name, i);
    }

    m_controls.playmenu.mapSelect->SetCurrent(tools::rand(0, globalSettings.availableMap.size()));
}

void AppManager::SetupBackgroundScene()
{
    using namespace tbe::scene;

    // SCENE -------------------------------------------------------------------

    m_sceneManager->ClearAll();

    MeshParallelScene* meshScene = new MeshParallelScene;
    m_sceneManager->AddParallelScene("nodeScene", meshScene);

    m_logo = new OBJMesh("data/scene/logo.obj");
    meshScene->AddMesh("logo", m_logo);

    m_camera = new Camera(Camera::TARGET_RELATIVE);
    m_camera->SetRotate(Vector2f(180, 5));
    m_sceneManager->AddCamera("mainCamera", m_camera);

    DiriLight* light0 = new DiriLight;
    m_sceneManager->AddDynamicLight("light0", light0);

    m_sceneManager->SetAmbientLight(0.1);

    // PPE ---------------------------------------------------------------------

    if(globalSettings.video.usePpe)
    {
        using namespace ppe;

        m_ppeManager->ClearAll();

        #if 0
        BlurEffect* blur = new BlurEffect;
        blur->SetPasse(4);
        m_ppeManager->AddPostEffect("", blur);
        #endif

        BloomEffect* bloom = new BloomEffect;
        bloom->SetThreshold(0.1);
        bloom->SetIntensity(1.0);
        bloom->SetBlurPass(10);
        m_ppeManager->AddPostEffect("", bloom);

        MotionBlurEffect* mblur = new MotionBlurEffect;
        mblur->SetRttFrameSize(tools::next_p2(globalSettings.video.screenSize) / 2);
        m_ppeManager->AddPostEffect("", mblur);
    }
}

void AppManager::ProcessMainMenuEvent()
{
    if(m_guiManager->GetControl("newgame")->IsActivate())
        m_guiManager->SetSession(MENU_PLAY);

    else if(m_guiManager->GetControl("setting")->IsActivate())
        m_guiManager->SetSession(MENU_SETTING);

    else if(m_guiManager->GetControl("editor")->IsActivate())
        m_guiManager->SetSession(MENU_EDIT);

    else if(m_guiManager->GetControl("about")->IsActivate())
        m_guiManager->SetSession(MENU_ABOUT);

    else if(m_guiManager->GetControl("score")->IsActivate())
        m_guiManager->SetSession(MENU_SCORE);
}

void AppManager::ProcessPlayMenuEvent()
{
    if(m_controls.playmenu.mapSelect->IsActivate());

    else if(m_controls.playmenu.playerSelect->IsActivate());

    else if(m_guiManager->GetControl("play")->IsActivate())
    {
        unsigned indexOfLevel = m_controls.playmenu.mapSelect->GetData().GetValue<unsigned>();
        unsigned indexOfPlayer = m_controls.playmenu.playerSelect->GetData().GetValue<unsigned>();

        PlaySetting playSetting;

        playSetting.playMap = globalSettings.availableMap[indexOfLevel].file;
        playSetting.playerModel = globalSettings.availablePlayer[indexOfPlayer].file;
        playSetting.playerName = m_controls.playmenu.playerName->GetLabel();
        playSetting.playerCount = m_controls.playmenu.playerCount->GetValue();
        playSetting.playMod = m_controls.playmenu.modSelect->GetData().GetValue<unsigned>();
        playSetting.playTime = m_controls.playmenu.timeSelect->GetData().GetValue<unsigned>();

        ExecuteGame(playSetting);

        SetupBackgroundScene();
        m_guiManager->SetSession(MENU_PLAY);
    }

    else if(m_guiManager->GetControl("return")->IsActivate())
        m_guiManager->SetSession(MENU_MAIN);
}

void AppManager::ProcessEditMenuEvent()
{
    if(m_controls.edit.editMap->IsActivate())
    {
        unsigned index = m_controls.edit.mapSelect->GetData().GetValue<unsigned>();
        Settings::MapInfo& mi = globalSettings.availableMap[index];

        EditSetting es;
        es.createNew = false;
        es.editMap = mi.file;

        ExecuteEditor(es);

        SetupBackgroundScene();
        m_guiManager->SetSession(MENU_EDIT);
    }
    else if(m_controls.edit.newMap->IsActivate())
    {
        EditSetting es;
        es.createNew = true;

        ExecuteEditor(es);

        SetupBackgroundScene();
        m_guiManager->SetSession(MENU_EDIT);
    }
    else if(m_controls.edit.ret->IsActivate())
        m_guiManager->SetSession(MENU_MAIN);
}

void AppManager::ProcessSettingMenuEvent()
{
    if(m_guiManager->GetControl("keySetting")->IsActivate())
        m_guiManager->SetSession(MENU_SETTING_KEYS);

    if(m_guiManager->GetControl("return")->IsActivate())
        m_guiManager->SetSession(MENU_MAIN);

    else if(m_guiManager->GetControl("apply")->IsActivate())
    {
        globalSettings.FillWindowSettingsFromGui(m_guiManager);

        globalSettings.SaveSetting();

        SetupVideoMode();

        m_guiManager->SetSession(MENU_MAIN);
    }
}

void AppManager::ProcessSettingKeyMenuEvent()
{
    if(m_guiManager->GetControl("return")->IsActivate())
        m_guiManager->SetSession(MENU_SETTING);

    else if(m_guiManager->GetControl("apply")->IsActivate())
    {
        globalSettings.FillControlSettingsFromGui(m_guiManager);

        m_guiManager->SetSession(MENU_SETTING);
    }
}

void AppManager::ProcessScoreMenuEvent()
{
    if(m_controls.score.ret->IsActivate())
        m_guiManager->SetSession(MENU_MAIN);

    else if(m_controls.score.sortType->IsActivate())
    {
        switch(m_controls.score.sortType->GetData().GetValue<int>())
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

        const string& txt = WriteScore(globalSettings.availableScore);
        m_controls.score.scoreText->Write(txt);
    }
}

void AppManager::ExecuteMenu()
{
    using namespace tbe::gui;
    using namespace tbe::scene;

    #ifndef DISABLE_MUSIC
    FSOUND_Stream_Play(FSOUND_FREE, m_mainMusic);
    #endif

    bool done = false;
    while(!done)
    {
        if(m_fpsMng->DoARender())
        {
            m_gameEngine->PollEvent();

            if(m_eventMng->notify == EventManager::EVENT_WIN_QUIT)
                done = true;

            switch(m_guiManager->GetSession())
            {
                case MENU_MAIN:
                    if(m_guiManager->GetControl("quit")->IsActivate())
                        done = true;
                    else
                        ProcessMainMenuEvent();
                    break;

                case MENU_SCORE:
                    ProcessScoreMenuEvent();
                    break;

                case MENU_SETTING_KEYS:
                    ProcessSettingKeyMenuEvent();
                    break;

                case MENU_SETTING:
                    ProcessSettingMenuEvent();
                    break;

                case MENU_ABOUT:
                    if(m_guiManager->GetControl("return")->IsActivate())
                        m_guiManager->SetSession(MENU_MAIN);
                    break;

                case MENU_PLAY:
                    ProcessPlayMenuEvent();
                    break;

                case MENU_EDIT:
                    ProcessEditMenuEvent();
                    break;
            }

            m_camera->SetPos(-m_camera->GetTarget() * 8.0f);

            m_logo->GetMatrix().SetRotateY(0.01);

            m_gameEngine->BeginScene();

            if(globalSettings.video.usePpe)
            {
                Rtt* ppeRtt = m_ppeManager->GetRtt();
                ppeRtt->Use(true);
                ppeRtt->Clear();
                m_sceneManager->Render();
                ppeRtt->Use(false);
                m_ppeManager->Render();
            }

            else
            {
                m_sceneManager->Render();
            }

            m_guiManager->Render();
            m_gameEngine->EndScene();
        }

        m_fpsMng->Update();
    }
}

void AppManager::ExecuteGame(const PlaySetting& playSetting)
{
    cout << "ExecuteGame :" << endl
            << "playLevel = " << playSetting.playMap << endl
            << "playMod = " << playSetting.playMod << endl
            << "playTime = " << playSetting.playTime << endl
            << "playerModel = " << playSetting.playerModel << endl
            << "playerName = " << playSetting.playerName << endl;

    m_sceneManager->ClearAll();
    m_ppeManager->ClearAll();

    #ifndef DISABLE_MUSIC
    FSOUND_Stream_Stop(m_mainMusic);
    #endif

    // Affichage de l'ecran de chargement --------------------------------------

    m_guiManager->SetSession(MENU_LOAD);

    m_guiManager->GetControl<gui::TextBox > ("load:stateText")->Write("Chargement en cours...");

    m_gameEngine->BeginScene();
    m_guiManager->Render();
    m_gameEngine->EndScene();

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

    gameManager->SetupMap(playSetting);
    gameManager->SetupGui();

    // Attente de réponse ------------------------------------------------------

    m_guiManager->SetSession(MENU_LOAD);

    m_guiManager->GetControl<gui::TextBox > ("load:stateText")->Write("Appuyer sur \"Espace\" pour continuer...");

    while(!m_eventMng->keyState[EventManager::KEY_SPACE])
    {
        m_gameEngine->PollEvent();
        m_gameEngine->BeginScene();
        m_guiManager->Render();
        m_gameEngine->EndScene();
    }

    // Début du jeu ------------------------------------------------------------

    cout << "Start game" << endl;

    gameManager->OnStartGame();

    while(gameManager->running)
    {
        if(m_fpsMng->DoARender())
        {
            gameManager->EventProcess();
            gameManager->GameProcess();
            gameManager->HudProcess();

            gameManager->Render();
        }

        m_fpsMng->Update();
    }

    cout << "End game" << endl;

    delete gameManager;

    #ifndef DISABLE_MUSIC
    FSOUND_Stream_Play(FSOUND_FREE, m_mainMusic);
    #endif
}

void AppManager::ExecuteEditor(const EditSetting& editSetting)
{
    cout << "ExecuteEditor" << endl;

    m_sceneManager->ClearAll();
    m_ppeManager->ClearAll();

    #ifndef DISABLE_MUSIC
    FSOUND_Stream_Stop(m_mainMusic);
    #endif

    // Affichage de l'ecran de chargement --------------------------------------

    m_guiManager->SetSession(MENU_LOAD);

    m_guiManager->GetControl<gui::TextBox > ("load:stateText")->Write("Chargement en cours...");

    m_gameEngine->BeginScene();
    m_guiManager->Render();
    m_gameEngine->EndScene();

    // Chargement de la carte --------------------------------------------------

    EditorManager* editorManager = new EditorManager(this);

    editorManager->SetupMap(editSetting);
    editorManager->SetupGui();

    // Attente de réponse ------------------------------------------------------

    m_guiManager->SetSession(MENU_LOAD);

    m_guiManager->GetControl<gui::TextBox > ("load:stateText")->Write("Appuyer sur \"Espace\" pour continuer...");

    while(!m_eventMng->keyState[EventManager::KEY_SPACE])
    {
        m_gameEngine->PollEvent();
        m_gameEngine->BeginScene();
        m_guiManager->Render();
        m_gameEngine->EndScene();
    }

    // Début du jeu ------------------------------------------------------------

    while(editorManager->running)
    {
        if(m_fpsMng->DoARender())
        {
            editorManager->EventProcess();
            editorManager->HudProcess();

            editorManager->Render();
        }

        m_fpsMng->Update();
    }

    delete editorManager;

    #ifndef DISABLE_MUSIC
    FSOUND_Stream_Play(FSOUND_FREE, m_mainMusic);
    #endif
}

tbe::EventManager* AppManager::GetEventMng() const
{
    return m_eventMng;
}

tbe::ticks::FpsManager* AppManager::GetFpsMng() const
{
    return m_fpsMng;
}

tbe::scene::SceneManager* AppManager::GetSceneMng() const
{
    return m_sceneManager;
}

tbe::gui::GuiManager* AppManager::GetGuiMng() const
{
    return m_guiManager;
}

tbe::SDLDevice* AppManager::GetGameEngine() const
{
    return m_gameEngine;
}

int main(int argc, char** argv)
{
    #ifndef THEBALL_COMPILE_DEBUG
    ofstream log("theball.log");
    streambuf* defaultCout = cout.rdbuf();
    cout.rdbuf(log.rdbuf());
    #endif

    try
    {
        cout
                << "theBall (b4n92uid)" << endl
                << "Start in " << ticks::Clock::Date("%X - %x") << endl
                << "Build in " << __DATE__ << endl
                << "Current dir : " << SDLDevice::GetCWD() << endl;

        AppManager theBall;

        if(argc > 1)
        {
            if(string(argv[1]) == "play")
            {
                // NOTE play <name> <player> <level> <playerCount> <playMod> <playtime>

                AppManager::PlaySetting ps;
                ps.playerName = argv[2];
                ps.playerModel = argv[3];
                ps.playMap = argv[4];
                ps.playerCount = tools::StrToNum<unsigned>(argv[5]);
                ps.playMod = tools::StrToNum<unsigned> (argv[6]);
                ps.playTime = tools::StrToNum<int>(argv[7]);

                theBall.ExecuteGame(ps);
            }

            else if(string(argv[1]) == "edit")
            {
                // NOTE edit <level>

                AppManager::EditSetting es;
                es.editMap = argv[2];
                es.createNew = false;

                theBall.ExecuteEditor(es);
            }
        }

        else
        {
            theBall.ExecuteMenu();
        }

    }

    catch(std::exception& e)
    {
        #ifdef COMPILE_FOR_WINDOWS
        MessageBox(0, e.what(), "Fatal error", MB_ICONERROR);
        #endif
        cout << e.what() << endl;
    }

    catch(...)
    {
        cout << "Unhandled exception catch" << endl;
    }

    cout << "Exit success :)" << endl;

    #ifndef THEBALL_COMPILE_DEBUG
    cout.rdbuf(defaultCout);
    log.close();
    #endif

    return 0;
}
