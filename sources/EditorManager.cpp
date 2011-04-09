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
    m_FFCamera->setSpeed(4);
    manager.scene->addCamera(m_FFCamera);

    m_OCamera = new scene::OrbitalCamera;
    manager.scene->addCamera(m_OCamera);

    m_camera = m_OCamera;

    m_timeTo = VIEW;

    m_navigation = ENTITY;

    parallelscene.newton->setEnable(false);

    m_completeHelp = false;
}

EditorManager::~EditorManager()
{
    manager.gui->destroySession(EDITOR_EDIT_ENTITY);
    manager.gui->destroySession(EDITOR_EDIT_LIGHT);
    manager.gui->destroySession(EDITOR_EDIT_FOG);
    manager.gui->destroySession(EDITOR_EDIT_SKYBOX);
    manager.gui->destroySession(EDITOR_EDIT_MUSIC);
    manager.gui->destroySession(EDITOR_VIEW);
    manager.gui->destroySession(EDITOR_GUI);
}

void EditorManager::setupMap(const Settings::EditSetting& editsetting)
{
    m_editSetting = editsetting;

    m_fog = manager.scene->getFog();
    m_sky = manager.scene->getSkybox();

    if(m_editSetting.createNew)
    {
        map.name = "NewMap";
        m_selectedLight = NULL;
    }

    else
    {
        manager.level->loadLevel(m_editSetting.editMap.file);

        parallelscene.newton->setWorldSize(map.aabb);

        m_allEntity.reserve(map.staticObjects.size() + map.dynamicObjects.size() + map.items.size());
        m_allEntity.insert(m_allEntity.end(), map.staticObjects.begin(), map.staticObjects.end());
        m_allEntity.insert(m_allEntity.end(), map.dynamicObjects.begin(), map.dynamicObjects.end());
        m_allEntity.insert(m_allEntity.end(), map.items.begin(), map.items.end());

        m_selectedLight = map.lights.empty() ? NULL : map.lights.front();

        manager.scene->setZFar(m_fog->isEnable() ? m_fog->getEnd() : map.aabb.getSize() * 2);
        manager.scene->updateViewParameter();
    }

    m_axes = new scene::Axes(parallelscene.meshs, 2, 2);
    manager.scene->getRootNode()->addChild(m_axes);

    for(unsigned i = 0; i < map.spawnPoints.size(); i++)
    {
        StaticObject* visualSpawn = new StaticObject(this, OBJ_SPAWN, map.spawnPoints[i]);

        m_visualSpawnPoints.push_back(visualSpawn);

        newEntity(visualSpawn);
    }

    // PPE ---------------------------------------------------------------------

    const Settings::Video& vidsets = manager.app->globalSettings.video;

    if(manager.app->globalSettings.video.usePpe)
    {
        using namespace tbe::ppe;

        ppe.bloom = new BloomEffect;
        ppe.bloom->setRttFrameSize(vidsets.ppe.worldSize);
        ppe.bloom->setIntensity(vidsets.ppe.worldIntensity);
        ppe.bloom->setThreshold(vidsets.ppe.worldThershold);
        ppe.bloom->setBlurPass(vidsets.ppe.worldBlurPass);
        manager.ppe->addPostEffect("worldEffect", ppe.bloom);
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

void EditorManager::setupGui()
{
    using namespace gui;
    using namespace boost::filesystem;

    directory_iterator endDir;

    const Vector2f& screenSize = manager.app->globalSettings.video.screenSize;
    const float& sizeFactor = manager.app->globalSettings.video.guiSizeFactor;

    Pencil bigpen(GUI_FONT, int(sizeFactor * GUI_FONTSIZE * 1.5));

    // Entity
    manager.gui->setSession(EDITOR_EDIT_ENTITY);

    manager.gui->addLayout(Layout::Vertical, 10, 10);
    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayout(Layout::Horizental, 5, 5)
            ->setAlign(Layout::AFTER);
    hud.entity.info = manager.gui->addTextBox("");
    hud.entity.info->setBackground(GUI_TEXTBOX_V);
    hud.entity.info->setBackgroundPadding(16);
    hud.entity.info->setEnableBackground(true);
    manager.gui->addLayoutStretchSpace();
    hud.entity.list = manager.gui->addListBox("entity.list");
    hud.entity.list->setBackground(GUI_LISTBOX_V);
    hud.entity.list->setBackgroundPadding(8);
    hud.entity.list->setDefinedSize(true);
    hud.entity.list->setSize(Vector2f(192, 192) * sizeFactor);
    hud.entity.list->setEnable(false);
    manager.gui->endLayout();
    manager.gui->endLayout();

    // Light
    manager.gui->setSession(EDITOR_EDIT_LIGHT);

    manager.gui->addLayout(Layout::Horizental, 0, 10);

    manager.gui->addLayout(Layout::Vertical);
    manager.gui->addLayoutStretchSpace();
    hud.light.info = manager.gui->addTextBox("");
    hud.light.info->setBackground(GUI_TEXTBOX_H);
    hud.light.info->setBackgroundPadding(16);
    hud.light.info->setEnableBackground(true);
    manager.gui->endLayout();

    manager.gui->addLayoutStretchSpace();

    manager.gui->addLayout(Layout::Vertical, 5);
    hud.light.specular = manager.gui->addVectorBox("light.specular", 0);
    manager.gui->addTextBox("")->write("Spéculaire");
    hud.light.diffuse = manager.gui->addVectorBox("light.diffuse", 0);
    manager.gui->addTextBox("")->write("Diffuse");
    hud.light.amibent = manager.gui->addVectorBox("light.ambiante", 0);
    manager.gui->addTextBox("")->write("Ambiante de la scene");
    hud.light.radius = manager.gui->addSwitchNumeric<float>("light.radius");
    manager.gui->addTextBox("")->write("Rayon");
    hud.light.type = manager.gui->addSwitchString("light.type");
    manager.gui->addTextBox("")->write("Type");
    hud.light.slector = manager.gui->addSwitchString("light.selector");
    manager.gui->addTextBox("")->write("Séléction");
    manager.gui->endLayout();

    manager.gui->endLayout();

    // Skybox
    manager.gui->setSession(EDITOR_EDIT_SKYBOX);

    manager.gui->addLayout(Layout::Horizental, 0, 10);

    manager.gui->addLayout(Layout::Vertical, 5);
    manager.gui->addLayoutStretchSpace();
    hud.sky.info = manager.gui->addTextBox("");
    hud.sky.info->setBackground(GUI_TEXTBOX_H);
    hud.sky.info->setBackgroundPadding(16);
    hud.sky.info->setEnableBackground(true);
    manager.gui->endLayout();

    manager.gui->addLayoutStretchSpace();

    manager.gui->addLayout(Layout::Vertical, 5);
    hud.sky.list = manager.gui->addListBox("sky.list");
    hud.sky.list->setBackground(GUI_LISTBOX_V);
    hud.sky.list->setBackgroundPadding(8);
    hud.sky.list->setDefinedSize(true);
    hud.sky.list->setSize(Vector2f(192, 192) * sizeFactor);
    hud.sky.face = manager.gui->addSwitchString("sky.face");
    hud.sky.apply = manager.gui->addButton("sky.apply", "Appliquer");
    hud.sky.clear = manager.gui->addButton("sky.clear", "Effacer");
    hud.sky.enable = manager.gui->addSwitchString("sky.enable");
    manager.gui->endLayout();

    // Fog
    manager.gui->setSession(EDITOR_EDIT_FOG);

    manager.gui->addLayout(Layout::Horizental, 0, 10);

    manager.gui->addLayout(Layout::Vertical, 5);
    manager.gui->addLayoutStretchSpace();
    hud.fog.info = manager.gui->addTextBox("");
    hud.fog.info->setBackground(GUI_TEXTBOX_H);
    hud.fog.info->setBackgroundPadding(16);
    hud.fog.info->setEnableBackground(true);
    manager.gui->endLayout();

    manager.gui->addLayoutStretchSpace();

    manager.gui->addLayout(Layout::Vertical, 5);
    hud.fog.end = manager.gui->addSwitchNumeric<float>("fog.end");
    manager.gui->addTextBox("")->write("Fin");
    hud.fog.start = manager.gui->addSwitchNumeric<float>("fog.start");
    manager.gui->addTextBox("")->write("Début");
    hud.fog.color = manager.gui->addVectorBox("fog.color", 0);
    manager.gui->addTextBox("")->write("Couleur");
    hud.fog.enable = manager.gui->addSwitchString("fog.enable");
    manager.gui->endLayout();

    manager.gui->endLayout();

    // Music
    manager.gui->setSession(EDITOR_EDIT_MUSIC);

    manager.gui->addLayout(Layout::Horizental, 0, 10);

    manager.gui->addLayout(Layout::Vertical, 5);
    manager.gui->addLayoutStretchSpace();
    hud.music.info = manager.gui->addTextBox("");
    hud.music.info->setBackground(GUI_TEXTBOX_H);
    hud.music.info->setBackgroundPadding(16);
    hud.music.info->setEnableBackground(true);
    manager.gui->endLayout();

    manager.gui->addLayoutStretchSpace();

    manager.gui->addLayout(Layout::Vertical, 5);
    hud.music.list = manager.gui->addListBox("music.list");
    hud.music.list->setBackground(GUI_LISTBOX_V);
    hud.music.list->setBackgroundPadding(8);
    hud.music.list->setDefinedSize(true);
    hud.music.list->setSize(Vector2f(192, 192) * sizeFactor);
    manager.gui->endLayout();

    manager.gui->endLayout();

    manager.gui->setSession(EDITOR_VIEW);

    manager.gui->setSession(EDITOR_GUI);

    Image* backPause = manager.gui->addImage("00:background", BACKGROUND_PAUSE);
    backPause->setSize(screenSize);

    manager.gui->addLayout(Layout::Horizental, 0, 10);
    manager.gui->addLayoutStretchSpace();
    manager.gui->addLayout(Layout::Vertical, 10);
    hud.pause.quit = manager.gui->addButton("hud.pause.quit", "Quitter");
    hud.pause.save = manager.gui->addButton("hud.pause.save", "Enregistrer");
    hud.pause.ret = manager.gui->addButton("hud.pause.ret", "Retour");
    manager.gui->addLayoutStretchSpace();
    hud.pause.name = manager.gui->addEditBox("hud.pause.name", map.name);
    manager.gui->endLayout();
    manager.gui->endLayout();

    hud.pause.status = manager.gui->addTextBox("hud.pause.status");
    hud.pause.status->setPencil(bigpen);
    hud.pause.status->setPos(20);

    hud.pause.confirm = false;

    // Remplisage --------------------------------------------------------------

    // Lumiere

    hud.light.amibent->setRange(0, 1);
    hud.light.diffuse->setRange(0, 1);
    hud.light.specular->setRange(0, 1);
    hud.light.radius->setRange(new SwitchNumeric<float>::Range(0, 128));

    hud.light.type->push("Diri").push("Point");

    if(!map.lights.empty())
    {
        for(unsigned i = 0; i < map.lights.size(); i++)
            hud.light.slector->push("Light #" + tools::numToStr(i), i);

        hud.light.slector->setCurrent(0);

        selectLight(0);
    }

    hud.light.amibent->setValue(vec43(manager.scene->getAmbientLight()));

    // Sky

    Texture* skyTex = m_sky->getTextures();

    hud.sky.enable->push("Desactiver").push("Activer");
    hud.sky.enable->setCurrent(m_sky->isEnable());

    hud.sky.face
            ->push("Devant", skyTex[0].getFilename())
            .push("Deriere", skyTex[1].getFilename())
            .push("Haut", skyTex[2].getFilename())
            .push("Bas", skyTex[3].getFilename())
            .push("Gauche", skyTex[4].getFilename())
            .push("Droite", skyTex[5].getFilename());

    {
        for(directory_iterator it(SKYBOX_DIR); it != endDir; it++)
        {
            const path& imgFilename = it->path();

            if(!is_directory(imgFilename))
                hud.sky.list->push(imgFilename.filename(), imgFilename.string());
        }

        hud.sky.list->update();
    }

    // Fog

    float fogStart = m_fog->getStart();
    float fogend = m_fog->getEnd();

    hud.fog.color->setValue(vec43(m_fog->getColor()));
    hud.fog.color->setRange(0, 1);

    hud.fog.start->setValue(fogStart);
    hud.fog.start->setRange(new gui::SwitchNumeric<float>::Range(0, fogend - 1));

    hud.fog.end->setValue(fogend);
    hud.fog.end->setRange(new gui::SwitchNumeric<float>::Range(fogStart + 1, 512));

    hud.fog.enable->push("Désactiver").push("Activer");
    hud.fog.enable->setCurrent(m_fog->isEnable());

    // Statique
    {
        for(directory_iterator it(MODELS_MAPS_DIR); it != endDir; it++)
        {
            const path& staricFilename = it->path();

            if(!is_directory(staricFilename) && staricFilename.extension() == ".obj")
                hud.entity.list->push(staricFilename.filename(), staricFilename.string());
        }

        hud.entity.list->update();
    }

    // Musique
    {
        for(directory_iterator it(MUSIC_DIR); it != endDir; it++)
        {
            const path& musicFilename = it->path();

            if(!is_directory(musicFilename) && MusicCheck(musicFilename))
                hud.music.list->push(musicFilename.filename(), musicFilename.string());
        }

        hud.music.list->update();
    }
}

bool EditorManager::settingMusicEvent(tbe::EventManager* event)
{
    if(!manager.app->globalSettings.noaudio)
    {
        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(event->lastActiveKey.first == EventManager::KEY_DELETE)
            {
                if(map.musicStream)
                    FMOD_Sound_Release(map.musicStream);

                map.musicChannel = 0;
                map.musicStream = NULL;

                map.musicPath.clear();

                hud.music.list->cancelSelection();
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

        if(hud.music.list->isActivate())
        {
            if(map.musicStream)
                FMOD_Sound_Release(map.musicStream);

            map.musicPath = hud.music.list->getCurrentData().getValue<string > ();

            FMOD_RESULT res = FMOD_System_CreateStream(manager.fmodsys, map.musicPath.c_str(),
                                                       FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE,
                                                       0, &map.musicStream);

            if(res == FMOD_OK)
                FMOD_System_PlaySound(manager.fmodsys, FMOD_CHANNEL_FREE, map.musicStream, false, &map.musicChannel);

            hud.music.list->setActivate(false);
        }
    }

    return false;
}

bool EditorManager::settingSkyEvent(tbe::EventManager* event)
{
    if(hud.sky.enable->isActivate())
        m_sky->setEnable(hud.sky.enable->getCurrent());

    else if(hud.sky.clear->isActivate())
    {
        m_sky->clear();
    }

    else if(hud.sky.apply->isActivate())
    {
        Texture skyTex[6];

        for(unsigned i = 0; i < 6; i++)
        {
            string path = hud.sky.face->getData(i).getValue<string > ();

            if(!path.empty())
                skyTex[i] = path;
        }

        m_sky->setTextures(skyTex);
    }

    else if(hud.sky.list->isActivate())
    {
        hud.sky.face->setData(hud.sky.list->getCurrentData());
    }

    else if(hud.sky.face->isActivate())
    {
        hud.sky.list->cancelSelection();
    }

    return false;
}

bool EditorManager::settingFogEvent(tbe::EventManager* event)
{
    if(hud.fog.enable->isActivate())
    {
        m_fog->setEnable(hud.fog.enable->getCurrent());

        manager.scene->setZFar(m_fog->isEnable() ? m_fog->getEnd() : map.aabb.getSize());
        manager.scene->updateViewParameter();
    }

    else if(hud.fog.color->isActivate())
        m_fog->setColor(vec34(hud.fog.color->getValue()));

    else if(hud.fog.start->isActivate())
    {
        m_fog->setStart(hud.fog.start->getValue());

        gui::SwitchNumeric<float>::Range* range = new gui::SwitchNumeric<float>::Range(hud.fog.start->getValue() + 1, 512);

        hud.fog.end->setRange(range);
    }

    else if(hud.fog.end->isActivate())
    {
        m_fog->setEnd(hud.fog.end->getValue());;

        gui::SwitchNumeric<float>::Range* range = new gui::SwitchNumeric<float>::Range(0, hud.fog.end->getValue() - 1);

        hud.fog.start->setRange(range);

        manager.scene->setZFar(m_fog->getEnd());
        manager.scene->updateViewParameter();
    }

    return false;
}

void EditorManager::editEventPorcess(tbe::EventManager* event)
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
            manager.gameEngine->setGrabInput(true);
            manager.gameEngine->setMouseVisible(false);

            m_timeTo = VIEW;
            return;
        }
        else if(event->mouseState[EventManager::MOUSE_BUTTON_LEFT])
        {
            if(!map.aabb.isInner(m_3dSelect))
                m_axes->setPos(0);

            else
                m_axes->setPos(m_3dSelect);
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
            if(!settingEntityEvent(event))
                if(!selectEntityEvent(event))
                    if(!deleteEntityEvent(event))
                        allocEntityEvent(event);
            break;
        case LIGHT:
            if(!settingLightEvent(event))
                if(!deleteLightEvent(event))
                    allocLightEvent(event);
            break;

        case SKYBOX:
            settingSkyEvent(event);
            break;

        case FOG:
            settingFogEvent(event);
            break;

        case MUSIC:
            settingMusicEvent(event);
            break;
    }
}

void EditorManager::viewEventPorcess(tbe::EventManager* event)
{
    if(event->notify == EventManager::EVENT_MOUSE_DOWN)
    {
        if(event->mouseState[EventManager::MOUSE_BUTTON_MIDDLE])
        {
            manager.gameEngine->setGrabInput(false);
            manager.gameEngine->setMouseVisible(true);

            m_timeTo = EDIT;
            return;
        }
    }
    else if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
        switch(event->lastActiveKey.first)
        {
            case 'r':
                m_axes->setPos(0);
                m_OCamera->setCenter(0);
                m_FFCamera->setPos(2);
                m_FFCamera->setTarget(-1);
                m_selectedNode = NULL;
                break;

            case 'v':
                if(m_camera == m_FFCamera)
                {
                    m_OCamera->setPos(m_FFCamera->getPos());
                    m_camera = m_OCamera;
                }
                else
                {
                    m_FFCamera->setPos(m_OCamera->getPos());
                    m_camera = m_FFCamera;
                }

                manager.scene->setCurCamera(m_camera);
                break;
        }

    }

    m_OCamera->setCenter(m_axes->getPos());
    m_camera->onEvent(event);
}

