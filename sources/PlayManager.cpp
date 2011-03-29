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

    m_camera = new scene::Camera(scene::Camera::TARGET_RELATIVE);
    manager.scene->AddCamera(m_camera);

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

        cout << "Saving Score" << endl;

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

        party.SetAttribute("score", m_userPlayer->GetScore());

        root->InsertEndChild(party);

        scoreDoc.SaveFile();
    }

    manager.gui->DestroySession(SCREEN_GAMEOVER);
    manager.gui->DestroySession(SCREEN_PAUSEMENU);
    manager.gui->DestroySession(SCREEN_HUD);
    manager.gui->DestroySession(SCREEN_PLAYERSLIST);

    delete m_bullettime;
}

void PlayManager::SetupMap(const Settings::PartySetting& playSetting)
{
    m_playSetting = playSetting;

    cout << "Loading level" << endl;

    // SCENE -------------------------------------------------------------------

    manager.level->LoadLevel(m_playSetting.playMap.file);

    // Marge
    map.aabb.Add(AABB(Vector3f(-8, -8, -8), Vector3f(8, 64, 8)));

    AABB newtonWordSize = map.aabb;
    newtonWordSize.Add(Vector3f(32.0f));
    parallelscene.newton->SetWorldSize(newtonWordSize);

    m_playTimeManager.startChrono = m_playTimeManager.curChrono = m_playSetting.playTime;

    scene::Fog* fog = manager.scene->GetFog();

    manager.scene->SetZFar(fog->IsEnable() ? fog->GetEnd() : map.aabb.GetSize() * 2);
    manager.scene->UpdateViewParameter();

    // PLAYERS -----------------------------------------------------------------

    ifstream namefile("NAMES.txt");

    do m_botNames.push_back(string());
    while(getline(namefile, m_botNames.back()));

    m_botNames.pop_back();

    namefile.close();

    m_userPlayer = new Player(this, m_playSetting.playerName.nick, m_playSetting.playerName.model);
    m_userPlayer->AttachController(new UserControl(this));

    manager.scene->GetRootNode()->AddChild(m_userPlayer);

    RegisterPlayer(m_userPlayer);
    ModSetupUser(m_userPlayer);

    m_bullettime = new BulletTime(this);

    // PPE ---------------------------------------------------------------------

    const Settings::Video& vidSets = manager.app->globalSettings.video;

    if(vidSets.usePpe)
    {
        using namespace tbe::ppe;

        ppe.boost = new MotionBlurEffect;
        ppe.boost->SetEnable(false);
        ppe.boost->SetIntensity(vidSets.ppe.boostIntensity);
        manager.ppe->AddPostEffect("boostEffect", ppe.boost);

        ppe.bullettime = new ColorEffect;
        ppe.bullettime->SetInternalPass(true);
        ppe.bullettime->SetRttFrameSize(vidSets.ppe.bullettimeSize);
        ppe.bullettime->SetFusionMode(ColorEffect::BLACK_WHITE);
        ppe.bullettime->SetColor(vidSets.ppe.bullettimeColor);
        ppe.bullettime->SetEnable(false);
        manager.ppe->AddPostEffect("blettimeEffect", ppe.bullettime);

        ppe.dammage = new ColorEffect;
        ppe.dammage->SetInternalPass(true);
        ppe.dammage->SetFusionMode(ColorEffect::MULTIPLICATION_COLOR);
        ppe.dammage->SetColor(vidSets.ppe.dammageColor);
        ppe.dammage->SetEnable(false);
        manager.ppe->AddPostEffect("dammageEffect", ppe.dammage);

        ppe.gameover = new BlurEffect;
        ppe.gameover->SetPasse(vidSets.ppe.gameoverPass);
        ppe.gameover->SetEnable(false);
        manager.ppe->AddPostEffect("gameoverEffect", ppe.gameover);

        ppe.bloom = new BloomEffect;
        ppe.bloom->SetRttFrameSize(vidSets.ppe.worldSize);
        ppe.bloom->SetIntensity(vidSets.ppe.worldIntensity);
        ppe.bloom->SetThreshold(vidSets.ppe.worldThershold);
        ppe.bloom->SetBlurPass(vidSets.ppe.worldBlurPass);
        manager.ppe->AddPostEffect("worldEffect", ppe.bloom);
    }
}

