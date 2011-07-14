/*
 * File:   Settings.cpp
 * Author: b4n92uid
 *
 * Created on 01 février 2010, 20:34
 */

#include "GameManager.h"

#include "AppManager.h"
#include "SoundManager.h"
#include "MaterialManager.h"
#include "UserControl.h"

#include "StaticElement.h"
#include "Player.h"

#include "Define.h"
#include "Tools.h"

#include "Weapon.h"
#include "Bullet.h"

#include "Power.h"
#include "BulletTime.h"
#include "Boost.h"

#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <cmath>

using namespace std;
using namespace tbe;

GameManager::GameManager(AppManager* appManager)
{
    using namespace scene;

    cout << "--- Initing game manager" << endl;

    manager.app = appManager;

    worldSettings = manager.app->globalSettings.world;

    m_running = true;

    manager.gameEngine = manager.app->getGameEngine();
    manager.gameEngine->setGrabInput(true);
    manager.gameEngine->setMouseVisible(false);

    manager.fps = manager.gameEngine->getFpsManager();
    manager.gui = manager.gameEngine->getGuiManager();
    manager.scene = manager.gameEngine->getSceneManager();
    manager.ppe = manager.gameEngine->getPostProcessManager();

    parallelscene.light = new scene::LightParallelScene;
    manager.scene->addParallelScene(parallelscene.light);

    parallelscene.meshs = new scene::MeshParallelScene;
    manager.scene->addParallelScene(parallelscene.meshs);

    parallelscene.newton = new scene::NewtonParallelScene;
    parallelscene.newton->setGravity(worldSettings.gravity);
    NewtonSetSolverModel(parallelscene.newton->getNewtonWorld(), 8);
    NewtonSetFrictionModel(parallelscene.newton->getNewtonWorld(), 1);
    manager.scene->addParallelScene(parallelscene.newton);

    parallelscene.particles = new scene::ParticlesParallelScene;
    manager.scene->addParallelScene(parallelscene.particles);

    parallelscene.marks = new scene::MapMarkParallelScene;
    manager.scene->addParallelScene(parallelscene.marks);

    manager.fmodsys = appManager->getFmodSystem();

    manager.parser = new SceneParser(manager.scene);
    manager.parser->setMeshScene(parallelscene.meshs);
    manager.parser->setParticlesScene(parallelscene.particles);
    manager.parser->setLightScene(parallelscene.light);
    manager.parser->setMarkScene(parallelscene.marks);

    manager.material = new MaterialManager(this);
    manager.sound = new SoundManager(this);
    manager.script = new ScriptManager(this);

    m_timeTo = TIME_TO_PLAY;

    m_gameOver = false;

    m_cursorOnPlayer = false;

    m_winnerPlayer = NULL;

    m_camera = new scene::Camera(scene::Camera::TARGET_RELATIVE);
    m_camera->rotate(0);
    manager.scene->addCamera(m_camera);

    m_numslot[38] = 1;
    m_numslot[233] = 2;
    m_numslot[34] = 3;
    m_numslot[39] = 4;
    m_numslot[40] = 5;
    m_numslot[45] = 6;
    m_numslot[232] = 7;
    m_numslot[95] = 8;
    m_numslot[231] = 9;
    m_numslot[224] = 0;

    m_earthquake.intensity = 0;
    m_earthquake.physical = false;
}

GameManager::~GameManager()
{
    cout << "--- Cleaning game manager" << endl;

    manager.gui->setSession(MENU_MAPCHOOSE);

    manager.gameEngine->setGrabInput(false);
    manager.gameEngine->setMouseVisible(true);

    manager.gui->destroySession(SCREEN_GAMEOVER);
    manager.gui->destroySession(SCREEN_PAUSEMENU);
    manager.gui->destroySession(SCREEN_HUD);

    delete manager.sound;
    delete manager.material;
    delete manager.script;
    delete manager.parser;

    Boost::clearSingleTone(this);
    BulletTime::clearSingleTone(this);

    manager.scene->clearAll();
}

