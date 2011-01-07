/*
 * File:   EditorManager.cpp
 * Author: b4n92uid
 *
 * Created on 30 décembre 2009, 20:26
 */

#include "EditorManager.h"

#include "GameManager.h"
#include "AppManager.h"
#include "BldParser.h"

using namespace std;
using namespace tbe;

EditorManager::EditorManager(AppManager* appManager) : GameManager(appManager)
{
    m_selectedNode = NULL;

    m_FFCamera = new scene::FreeFlyCamera;
    manager.scene->AddCamera(m_FFCamera);

    m_OCamera = new scene::OrbitalCamera;
    manager.scene->AddCamera(m_OCamera);

    m_camera = m_OCamera;

    m_timeTo = VIEW;

    m_navigation = ENTITY;

    parallelscene.newton->SetEnable(false);

    m_completeHelp = false;
}

EditorManager::~EditorManager()
{
    manager.gui->DestroySession(EDITOR_EDIT_ENTITY);
    manager.gui->DestroySession(EDITOR_EDIT_LIGHT);
    manager.gui->DestroySession(EDITOR_EDIT_FOG);
    manager.gui->DestroySession(EDITOR_EDIT_SKYBOX);
    manager.gui->DestroySession(EDITOR_EDIT_MUSIC);
    manager.gui->DestroySession(EDITOR_VIEW);
    manager.gui->DestroySession(EDITOR_GUI);
}

void EditorManager::SetupMap(const Settings::EditSetting& editSetting)
{
    m_editSetting = editSetting;

    m_fog = manager.scene->GetFog();
    m_sky = manager.scene->GetSkybox();

    if(m_editSetting.createNew)
    {
        map.name = "NewMap";
        m_selectedLight = NULL;
    }

    else
    {
        manager.level->LoadLevel(m_editSetting.editMap.file);

        parallelscene.newton->SetWorldSize(map.aabb);

        m_allEntity.reserve(map.staticObjects.size() + map.dynamicObjects.size() + map.items.size());
        m_allEntity.insert(m_allEntity.end(), map.staticObjects.begin(), map.staticObjects.end());
        m_allEntity.insert(m_allEntity.end(), map.dynamicObjects.begin(), map.dynamicObjects.end());
        m_allEntity.insert(m_allEntity.end(), map.items.begin(), map.items.end());

        m_selectedLight = map.lights.empty() ? NULL : map.lights.front();

        manager.scene->SetZFar(m_fog->IsEnable() ? m_fog->GetEnd() : map.aabb.GetSize() * 2);
        manager.scene->UpdateViewParameter();
    }

    m_axes = new scene::Axes(parallelscene.meshs, 2, 2);
    manager.scene->GetRootNode()->AddChild(m_axes);

    for(unsigned i = 0; i < map.spawnPoints.size(); i++)
    {
        StaticObject* visualSpawn = new StaticObject(this, OBJ_SPAWN, map.spawnPoints[i]);

        m_visualSpawnPoints.push_back(visualSpawn);
        manager.scene->GetRootNode()->AddChild(visualSpawn);

        NewEntity(visualSpawn);
    }

    // PPE ---------------------------------------------------------------------

    if(manager.app->globalSettings.video.usePpe)
    {
        using namespace tbe::ppe;

        ppe.bloom = new BloomEffect;
        ppe.bloom->SetThreshold(0.5);
        ppe.bloom->SetIntensity(0.75);
        ppe.bloom->SetBlurPass(10);
        manager.ppe->AddPostEffect("worldEffect", ppe.bloom);

        //        ppe.blur = new BlurEffect;
        //        ppe.blur->SetPasse(5);
        //        ppe.blur->SetEnable(false);
        //        manager.ppe->AddPostEffect("blur", ppe.blur);
    }
}

inline bool MusicCheck(const boost::filesystem::path& filename)
{
    string validExt[] = {
        ".mp3", ".wav", ".ogg"
    };

    string* end = validExt + 3;

    return find(validExt, end, filename.extension()) != end;
}