void EditorManager::guiEventPorcess(tbe::EventManager* event)
{
    if(hud.pause.status->isEnable() && hud.pause.statusClock.isEsplanedTime(4000))
        hud.pause.status->setEnable(false);

    if(hud.pause.quit->isActivate())
    {
        hud.pause.quit->setActivate(false);

        if(manager.level->isChanged() && !hud.pause.confirm)
        {
            hud.pause.status->setEnable(true);
            hud.pause.status->write("La Carte a changer !\nEtes vous sure de quitter ?");
            hud.pause.confirm = true;
        }
        else
        {
            running = false;
        }
    }
    else if(hud.pause.save->isActivate())
    {
        hud.pause.save->setActivate(false);

        map.name = hud.pause.name->getLabel();
        string saveFileName = MAPS_DIR + tools::unixName(map.name) + ".bld";

        if(!map.name.empty())
        {
            map.spawnPoints.clear();

            for(unsigned i = 0; i < m_visualSpawnPoints.size(); i++)
                map.spawnPoints.push_back(m_visualSpawnPoints[i]->getPos());

            if(m_editSetting.createNew)
            {
                manager.level->saveLevel(saveFileName);
            }

            else
            {
                manager.level->saveLevel();

                std::rename(manager.level->getOpenFileName().c_str(), saveFileName.c_str());
            }

            manager.app->globalSettings.readMapInfo();
            manager.app->updateGuiContent();

            hud.pause.status->setEnable(true);
            hud.pause.status->write("Carte enregistrer !");
        }

    }
    else if(hud.pause.ret->isActivate())
    {
        manager.gameEngine->setGrabInput(true);
        manager.gameEngine->setMouseVisible(false);

        m_timeTo = VIEW;
    }
}