void GameManager::setupMap(const Settings::PartySetting& playSetting)
{
    m_playSetting = playSetting;

    cout << "--- Loading level" << endl;

    // SCENE -------------------------------------------------------------------

    manager.parser->loadScene(m_playSetting.map.filename);
    manager.parser->buildScene();

    for(Iterator<scene::Mesh*> it = parallelscene.meshs->iterator(); it; it++)
    {
        StaticElement* elem = new StaticElement(this, *it);

        registerElement(elem);
    }

    for(Iterator<scene::ParticlesEmiter*> it = parallelscene.particles->iterator(); it; it++)
    {
        MapElement* elem = new MapElement(this);
        elem->setVisualBody(*it);
        elem->setId(it->getName());

        registerElement(elem);
    }

    for(Iterator<scene::MapMark*> it = parallelscene.marks->iterator(); it; it++)
    {
        MapElement* elem = new MapElement(this);
        elem->setVisualBody(*it);
        elem->setId(it->getName());

        registerElement(elem);
    }

    for(Iterator<scene::Light*> it = parallelscene.light->iterator(); it; it++)
    {
        MapElement* elem = new MapElement(this);
        elem->setVisualBody(*it);
        elem->setId(it->getName());

        registerElement(elem);
    }

    // Marge
    map.aabb.add(AABB(Vector3f(-8, -8, -8), Vector3f(8, 64, 8)));

    AABB newtonWordSize = map.aabb;
    newtonWordSize.add(Vector3f(32.0f));
    parallelscene.newton->setWorldSize(newtonWordSize);

    scene::Fog* fog = manager.scene->getFog();

    manager.scene->setZFar(fog->isEnable() ? fog->getEnd() + 8 : map.aabb.getLength() * 2);
    manager.scene->updateViewParameter();

    // PLAYERS -----------------------------------------------------------------

    m_userPlayer = new Player(this, m_playSetting.player.nick, m_playSetting.player.model);
    m_userPlayer->attachController(new UserControl(this));

    registerPlayer(m_userPlayer);

    manager.scene->getRootNode()->addChild(m_userPlayer->getVisualBody());

    // SCRIPT ------------------------------------------------------------------

    manager.script->load(m_playSetting.map.script);

    // PPE ---------------------------------------------------------------------

    const Settings::Video& vidSets = manager.app->globalSettings.video;

    if(vidSets.ppeUse)
    {
        using namespace tbe::ppe;

        ppe.dammage = new ColorEffect;
        ppe.dammage->setInternalPass(true);
        ppe.dammage->setFusionMode(ColorEffect::MULTIPLICATION_COLOR);
        ppe.dammage->setColor(Vector4f(1, 0, 0, 1));
        ppe.dammage->setEnable(false);
        manager.ppe->addPostEffect("dammageEffect", ppe.dammage);

        ppe.gameover = new BlurEffect;
        ppe.gameover->setPasse(1);
        ppe.gameover->setEnable(false);
        manager.ppe->addPostEffect("gameoverEffect", ppe.gameover);

        ppe.bloom = new BloomEffect;
        ppe.bloom->setRttFrameSize(vidSets.ppeSize);
        ppe.bloom->setIntensity(0.75);
        ppe.bloom->setThreshold(0.25);
        ppe.bloom->setBlurPass(10);
        manager.ppe->addPostEffect("bloomEffect", ppe.bloom);
    }
}

