#include "GameManager.h"

#include "AppManager.h"
#include "SoundManager.h"
#include "MaterialManager.h"

using namespace std;
using namespace tbe;

GameManager::GameManager(AppManager* appManager)
{
    using namespace scene;

    cout << "Initing game manager" << endl;

    manager.app = appManager;

    worldSettings = manager.app->globalSettings.physics;

    running = true;

    manager.gameEngine = manager.app->GetGameEngine();
    manager.gameEngine->SetGrabInput(true);
    manager.gameEngine->SetMouseVisible(false);

    manager.fps = manager.gameEngine->GetFpsManager();
    manager.gui = manager.gameEngine->GetGuiManager();
    manager.scene = manager.gameEngine->GetSceneManager();
    manager.ppe = manager.gameEngine->GetPostProcessManager();

    parallelscene.light = new scene::LightParallelScene;
    manager.scene->AddParallelScene(parallelscene.light);

    parallelscene.meshs = new scene::MeshParallelScene;
    manager.scene->AddParallelScene(parallelscene.meshs);

    parallelscene.newton = new scene::NewtonParallelScene;
    parallelscene.newton->SetGravity(worldSettings.gravity);
    manager.scene->AddParallelScene(parallelscene.newton);

    parallelscene.particles = new scene::ParticlesParallelScene;
    manager.scene->AddParallelScene(parallelscene.particles);

    #ifndef THEBALL_NO_AUDIO
    manager.fmodsys = appManager->GetFmodSystem();
    #else
    manager.fmodsys = NULL;
    #endif

    manager.material = new MaterialManager(this);
    manager.sound = new SoundManager(this);
    manager.level = new BldParser(this);

    map.musicStream = NULL;
    map.musicChannel = 0;
}

GameManager::~GameManager()
{
    cout << "Cleaning game manager" << endl;

    manager.gui->SetSession(MENU_PLAY);

    manager.gameEngine->SetGrabInput(false);
    manager.gameEngine->SetMouseVisible(true);

    #if !defined(THEBALL_DISABLE_MUSIC) && !defined(THEBALL_NO_AUDIO)
    FMOD_Sound_Release(map.musicStream);
    #endif

    delete manager.sound;
    delete manager.material;
    delete manager.level;

    manager.scene->ClearAll();
}

void GameManager::RegisterItem(Item* item)
{
    manager.material->AddItem(item);
    map.items.push_back(item);
}

void GameManager::RegisterStatic(StaticObject* staticObject)
{
    manager.material->AddStatic(staticObject);
    map.staticObjects.push_back(staticObject);
    map.aabb.Count(staticObject);
}

void GameManager::RegisterDynamic(DynamicObject* dynamicObject)
{
    manager.material->AddDynamic(dynamicObject);
    map.dynamicObjects.push_back(dynamicObject);
}

void GameManager::UnRegisterItem(Item* item)
{
    Item::Array::iterator it = find(map.items.begin(),
                                    map.items.end(), item);

    map.items.erase(it);
}

void GameManager::UnRegisterStatic(StaticObject* staticObject)
{
    StaticObject::Array::iterator it = find(map.staticObjects.begin(),
                                            map.staticObjects.end(), staticObject);

    map.staticObjects.erase(it);
}

void GameManager::UnRegisterDynamic(DynamicObject* dynamicObject)
{
    DynamicObject::Array::iterator it = find(map.dynamicObjects.begin(),
                                             map.dynamicObjects.end(), dynamicObject);

    map.dynamicObjects.erase(it);
}
