#include "GameManager.h"

#include "AppManager.h"
#include "SoundManager.h"
#include "MaterialManager.h"

using namespace std;
using namespace tbe;

GameManager::GameManager(AppManager* appManager)
{
    using namespace scene;

    cout << "--- Initing game manager" << endl;

    manager.app = appManager;

    worldSettings = manager.app->globalSettings.world;

    running = true;

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

    if(manager.app->globalSettings.noaudio)
        manager.fmodsys = NULL;
    else
        manager.fmodsys = appManager->getFmodSystem();

    manager.material = new MaterialManager(this);
    manager.sound = new SoundManager(this);
    manager.level = new BldParser(this);

    map.musicStream = NULL;
    map.musicChannel = 0;
}

GameManager::~GameManager()
{
    cout << "--- Cleaning game manager" << endl;

    manager.gui->setSession(MENU_QUICKPLAY);

    manager.gameEngine->setGrabInput(false);
    manager.gameEngine->setMouseVisible(true);

    if(!manager.app->globalSettings.noaudio && !manager.app->globalSettings.nomusic)
        FMOD_Sound_Release(map.musicStream);

    delete manager.sound;
    delete manager.material;
    delete manager.level;

    manager.scene->clearAll();
}

void GameManager::registerItem(Item* item)
{
    manager.material->addItem(item);
    map.items.push_back(item);
}

void GameManager::registerStatic(StaticObject* staticObject)
{
    manager.material->addStatic(staticObject);
    map.staticObjects.push_back(staticObject);
    map.aabb.count(staticObject);
}

void GameManager::registerDynamic(DynamicObject* dynamicObject)
{
    manager.material->addDynamic(dynamicObject);
    map.dynamicObjects.push_back(dynamicObject);
}

void GameManager::unregisterItem(Item* item)
{
    Item::Array::iterator it = find(map.items.begin(),
                                    map.items.end(), item);

    map.items.erase(it);
}

void GameManager::unregisterStatic(StaticObject* staticObject)
{
    StaticObject::Array::iterator it = find(map.staticObjects.begin(),
                                            map.staticObjects.end(), staticObject);

    map.staticObjects.erase(it);
}

void GameManager::unregisterDynamic(DynamicObject* dynamicObject)
{
    DynamicObject::Array::iterator it = find(map.dynamicObjects.begin(),
                                             map.dynamicObjects.end(), dynamicObject);

    map.dynamicObjects.erase(it);
}