void GameManager::setupGui()
{
    using namespace boost;
    using namespace tbe::gui;

    cout << "--- Loading GUI" << endl;

    Settings::Gui& guisets = manager.app->globalSettings.gui;

    Vector2i screenSize = manager.app->globalSettings.video.screenSize;

    property_tree::ptree parser;

    property_tree::read_ini("gui.ini", parser);

    #define path(s) parser.get<string>(s)

    GuiSkin* ingame_skin = new GuiSkin;

    ingame_skin->button(guisets.button);
    ingame_skin->buttonSize(ingame_skin->button.getSize());
    ingame_skin->buttonMetaCount = 4;

    ingame_skin->pencil(path("game.fontpath"), parser.get<int>("game.fontsize"));
    ingame_skin->pencil.setColor(1);

    Pencil whiteSmlPen(path("game.fontpath"), parser.get<int>("game.statusfontsize"));
    whiteSmlPen.setColor(parser.get<Vector4f > ("game.statusfontcolor"));

    Pencil whiteBigPen(path("game.fontpath"), parser.get<int>("game.msgfontsize"));
    whiteBigPen.setColor(parser.get<Vector4f > ("game.statusfontcolor"));

    // GameOver ----------------------------------------------------------------

    manager.gui->setSession(SCREEN_GAMEOVER, ingame_skin);

    Texture black;
    black.build(128, Vector4f(0, 0, 0, 1));

    hud.background.gameover = manager.gui->addImage("hud.background.gameover", black);
    hud.background.gameover->setSize(screenSize);
    hud.background.gameover->setOpacity(0);
    hud.background.gameover->setEnable(false);

    manager.gui->addLayout(Layout::Vertical, 10, 10);
    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayout(Layout::Horizental, 10, 10);
    manager.gui->addLayoutStretchSpace();

    hud.gameover = manager.gui->addTextBox("hud.gameover");
    hud.gameover->setSize(Vector2f(screenSize) * Vector2f(0.75, 0.75));
    hud.gameover->setDefinedSize(true);
    hud.gameover->setPencil(whiteBigPen);
    hud.gameover->setBackground(guisets.textbox);
    hud.gameover->setTextPadding(16);
    hud.gameover->setBackgroundMask(guisets.maskH);
    hud.gameover->setArrowTexture(guisets.udarrow);
    hud.gameover->setTextAlign(gui::LEFT | gui::TOP);

    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();
    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();

    // Pause Menu --------------------------------------------------------------

    manager.gui->setSession(SCREEN_PAUSEMENU);

    Image* backPause = manager.gui->addImage("00:background", path("game.pausemenu"));
    backPause->setSize(screenSize);

    manager.gui->addLayout(Layout::Horizental, 10, 10);
    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayout(Layout::Vertical, 10, 10)->setAlign(Layout::MIDDLE);
    manager.gui->addLayoutStretchSpace();

    hud.playmenu.quit = manager.gui->addButton("hud.playmenu.quit", "Quitter");

    hud.playmenu.ret = manager.gui->addButton("hud.playmenu.ret", "Retour");

    manager.gui->addLayoutSpace(64);

    TextBox* pauseLabel = manager.gui->addTextBox("hud.pauseLabel");
    pauseLabel->setPencil(whiteBigPen);
    pauseLabel->write("Menu Pause !");

    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();
    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();

    manager.gui->endLayout();

    // HUD ---------------------------------------------------------------------

    Texture white;
    white.build(512, 1);

    manager.gui->setSession(SCREEN_HUD, ingame_skin);

    hud.background.dammage = manager.gui->addImage("0:hud.background.dammage", path("game.dammage"));
    hud.background.dammage->setSize(screenSize);
    hud.background.dammage->setEnable(false);

    hud.background.flash = manager.gui->addImage("1:hud.background.flash", path("game.flash"));
    hud.background.flash->setSize(screenSize);
    hud.background.flash->setEnable(false);

    StateShow* croshair = manager.gui->addStateShow("0:croshair", path("game.crosshair"), 4);
    croshair->setPos(Vector2f(screenSize) / 2.0f - Vector2f(64) / 2.0f);
    croshair->setSize(64);

    Image* core_weapon = manager.gui->addImage("core_weapon", path("game.coreweapon"));
    Vector2f weaponParentPos(16);
    core_weapon->setPos(weaponParentPos);

    Image* core_power = manager.gui->addImage("core_power", path("game.corepower"));
    Vector2f powerParentPos(screenSize.x - core_power->getSize().x - 16, 16);
    core_power->setPos(powerParentPos);

    hud.ammoGauge = manager.gui->addGauge("hud.ammoGauge", "");
    hud.ammoGauge->setPos(weaponParentPos + Vector2f(96, 32));
    hud.ammoGauge->setSize(Vector2f(256 - 96, 32));
    hud.ammoGauge->setBackground(path("game.ammogauge"));
    hud.ammoGauge->setSmooth(true, 1);

    hud.ammo = manager.gui->addTextBox("hud.ammo");
    hud.ammo->setPos(weaponParentPos + Vector2f(105, 8));
    hud.ammo->setSize(Vector2f(96, 32));
    hud.ammo->setTextAlign(gui::HCENTER | gui::VCENTER);
    hud.ammo->setDefinedSize(true);

    hud.weaponIcon = manager.gui->addStateShow("hud.weaponIcon", path("game.weaponicon"), 4);
    hud.weaponIcon->setSize(96);
    hud.weaponIcon->setPos(weaponParentPos);

    hud.energyGauge = manager.gui->addGauge("hud.energyGauge", "");
    hud.energyGauge->setPos(powerParentPos + Vector2f(0, 32));
    hud.energyGauge->setSize(Vector2f(256 - 96, 32));
    hud.energyGauge->setBackground(path("game.energygauge"));
    hud.energyGauge->setReverse(true);

    hud.life = manager.gui->addTextBox("hud.life");
    hud.life->setPos(powerParentPos + Vector2f(64, 14));
    hud.life->setTextAlign(gui::HCENTER);

    hud.powerIcon = manager.gui->addStateShow("hud.powerIcon", path("game.powericon"), 4);
    hud.powerIcon->setSize(96);
    hud.powerIcon->setPos(powerParentPos + Vector2f(160, 0));

    // -------- Log
    manager.gui->addLayout(Layout::Horizental, 0, 10);
    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayout(Layout::Vertical, 0, 10);
    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayoutStretchSpace();

    hud.log = manager.gui->addTextBox("hud.log");
    hud.log->setPencil(whiteBigPen);
    hud.log->setBackground(guisets.textbox);
    hud.log->setBackgroundMask(path("game.msgmask"));
    hud.log->setTextPadding(16);
    hud.log->setEnable(false);
    hud.log->setPos(false);

    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();
    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();
    // --------

    // -------- State
    manager.gui->addLayout(Layout::Vertical, 0, 10);
    manager.gui->addLayoutStretchSpace();
    hud.state = manager.gui->addTextBox("hud.state");
    hud.state->setPencil(whiteSmlPen);
    hud.state->setBackground(guisets.textbox);
    hud.state->setBackgroundMask(path("game.statemask"));
    hud.state->setTextPadding(8);
    manager.gui->endLayout();
    // --------

    #undef path
}