void EditorManager::SetupGui()
{
    using namespace gui;
    using namespace boost::filesystem;

    directory_iterator endDir;

    const Vector2f& screenSize = manager.app->globalSettings.video.screenSize;
    const float& sizeFactor = manager.app->globalSettings.video.guiSizeFactor;

    // Entity
    manager.gui->SetSession(EDITOR_EDIT_ENTITY);

    manager.gui->AddLayout(Layout::Vertical, 10, 10);
    manager.gui->AddLayoutStretchSpace();
    manager.gui->AddLayout(Layout::Horizental, 5, 5)
            ->SetAlign(Layout::AFTER);
    hud.entity.info = manager.gui->AddTextBox("");
    hud.entity.info->SetBackground(GUI_TEXTBOX_V);
    hud.entity.info->SetBackgroundPadding(16);
    hud.entity.info->SetEnableBackground(true);
    manager.gui->AddLayoutStretchSpace();
    hud.entity.list = manager.gui->AddListBox("entity.list");
    hud.entity.list->SetBackground(GUI_LISTBOX_V);
    hud.entity.list->SetBackgroundPadding(8);
    hud.entity.list->SetDefinedSize(true);
    hud.entity.list->SetSize(Vector2f(192, 192) * sizeFactor);
    hud.entity.list->SetEnable(false);
    manager.gui->EndLayout();
    manager.gui->EndLayout();

    // Light
    manager.gui->SetSession(EDITOR_EDIT_LIGHT);

    manager.gui->AddLayout(Layout::Horizental, 0, 10);

    manager.gui->AddLayout(Layout::Vertical);
    manager.gui->AddLayoutStretchSpace();
    hud.light.info = manager.gui->AddTextBox("");
    hud.light.info->SetBackground(GUI_TEXTBOX_H);
    hud.light.info->SetBackgroundPadding(16);
    hud.light.info->SetEnableBackground(true);
    manager.gui->EndLayout();

    manager.gui->AddLayoutStretchSpace();

    manager.gui->AddLayout(Layout::Vertical, 5);
    hud.light.specular = manager.gui->AddVectorBox("light.specular", 0);
    manager.gui->AddTextBox("")->Write("Spéculaire");
    hud.light.diffuse = manager.gui->AddVectorBox("light.diffuse", 0);
    manager.gui->AddTextBox("")->Write("Diffuse");
    hud.light.amibent = manager.gui->AddVectorBox("light.ambiante", 0);
    manager.gui->AddTextBox("")->Write("Ambiante de la scene");
    hud.light.radius = manager.gui->AddSwitchNumeric<float>("light.radius");
    manager.gui->AddTextBox("")->Write("Rayon");
    hud.light.type = manager.gui->AddSwitchString("light.type");
    manager.gui->AddTextBox("")->Write("Type");
    hud.light.slector = manager.gui->AddSwitchString("light.selector");
    manager.gui->AddTextBox("")->Write("Séléction");
    manager.gui->EndLayout();

    manager.gui->EndLayout();

    // Skybox
    manager.gui->SetSession(EDITOR_EDIT_SKYBOX);

    manager.gui->AddLayout(Layout::Horizental, 0, 10);

    manager.gui->AddLayout(Layout::Vertical, 5);
    manager.gui->AddLayoutStretchSpace();
    hud.sky.info = manager.gui->AddTextBox("");
    hud.sky.info->SetBackground(GUI_TEXTBOX_H);
    hud.sky.info->SetBackgroundPadding(16);
    hud.sky.info->SetEnableBackground(true);
    manager.gui->EndLayout();

    manager.gui->AddLayoutStretchSpace();

    manager.gui->AddLayout(Layout::Vertical, 5);
    hud.sky.list = manager.gui->AddListBox("sky.list");
    hud.sky.list->SetBackground(GUI_LISTBOX_V);
    hud.sky.list->SetBackgroundPadding(8);
    hud.sky.list->SetDefinedSize(true);
    hud.sky.list->SetSize(Vector2f(192, 192) * sizeFactor);
    hud.sky.face = manager.gui->AddSwitchString("sky.face");
    hud.sky.apply = manager.gui->AddButton("sky.apply", "Appliquer");
    hud.sky.clear = manager.gui->AddButton("sky.clear", "Effacer");
    hud.sky.enable = manager.gui->AddSwitchString("sky.enable");
    manager.gui->EndLayout();

    // Fog
    manager.gui->SetSession(EDITOR_EDIT_FOG);

    manager.gui->AddLayout(Layout::Horizental, 0, 10);

    manager.gui->AddLayout(Layout::Vertical, 5);
    manager.gui->AddLayoutStretchSpace();
    hud.fog.info = manager.gui->AddTextBox("");
    hud.fog.info->SetBackground(GUI_TEXTBOX_H);
    hud.fog.info->SetBackgroundPadding(16);
    hud.fog.info->SetEnableBackground(true);
    manager.gui->EndLayout();

    manager.gui->AddLayoutStretchSpace();

    manager.gui->AddLayout(Layout::Vertical, 5);
    hud.fog.end = manager.gui->AddSwitchNumeric<float>("fog.end");
    manager.gui->AddTextBox("")->Write("Fin");
    hud.fog.start = manager.gui->AddSwitchNumeric<float>("fog.start");
    manager.gui->AddTextBox("")->Write("Début");
    hud.fog.color = manager.gui->AddVectorBox("fog.color", 0);
    manager.gui->AddTextBox("")->Write("Couleur");
    hud.fog.enable = manager.gui->AddSwitchString("fog.enable");
    manager.gui->EndLayout();

    manager.gui->EndLayout();

    // Music
    manager.gui->SetSession(EDITOR_EDIT_MUSIC);

    manager.gui->AddLayout(Layout::Horizental, 0, 10);

    manager.gui->AddLayout(Layout::Vertical, 5);
    manager.gui->AddLayoutStretchSpace();
    hud.music.info = manager.gui->AddTextBox("");
    hud.music.info->SetBackground(GUI_TEXTBOX_H);
    hud.music.info->SetBackgroundPadding(16);
    hud.music.info->SetEnableBackground(true);
    manager.gui->EndLayout();

    manager.gui->AddLayoutStretchSpace();

    manager.gui->AddLayout(Layout::Vertical, 5);
    hud.music.list = manager.gui->AddListBox("music.list");
    hud.music.list->SetBackground(GUI_LISTBOX_V);
    hud.music.list->SetBackgroundPadding(8);
    hud.music.list->SetDefinedSize(true);
    hud.music.list->SetSize(Vector2f(192, 192) * sizeFactor);
    manager.gui->EndLayout();

    manager.gui->EndLayout();

    manager.gui->SetSession(EDITOR_VIEW);
    // Clear

    manager.gui->SetSession(EDITOR_GUI);

    Image* backPause = manager.gui->AddImage("00:background", BACKGROUND_PAUSE);
    backPause->SetSize(screenSize);

    manager.gui->AddLayout(Layout::Horizental, 0, 10);
    manager.gui->AddLayoutStretchSpace();
    manager.gui->AddLayout(Layout::Vertical, 10);
    hud.pause.quit = manager.gui->AddButton("hud.pause.quit", "Quitter");
    hud.pause.save = manager.gui->AddButton("hud.pause.save", "Enregistrer");
    hud.pause.ret = manager.gui->AddButton("hud.pause.ret", "Retour");
    manager.gui->AddLayoutStretchSpace();
    hud.pause.name = manager.gui->AddEditBox("hud.pause.name", map.name);
    manager.gui->EndLayout();
    manager.gui->EndLayout();

    // Remplisage --------------------------------------------------------------

    // Lumiere

    hud.light.amibent->SetRange(0, 1);
    hud.light.diffuse->SetRange(0, 1);
    hud.light.specular->SetRange(0, 1);
    hud.light.radius->SetRange(new SwitchNumeric<float>::Range(0, 128));

    hud.light.type->Push("Diri").Push("Point");

    if(!map.lights.empty())
    {
        for(unsigned i = 0; i < map.lights.size(); i++)
            hud.light.slector->Push("Light #" + tools::numToStr(i), i);

        hud.light.slector->SetCurrent(0);

        SelectLight(0);
    }

    hud.light.amibent->SetValue(vec43(manager.scene->GetAmbientLight()));

    // Sky

    Texture* skyTex = m_sky->GetTextures();

    hud.sky.enable->Push("Desactiver").Push("Activer");
    hud.sky.enable->SetCurrent(m_sky->IsEnable());

    hud.sky.face
            ->Push("Devant", skyTex[0].GetFilename())
            .Push("Deriere", skyTex[1].GetFilename())
            .Push("Haut", skyTex[2].GetFilename())
            .Push("Bas", skyTex[3].GetFilename())
            .Push("Gauche", skyTex[4].GetFilename())
            .Push("Droite", skyTex[5].GetFilename());

    {
        for(directory_iterator it(SKYBOX_DIR); it != endDir; it++)
        {
            const path& imgFilename = it->path();

            if(!is_directory(imgFilename))
                hud.sky.list->Push(imgFilename.filename(), imgFilename.string());
        }

        hud.sky.list->Update();
    }

    // Fog

    float fogStart = m_fog->GetStart();
    float fogEnd = m_fog->GetEnd();

    hud.fog.color->SetValue(vec43(m_fog->GetColor()));
    hud.fog.color->SetRange(0, 1);

    hud.fog.start->SetValue(fogStart);
    hud.fog.start->SetRange(new gui::SwitchNumeric<float>::Range(0, fogEnd - 1));

    hud.fog.end->SetValue(fogEnd);
    hud.fog.end->SetRange(new gui::SwitchNumeric<float>::Range(fogStart + 1, 512));

    hud.fog.enable->Push("Désactiver").Push("Activer");
    hud.fog.enable->SetCurrent(m_fog->IsEnable());

    // Statique
    {
        for(directory_iterator it(MODELS_MAPS_DIR); it != endDir; it++)
        {
            const path& staricFilename = it->path();

            if(!is_directory(staricFilename) && staricFilename.extension() == ".obj")
                hud.entity.list->Push(staricFilename.filename(), staricFilename.string());
        }

        hud.entity.list->Update();
    }

    // Musique
    {
        for(directory_iterator it(MUSIC_DIR); it != endDir; it++)
        {
            const path& musicFilename = it->path();

            if(!is_directory(musicFilename) && MusicCheck(musicFilename))
                hud.music.list->Push(musicFilename.filename(), musicFilename.string());
        }

        hud.music.list->Update();
    }
}