void PlayManager::SetupGui()
{
    using namespace tbe::gui;

    cout << "Loading GUI" << endl;

    Vector2f screenSize = manager.app->globalSettings.video.screenSize;

    Pencil bigPen(GUI_FONT, int(GUI_FONTSIZE * manager.app->globalSettings.video.guiSizeFactor * 1.5));

    // Tabaleau des joueur ------------------------------------------------------

    manager.gui->SetSession(SCREEN_PLAYERSLIST);

    manager.gui->AddLayout(Layout::Vertical);
    manager.gui->AddLayoutStretchSpace();
    manager.gui->AddLayout(Layout::Horizental);
    manager.gui->AddLayoutStretchSpace();

    hud.scorelist = manager.gui->AddTextBox("hud.scorelist");
    hud.scorelist->SetPencil(bigPen);
    hud.scorelist->SetSize(screenSize * Vector2f(0.75, 0.75));
    hud.scorelist->SetDefinedSize(true);
    hud.scorelist->SetBackground(GUI_SCORE);
    hud.scorelist->SetTextAlign(TextBox::LEFT);
    hud.scorelist->SetBackgroundPadding(Vector2f(16, 8));

    manager.gui->AddLayoutStretchSpace();
    manager.gui->EndLayout();
    manager.gui->AddLayoutStretchSpace();
    manager.gui->EndLayout();

    // GameOver ----------------------------------------------------------------

    manager.gui->SetSession(SCREEN_GAMEOVER);

    Texture black;
    black.Build(128, 0);

    hud.background.gameover = manager.gui->AddImage("hud.background.gameover", black);
    hud.background.gameover->SetSize(screenSize);
    hud.background.gameover->SetOpacity(0.75);
    hud.background.gameover->SetEnable(false);

    manager.gui->AddLayout(Layout::Vertical, 10, 10);
    manager.gui->AddLayoutStretchSpace();
    manager.gui->AddLayout(Layout::Horizental, 10, 10);
    manager.gui->AddLayoutStretchSpace();
    hud.gameover = manager.gui->AddTextBox("hud.gameover");
    hud.gameover->SetSize(screenSize * Vector2f(0.75, 0.75));
    hud.gameover->SetDefinedSize(true);
    hud.gameover->SetPencil(bigPen);
    hud.gameover->SetBackground(GUI_SCORE);
    hud.gameover->SetBackgroundPadding(8);
    hud.gameover->SetTextAlign(TextBox::LEFT);
    manager.gui->AddLayoutStretchSpace();
    manager.gui->EndLayout();
    manager.gui->AddLayoutStretchSpace();
    manager.gui->EndLayout();

    // Pause Menu --------------------------------------------------------------

    manager.gui->SetSession(SCREEN_PAUSEMENU);

    Image* backPause = manager.gui->AddImage("00:background", BACKGROUND_PAUSE);
    backPause->SetSize(screenSize);

    manager.gui->AddLayout(Layout::Vertical, 0, 10);

    manager.gui->AddLayout(Layout::Horizental, 10);
    hud.playmenu.ret = manager.gui->AddButton("hud.playmenu.ret", "Retour");
    hud.playmenu.quit = manager.gui->AddButton("hud.playmenu.quit", "Quitter");
    manager.gui->EndLayout();

    manager.gui->AddLayoutStretchSpace();

    TextBox* pauseLabel = manager.gui->AddTextBox("hud.pauseLabel");
    pauseLabel->SetPencil(bigPen);
    pauseLabel->Write("Menu Pause !");

    manager.gui->EndLayout();

    // HUD ---------------------------------------------------------------------

    manager.gui->SetSession(SCREEN_HUD);

    hud.background.dammage = manager.gui->AddImage("0:hud.background.dammage", BACKGROUND_DAMMAGE);
    hud.background.dammage->SetSize(screenSize);
    hud.background.dammage->SetEnable(false);

    hud.background.bullettime = manager.gui->AddImage("1:hud.background.bullettime", BACKGROUND_BULLETTIME);
    hud.background.bullettime->SetOpacity(0.5);
    hud.background.bullettime->SetSize(screenSize);
    hud.background.bullettime->SetEnable(false);

    manager.gui->AddImage("2:background", BACKGROUND_HUD)->SetSize(screenSize);

    Image* croshair = manager.gui->AddImage("01:croshair", GUI_CROSHAIR);
    croshair->SetPos(Vector2f(manager.app->globalSettings.video.screenSize) / 2.0f - croshair->GetSize() / 2.0f);

    manager.gui->AddLayout(Layout::Vertical, 10, 10);

    // -------- Barre des gauges
    manager.gui->AddLayout(Layout::Horizental, 10);
    manager.gui->AddLayoutStretchSpace();

    hud.item = manager.gui->AddStateShow("hud.item", GUI_ITEM, 2);
    hud.life = manager.gui->AddGauge("hud.life", "Vie");
    hud.life->SetSmooth(true, 1);
    hud.ammo = manager.gui->AddGauge("hud.ammo", "Munition");
    hud.ammo->SetSmooth(true, 1);
    hud.bullettime = manager.gui->AddGauge("hud.bullettime", "Bulettime");
    hud.bullettime->SetSmooth(true, 1);
    hud.boost = manager.gui->AddStateShow("hud.boost", GUI_BOOST, 2);

    manager.gui->AddLayoutStretchSpace();
    manager.gui->EndLayout();
    // --------

    manager.gui->AddLayoutStretchSpace();
    manager.gui->AddLayoutStretchSpace();

    // -------- Log
    manager.gui->AddLayout(Layout::Horizental);
    manager.gui->AddLayoutStretchSpace();

    hud.log = manager.gui->AddTextBox("hud.log");
    hud.log->SetPencil(bigPen);
    hud.log->SetBackground(GUI_NOTIFY);
    hud.log->SetBackgroundPadding(Vector2f(32, 16));
    hud.log->SetEnable(false);

    manager.gui->AddLayoutStretchSpace();
    manager.gui->EndLayout();
    // --------

    manager.gui->AddLayoutStretchSpace();

    // -------- State
    hud.state = manager.gui->AddTextBox("hud.state");
    // --------

    manager.gui->EndLayout();
}