void GameManager::onStartGame()
{
}

tbe::Vector3f GameManager::getRandomPosOnTheFloor()
{
    Vector3f randPos;

    // Réduit le champs d'application de 5%
    float factor = 5 * map.aabb.getLength() / 100;

    do
    {
        randPos = (map.aabb - Vector3f(factor)).randPos();
        randPos.y = 1;
        randPos = parallelscene.newton->findFloor(randPos);
    }
    while(randPos.y < map.aabb.min.y);

    return randPos;
}

tbe::Vector3f GameManager::getRandomPosOnTheFloor(Vector3f pos, float radius)
{
    Vector3f randPos;

    do
    {
        randPos = pos + AABB(radius).randPos();
        randPos.y = 1;
        randPos = parallelscene.newton->findFloor(randPos);
    }
    while(randPos.y < map.aabb.min.y);

    return randPos;
}

void GameManager::display(std::string msg)
{
    hud.log->write(msg);
    hud.log->setEnable(true);

    m_logClock.snapShoot();
}

void GameManager::status(std::string msg)
{
    hud.state->write(msg);
}

void GameManager::processDevelopperCodeEvent()
{
    EventManager* event = manager.gameEngine->getEventManager();

    if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
        // F1 : Dammage 10%
        if(event->keyState[EventManager::KEY_F1])
        {
            m_userPlayer->takeDammage(10);
        }

        // F2 : Kill
        if(event->keyState[EventManager::KEY_F2])
            m_userPlayer->kill(NULL);

        if(event->keyState[EventManager::KEY_F5])
        {
            Player::Array copy = m_players;

            for(unsigned i = 0; i < copy.size(); i++)
                if(copy[i] != m_userPlayer)
                {
                    unregisterPlayer(copy[i]);
                    delete copy[i];
                }

            m_players.clear();
        }

        // F6 : TEST BOT
        if(event->keyState[EventManager::KEY_F6])
        {
            unsigned select = math::rand(0, manager.app->globalSettings.availablePlayer.size());

            Settings::PlayerInfo& pi = manager.app->globalSettings.availablePlayer[select];

            Player* player = new Player(this, "TEST_BOT", pi.model);
            player->attachController(NULL);

            registerPlayer(player);

            manager.scene->getRootNode()->addChild(player->getVisualBody());
        }

        // F6 : TEST BOT AI
        if(event->keyState[EventManager::KEY_F7])
        {
            unsigned select = math::rand(0, manager.app->globalSettings.availablePlayer.size());

            Settings::PlayerInfo& pi = manager.app->globalSettings.availablePlayer[select];

            Player* player = new Player(this, "TEST_BOT_AI", pi.model);

            registerPlayer(player);

            manager.scene->getRootNode()->addChild(player->getVisualBody());
        }

        // F9 : Next level
        if(event->keyState[EventManager::KEY_F9])
        {
            setGameOver(m_userPlayer, "Next Level >>");
        }

        if(event->keyState['p'])
        {
            m_earthquake.intensity = 1.0;
            m_earthquake.physical = true;

            manager.sound->playSound("quake");
        }

        if(event->keyState['o'])
        {
            flashEffect();
        }
    }
}

