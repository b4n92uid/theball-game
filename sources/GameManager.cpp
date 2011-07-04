#include "GameManager.h"

#include "AppManager.h"
#include "SoundManager.h"
#include "MaterialManager.h"
#include "UserControl.h"

#include "Player.h"
#include "Item.h"

#include "Define.h"
#include "Tools.h"
#include "Weapon.h"
#include "Bullet.h"
#include "SoundManager.h"
#include "StaticElement.h"

#include <boost/format.hpp>

#include <tinyxml.h>
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
}

GameManager::~GameManager()
{
    cout << "--- Cleaning game manager" << endl;

    manager.gui->setSession(MENU_QUICKPLAY);

    manager.gameEngine->setGrabInput(false);
    manager.gameEngine->setMouseVisible(true);

    manager.gui->destroySession(SCREEN_GAMEOVER);
    manager.gui->destroySession(SCREEN_PAUSEMENU);
    manager.gui->destroySession(SCREEN_HUD);

    delete manager.sound;
    delete manager.material;
    delete manager.script;
    delete manager.parser;

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
    using namespace tbe::gui;

    cout << "--- Loading GUI" << endl;

    Settings::Gui& guisets = manager.app->globalSettings.gui;
    Settings::Video& vidsets = manager.app->globalSettings.video;

    GuiSkin* ingame_skin = new GuiSkin;

    ingame_skin->button(guisets.button);
    ingame_skin->buttonSize(guisets.buttonSize);
    ingame_skin->buttonMetaCount = 4;

    ingame_skin->gauge(guisets.gauge);
    ingame_skin->gaugeSize(guisets.gaugeSize);
    ingame_skin->gaugeMetaCount = 2;

    ingame_skin->editBox(guisets.editBox);
    ingame_skin->editBoxSize(guisets.editBoxSize);
    ingame_skin->editBoxMetaCount = 4;

    ingame_skin->switchBox(guisets.switchBox);
    ingame_skin->switchBoxSize(guisets.switchBoxSize);
    ingame_skin->switchBoxMetaCount = 4;

    ingame_skin->pencil(guisets.font, guisets.fontSize);
    ingame_skin->pencil.setColor(1);

    Pencil whiteBigPen(guisets.font, guisets.fontSize * 1.5);
    whiteBigPen.setColor(1);

    Pencil blackBigPen(guisets.font, guisets.fontSize * 1.5);
    blackBigPen.setColor(Vector4f(0, 0, 0, 1));

    // GameOver ----------------------------------------------------------------

    manager.gui->setSession(SCREEN_GAMEOVER, ingame_skin);

    Texture black;
    black.build(128, Vector4f(0, 0, 0, 1));

    hud.background.gameover = manager.gui->addImage("hud.background.gameover", black);
    hud.background.gameover->setSize(vidsets.screenSize);
    hud.background.gameover->setOpacity(0);
    hud.background.gameover->setEnable(false);

    manager.gui->addLayout(Layout::Vertical, 10, 10);
    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayout(Layout::Horizental, 10, 10);
    manager.gui->addLayoutStretchSpace();

    hud.gameover = manager.gui->addTextBox("hud.gameover");
    hud.gameover->setSize(Vector2f(vidsets.screenSize) * Vector2f(0.75, 0.75));
    hud.gameover->setDefinedSize(true);
    hud.gameover->setPencil(whiteBigPen);
    hud.gameover->setBackground(guisets.backgroundTextbox);
    hud.gameover->setPadding(16);
    hud.gameover->setBackgroundMask(guisets.maskH);
    hud.gameover->setArrowTexture(guisets.backgroundUpDownArrow);
    hud.gameover->setTextAlign(TextBox::LEFT);

    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();
    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();

    // Pause Menu --------------------------------------------------------------

    manager.gui->setSession(SCREEN_PAUSEMENU);

    Image* backPause = manager.gui->addImage("00:background", guisets.backgroundPause);
    backPause->setSize(vidsets.screenSize);

    manager.gui->addLayout(Layout::Vertical, 0, 10);

    manager.gui->addLayout(Layout::Horizental, 10);
    hud.playmenu.ret = manager.gui->addButton("hud.playmenu.ret", "Retour");
    hud.playmenu.quit = manager.gui->addButton("hud.playmenu.quit", "Quitter");
    manager.gui->endLayout();

    manager.gui->addLayoutStretchSpace();

    TextBox* pauseLabel = manager.gui->addTextBox("hud.pauseLabel");
    pauseLabel->setPencil(whiteBigPen);
    pauseLabel->write("Menu Pause !");

    manager.gui->endLayout();

    // HUD ---------------------------------------------------------------------

    manager.gui->setSession(SCREEN_HUD, ingame_skin);

    hud.background.dammage = manager.gui->addImage("0:hud.background.dammage", guisets.backgroundDammage);
    hud.background.dammage->setSize(vidsets.screenSize);
    hud.background.dammage->setEnable(false);

    Image* croshair = manager.gui->addImage("1:croshair", GUI_CROSHAIR);
    croshair->setPos(Vector2f(vidsets.screenSize) / 2.0f - croshair->getSize() / 2.0f);

    manager.gui->addLayout(Layout::Vertical, 10, 10);

    // -------- Barre des gauges
    manager.gui->addLayout(Layout::Horizental, 10);
    manager.gui->addLayoutStretchSpace();

    hud.life = manager.gui->addGauge("hud.life", "Vie");
    hud.life->setSmooth(true, 1);
    hud.ammo = manager.gui->addGauge("hud.ammo", "Munition");
    hud.ammo->setSmooth(true, 1);
    hud.bullettime = manager.gui->addGauge("hud.bullettime", "Bulettime");
    hud.bullettime->setSmooth(true, 1);

    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();
    // --------

    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayoutStretchSpace();

    // -------- Log
    manager.gui->addLayout(Layout::Horizental);
    manager.gui->addLayoutStretchSpace();

    hud.log = manager.gui->addTextBox("hud.log");
    hud.log->setPencil(blackBigPen);
    hud.log->setBackground(guisets.backgroundTextbox);
    hud.log->setPadding(16);
    hud.log->setEnable(false);

    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();
    // --------

    manager.gui->addLayoutStretchSpace();

    // -------- State
    hud.state = manager.gui->addTextBox("hud.state");
    // --------

    manager.gui->endLayout();
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

void GameManager::gameProcess()
{
    using namespace tbe;
    using namespace tbe::scene;

    if(m_gameOver)
        return;

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

    /*
     * Pour chaque Items
     * - les items pris pour les remplacer
     * - les items hors-aréne
     */
    for(unsigned i = 0; i < map.items.size(); i++)
    {
        Item*& item = map.items[i];

        if(item->isTaked())
        {
            if(item->isReadyToReborn())
                item->reborn();
        }

        else if(!map.aabb.isInner(item->getVisualBody()))
            item->resetPosition();
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

        const Weapon* curWeapon = m_userPlayer->getCurWeapon();

        const int &ammoCount = curWeapon->getAmmoCount(),
                &ammoCountMax = curWeapon->getMaxAmmoCount(),
                &life = m_userPlayer->getLife(),
                &energy = m_userPlayer->getEnergy();

        if(!m_userPlayer->isKilled())
        {
            // Mise a jour des bar de progression (Vie, Muinition, Bullettime)

            hud.ammo->setLabel((format("%1% %2%/%3%")
                               % curWeapon->getWeaponName() % ammoCount % ammoCountMax).str());

            hud.ammo->setValue(ammoCount * 100 / ammoCountMax);

            hud.life->setLabel((format("Santé %1%/100") % life).str());

            hud.life->setValue(life);

            hud.bullettime->setLabel((format("Energie %1%/100") % energy).str());

            hud.bullettime->setValue(energy);

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
        }
        else
        {
            hud.life->setLabel("Mort !");
            hud.life->setValue(0);
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
    float inertia[3], masse;
    NewtonBodyGetMassMatrix(body, &masse, &inertia[0], &inertia[1], &inertia[2]);

    float& hit = *static_cast<float*>(userData);

    if(!(masse > 0.0f))
    {
        hit = intersectParam;
        return 0.0;
    }

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

    AABB useraabb = m_userPlayer->getVisualBody()->getAbsolutAabb();

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

void GameManager::hudDammage(bool status)
{
    if(manager.app->globalSettings.video.ppeUse)
    {
        ppe.dammage->setEnable(status);

        if(status)
            ppe.dammage->setColor(Vector4f(1, 0, 0, 0.5));
    }
    else
    {
        hud.background.dammage->setEnable(status);

        if(status)
            hud.background.dammage->setOpacity(0.75);
    }
}

const Player::Array GameManager::getTargetsOf(Player* player) const
{
    Player::Array array(m_players);
    array.erase(find(array.begin(), array.end(), player));

    return array;
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

void GameManager::registerItem(Item* item)
{
    manager.material->addItem(item);
    map.items.push_back(item);
}

void GameManager::unregisterItem(Item* item)
{
    Item::Array::iterator it = find(map.items.begin(),
                                    map.items.end(), item);

    map.items.erase(it);
}
