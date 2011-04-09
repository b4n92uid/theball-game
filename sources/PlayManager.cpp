/*
 * File:   PlayManager.cpp
 * Author: b4n92uid
 *
 * Created on 2 janvier 2010, 20:24
 */

#include "PlayManager.h"

#include "AppManager.h"
#include "MaterialManager.h"
#include "SoundManager.h"
#include "UserControl.h"

#include "Player.h"
#include "Item.h"

#include "Define.h"
#include "Tools.h"
#include "Weapon.h"
#include "FragModeAi.h"
#include "PlaySoundManager.h"

#include <boost/format.hpp>

#include <tinyxml.h>
#include <cmath>

using namespace std;
using namespace tbe;

PlayManager::PlayManager(AppManager* appManager) : GameManager(appManager)
{
    m_timeTo = TIME_TO_PLAY;

    m_playTimeManager.startTimestamp = time(NULL);

    m_gameOver = false;

    m_cursorOnPlayer = false;

    m_camera = new scene::Camera(scene::Camera::TARGET_RELATIVE);
    manager.scene->addCamera(m_camera);

    m_cameraBody = new scene::NewtonNode(parallelscene.newton, &m_cameratMat);
    m_cameraBody->buildSphereNode(0.1, 0.1);
    NewtonBodySetForceAndTorqueCallback(m_cameraBody->getBody(), NULL);

    delete manager.sound, manager.sound = new PlaySoundManager(this);

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

PlayManager::~PlayManager()
{
    if(m_gameOver && m_playSetting.playTime > 0)
    {
        using namespace ticks;

        cout << "--- Saving Score" << endl;

        TiXmlDocument scoreDoc;

        if(!scoreDoc.LoadFile("score.xml"))
            throw;

        TiXmlElement* root = scoreDoc.RootElement();

        TiXmlElement party("party");

        party.SetAttribute("playerName", m_playSetting.playerName.name);
        party.SetAttribute("playerModel", m_playSetting.playerName.model);

        party.SetAttribute("levelName", m_playSetting.playMap.name);
        party.SetAttribute("levelPath", m_playSetting.playMap.file);

        party.SetAttribute("playMod", m_playSetting.playMod);
        party.SetAttribute("playTime", m_playSetting.playTime);

        party.SetAttribute("playersCount", m_playSetting.playerCount);

        party.SetAttribute("timestamp", m_playTimeManager.startTimestamp);

        party.SetAttribute("score", m_userPlayer->getScore());

        root->InsertEndChild(party);

        scoreDoc.SaveFile();
    }

    manager.gui->destroySession(SCREEN_GAMEOVER);
    manager.gui->destroySession(SCREEN_PAUSEMENU);
    manager.gui->destroySession(SCREEN_HUD);
    manager.gui->destroySession(SCREEN_PLAYERSLIST);

    delete m_bullettime;
}

void PlayManager::setupMap(const Settings::PartySetting& playSetting)
{
    m_playSetting = playSetting;

    cout << "--- Loading level" << endl;

    // SCENE -------------------------------------------------------------------

    manager.level->loadLevel(m_playSetting.playMap.file);

    // Marge
    map.aabb.add(AABB(Vector3f(-8, -8, -8), Vector3f(8, 64, 8)));

    AABB newtonWordSize = map.aabb;
    newtonWordSize.add(Vector3f(32.0f));
    parallelscene.newton->setWorldSize(newtonWordSize);

    m_playTimeManager.startChrono = m_playTimeManager.curChrono = m_playSetting.playTime;

    scene::Fog* fog = manager.scene->getFog();

    manager.scene->setZFar(fog->isEnable() ? fog->getEnd() : map.aabb.getSize() * 2);
    manager.scene->updateViewParameter();

    // PLAYERS -----------------------------------------------------------------

    ifstream namefile("NAMES.txt");

    do m_botNames.push_back(string());
    while(getline(namefile, m_botNames.back()));

    m_botNames.pop_back();

    namefile.close();

    m_userPlayer = new Player(this, m_playSetting.playerName.nick, m_playSetting.playerName.model);
    m_userPlayer->attachController(new UserControl(this));

    manager.scene->getRootNode()->addChild(m_userPlayer);

    registerPlayer(m_userPlayer);
    modSetupUser(m_userPlayer);

    m_bullettime = new BulletTime(this);

    // PPE ---------------------------------------------------------------------

    const Settings::Video& vidSets = manager.app->globalSettings.video;

    if(vidSets.usePpe)
    {
        using namespace tbe::ppe;

        ppe.boost = new MotionBlurEffect;
        ppe.boost->setEnable(false);
        ppe.boost->setIntensity(vidSets.ppe.boostIntensity);
        manager.ppe->addPostEffect("boostEffect", ppe.boost);

        ppe.bullettime = new ColorEffect;
        ppe.bullettime->setInternalPass(true);
        ppe.bullettime->setRttFrameSize(vidSets.ppe.bullettimeSize);
        ppe.bullettime->setFusionMode(ColorEffect::BLACK_WHITE);
        ppe.bullettime->setColor(vidSets.ppe.bullettimeColor);
        ppe.bullettime->setEnable(false);
        manager.ppe->addPostEffect("blettimeEffect", ppe.bullettime);

        ppe.dammage = new ColorEffect;
        ppe.dammage->setInternalPass(true);
        ppe.dammage->setFusionMode(ColorEffect::MULTIPLICATION_COLOR);
        ppe.dammage->setColor(vidSets.ppe.dammageColor);
        ppe.dammage->setEnable(false);
        manager.ppe->addPostEffect("dammageEffect", ppe.dammage);

        ppe.gameover = new BlurEffect;
        ppe.gameover->setPasse(vidSets.ppe.gameoverPass);
        ppe.gameover->setEnable(false);
        manager.ppe->addPostEffect("gameoverEffect", ppe.gameover);

        ppe.bloom = new BloomEffect;
        ppe.bloom->setRttFrameSize(vidSets.ppe.worldSize);
        ppe.bloom->setIntensity(vidSets.ppe.worldIntensity);
        ppe.bloom->setThreshold(vidSets.ppe.worldThershold);
        ppe.bloom->setBlurPass(vidSets.ppe.worldBlurPass);
        manager.ppe->addPostEffect("worldEffect", ppe.bloom);
    }
}

void PlayManager::setupGui()
{
    using namespace tbe::gui;

    cout << "--- Loading GUI" << endl;

    Vector2f screenSize = manager.app->globalSettings.video.screenSize;

    Pencil bigPen(GUI_FONT, int(GUI_FONTSIZE * manager.app->globalSettings.video.guiSizeFactor * 1.5));

    // Tabaleau des joueur ------------------------------------------------------

    manager.gui->setSession(SCREEN_PLAYERSLIST);

    manager.gui->addLayout(Layout::Vertical);
    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayout(Layout::Horizental);
    manager.gui->addLayoutStretchSpace();

    hud.scorelist = manager.gui->addTextBox("hud.scorelist");
    hud.scorelist->setPencil(bigPen);
    hud.scorelist->setSize(screenSize * Vector2f(0.75, 0.75));
    hud.scorelist->setDefinedSize(true);
    hud.scorelist->setBackground(GUI_SCORE);
    hud.scorelist->setTextAlign(TextBox::LEFT);
    hud.scorelist->setBackgroundPadding(Vector2f(16, 8));

    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();
    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();

    // GameOver ----------------------------------------------------------------

    manager.gui->setSession(SCREEN_GAMEOVER);

    Texture black;
    black.build(128, 0);

    hud.background.gameover = manager.gui->addImage("hud.background.gameover", black);
    hud.background.gameover->setSize(screenSize);
    hud.background.gameover->setOpacity(0.75);
    hud.background.gameover->setEnable(false);

    manager.gui->addLayout(Layout::Vertical, 10, 10);
    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayout(Layout::Horizental, 10, 10);
    manager.gui->addLayoutStretchSpace();
    hud.gameover = manager.gui->addTextBox("hud.gameover");
    hud.gameover->setSize(screenSize * Vector2f(0.75, 0.75));
    hud.gameover->setDefinedSize(true);
    hud.gameover->setPencil(bigPen);
    hud.gameover->setBackground(GUI_SCORE);
    hud.gameover->setBackgroundPadding(8);
    hud.gameover->setTextAlign(TextBox::LEFT);
    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();
    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();

    // Pause Menu --------------------------------------------------------------

    manager.gui->setSession(SCREEN_PAUSEMENU);

    Image* backPause = manager.gui->addImage("00:background", BACKGROUND_PAUSE);
    backPause->setSize(screenSize);

    manager.gui->addLayout(Layout::Vertical, 0, 10);

    manager.gui->addLayout(Layout::Horizental, 10);
    hud.playmenu.ret = manager.gui->addButton("hud.playmenu.ret", "Retour");
    hud.playmenu.quit = manager.gui->addButton("hud.playmenu.quit", "Quitter");
    manager.gui->endLayout();

    manager.gui->addLayoutStretchSpace();

    TextBox* pauseLabel = manager.gui->addTextBox("hud.pauseLabel");
    pauseLabel->setPencil(bigPen);
    pauseLabel->write("Menu Pause !");

    manager.gui->endLayout();

    // HUD ---------------------------------------------------------------------

    manager.gui->setSession(SCREEN_HUD);

    hud.background.dammage = manager.gui->addImage("0:hud.background.dammage", BACKGROUND_DAMMAGE);
    hud.background.dammage->setSize(screenSize);
    hud.background.dammage->setEnable(false);

    hud.background.bullettime = manager.gui->addImage("1:hud.background.bullettime", BACKGROUND_BULLETTIME);
    hud.background.bullettime->setOpacity(0.5);
    hud.background.bullettime->setSize(screenSize);
    hud.background.bullettime->setEnable(false);

    manager.gui->addImage("2:background", BACKGROUND_HUD)->setSize(screenSize);

    Image* croshair = manager.gui->addImage("01:croshair", GUI_CROSHAIR);
    croshair->setPos(Vector2f(manager.app->globalSettings.video.screenSize) / 2.0f - croshair->getSize() / 2.0f);

    manager.gui->addLayout(Layout::Vertical, 10, 10);

    // -------- Barre des gauges
    manager.gui->addLayout(Layout::Horizental, 10);
    manager.gui->addLayoutStretchSpace();

    hud.item = manager.gui->addStateShow("hud.item", GUI_ITEM, 2);
    hud.life = manager.gui->addGauge("hud.life", "Vie");
    hud.life->setSmooth(true, 1);
    hud.ammo = manager.gui->addGauge("hud.ammo", "Munition");
    hud.ammo->setSmooth(true, 1);
    hud.bullettime = manager.gui->addGauge("hud.bullettime", "Bulettime");
    hud.bullettime->setSmooth(true, 1);
    hud.boost = manager.gui->addStateShow("hud.boost", GUI_BOOST, 2);

    manager.gui->addLayoutStretchSpace();
    manager.gui->endLayout();
    // --------

    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayoutStretchSpace();

    // -------- Log
    manager.gui->addLayout(Layout::Horizental);
    manager.gui->addLayoutStretchSpace();

    hud.log = manager.gui->addTextBox("hud.log");
    hud.log->setPencil(bigPen);
    hud.log->setBackground(GUI_NOTIFY);
    hud.log->setBackgroundPadding(Vector2f(32, 16));
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

void PlayManager::onStartGame()
{
    if(!manager.app->globalSettings.noaudio && !manager.app->globalSettings.nomusic)
    {
        if(map.musicStream)
            FMOD_System_PlaySound(manager.fmodsys, FMOD_CHANNEL_FREE, map.musicStream, false, &map.musicChannel);

        FMOD_CHANNELGROUP* chgrp;
        FMOD_System_GetMasterChannelGroup(manager.fmodsys, &chgrp);

        FMOD_CHANNELGROUP* musicChGrp;
        FMOD_System_CreateChannelGroup(manager.fmodsys, "musicChGrp", &musicChGrp);
        FMOD_Channel_SetChannelGroup(map.musicChannel, musicChGrp);

        FMOD_ChannelGroup_SetVolume(chgrp, 0.75);
        FMOD_ChannelGroup_SetVolume(musicChGrp, 1.00);
    }
}

void PlayManager::log(std::string msg)
{
    hud.log->write(msg);
    hud.log->setEnable(true);

    m_logClock.snapShoot();
}

void PlayManager::processDevelopperCodeEvent()
{
    EventManager* event = manager.gameEngine->getEventManager();

    if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
        // F1 : Dammage 10%
        if(event->keyState[EventManager::KEY_F1])
        {
            Bullet b(this);
            b.setDammage(10);
            m_userPlayer->takeDammage(&b);
        }

        // F2 : Kill
        if(event->keyState[EventManager::KEY_F2])
            m_userPlayer->kill();

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
            unsigned select = tools::rand(0, manager.app->globalSettings.availablePlayer.size());

            Settings::PlayerInfo& pi = manager.app->globalSettings.availablePlayer[select];

            Player* player = new Player(this, "TEST_BOT", pi.model);
            registerPlayer(player);
            modSetupAi(player);

            player->attachController(NULL);

            manager.scene->getRootNode()->addChild(player);
        }

        // F6 : TEST BOT AI
        if(event->keyState[EventManager::KEY_F7])
        {
            unsigned select = tools::rand(0, manager.app->globalSettings.availablePlayer.size());

            Settings::PlayerInfo& pi = manager.app->globalSettings.availablePlayer[select];

            Player* player = new Player(this, "TEST_BOT_AI", pi.model);

            registerPlayer(player);
            modSetupAi(player);

            manager.scene->getRootNode()->addChild(player);
        }

        // F9 : Next level
        if(event->keyState[EventManager::KEY_F9])
        {
            m_userPlayer->setScore(m_playSetting.winCond);
            setGameOver();
        }
    }
}