void EditorManager::eventProcess()
{
    EventManager* event = manager.gameEngine->getEventManager();

    manager.gameEngine->pollEvent();

    if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
        if(event->lastActiveKey.first == EventManager::KEY_ESCAPE)
        {
            manager.gameEngine->setGrabInput(false);
            manager.gameEngine->setMouseVisible(true);

            m_timeTo = PAUSE;
        }
    }

    switch(m_timeTo)
    {
        case EDIT:
            editEventPorcess(event);
            break;

        case VIEW:
            viewEventPorcess(event);
            break;

        case PAUSE:
            guiEventPorcess(event);
            break;
    }
}

inline string skyTexBaseName(const Texture& tex)
{
    string filename = tex.getFilename();

    unsigned pos = filename.find_last_of('\\');

    if(pos == string::npos)
        pos = filename.find_last_of('/');

    return filename.substr(pos + 1);
}

void EditorManager::hudProcess()
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
                info << "Selection : " << m_selectedNode->getName() << " ; " << m_selectedNode->getPos();
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

            hud.entity.info->write(info.str());
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

            hud.light.info->write(info.str());
            break;

        case SKYBOX:
            info << "Skybox";

            if(m_sky)
            {
                Texture* skyTex = m_sky->getTextures();

                info << endl
                        << "Devant: " << skyTexBaseName(skyTex[0]) << endl
                        << "Derierre: " << skyTexBaseName(skyTex[1]) << endl
                        << "Haut: " << skyTexBaseName(skyTex[2]) << endl
                        << "Bas: " << skyTexBaseName(skyTex[3]) << endl
                        << "Gauche: " << skyTexBaseName(skyTex[4]) << endl
                        << "Droite: " << skyTexBaseName(skyTex[5]);
            }

            hud.sky.info->write(info.str());
            break;

        case FOG:
            info << "Brouiallrd (Fog)";

            hud.fog.info->write(info.str());
            break;

        case MUSIC:
            info << "Musique" << endl
                    << "Espace  : Lancer/Arreter la musique" << endl
                    << "Suppr   : Désactiver la musique pour la carte";

            if(map.musicStream)
                info << endl << ">> " << map.musicPath;

            hud.music.info->write(info.str());
            break;
    }
}