void GameManager::eventProcess()
{
    using namespace tbe::scene;

    manager.gameEngine->pollEvent();

    EventManager* event = manager.gameEngine->getEventManager();

    // Session de jeu
    if(m_timeTo == TIME_TO_PLAY)
    {
        manager.gui->setSession(SCREEN_HUD);

        if(m_userPlayer->isKilled())
            return;

        // Vue
        if(event->notify == EventManager::EVENT_MOUSE_MOVE)
            manager.scene->getCurCamera()->rotate(event->mousePosRel);

        // Séléction d'arme
        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(m_numslot.count(event->lastActiveKey.first))
                m_userPlayer->slotWeapon(m_numslot[event->lastActiveKey.first]);
        }

        if(event->notify == EventManager::EVENT_KEY_UP
           && event->lastActiveKey.first == EventManager::KEY_TAB)
        {
            manager.gameEngine->setMouseVisible(false);

            m_timeTo = TIME_TO_PLAY;
        }

        // Touche de pause (Esc)
        if(event->keyState[EventManager::KEY_ESCAPE])
        {
            manager.gameEngine->setGrabInput(false);
            manager.gameEngine->setMouseVisible(true);

            if(m_userPlayer->getCurPower())
                m_userPlayer->getCurPower()->diactivate();

            m_timeTo = TIME_TO_PAUSE;
        }

        // Code developper
        #ifdef THEBALL_COMPILE_DEBUG
        processDevelopperCodeEvent();
        #endif
    }

        // Session de pause
    else if(m_timeTo == TIME_TO_PAUSE)
    {
        manager.gui->setSession(SCREEN_PAUSEMENU);

        bool done = false;
        while(!done)
        {
            manager.gameEngine->pollEvent();

            if(hud.playmenu.ret->isActivate())
            {
                manager.gameEngine->setGrabInput(true);
                manager.gameEngine->setMouseVisible(false);

                done = true;
                m_timeTo = TIME_TO_PLAY;
            }

            else if(hud.playmenu.quit->isActivate())
            {
                manager.gameEngine->setGrabInput(true);
                manager.gameEngine->setMouseVisible(false);

                m_running = false;
                done = true;
            }

            manager.gameEngine->beginScene();
            manager.gui->render();
            manager.gameEngine->endScene();
        }
    }

    else if(m_timeTo == TIME_TO_GAMEOVER)
    {
        if(event->keyState[EventManager::KEY_SPACE]
           && m_validGameOver.isEsplanedTime(2000))
            m_running = false;
    }
}

void EarthQuakeProcess(const NewtonBody* body, void* userData)
{
    MapElement* elem = (MapElement*)NewtonBodyGetUserData(body);

    scene::NewtonNode* pbody = elem->getPhysicBody();

    float intensity = *(float*)userData;

    Vector3f pointPos = pbody->getPos() + AABB(1).randPos();

    NewtonBodyAddImpulse(body, AABB(intensity * 4).randPos().normalize(), pointPos);
}

