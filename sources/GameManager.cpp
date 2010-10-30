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

    parallelscene.meshs = new scene::MeshParallelScene;
    manager.scene->AddParallelScene("1:MeshScene", parallelscene.meshs);

    parallelscene.newton = new scene::NewtonParallelScene;
    parallelscene.newton->SetSharedNode(true);
    parallelscene.newton->SetGravity(worldSettings.gravity);
    manager.scene->AddParallelScene("2:NewtonNodeScene", parallelscene.newton);

    parallelscene.particles = new scene::ParticlesParallelScene;
    manager.scene->AddParallelScene("3:ParticlesScene", parallelscene.particles);

    manager.fmodsys = appManager->GetFmodSystem();

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

    #ifndef THEBALL_DISABLE_MUSIC
    if(map.musicStream)
        FMOD_Sound_Release(map.musicStream);
    #endif

    delete manager.sound;
    delete manager.material;
    delete manager.level;

    manager.scene->ClearAll();
}

void GameManager::RegisterItem(Item* item)
{
    std::string name = tools::NameGen(map.items, "ITEM#");

    parallelscene.meshs->AddMesh(name, item);
    parallelscene.newton->AddNode(name, item);
    manager.material->AddItem(item);
    map.items.push_back(item);
}

void GameManager::RegisterStatic(StaticObject* staticObject)
{
    std::string name = tools::NameGen(map.staticObjects, "STATIC#");

    parallelscene.meshs->AddMesh(name, staticObject);
    parallelscene.newton->AddNode(name, staticObject);
    manager.material->AddStatic(staticObject);
    map.staticObjects.push_back(staticObject);
    map.aabb.Count(staticObject);
}

void GameManager::RegisterDynamic(DynamicObject* dynamicObject)
{
    std::string name = tools::NameGen(map.dynamicObjects, "DYNAMIC#");

    parallelscene.meshs->AddMesh(name, dynamicObject);
    parallelscene.newton->AddNode(name, dynamicObject);
    manager.material->AddDynamic(dynamicObject);
    map.dynamicObjects.push_back(dynamicObject);
}

void GameManager::UnRegisterItem(Item* item)
{
    parallelscene.meshs->ReleaseMesh(item->GetName());
    parallelscene.newton->ReleaseNode(item->GetName());

    Item::Array::iterator it = find(map.items.begin(),
                                    map.items.end(), item);

    map.items.erase(it);
}

void GameManager::UnRegisterStatic(StaticObject* staticObject)
{
    parallelscene.meshs->ReleaseMesh(staticObject->GetName());
    parallelscene.newton->ReleaseNode(staticObject->GetName());

    StaticObject::Array::iterator it = find(map.staticObjects.begin(),
                                            map.staticObjects.end(), staticObject);

    map.staticObjects.erase(it);

    map.aabb.Count(staticObject);
}

void GameManager::UnRegisterDynamic(DynamicObject* dynamicObject)
{
    parallelscene.meshs->ReleaseMesh(dynamicObject->GetName());
    parallelscene.newton->ReleaseNode(dynamicObject->GetName());

    DynamicObject::Array::iterator it = find(map.dynamicObjects.begin(),
                                             map.dynamicObjects.end(), dynamicObject);

    map.dynamicObjects.erase(it);
}