void EditorManager::render()
{
    EventManager* event = manager.gameEngine->getEventManager();

    switch(m_timeTo)
    {
        case VIEW:
            manager.gui->setSession(EDITOR_VIEW);
            break;
        case EDIT:
            switch(m_navigation)
            {
                case ENTITY:
                    manager.gui->setSession(EDITOR_EDIT_ENTITY);
                    break;
                case LIGHT:
                    manager.gui->setSession(EDITOR_EDIT_LIGHT);
                    break;
                case SKYBOX:
                    manager.gui->setSession(EDITOR_EDIT_SKYBOX);
                    break;
                case FOG:
                    manager.gui->setSession(EDITOR_EDIT_FOG);
                    break;
                case MUSIC:
                    manager.gui->setSession(EDITOR_EDIT_MUSIC);
                    break;
            }
            break;
        case PAUSE:
            manager.gui->setSession(EDITOR_GUI);
            break;
    }

    manager.gameEngine->beginScene();

    if(manager.app->globalSettings.video.usePpe)
    {
        Rtt* rtt = manager.ppe->getRtt();

        rtt->use(true);
        rtt->clear();
        manager.scene->render();
        rtt->use(false);

        m_3dSelect = manager.scene->screenToWorld(event->mousePos, rtt);

        manager.ppe->render();
    }
    else
    {
        manager.scene->render();
        m_3dSelect = manager.scene->screenToWorld(event->mousePos);
    }

    manager.gui->render();

    manager.gameEngine->endScene();
}
