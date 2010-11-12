#include "EditorManager.h"

#include "GameManager.h"
#include "AppManager.h"
#include "BldParser.h"

using namespace std;
using namespace tbe;

bool EditorManager::DeleteEntityEvent(tbe::EventManager* event)
{
    if(!m_selectedNode)
        return false;

    if(event->notify != EventManager::EVENT_KEY_DOWN)
        return false;

    if(event->lastActiveKey.first == EventManager::KEY_DELETE)
    {
        {
            StaticObject* toSt = dynamic_cast<StaticObject*>(m_selectedNode);
            if(find(map.staticObjects.begin(), map.staticObjects.end(), toSt) != map.staticObjects.end())
                UnRegisterStatic(toSt);
        }
        {
            DynamicObject* toDy = dynamic_cast<DynamicObject*>(m_selectedNode);
            if(find(map.dynamicObjects.begin(), map.dynamicObjects.end(), toDy) != map.dynamicObjects.end())
                UnRegisterDynamic(toDy);
        }
        {
            Item* toItem = dynamic_cast<Item*>(m_selectedNode);

            if(find(map.items.begin(), map.items.end(), toItem) != map.items.end())
                UnRegisterItem(toItem);
        }

        Object::Array::iterator it = find(m_allEntity.begin(), m_allEntity.end(), m_selectedNode);

        m_allEntity.erase(it);

        delete m_selectedNode, m_selectedNode = NULL;

        return true;
    }

    return false;
}

void EditorManager::NewEntity(Object* ent)
{
    Vector3f pos = ent->GetPos();

    m_allEntity.push_back(ent);

    m_axes->SetPos(pos);
    m_OCamera->SetCenter(pos);
}

bool EditorManager::AllocEntityEvent(EventManager* event)
{
    Object* newObj = NULL;

    int c = event->lastActiveKey.first;

    if(c == 'c' && event->notify == EventManager::EVENT_KEY_DOWN && m_selectedNode)
    {
        Object* clone = m_selectedNode->CloneToObject();
        NewEntity(clone);

        m_selectedNode = clone;

        return true;
    }

    // Dynamics
    if(event->keyState['d'] && event->notify == EventManager::EVENT_KEY_DOWN)
    {
        DynamicObject* dy = NULL;

        switch(c)
        {
            case 'j':
                dy = new DYJumper(this, m_axes->GetPos());
                break;
            case 't':
                dy = new DYTeleporter(this, m_axes->GetPos());
                break;
        }

        if(dy)
        {
            RegisterDynamic(dy);
            newObj = dy;
        }
    }

        // Item
    else if(event->keyState['t'] && event->notify == EventManager::EVENT_KEY_DOWN)
    {
        Item* it = NULL;

        switch(c)
        {
            case 'l':
                it = new ItemAddLife(this, m_axes->GetPos());
                break;
            case 'a':
                it = new ItemAddAmmo(this, m_axes->GetPos());
                break;
            case 's':
                it = new ItemSuperLife(this, m_axes->GetPos());
                break;
            case 'f':
                it = new ItemFatalShot(this, m_axes->GetPos());
                break;
            case 'i':
                it = new ItemAddFinder(this, m_axes->GetPos());
                break;
            case 'h':
                it = new ItemAddShotgun(this, m_axes->GetPos());
                break;
            case 'b':
                it = new ItemAddBomb(this, m_axes->GetPos());
                break;
        }

        if(it)
        {
            RegisterItem(it);
            newObj = it;
        }
    }

        // Static
    else
    {
        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(event->lastActiveKey.first == 's')
                hud.entity.list->SetEnable(true);

            else if(event->lastActiveKey.first == EventManager::KEY_ESCAPE)
                hud.entity.list->CancelSelection();
        }

        else if(event->notify == EventManager::EVENT_KEY_UP)
        {
            if(event->lastActiveKey.first == 's')
            {
                if(hud.entity.list->IsSelection())
                {
                    string filename = hud.entity.list->GetCurrentData().GetValue<string > ();

                    StaticObject* so = new StaticObject(this, filename, m_axes->GetPos());
                    RegisterStatic(so);
                    newObj = so;

                    parallelscene.newton->SetWorldSize(map.aabb);

                    hud.entity.list->CancelSelection();
                }

                hud.entity.list->SetEnable(false);
            }
        }
    }

    if(newObj)
    {
        NewEntity(newObj);
        m_selectedNode = newObj;

        return true;
    }

    return false;
}

