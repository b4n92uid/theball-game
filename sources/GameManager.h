#ifndef _GAMEMANAGER_H
#define _GAMEMANAGER_H

#include <Tbe.h>
#include <NewtonBall/NewtonBall.h>
#include <SDLDevice/SDLDevice.h>

#include <fmod.h>

#include "AppManager.h"

#include "Player.h"
#include "Item.h"
#include "StaticObject.h"
#include "DynamicObject.h"
#include "BldParser.h"

class SoundManager;

class GameManager
{
public:

    GameManager(AppManager* appManager);
    virtual ~GameManager();

    // Enregistrement des entités ----------------------------------------------

    virtual void registerItem(Item* item);
    virtual void registerStatic(StaticObject* staticObject);
    virtual void registerDynamic(DynamicObject* dynamicObject);

    virtual void unregisterItem(Item* item);
    virtual void unregisterStatic(StaticObject* staticObject);
    virtual void unregisterDynamic(DynamicObject* dynamicObject);

    // Accés -------------------------------------------------------------------

    struct
    {
        std::string name;

        FMOD_SOUND* musicStream;
        FMOD_CHANNEL* musicChannel;
        std::string musicPath;

        Item::Array items;
        StaticObject::Array staticObjects;
        DynamicObject::Array dynamicObjects;
        tbe::scene::Light::Array lights;

        tbe::AABB aabb;

        tbe::Vector3f::Array spawnPoints;

    } map;

    struct
    {
        tbe::SDLDevice* gameEngine;
        tbe::scene::SceneManager* scene;
        tbe::gui::GuiManager* gui;
        tbe::ppe::PostProcessManager* ppe;
        tbe::ticks::FpsManager* fps;

        MaterialManager* material;
        SoundManager* sound;
        BldParser* level;

        AppManager* app;

        FMOD_SYSTEM* fmodsys;

    } manager;

    struct
    {
        tbe::scene::MeshParallelScene* meshs;
        tbe::scene::ParticlesParallelScene* particles;
        tbe::scene::LightParallelScene* light;
        tbe::scene::NewtonParallelScene* newton;

    } parallelscene;

    Settings::World worldSettings;

    bool running;
};

#endif // _GAMEMANAGER_H