void PlayManager::OnStartGame()
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

void PlayManager::Log(std::string msg)
{
    hud.log->Write(msg);
    hud.log->SetEnable(true);

    m_logClock.SnapShoot();
}

void PlayManager::ProcessDevelopperCodeEvent()
{
    EventManager* event = manager.gameEngine->GetEventManager();

    if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
        if(event->keyState[EventManager::KEY_F1])
        {
            Bullet b(this);
            b.SetDammage(10);
            m_userPlayer->TakeDammage(&b);
        }

        if(event->keyState[EventManager::KEY_F2])
            m_userPlayer->Kill();

        if(event->keyState[EventManager::KEY_F5])
        {
            Player::Array copy = m_players;

            for(unsigned i = 0; i < copy.size(); i++)
                if(copy[i] != m_userPlayer)
                {
                    UnRegisterPlayer(copy[i]);
                    delete copy[i];
                }

            m_players.clear();
        }

        if(event->keyState[EventManager::KEY_F6])
        {
            unsigned select = tools::rand(0, manager.app->globalSettings.availablePlayer.size());

            Settings::PlayerInfo& pi = manager.app->globalSettings.availablePlayer[select];

            Player* player = new Player(this, "TEST_BOT", pi.model);
            RegisterPlayer(player);
            ModSetupAi(player);

            player->AttachController(NULL);

            manager.scene->GetRootNode()->AddChild(player);
        }

        if(event->keyState[EventManager::KEY_F7])
        {
            unsigned select = tools::rand(0, manager.app->globalSettings.availablePlayer.size());

            Settings::PlayerInfo& pi = manager.app->globalSettings.availablePlayer[select];

            Player* player = new Player(this, "TEST_BOT_AI", pi.model);

            RegisterPlayer(player);
            ModSetupAi(player);

            manager.scene->GetRootNode()->AddChild(player);
        }
    }
}