bool EditorManager::SettingMusicEvent(tbe::EventManager* event)
{
    #ifndef THEBALL_NO_AUDIO
    if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
        if(event->lastActiveKey.first == EventManager::KEY_DELETE)
        {
            if(map.musicStream)
                FMOD_Sound_Release(map.musicStream);

            map.musicChannel = 0;
            map.musicStream = NULL;

            map.musicPath.clear();

            hud.music.list->CancelSelection();
        }

        if(event->lastActiveKey.first == EventManager::KEY_SPACE)
        {
            FMOD_BOOL state = false;
            FMOD_Channel_IsPlaying(map.musicChannel, &state);

            if(state)
                FMOD_Channel_Stop(map.musicChannel);

            else
                FMOD_System_PlaySound(manager.fmodsys, FMOD_CHANNEL_FREE,
                                      map.musicStream, false, &map.musicChannel);
        }
    }

    if(hud.music.list->IsActivate())
    {
        if(map.musicStream)
            FMOD_Sound_Release(map.musicStream);

        map.musicPath = hud.music.list->GetCurrentData().GetValue<string > ();

        FMOD_RESULT res = FMOD_System_CreateStream(manager.fmodsys, map.musicPath.c_str(),
                                                   FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE,
                                                   0, &map.musicStream);

        if(res == FMOD_OK)
            FMOD_System_PlaySound(manager.fmodsys, FMOD_CHANNEL_FREE, map.musicStream, false, &map.musicChannel);

        hud.music.list->SetActivate(false);
    }
    #endif

    return false;
}