void PlayManager::eventProcess()
{
    using namespace tbe::scene;

    manager.gameEngine->pollEvent();

    EventManager* event = manager.gameEngine->getEventManager();

    // Session de jeu
    if(m_timeTo == TIME_TO_PLAY || m_timeTo == TIME_TO_VIEWSCORE)
    {
        manager.gui->setSession(SCREEN_HUD);

        if(m_userPlayer->isKilled())
            return;

        // Vue
        if(event->notify == EventManager::EVENT_MOUSE_MOVE)
            manager.scene->getCurCamera()->rotate(event->mousePosRel);

        // Affichage des scores
        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(m_numslot.count(event->lastActiveKey.first))
                m_userPlayer->slotWeapon(m_numslot[event->lastActiveKey.first]);

            if(event->lastActiveKey.first == EventManager::KEY_TAB)
            {
                manager.gameEngine->setMouseVisible(true);

                m_timeTo = TIME_TO_VIEWSCORE;
            }
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

            for(unsigned i = 0; i < m_players.size(); i++)
                m_bullettime->setActive(false);

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

                running = false;
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
            running = false;
    }
}

void PlayManager::gameProcess()
{
    using namespace tbe;
    using namespace tbe::scene;

    if(m_gameOver)
        return;

    for(unsigned i = m_players.size(); i < m_playSetting.playerCount; i++)
        if(m_spawnPlayer.isEsplanedTime(4000))
        {
            unsigned selectPlayer = tools::rand(0, manager.app->globalSettings.availablePlayer.size());

            Settings::PlayerInfo& pi = manager.app->globalSettings.availablePlayer[selectPlayer];

            unsigned selectName = tools::rand(0, m_botNames.size());

            Player* player = new Player(this, m_botNames[selectName], pi.model);
            registerPlayer(player);

            manager.scene->getRootNode()->addChild(player);

            modSetupAi(player);

            manager.sound->play("respawn", player);

            break;
        }

    /*
     * Pour chaque joueurs
     *  - les joueurs mort pour les reconstruires
     *  - les joueurs en fin de vie pour la préparation a la mort ;)
     *  - les joueurs hors de l'arene pour les remetren place
     */
    for(unsigned i = 0; i < m_players.size(); i++)
    {
        Player* player = m_players[i];

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

                manager.sound->play("respawn", player);
            }
        }

        else
        {

            if(!map.aabb.isInner(player))
            {
                player->toNextSpawnPos();
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

        else if(!map.aabb.isInner(item))
            item->resetPosition();
    }

    // Gestion du temps de jeu
    if(m_playSetting.playTime > 0)
    {
        if(m_playTimeManager.curChrono == 0 && !m_gameOver)
        {
            setGameOver();
        }

        else if(m_playTimeManager.clock.isEsplanedTime(1000))
        {
            m_playTimeManager.curChrono--;

            if(m_playTimeManager.curChrono <= 10)
                manager.sound->play("notime", m_userPlayer);
        }
    }

    if(hud.log->isEnable() && m_logClock.isEsplanedTime(3000))
        hud.log->setEnable(false);
}