void PlayManager::EventProcess()
{
    using namespace tbe::scene;

    manager.gameEngine->PollEvent();

    EventManager* event = manager.gameEngine->GetEventManager();

    // Session de jeu
    if(m_timeTo == TIME_TO_PLAY || m_timeTo == TIME_TO_VIEWSCORE)
    {
        manager.gui->SetSession(SCREEN_HUD);

        if(m_userPlayer->IsKilled())
            return;

        // Vue
        if(event->notify == EventManager::EVENT_MOUSE_MOVE)
            manager.scene->GetCurCamera()->SetRelRotate(event->mousePosRel);

        // Affichage des scores
        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(m_numslot.count(event->lastActiveKey.first))
                m_userPlayer->SlotWeapon(m_numslot[event->lastActiveKey.first]);

            if(event->lastActiveKey.first == EventManager::KEY_TAB)
            {
                manager.gameEngine->SetMouseVisible(true);

                m_timeTo = TIME_TO_VIEWSCORE;
            }
        }

        if(event->notify == EventManager::EVENT_KEY_UP
           && event->lastActiveKey.first == EventManager::KEY_TAB)
        {
            manager.gameEngine->SetMouseVisible(false);

            m_timeTo = TIME_TO_PLAY;
        }

        // Touche de pause (Esc)
        if(event->keyState[EventManager::KEY_ESCAPE])
        {
            manager.gameEngine->SetGrabInput(false);
            manager.gameEngine->SetMouseVisible(true);

            for(unsigned i = 0; i < m_players.size(); i++)
                m_bullettime->SetActive(false);

            m_timeTo = TIME_TO_PAUSE;
        }

        // Code developper
        #ifdef THEBALL_COMPILE_DEBUG
        ProcessDevelopperCodeEvent();
        #endif
    }

        // Session de pause
    else if(m_timeTo == TIME_TO_PAUSE)
    {
        manager.gui->SetSession(SCREEN_PAUSEMENU);

        bool done = false;
        while(!done)
        {
            manager.gameEngine->PollEvent();

            if(hud.playmenu.ret->IsActivate())
            {
                manager.gameEngine->SetGrabInput(true);
                manager.gameEngine->SetMouseVisible(false);

                done = true;
                m_timeTo = TIME_TO_PLAY;
            }

            else if(hud.playmenu.quit->IsActivate())
            {
                manager.gameEngine->SetGrabInput(true);
                manager.gameEngine->SetMouseVisible(false);

                running = false;
                done = true;
            }

            manager.gameEngine->BeginScene();
            manager.gui->Render();
            manager.gameEngine->EndScene();
        }
    }

    else if(m_timeTo == TIME_TO_GAMEOVER)
    {
        if(event->keyState[EventManager::KEY_SPACE]
           && m_validGameOver.IsEsplanedTime(2000))
            running = false;
    }
}