bool EditorManager::SettingSkyEvent(tbe::EventManager* event)
{
    if(hud.sky.enable->IsActivate())
        m_sky->SetEnable(hud.sky.enable->GetCurrent());

    else if(hud.sky.clear->IsActivate())
    {
        m_sky->Clear();
    }

    else if(hud.sky.apply->IsActivate())
    {
        Texture skyTex[6];

        for(unsigned i = 0; i < 6; i++)
        {
            string path = hud.sky.face->GetData(i).GetValue<string > ();

            if(!path.empty())
                skyTex[i] = path;
        }

        m_sky->SetTextures(skyTex);
    }

    else if(hud.sky.list->IsActivate())
    {
        hud.sky.face->SetData(hud.sky.list->GetCurrentData());
    }

    else if(hud.sky.face->IsActivate())
    {
        hud.sky.list->CancelSelection();
    }

    return false;
}

bool EditorManager::SettingFogEvent(tbe::EventManager* event)
{
    if(hud.fog.enable->IsActivate())
    {
        m_fog->SetEnable(hud.fog.enable->GetCurrent());

        manager.scene->SetZNear(m_fog->IsEnable() ? m_fog->GetEnd() : map.aabb.GetSize());
        manager.scene->UpdateViewParameter();
    }

    else if(hud.fog.color->IsActivate())
        m_fog->SetColor(vec34(hud.fog.color->GetValue()));

    else if(hud.fog.start->IsActivate())
    {
        m_fog->SetStart(hud.fog.start->GetValue());

        gui::SwitchNumeric<float>::Range* range = new gui::SwitchNumeric<float>::Range(hud.fog.start->GetValue() + 1, 512);

        hud.fog.end->SetRange(range);
    }

    else if(hud.fog.end->IsActivate())
    {
        m_fog->SetEnd(hud.fog.end->GetValue());

        gui::SwitchNumeric<float>::Range* range = new gui::SwitchNumeric<float>::Range(0, hud.fog.end->GetValue() - 1);

        hud.fog.start->SetRange(range);

        manager.scene->SetZNear(m_fog->GetEnd());
        manager.scene->UpdateViewParameter();
    }

    return false;
}