void GameManager::gameProcess()
{
    using namespace tbe;
    using namespace tbe::scene;

    if(m_gameOver)
        return;

    for(unsigned i = 0; i < map.staticElements.size(); i++)
    {
        StaticElement* elem = map.staticElements[i];

        if(!map.aabb.isInner(elem->getVisualBody()))
            elem->resetInitialMatrix();
    }

    if(m_backImpulse.intensity > 0)
    {

        foreach(Vector3f & pos, m_playerPosRec)
        {
            pos += -m_camera->getTarget() * m_backImpulse.intensity;
        }

        m_backImpulse.intensity -= m_backImpulse.push;
    }

    /*
     * Gestion de l'effet tremblement de terre :(
     */
    if(m_earthquake.intensity > 0)
    {

        foreach(Vector3f& pos, m_playerPosRec)
        {
            Vector3f quake = AABB(0.5).randPos();

            pos += quake * m_earthquake.intensity;
        }

        if(m_earthquake.physical)
        {
            NewtonWorldForEachBodyInAABBDo(parallelscene.newton->getNewtonWorld(),
                                           map.aabb.min, map.aabb.max,
                                           EarthQuakeProcess, &m_earthquake.intensity);
        }
        m_earthquake.intensity -= 0.01;
    }

    /*
     * Pour chaque joueurs
     *  - les joueurs mort pour les reconstruires
     *  - les joueurs en fin de vie pour la préparation a la mort ;)
     *  - les joueurs hors de l'arene pour les remetr en place
     */
    for(unsigned i = 0; i < m_players.size(); i++)
    {
        Player* player = m_players[i];

        player->process();

        if(player->isKilled())
        {
            if(player == m_userPlayer)
            {
                manager.gameEngine->setMouseVisible(false);

                m_timeTo = TIME_TO_PLAY;
            }

            if(player->clocks.readyToDelete.isEsplanedTime(2000))
            {
                player->reBorn();

                manager.sound->playSound("respawn", player);
            }
        }

        else
        {

            if(!map.aabb.isInner(player->getVisualBody()))
            {
                if(!onOutOfArena(player))
                    player->setInLastSpawnPoint();
            }
        }
    }

    if(hud.log->isEnable() && m_logClock.isEsplanedTime(3000))
        hud.log->setEnable(false);
}

void GameManager::hudProcess()
{
    using namespace tbe::gui;

    // Gestion de l'ETH en jeu -------------------------------------------------

    if(m_timeTo == TIME_TO_PLAY)
    {
        using boost::format;

        manager.gui->setSession(SCREEN_HUD);

        // Mise a jour des bar de progression (Vie, Muinition, Bullettime)

        if(!m_userPlayer->isKilled())
        {
            const Weapon* curWeapon = m_userPlayer->getCurWeapon();

            if(curWeapon)
            {
                hud.weaponIcon->setEnable(true);
                hud.weaponIcon->setCurState(curWeapon->getSlot() - 1);

                const int &ammoCount = curWeapon->getAmmoCount(),
                        &ammoCountMax = curWeapon->getMaxAmmoCount();

                format weaponGaugeFormat = format("%1%/%2%")
                        % ammoCount % ammoCountMax;

                hud.ammo->write(weaponGaugeFormat.str());
                hud.ammoGauge->setValue(ammoCount * 100 / ammoCountMax);
            }
            else
            {
                hud.ammo->write("xx/xx");
                hud.ammoGauge->setValue(0);
                hud.weaponIcon->setEnable(false);
            }

            const Power* curPower = m_userPlayer->getCurPower();

            if(curPower)
            {
                hud.powerIcon->setEnable(true);
                hud.powerIcon->setCurState(curPower->getSlot() - 1);

                const int &energy = m_userPlayer->getEnergy();

                // format powerGaugeFormat = format("%1% %2%/100") % curPower->getName() % energy;

                //hud.energy->write(powerGaugeFormat.str());
                hud.energyGauge->setValue(energy);
            }
            else
            {
                // hud.energy->write("Pas de pouvoir :(");
                hud.powerIcon->setEnable(false);
                hud.energyGauge->setValue(0);
            }

            const int &life = m_userPlayer->getLife();
            hud.life->write((format("%1%/100") % life).str());
            // hud.life->setValue(life);

            // Affichage de l'ecran de dommage si besoins

            if(manager.app->globalSettings.video.ppeUse)
            {
                if(ppe.dammage->isEnable())
                {
                    Vector4f color = ppe.dammage->getColor();

                    if(color.w > 0)
                    {
                        color.w -= 0.01f;
                        ppe.dammage->setColor(color);
                    }
                    else
                        ppe.dammage->setEnable(false);
                }
            }
            else
            {
                if(hud.background.dammage->isEnable())
                {
                    float opacity = hud.background.dammage->getOpacity();

                    if(opacity > 0)
                        hud.background.dammage->setOpacity(opacity - 0.01f);
                    else
                        hud.background.dammage->setEnable(false);
                }
            }

            if(hud.background.flash->isEnable())
            {
                float opacity = hud.background.flash->getOpacity();

                if(opacity > 0)
                    hud.background.flash->setOpacity(opacity - 0.05f);
                else
                    hud.background.flash->setEnable(false);
            }
        }
        else
        {
            hud.life->write("Mort !");
            //hud.life->setValue(0);
        }
    }

    // Gestion de l'ETH en gameover --------------------------------------------

    if(m_timeTo == TIME_TO_GAMEOVER)
    {
        manager.gui->setSession(SCREEN_GAMEOVER);

        // Affichage de l'ecran gameover si besoin

        float opacity = hud.background.gameover->getOpacity();

        if(opacity < 0.5)
            opacity += 0.01;

        hud.background.gameover->setOpacity(opacity);
    }
}