void PlayManager::GameProcess()
{
    using namespace tbe;
    using namespace tbe::scene;

    if(m_gameOver)
        return;

    for(unsigned i = m_players.size(); i < m_playSetting.playerCount; i++)
        if(m_spawnPlayer.IsEsplanedTime(4000))
        {
            unsigned selectPlayer = tools::rand(0, manager.app->globalSettings.availablePlayer.size());

            Settings::PlayerInfo& pi = manager.app->globalSettings.availablePlayer[selectPlayer];

            unsigned selectName = tools::rand(0, m_botNames.size());

            Player* player = new Player(this, m_botNames[selectName], pi.model);
            RegisterPlayer(player);

            manager.scene->GetRootNode()->AddChild(player);

            ModSetupAi(player);

            manager.sound->Play("respawn", player);

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

        if(player->IsKilled())
        {
            if(player == m_userPlayer)
            {
                manager.gameEngine->SetMouseVisible(false);

                m_timeTo = TIME_TO_PLAY;
            }

            if(player->clocks.readyToDelete.IsEsplanedTime(2000))
            {
                player->ReBorn();

                manager.sound->Play("respawn", player);
            }
        }

        else
        {

            if(!map.aabb.IsInner(player))
            {
                player->SetRandomSpawnPos();
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

        if(item->IsTaked())
        {
            if(item->IsReadyToReborn())
                item->Reborn();
        }

        else if(!map.aabb.IsInner(item))
            item->ResetPosition();
    }

    // Gestion du temps de jeu
    if(m_playSetting.playTime > 0)
    {
        if(m_playTimeManager.curChrono == 0 && !m_gameOver)
        {
            SetGameOver();
        }

        else if(m_playTimeManager.clock.IsEsplanedTime(1000))
        {
            m_playTimeManager.curChrono--;

            if(m_playTimeManager.curChrono <= 10)
                manager.sound->Play("notime", m_userPlayer);
        }
    }

    if(hud.log->IsEnable() && m_logClock.IsEsplanedTime(3000))
        hud.log->SetEnable(false);
}

void PlayManager::HudProcess()
{
    using namespace tbe::gui;

    // Gestion de l'ETH en jeu -------------------------------------------------

    sort(m_players.begin(), m_players.end(), PlayerScoreSortProcess);

    if(m_timeTo == TIME_TO_PLAY)
    {
        using boost::format;

        manager.gui->SetSession(SCREEN_HUD);

        const Weapon* curWeapon = m_userPlayer->GetCurWeapon();

        const int &ammoCount = curWeapon->GetAmmoCount(),
                &ammoCountMax = curWeapon->GetMaxAmmoCount(),
                &life = m_userPlayer->GetLife(),
                &bullettime = static_cast<int>(m_bullettime->GetValue() * 100);

        if(!m_userPlayer->IsKilled())
        {
            // Mise a jour des bar de progression (Vie, Muinition, Bullettime, Boost)

            hud.ammo->SetLabel((format("%1% %2%/%3%")
                               % curWeapon->GetWeaponName() % ammoCount % ammoCountMax).str());

            hud.ammo->SetValue(ammoCount * 100 / ammoCountMax);

            hud.life->SetLabel((format("Santé %1%/100") % life).str());

            hud.life->SetValue(life);

            hud.bullettime->SetLabel((format("Bullettilme %1%/100") % bullettime).str());

            hud.bullettime->SetValue(bullettime);

            hud.boost->SetCurState(m_userPlayer->IsBoostAvalaible());

            ostringstream ss;
            ModUpdateStateText(ss);

            hud.state->Write(ss.str());

            // Affichage de l'ecran de dommage si besoins

            if(manager.app->globalSettings.video.usePpe)
            {
                if(ppe.dammage->IsEnable())
                {
                    Vector4f color = ppe.dammage->GetColor();

                    if(color.w > 0)
                    {
                        color.w -= 0.01f;
                        ppe.dammage->SetColor(color);
                    }
                    else
                        ppe.dammage->SetEnable(false);
                }
            }
            else
            {
                if(hud.background.dammage->IsEnable())
                {
                    float opacity = hud.background.dammage->GetOpacity();

                    if(opacity > 0)
                        hud.background.dammage->SetOpacity(opacity - 0.01f);
                    else
                        hud.background.dammage->SetEnable(false);
                }
            }
        }
        else
        {
            hud.life->SetLabel("Mort !");
            hud.life->SetValue(0);
        }
    }

    // Gestion de l'ETH en affichage des scores --------------------------------

    if(m_timeTo == TIME_TO_VIEWSCORE && !m_userPlayer->IsKilled())
    {
        manager.gui->SetSession(SCREEN_PLAYERSLIST);

        ostringstream ss;
        ModUpdateScoreListText(ss);

        hud.scorelist->Write(ss.str());
    }

    // Gestion de l'ETH en gameover --------------------------------------------

    if(m_timeTo == TIME_TO_GAMEOVER)
    {
        manager.gui->SetSession(SCREEN_GAMEOVER);

        ostringstream ss;
        ModUpdateGameOverText(ss);

        hud.gameover->Write(ss.str());

        // Affichage de l'ecran gameover si besoin

        float opacity = hud.background.gameover->GetOpacity();

        if(opacity < 0.5)
            opacity += 0.01;

        hud.background.gameover->SetOpacity(opacity);
    }
}

void PlayManager::Render()
{
    Vector2i& screenSize = manager.app->globalSettings.video.screenSize;

    // Positionement camera ----------------------------------------------------

    Vector3f setPos = m_userPlayer->GetPos();

    if(!m_playerPosRec.empty())
        setPos.y -= (setPos.y - m_playerPosRec.back().y) / 3 * 2;

    m_playerPosRec.push_back(setPos);

    m_camera->SetPos(m_playerPosRec.front() + Vector3f(0, worldSettings.playerSize * 2, 0) - m_camera->GetTarget() * 2.5f);

    if(m_playerPosRec.size() > 2)
        m_playerPosRec.pop_front();

    // Physique ----------------------------------------------------------------

    parallelscene.newton->SetWorldTimestep(1.0f / m_newtonClock.GetEsplanedTime());

    m_bullettime->Process();

    // Son 3D ------------------------------------------------------------------

    if(!manager.app->globalSettings.noaudio)
    {
        FMOD_System_Set3DListenerAttributes(manager.fmodsys, 0, (FMOD_VECTOR*)(float*)m_camera->GetPos(), 0,
                                            (FMOD_VECTOR*)(float*)m_camera->GetTarget(),
                                            (FMOD_VECTOR*)(float*)m_camera->GetUp());

        FMOD_System_Update(manager.fmodsys);
    }

    // Rendue ------------------------------------------------------------------

    manager.gameEngine->BeginScene();

    if(manager.app->globalSettings.video.usePpe)
    {
        Rtt* rtt = manager.ppe->GetRtt();

        rtt->Use(true);
        rtt->Clear();
        manager.scene->Render();
        rtt->Use(false);

        m_shootTarget = manager.scene->ScreenToWorld(screenSize / 2, rtt);

        manager.ppe->Render();
    }
    else
    {
        manager.scene->Render();
        m_shootTarget = manager.scene->ScreenToWorld(screenSize / 2);
    }

    manager.gui->Render();

    manager.gameEngine->EndScene();
}

tbe::Vector3f PlayManager::GetShootTarget() const
{
    return m_shootTarget;
}

Player* PlayManager::GetUserPlayer() const
{
    return m_userPlayer;
}

const Player::Array& PlayManager::GetPlayers() const
{
    return m_players;
}

int PlayManager::ModulatScore(int score)
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

void PlayManager::SetGameOver()
{
    m_gameOver = true;
    m_timeTo = TIME_TO_GAMEOVER;

    for(unsigned i = 0; i < m_players.size(); i++)
        NewtonBodySetFreezeState(m_players[i]->GetPhysicBody()->GetBody(), true);

    manager.gameEngine->SetMouseVisible(true);

    hud.background.gameover->SetEnable(true);

    if(manager.app->globalSettings.video.usePpe)
        ppe.gameover->SetEnable(true);

    ostringstream ss;
    ModUpdateScoreListText(ss);

    hud.scorelist->Write(ss.str());
}

bool PlayManager::IsGameOver() const
{
    return m_gameOver;
}

void PlayManager::RegisterPlayer(Player* player)
{
    manager.material->AddPlayer(player);

    m_players.push_back(player);
}

void PlayManager::UnRegisterPlayer(Player* player)
{
    Player::Array::iterator it = find(m_players.begin(), m_players.end(), player);

    m_players.erase(it);
}

void PlayManager::HudItem(bool status)
{
    hud.item->SetCurState(status);
}

void PlayManager::HudBullettime(bool status)
{
    if(manager.app->globalSettings.video.usePpe)
    {
        ppe.bullettime->SetEnable(status);
        ppe.bloom->SetEnable(!status);
    }
    else
    {
        hud.background.bullettime->SetEnable(status);
    }
}

void PlayManager::HudBoost(bool status)
{
    if(manager.app->globalSettings.video.usePpe)
    {
        ppe.boost->SetEnable(status);
    }
    else
    {

    }
}

void PlayManager::HudDammage(bool status)
{
    if(manager.app->globalSettings.video.usePpe)
    {
        ppe.dammage->SetEnable(status);

        if(status)
            ppe.dammage->SetColor(Vector4f(1, 0, 0, 0.5));
    }
    else
    {
        hud.background.dammage->SetEnable(status);

        if(status)
            hud.background.dammage->SetOpacity(0.75);
    }
}

BulletTime* PlayManager::GetBullettime() const
{
    return m_bullettime;
}

const Player::Array PlayManager::GetTargetsOf(Player* player) const
{
    Player::Array array(m_players);
    array.erase(find(array.begin(), array.end(), player));

    return array;
}