void PlayManager::hudProcess()
{
    using namespace tbe::gui;

    // Gestion de l'ETH en jeu -------------------------------------------------

    sort(m_players.begin(), m_players.end(), playerScoreSortProcess);

    if(m_timeTo == TIME_TO_PLAY)
    {
        using boost::format;

        manager.gui->setSession(SCREEN_HUD);

        const Weapon* curWeapon = m_userPlayer->getCurWeapon();

        const int &ammoCount = curWeapon->getAmmoCount(),
                &ammoCountMax = curWeapon->getMaxAmmoCount(),
                &life = m_userPlayer->getLife(),
                &bullettime = static_cast<int>(m_bullettime->getValue() * 100);

        if(!m_userPlayer->isKilled())
        {
            // Mise a jour des bar de progression (Vie, Muinition, Bullettime, Boost)

            hud.ammo->setLabel((format("%1% %2%/%3%")
                               % curWeapon->getWeaponName() % ammoCount % ammoCountMax).str());

            hud.ammo->setValue(ammoCount * 100 / ammoCountMax);

            hud.life->setLabel((format("Santé %1%/100") % life).str());

            hud.life->setValue(life);

            hud.bullettime->setLabel((format("Bullettilme %1%/100") % bullettime).str());

            hud.bullettime->setValue(bullettime);

            hud.boost->setCurState(m_userPlayer->isBoostAvalaible());

            ostringstream ss;
            modUpdateStateText(ss);

            hud.state->write(ss.str());

            // Affichage de l'ecran de dommage si besoins

            if(manager.app->globalSettings.video.usePpe)
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

    // Gestion de l'ETH en affichage des scores --------------------------------

    if(m_timeTo == TIME_TO_VIEWSCORE && !m_userPlayer->isKilled())
    {
        manager.gui->setSession(SCREEN_PLAYERSLIST);

        ostringstream ss;
        modUpdateScoreListText(ss);

        hud.scorelist->write(ss.str());
    }

    // Gestion de l'ETH en gameover --------------------------------------------

    if(m_timeTo == TIME_TO_GAMEOVER)
    {
        manager.gui->setSession(SCREEN_GAMEOVER);

        ostringstream ss;
        modUpdateGameOverText(ss);

        hud.gameover->write(ss.str());

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

void PlayManager::render()
{
    //    Vector2i& screenSize = manager.app->globalSettings.video.screenSize;

    // Positionement camera ----------------------------------------------------

    Vector3f setPos = m_userPlayer->getPos();

    if(!m_playerPosRec.empty())
        setPos.y -= (setPos.y - m_playerPosRec.back().y) / 3 * 2;

    m_playerPosRec.push_back(setPos);

    float cameraback = 2.5f;

    Vector3f camzeropos = m_playerPosRec.front() + Vector3f(0, worldSettings.playerSize * 2, 0);
    Vector3f camendpos = camzeropos + (-m_camera->getTarget()) * (cameraback + 1.5f);

    float hit = 1;
    NewtonWorldRayCast(parallelscene.newton->getNewtonWorld(), camzeropos, camendpos, rayFilter, &hit, NULL);

    hit = hit * cameraback / 1.0f;

    Vector3f campos = camzeropos - m_camera->getTarget() * min(hit, cameraback);

    m_camera->setPos(campos);

    if(m_playerPosRec.size() > 2)
        m_playerPosRec.pop_front();

    // Physique ----------------------------------------------------------------

    parallelscene.newton->setWorldTimestep(1.0f / m_newtonClock.getEsplanedTime());

    m_bullettime->process();

    // Son 3D ------------------------------------------------------------------

    if(!manager.app->globalSettings.noaudio)
    {
        FMOD_System_Set3DListenerAttributes(manager.fmodsys, 0, (FMOD_VECTOR*)(float*)m_camera->getPos(), 0,
                                            (FMOD_VECTOR*)(float*)m_camera->getTarget(),
                                            (FMOD_VECTOR*)(float*)m_camera->getUp());

        FMOD_System_Update(manager.fmodsys);
    }

    // Rendue ------------------------------------------------------------------

    manager.gameEngine->beginScene();

    if(manager.app->globalSettings.video.usePpe)
    {
        Rtt* rtt = manager.ppe->getRtt();

        rtt->use(true);
        rtt->clear();
        manager.scene->render();
        rtt->use(false);

        // m_shootTarget = manager.scene->ScreenToWorld(screenSize / 2, rtt);

        manager.ppe->render();
    }
    else
    {
        manager.scene->render();

        // m_shootTarget = manager.scene->ScreenToWorld(screenSize / 2);
    }

    m_shootTarget = parallelscene.newton->findAnyBody(m_camera->getPos(), m_camera->getPos() + m_camera->getTarget() * map.aabb.getSize());

    AABB useraabb = m_userPlayer->getAbsolutAabb().add(Vector3f(0.1f));

    if(useraabb.isInner(m_camera->getPos()))
    {
        m_shootTarget = parallelscene.newton->findZeroMassBody(m_camera->getPos(), m_camera->getPos() + m_camera->getTarget() * map.aabb.getSize());

        m_cursorOnPlayer = true;

        m_userPlayer->makeTransparent(true, 0.1);
    }

    else if(useraabb.isInner(m_shootTarget))
    {
        m_shootTarget = parallelscene.newton->findZeroMassBody(m_camera->getPos(), m_camera->getPos() + m_camera->getTarget() * map.aabb.getSize());

        m_cursorOnPlayer = true;

        m_userPlayer->makeTransparent(true, 0.5);
    }

    else if(m_cursorOnPlayer)
    {
        m_cursorOnPlayer = false;

        m_userPlayer->makeTransparent(false);
    }

    manager.gui->render();

    manager.gameEngine->endScene();
}

tbe::Vector3f PlayManager::getShootTarget() const
{
    return m_shootTarget;
}

Player* PlayManager::getUserPlayer() const
{
    return m_userPlayer;
}

const Player::Array& PlayManager::getPlayers() const
{
    return m_players;
}

int PlayManager::modulatScore(int score)
{
    if(m_playSetting.playerCount)
    {
        float timeFactor = tools::clamp(1.0f - (m_playSetting.playTime / 1200.0f), 0.1f, 1.0f);
        float countFactor = tools::clamp(1.0f - (m_playSetting.playerCount / 32.0f), 0.1f, 1.0f);
        return ceil(score * timeFactor / countFactor);
    }
    else
        return 0;
}

void PlayManager::setGameOver()
{
    m_gameOver = true;
    m_timeTo = TIME_TO_GAMEOVER;

    for(unsigned i = 0; i < m_players.size(); i++)
        NewtonBodySetFreezeState(m_players[i]->getPhysicBody()->getBody(), true);

    manager.gameEngine->setMouseVisible(true);

    hud.background.gameover->setEnable(true);

    if(manager.app->globalSettings.video.usePpe)
        ppe.gameover->setEnable(true);

    ostringstream ss;
    modUpdateScoreListText(ss);

    hud.scorelist->write(ss.str());
}

bool PlayManager::isGameOver() const
{
    return m_gameOver;
}

void PlayManager::registerPlayer(Player* player)
{
    manager.material->addPlayer(player);

    m_players.push_back(player);
}

void PlayManager::unregisterPlayer(Player* player)
{
    Player::Array::iterator it = find(m_players.begin(), m_players.end(), player);

    m_players.erase(it);
}

void PlayManager::hudItem(bool status)
{
    hud.item->setCurState(status);
}

void PlayManager::hudBullettime(bool status)
{
    if(manager.app->globalSettings.video.usePpe)
    {
        ppe.bullettime->setEnable(status);
        ppe.bloom->setEnable(!status);
    }
    else
    {
        hud.background.bullettime->setEnable(status);
    }
}

void PlayManager::hudBoost(bool status)
{
    if(manager.app->globalSettings.video.usePpe)
    {
        ppe.boost->setEnable(status);
    }
    else
    {

    }
}

void PlayManager::hudDammage(bool status)
{
    if(manager.app->globalSettings.video.usePpe)
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

BulletTime* PlayManager::getBullettime() const
{
    return m_bullettime;
}

const Player::Array PlayManager::getTargetsOf(Player* player) const
{
    Player::Array array(m_players);
    array.erase(find(array.begin(), array.end(), player));

    return array;
}