float rayFilter(const NewtonBody* body, const float*, int, void* userData, float intersectParam)
{
    MapElement* elem = (MapElement*)NewtonBodyGetUserData(body);

    if(elem == elem->getGameManager()->getUserPlayer())
        return intersectParam;

    float& hit = *static_cast<float*>(userData);

    if(intersectParam > 0.2 && intersectParam < hit)
        hit = intersectParam;

    return intersectParam;
}

void GameManager::render()
{
    if(!m_gameOver)
        onEachFrame(m_userPlayer);

    Vector3f campos = m_camera->getPos();
    Vector3f camtar = m_camera->getTarget();

    // Positionement camera ----------------------------------------------------

    /*
     * La position de la camera est caluclé a partir de la position du joueur,
     * et en reculant de quelques unités vers l'arrier.
     *
     * Un lancé de rayon est éffectuer pour savoir si la position de la camera
     * entre en collision avec un objet, si c'est le cas alors la position
     * arrier diminue en fonction du taux d'intersection.
     */

    Vector3f setPos = m_userPlayer->getVisualBody()->getPos();

    if(!m_playerPosRec.empty())
        setPos.y -= (setPos.y - m_playerPosRec.back().y) / 3 * 2;

    m_playerPosRec.push_back(setPos);

    Vector3f camzeropos = m_playerPosRec.front() + Vector3f(0, worldSettings.cameraUp, 0);
    Vector3f camendpos = camzeropos + (-camtar) * worldSettings.cameraBack;

    float hit = 1;
    NewtonWorldRayCast(parallelscene.newton->getNewtonWorld(), camzeropos, camendpos, rayFilter, &hit, NULL);

    hit = hit * worldSettings.cameraBack;

    campos = camzeropos - camtar * min(hit, worldSettings.cameraBack);

    m_camera->setPos(campos);

    if(m_playerPosRec.size() > 2)
        m_playerPosRec.pop_front();

    // Physique ----------------------------------------------------------------

    parallelscene.newton->setWorldTimestep(1.0f / m_newtonClock.getEsplanedTime());

    // Son 3D ------------------------------------------------------------------

    /*
     * Met a jour la position virtuelle de la camera du moteur audio pour
     * calculer les son 3D.
     */

    if(!manager.app->globalSettings.noaudio)
    {
        manager.sound->syncronizeSoundsPosition();

        FMOD_System_Set3DListenerAttributes(manager.fmodsys, 0, (FMOD_VECTOR*)(float*)campos, 0,
                                            (FMOD_VECTOR*)(float*)camtar,
                                            (FMOD_VECTOR*)(float*)m_camera->getUp());

        FMOD_System_Update(manager.fmodsys);
    }

    // Pick --------------------------------------------------------------------

    /*
     * Ici sera spécifier le vecteur de visé du joueur
     * en effectuant un lancé de rayon grace au moteur physique
     * depuis la position de la camera vers un vecteur aligé sur
     * le vecteur cible de la camera.
     *
     * Si le vecteur de visé se trouve a l'interieur du joueur
     * on relance un second rayon en ignorant le joueur et en affectant une
     * légere transparence a ce dernier.
     *
     * Si la position de la camera se trouve a l'interieur du joueur
     * on relance un second rayon en ignorant le joueur et en affectant une
     * forte transparence a ce dernier.
     */

    Vector3f endray = campos + camtar * map.aabb.getLength();

    m_shootTarget = parallelscene.newton->findAnyBody(campos, endray);

    AABB useraabb = m_userPlayer->getVisualBody()->getAbsolutAabb().add(0.1f);

    if(useraabb.isInner(campos))
    {
        m_shootTarget = parallelscene.newton->findZeroMassBody(campos, endray);

        m_cursorOnPlayer = true;

        m_userPlayer->makeTransparent(true, 0.1);
    }

    else if(useraabb.isInner(m_shootTarget))
    {
        m_shootTarget = parallelscene.newton->findZeroMassBody(campos, endray);

        m_cursorOnPlayer = true;

        m_userPlayer->makeTransparent(true, 0.5);
    }

    else if(m_cursorOnPlayer)
    {
        m_cursorOnPlayer = false;

        m_userPlayer->makeTransparent(false);
    }

    // Rendue ------------------------------------------------------------------

    manager.gameEngine->beginScene();

    if(manager.app->globalSettings.video.ppeUse)
    {
        Rtt* rtt = manager.ppe->getRtt();

        rtt->use(true);
        rtt->clear();
        manager.scene->render();
        rtt->use(false);

        manager.ppe->render();
    }
    else
    {
        manager.scene->render();
    }

    manager.gui->render();

    manager.gameEngine->endScene();
}

