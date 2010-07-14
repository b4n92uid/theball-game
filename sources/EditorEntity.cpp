#include "EditorManager.h"

#include "GameManager.h"
#include "AppManager.h"
#include "BldParser.h"

using namespace std;
using namespace tbe;

void EditorManager::DeleteEntityEvent(tbe::EventManager* event)
{
    if(!m_selectedNode)
        return;

    if(event->notify != EventManager::EVENT_KEY_DOWN)
        return;

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
    }
}

void EditorManager::NewEntity(Object* ent)
{
    Vector3f pos = ent->NewtonNode::GetPos();

    m_allEntity.push_back(ent);

    m_axes->SetPos(pos);
    m_OCamera->SetCenter(pos);
}

void EditorManager::AllocEntityEvent(EventManager* event)
{
    Object* newObj = NULL;

    int c = event->lastActiveKey.first;

    if(c == 'c' && event->notify == EventManager::EVENT_KEY_DOWN && m_selectedNode)
    {
        Object* clone = m_selectedNode->Clone();
        NewEntity(clone);

        m_selectedNode = clone;
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
    }
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

void EditorManager::SelectEntityEvent(tbe::EventManager * event)
{
    if(event->notify != EventManager::EVENT_MOUSE_DOWN)
        return;

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
            m_axes->SetPos(m_selectedNode->NewtonNode::GetPos());
            m_OCamera->SetCenter(m_selectedNode->NewtonNode::GetPos());
        }
    }
}

void EditorManager::SettingEntityEvent(EventManager * event)
{
    Vector3f grid(1);

    float moveSpeed = 0.05;
    float rotateSpeed = 45 * M_PI / 180;

    if(!m_selectedNode)
        return;

    if(event->keyState[EventManager::KEY_SPACE])
        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            Matrix4f rotate = m_selectedNode->NewtonNode::GetMatrix();

            if(event->keyState['a'])
                rotate.SetRotateX(-rotateSpeed);
            if(event->keyState['e'])
                rotate.SetRotateX(rotateSpeed);
            if(event->keyState['q'])
                rotate.SetRotateY(-rotateSpeed);
            if(event->keyState['d'])
                rotate.SetRotateY(rotateSpeed);
            if(event->keyState['w'])
                rotate.SetRotateZ(-rotateSpeed);
            if(event->keyState['c'])
                rotate.SetRotateZ(rotateSpeed);

            rotate.SetPos(m_selectedNode->NewtonNode::GetPos());

            m_selectedNode->NewtonNode::SetMatrix(rotate);

            event->notify = EventManager::EVENT_NO_EVENT;
        }

    if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
        Vector3f transform = m_selectedNode->NewtonNode::GetPos();

        if(event->keyState['f'])
        {
            if(event->keyState[EventManager::KEY_LALT])
                transform.y = -m_selectedNode->GetAabb().min.y;
            else
                transform.y = 0;

            transform = parallelscene.newton->FindFloor(transform);

            m_selectedNode->NewtonNode::SetPos(transform);
            m_axes->SetPos(transform);
        }

        if(event->keyState['g'])
        {
            tools::round(transform, grid);

            m_selectedNode->NewtonNode::SetPos(transform);
            m_axes->SetPos(transform);
        }
    }

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

            transform += m_selectedNode->NewtonNode::GetPos();


            m_selectedNode->NewtonNode::SetPos(transform);
            m_axes->SetPos(transform);
        }
    }

    else
    {
        manager.gameEngine->SetMouseVisible(true);
        manager.gameEngine->SetGrabInput(false);
    }
}