struct SelectionSort
{
    tbe::Vector3f cameraPos;
    bool byPos;

    bool operator()(const tbe::scene::Node* node1, const tbe::scene::Node * node2)
    {
        if(byPos)
            return(node1->GetPos() - cameraPos) < (node2->GetPos() - cameraPos);
        else
            return node1->GetAabb().GetSize() < node2->GetAabb().GetSize();
    }
};

bool EditorManager::SelectEntityEvent(tbe::EventManager * event)
{
    if(event->notify != EventManager::EVENT_MOUSE_DOWN)
        return false;

    if(event->mouseState[EventManager::MOUSE_BUTTON_RIGHT])
    {
        m_selectedNode = NULL;

        SelectionSort pred;

        pred.cameraPos = m_camera->GetPos();
        pred.byPos = true;
        std::sort(m_allEntity.begin(), m_allEntity.end(), pred);

        pred.byPos = false;
        std::sort(m_allEntity.begin(), m_allEntity.end(), pred);

        for(unsigned i = 0; i < m_allEntity.size(); i++)
        {
            if(m_allEntity[i]->GetAbsolutAabb().Add(0.5).IsInner(m_3dSelect))
            {
                m_selectedNode = m_allEntity[i];
                break;
            }
        }

        if(m_selectedNode)
        {
            m_axes->SetPos(m_selectedNode->GetPos());
            m_OCamera->SetCenter(m_selectedNode->GetPos());

            return true;
        }
    }

    return false;
}

bool EditorManager::SettingEntityEvent(EventManager* event)
{
    if(!m_selectedNode)
        return false;

    Vector3f grid(1);

    float moveSpeed = 0.05;
    float rotateSpeed = 45 * M_PI / 180;

    if(event->keyState[EventManager::KEY_LSHIFT])
    {
        manager.gameEngine->SetMouseVisible(false);
        manager.gameEngine->SetGrabInput(true);

        if(event->notify == EventManager::EVENT_MOUSE_MOVE)
        {
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

            transform += m_selectedNode->GetPos();


            m_selectedNode->SetPos(transform);
            m_axes->SetPos(transform);
        }

        else if(event->notify == EventManager::EVENT_MOUSE_DOWN)
        {
            Vector3f transform;

            if(event->lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_UP)
                transform.y++;
            else if(event->lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_DOWN)
                transform.y--;

            transform += m_selectedNode->GetPos();


            m_selectedNode->SetPos(transform);
            m_axes->SetPos(transform);
        }

        else if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            Vector3f pos = m_selectedNode->GetPos();
            Matrix4f matrix = m_selectedNode->GetMatrix();

            // Rotation
            if(event->keyState['A'])
                matrix.SetRotateX(-rotateSpeed);
            if(event->keyState['Z'])
                matrix.SetRotateX(rotateSpeed);
            if(event->keyState['Q'])
                matrix.SetRotateY(-rotateSpeed);
            if(event->keyState['S'])
                matrix.SetRotateY(rotateSpeed);
            if(event->keyState['W'])
                matrix.SetRotateZ(-rotateSpeed);
            if(event->keyState['X'])
                matrix.SetRotateZ(rotateSpeed);

            // In the floor
            if(event->keyState['F'])
            {
                if(event->keyState[EventManager::KEY_LALT])
                    pos.y = -m_selectedNode->GetAabb().min.y;
                else
                    pos.y = 0;

                pos = parallelscene.newton->FindFloor(pos);
            }

            // In the gride
            if(event->keyState['G'])
                tools::round(pos, grid);

            // Apply
            matrix.SetPos(pos);

            m_selectedNode->SetMatrix(matrix);
            m_axes->SetPos(pos);
        }

        return true;
    }

    else
    {
        manager.gameEngine->SetMouseVisible(true);
        manager.gameEngine->SetGrabInput(false);
    }

    return false;
}
