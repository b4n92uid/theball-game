#include "EditorManager.h"

#include "GameManager.h"
#include "AppManager.h"
#include "BldParser.h"

using namespace std;
using namespace tbe;

void EditorManager::NewLight(tbe::scene::Light* light)
{
    unsigned index = map.lights.size();

    hud.light.slector->Push("Light #" + tools::numToStr(index), index);

    map.lights.push_back(light);

    manager.scene->AddDynamicLight(tools::numToStr(light), light);

    m_selectedLight = light;

    SelectLight(index);
}

void EditorManager::DeleteLight(unsigned index)
{
    hud.light.slector->Delete(index);

    map.lights.erase(map.lights.begin() + index);

    manager.scene->ReleaseDynamicLight(tools::numToStr(m_selectedLight));

    delete m_selectedLight, m_selectedLight = NULL;

    if(!map.lights.empty())
        SelectLight(index - 1);
}

void EditorManager::SelectLight(unsigned index)
{
    if(map.lights.empty())
    {
        m_selectedLight = NULL;
        return;
    }

    m_selectedLight = map.lights[index];

    hud.light.slector->SetCurrent(index);
    hud.light.type->SetCurrent(m_selectedLight->GetType());
    hud.light.diffuse->SetValue(vec43(m_selectedLight->GetDiffuse()));
    hud.light.specular->SetValue(vec43(m_selectedLight->GetSpecular()));
    hud.light.radius->SetValue(m_selectedLight->GetRadius());

    m_axes->SetPos(m_selectedLight->GetPos());
}

bool EditorManager::DeleteLightEvent(tbe::EventManager* event)
{
    if(!m_selectedLight || event->notify != EventManager::EVENT_KEY_DOWN)
        return false;

    if(event->lastActiveKey.first == EventManager::KEY_DELETE)
    {
        DeleteLight(hud.light.slector->GetData().GetValue<unsigned>());
        return true;
    }

    return false;
}

bool EditorManager::AllocLightEvent(tbe::EventManager* event)
{
    using namespace scene;

    if(event->notify != EventManager::EVENT_KEY_DOWN)
        return false;

    int c = event->lastActiveKey.first;

    Light* newLight = NULL;

    switch(c)
    {
        case 'p':
            newLight = new PointLight;
            newLight->SetPos(m_axes->GetPos());
            break;
        case 'd':
            newLight = new DiriLight;
            break;
    }

    if(newLight)
    {
        NewLight(newLight);
        return true;
    }

    return false;
}

bool EditorManager::SettingLightEvent(tbe::EventManager* event)
{
    if(!m_selectedLight)
        return false;

    // Séléction

    if(hud.light.slector->IsActivate())
    {
        unsigned index = hud.light.slector->GetData().GetValue<unsigned>();

        SelectLight(index);
    }

    // Attributes

    if(hud.light.type->IsActivate())
        m_selectedLight->SetType((scene::Light::Type)hud.light.type->GetCurrent());

    else if(hud.light.amibent->IsActivate())
        manager.scene->SetAmbientLight(vec34(hud.light.amibent->GetValue()));

    else if(hud.light.diffuse->IsActivate())
        m_selectedLight->SetDiffuse(vec34(hud.light.diffuse->GetValue()));

    else if(hud.light.specular->IsActivate())
        m_selectedLight->SetSpecular(vec34(hud.light.specular->GetValue()));

    else if(hud.light.radius->IsActivate())
        m_selectedLight->SetRadius(hud.light.radius->GetValue());

    // Mouvement

    if(m_selectedLight->GetType() == scene::Light::DIRI)
    {
        if(event->keyState[EventManager::KEY_LSHIFT])
        {
            Vector3f curPos = m_selectedLight->GetPos();
            Vector3f setPos;

            if(event->notify == EventManager::EVENT_MOUSE_MOVE)
            {
                if(event->keyState[EventManager::KEY_LALT])
                {
                    setPos.x = curPos.x;

                    setPos.y = (event->mousePos.y - event->mousePos.y / 2.0f)
                            / manager.app->globalSettings.video.screenSize.y * 2.0f;

                    setPos.z = curPos.z;
                }
                else
                {
                    setPos = Vector3f::Normalize(m_3dSelect);
                    setPos.y = curPos.y;
                }

                m_selectedLight->SetPos(setPos);
            }

            return true;
        }
    }
    else if(m_selectedLight->GetType() == scene::Light::POINT)
    {
        if(event->keyState[EventManager::KEY_LSHIFT])
        {
            manager.gameEngine->SetMouseVisible(false);
            manager.gameEngine->SetGrabInput(true);

            if(event->notify == EventManager::EVENT_MOUSE_MOVE)
            {
                float moveSpeed = 0.1f;

                Vector3f curPos = m_selectedLight->GetPos();

                Vector3f transform;
                Vector3f target = m_camera->GetTarget();
                Vector3f left = m_camera->GetLeft();

                if(event->keyState[EventManager::KEY_LALT])
                {
                    transform.y -= event->mousePosRel.y * moveSpeed;
                }
                else
                {
                    transform = left * (float)event->mousePosRel.x * moveSpeed;
                    transform -= target * (float)event->mousePosRel.y * moveSpeed;
                    transform.y = 0;
                }

                Vector3f setPos = curPos + transform;

                m_selectedLight->SetPos(setPos);

                m_axes->SetPos(setPos);
            }

            return true;
        }
        else
        {
            manager.gameEngine->SetMouseVisible(true);
            manager.gameEngine->SetGrabInput(false);
        }
    }

    return false;
}