tbe::Vector3f GameManager::getShootTarget() const
{
    return m_shootTarget;
}

tbe::Vector3f GameManager::getViewDirection() const
{
    Vector3f dir = m_camera->getTarget();
    dir.y = 0;

    return dir.normalize();
}

Player* GameManager::getUserPlayer() const
{
    return m_userPlayer;
}

Player* GameManager::getWinnerPlayer() const
{
    return m_winnerPlayer;
}

const Player::Array& GameManager::getPlayers() const
{
    return m_players;
}

void GameManager::setGameOver(Player* winner, std::string finalmsg)
{
    m_gameOver = true;
    m_timeTo = TIME_TO_GAMEOVER;

    hud.gameover->write(finalmsg);

    m_winnerPlayer = winner;

    for(unsigned i = 0; i < m_players.size(); i++)
    {
        m_players[i]->getPhysicBody()->setApplyForce(0);
        m_players[i]->getPhysicBody()->setVelocity(0);

        NewtonBodySetFreezeState(m_players[i]->getPhysicBody()->getBody(), true);
    }

    manager.gameEngine->setMouseVisible(true);

    hud.background.gameover->setEnable(true);

    if(manager.app->globalSettings.video.ppeUse)
        ppe.gameover->setEnable(true);
}

bool GameManager::isGameOver() const
{
    return m_gameOver;
}

bool GameManager::isRunning() const
{
    return m_running;
}

void GameManager::backImpulseEffect(float intensity, float push)
{
    m_backImpulse.intensity = intensity;
    m_backImpulse.push = push;
}

void GameManager::earthQuakeEffect(float intensity, bool physical)
{
    m_earthquake.intensity = intensity;
    m_earthquake.physical = physical;
}

void GameManager::flashEffect()
{
    hud.background.flash->setEnable(true);

    hud.background.flash->setOpacity(1);

    manager.sound->playSound("flash");
}

void GameManager::dammageEffect()
{
    if(manager.app->globalSettings.video.ppeUse)
    {
        ppe.dammage->setEnable(true);

        ppe.dammage->setColor(Vector4f(1, 0, 0, 0.5));
    }
    else
    {
        hud.background.dammage->setEnable(true);

        hud.background.dammage->setOpacity(0.75);
    }
}

const Player::Array GameManager::getTargetsOf(Player* player) const
{
    Player::Array array(m_players);
    array.erase(find(array.begin(), array.end(), player));

    return array;
}

void GameManager::registerElement(StaticElement* elem)
{
    if(elem->getPhysicBody())
        manager.material->addElement(elem);

    map.staticElements.push_back(elem);

    if(elem->getVisualBody())
        map.aabb.count(elem->getVisualBody());
}

void GameManager::unregisterElement(StaticElement* staticObject)
{
    StaticElement::Array::iterator it = find(map.staticElements.begin(),
                                             map.staticElements.end(), staticObject);

    map.staticElements.erase(it);
}

void GameManager::registerElement(MapElement* elem)
{
    if(elem->getPhysicBody())
        manager.material->addElement(elem);

    map.mapElements.push_back(elem);

    if(elem->getVisualBody())
        map.aabb.count(elem->getVisualBody());
}

void GameManager::unregisterElement(MapElement* staticObject)
{
    MapElement::Array::iterator it = find(map.mapElements.begin(),
                                          map.mapElements.end(), staticObject);

    map.mapElements.erase(it);
}

void GameManager::registerPlayer(Player* player)
{
    manager.material->addPlayer(player);
    m_players.push_back(player);
}

void GameManager::unregisterPlayer(Player* player)
{
    Player::Array::iterator it = find(m_players.begin(), m_players.end(), player);

    m_players.erase(it);
}