void EditorManager::EditEventPorcess(tbe::EventManager* event)
{
    if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
        switch(event->lastActiveKey.first)
        {
            case EventManager::KEY_TAB:
                m_completeHelp = !m_completeHelp;
                break;
        }
    }

    else if(event->notify == EventManager::EVENT_MOUSE_DOWN)
    {
        if(event->mouseState[EventManager::MOUSE_BUTTON_MIDDLE])
        {
            manager.gameEngine->SetGrabInput(true);
            manager.gameEngine->SetMouseVisible(false);

            m_timeTo = VIEW;
            return;
        }
        else if(event->mouseState[EventManager::MOUSE_BUTTON_LEFT])
        {
            if(!map.aabb.IsInner(m_3dSelect))
                m_axes->SetPos(0);

            else
                m_axes->SetPos(m_3dSelect);
        }
    }

    switch(event->lastActiveKey.first)
    {
        case EventManager::KEY_F1:
            m_navigation = ENTITY;
            break;
        case EventManager::KEY_F2:
            m_navigation = LIGHT;
            break;
        case EventManager::KEY_F5:
            m_navigation = SKYBOX;
            break;
        case EventManager::KEY_F6:
            m_navigation = FOG;
            break;
        case EventManager::KEY_F7:
            m_navigation = MUSIC;
            break;
    }

    switch(m_navigation)
    {
        case ENTITY:
            if(!SettingEntityEvent(event))
                if(!SelectEntityEvent(event))
                    if(!DeleteEntityEvent(event))
                        AllocEntityEvent(event);
            break;
        case LIGHT:
            if(!SettingLightEvent(event))
                if(!DeleteLightEvent(event))
                    AllocLightEvent(event);
            break;

        case SKYBOX:
            SettingSkyEvent(event);
            break;

        case FOG:
            SettingFogEvent(event);
            break;

        case MUSIC:
            SettingMusicEvent(event);
            break;
    }
}

