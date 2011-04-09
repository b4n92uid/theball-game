#include "EditorManager.h"

#include "GameManager.h"
#include "AppManager.h"
#include "BldParser.h"

using namespace std;
using namespace tbe;

void EditorManager::newLight(tbe::scene::Light* light)
{
    unsigned index = map.lights.size();

    hud.light.slector->push("Light #" + tools::numToStr(index), index);

    map.lights.push_back(light);

    manager.scene->getRootNode()->addChild(light);

    m_selectedLight = light;

    selectLight(index);
}

void EditorManager::deleteLight(unsigned index)
{
    hud.light.slector->deleteOn(index);

    map.lights.erase(map.lights.begin() + index);

    delete m_selectedLight, m_selectedLight = NULL;

    if(!map.lights.empty())
        selectLight(index - 1);
}

void EditorManager::selectLight(unsigned index)
{
    if(map.lights.empty())
    {
        m_selectedLight = NULL;
        return;
    }

    m_selectedLight = map.lights[index];

    hud.light.slector->setCurrent(index);
    hud.light.type->setCurrent(m_selectedLight->getType());
    hud.light.diffuse->setValue(vec43(m_selectedLight->getDiffuse()));
    hud.light.specular->setValue(vec43(m_selectedLight->getSpecular()));
    hud.light.radius->setValue(m_selectedLight->getRadius());

    m_axes->setPos(m_selectedLight->getPos());
}

bool EditorManager::deleteLightEvent(tbe::EventManager* event)
{
    if(!m_selectedLight || event->notify != EventManager::EVENT_KEY_DOWN)
        return false;

    if(event->lastActiveKey.first == EventManager::KEY_DELETE)
    {
        deleteLight(hud.light.slector->getData().getValue<unsigned>());
        return true;
    }

    return false;
}

bool EditorManager::allocLightEvent(tbe::EventManager* event)
{
    using namespace scene;

    if(event->notify != EventManager::EVENT_KEY_DOWN)
        return false;

    int c = event->lastActiveKey.first;

    Light* light = NULL;

    switch(c)
    {
        case 'p':
            light = new PointLight(parallelscene.light);
            light->setPos(m_axes->getPos());
            break;
        case 'd':
            light = new DiriLight(parallelscene.light);
            break;
    }

    if(light)
    {
        newLight(light);
        return true;
    }

    return false;
}

bool EditorManager::settingLightEvent(tbe::EventManager* event)
{
    if(!m_selectedLight)
        return false;

    // Séléction

    if(hud.light.slector->isActivate())
    {
        unsigned index = hud.light.slector->getData().getValue<unsigned>();

        selectLight(index);
    }

    // Attributes

    if(hud.light.type->isActivate())
        m_selectedLight->setType((scene::Light::Type)hud.light.type->getCurrent());

    else if(hud.light.amibent->isActivate())
        manager.scene->setAmbientLight(vec34(hud.light.amibent->getValue()));

    else if(hud.light.diffuse->isActivate())
        m_selectedLight->setDiffuse(vec34(hud.light.diffuse->getValue()));

    else if(hud.light.specular->isActivate())
        m_selectedLight->setSpecular(vec34(hud.light.specular->getValue()));

    else if(hud.light.radius->isActivate())
        m_selectedLight->setRadius(hud.light.radius->getValue());

    // Mouvement


    if(event->keyState[EventManager::KEY_LSHIFT])
    {
        manager.gameEngine->setMouseVisible(false);
        manager.gameEngine->setGrabInput(true);

        if(event->notify == EventManager::EVENT_MOUSE_MOVE)
        {
            float moveSpeed = 0.05;

            Vector3f curPos = m_selectedLight->getPos();

            Vector3f transform;
            Vector3f target = m_camera->getTarget();
            Vector3f left = m_camera->getLeft();

            if(event->keyState[EventManager::KEY_LALT])
            {
                transform.y = event->mousePosRel.y * moveSpeed;
            }
            else
            {
                transform = left * (float)event->mousePosRel.x * moveSpeed;
                transform += target * (float)event->mousePosRel.y * moveSpeed;
                transform.y = 0;
            }

            Vector3f setPos = curPos + transform;

            m_selectedLight->setPos(setPos);

            m_axes->setPos(setPos);
        }

        return true;
    }
    else
    {
        manager.gameEngine->setMouseVisible(true);
        manager.gameEngine->setGrabInput(false);
    }


    return false;
}