void EditorManager::ViewEventPorcess(tbe::EventManager* event)
{
    if(event->notify == EventManager::EVENT_MOUSE_DOWN)
    {
        if(event->mouseState[EventManager::MOUSE_BUTTON_MIDDLE])
        {
            manager.gameEngine->SetGrabInput(false);
            manager.gameEngine->SetMouseVisible(true);

            m_timeTo = EDIT;
            return;
        }
    }
    else if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
        switch(event->lastActiveKey.first)
        {
            case 'r':
                m_axes->SetPos(0);
                m_OCamera->SetCenter(0);
                m_FFCamera->SetPos(2);
                m_FFCamera->SetTarget(-1);
                m_selectedNode = NULL;
                break;

            case 'v':
                if(m_camera == m_FFCamera)
                    m_camera = m_OCamera;
                else
                    m_camera = m_FFCamera;

                manager.scene->SetCurCamera(m_camera);
                break;
        }

    }

    if(event->keyState[EventManager::KEY_LSHIFT])
        m_FFCamera->SetSpeed(1);
    else
        m_FFCamera->SetSpeed(0.25);

    m_OCamera->SetCenter(m_axes->GetPos());
    m_camera->OnEvent(event);
}

void EditorManager::GuiEventPorcess(tbe::EventManager* event)
{
    if(hud.pause.quit->IsActivate())
    {
        running = false;
    }
    else if(hud.pause.save->IsActivate())
    {
        map.name = hud.pause.name->GetLabel();
        string saveFileName = MAPS_DIR + tools::UnixName(map.name) + ".bld";

        if(!map.name.empty())
        {
            map.spawnPoints.clear();

            for(unsigned i = 0; i < m_visualSpawnPoints.size(); i++)
                map.spawnPoints.push_back(m_visualSpawnPoints[i]->GetPos());

            if(m_editSetting.createNew)
            {
                manager.level->SaveLevel(saveFileName);
            }

            else
            {
                manager.level->SaveLevel();

                std::rename(manager.level->GetOpenFileName().c_str(), saveFileName.c_str());
            }

            hud.pause.save->SetActivate(false);

            manager.app->globalSettings.ReadMapInfo();
            manager.app->UpdateGuiContent();
        }

    }
    else if(hud.pause.ret->IsActivate())
    {
        manager.gameEngine->SetGrabInput(true);
        manager.gameEngine->SetMouseVisible(false);

        m_timeTo = VIEW;
    }
}

void EditorManager::EventProcess()
{
    EventManager* event = manager.gameEngine->GetEventManager();

    manager.gameEngine->PollEvent();

    if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
        if(event->lastActiveKey.first == EventManager::KEY_ESCAPE)
        {
            manager.gameEngine->SetGrabInput(false);
            manager.gameEngine->SetMouseVisible(true);

            m_timeTo = PAUSE;
        }
    }

    switch(m_timeTo)
    {
        case EDIT:
            EditEventPorcess(event);
            break;

        case VIEW:
            ViewEventPorcess(event);
            break;

        case PAUSE:
            GuiEventPorcess(event);
            break;
    }
}

inline string skyTexBaseName(const Texture& tex)
{
    string filename = tex.GetFilename();

    unsigned pos = filename.find_last_of('\\');

    if(pos == string::npos)
        pos = filename.find_last_of('/');

    return filename.substr(pos + 1);
}

void EditorManager::HudProcess()
{
    using namespace gui;

    stringstream info;

    info << "Curseur 3D : " << m_3dSelect << endl;

    info << endl;

    switch(m_navigation)
    {
        case ENTITY:
            info << "Entités" << endl;

            if(m_selectedNode)
                info << "Selection : " << m_selectedNode->GetName() << " ; " << m_selectedNode->GetPos();
            else
                info << "Selection : NULL";

            if(m_completeHelp)
            {
                info << endl;
                info << "Statiques :" << endl
                        << "S   : Séléction" << endl

                        << "Dynamiques :" << endl
                        << "D+J : Jumper" << endl
                        << "D+T : Teleporter" << endl

                        << "Items :" << endl
                        << "T+L     : Santé" << endl
                        << "T+A     : Munnition" << endl
                        << "T+S     : Super-life" << endl
                        << "T+F     : Fata-shot" << endl
                        << "T+I     : Arme Finder" << endl
                        << "T+H     : Arme Shotgun" << endl
                        << "T+B     : Arme Bomb" << endl
                        << "C       : Cloner" << endl
                        << "Suppr   : Supprimer" << endl;

                info << "Mouvements :" << endl
                        << "SHIFT       : Déplacement X, Z" << endl
                        << "SHIFT+ALT   : Déplacement Y" << endl
                        << "SHIFT+F     : Au sol" << endl
                        << "SHIFT+G     : Aligner. Grille" << endl
                        << "SHIFT+[AE]  : Rotation X" << endl
                        << "SHIFT+[QD]  : Rotation Y" << endl
                        << "SHIFT+[WC]  : Rotation Z" << endl;

            }

            hud.entity.info->Write(info.str());
            break;

        case LIGHT:
            info << "Lumieres";

            if(m_completeHelp)
            {
                info << endl;
                info << "Création lumiers" << endl
                        << "P   : Ponctuelle" << endl
                        << "D   : Dirictionelle" << endl;

                info << "Config. lumiers" << endl
                        << "SHIFT       : Déplacement X, Z" << endl
                        << "ALT+SHIFT   : Déplacement Y" << endl
                        << "Suppr       : Supprimer";
            }

            hud.light.info->Write(info.str());
            break;

        case SKYBOX:
            info << "Skybox";

            if(m_sky)
            {
                Texture* skyTex = m_sky->GetTextures();

                info << endl
                        << "Devant: " << skyTexBaseName(skyTex[0]) << endl
                        << "Derierre: " << skyTexBaseName(skyTex[1]) << endl
                        << "Haut: " << skyTexBaseName(skyTex[2]) << endl
                        << "Bas: " << skyTexBaseName(skyTex[3]) << endl
                        << "Gauche: " << skyTexBaseName(skyTex[4]) << endl
                        << "Droite: " << skyTexBaseName(skyTex[5]);
            }

            hud.sky.info->Write(info.str());
            break;

        case FOG:
            info << "Brouiallrd (Fog)";

            hud.fog.info->Write(info.str());
            break;

        case MUSIC:
            info << "Musique" << endl
                    << "Espace  : Lancer/Arreter la musique" << endl
                    << "Suppr   : Désactiver la musique pour la carte";

            if(map.musicStream)
                info << endl << ">> " << map.musicPath;

            hud.music.info->Write(info.str());
            break;
    }
}

void EditorManager::Render()
{
    EventManager* event = manager.gameEngine->GetEventManager();

    switch(m_timeTo)
    {
        case VIEW:
            manager.gui->SetSession(EDITOR_VIEW);
            break;
        case EDIT:
            switch(m_navigation)
            {
                case ENTITY:
                    manager.gui->SetSession(EDITOR_EDIT_ENTITY);
                    break;
                case LIGHT:
                    manager.gui->SetSession(EDITOR_EDIT_LIGHT);
                    break;
                case SKYBOX:
                    manager.gui->SetSession(EDITOR_EDIT_SKYBOX);
                    break;
                case FOG:
                    manager.gui->SetSession(EDITOR_EDIT_FOG);
                    break;
                case MUSIC:
                    manager.gui->SetSession(EDITOR_EDIT_MUSIC);
                    break;
            }
            break;
        case PAUSE:
            manager.gui->SetSession(EDITOR_GUI);
            break;
    }

    manager.gameEngine->BeginScene();

    if(manager.app->globalSettings.video.usePpe)
    {
        Rtt* rtt = manager.ppe->GetRtt();

        rtt->Use(true);
        rtt->Clear();
        manager.scene->Render();
        rtt->Use(false);

        m_3dSelect = manager.scene->ScreenToWorld(event->mousePos, rtt);

        manager.ppe->Render();
    }
    else
    {
        manager.scene->Render();
        m_3dSelect = manager.scene->ScreenToWorld(event->mousePos);
    }

    manager.gui->Render();

    manager.gameEngine->EndScene();
}
