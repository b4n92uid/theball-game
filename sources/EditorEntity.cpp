#include "EditorManager.h"

#include "GameManager.h"
#include "AppManager.h"
#include "BldParser.h"

using namespace std;
using namespace tbe;

bool EditorManager::deleteEntityEvent(tbe::EventManager* event)
{
    if(!m_selectedNode)
        return false;

    if(event->notify != EventManager::EVENT_KEY_DOWN)
        return false;

    if(event->lastActiveKey.first == EventManager::KEY_DELETE)
    {
        if(tools::find(map.staticObjects, m_selectedNode))
            unregisterStatic(dynamic_cast<StaticObject*>(m_selectedNode));

        if(tools::find(map.dynamicObjects, m_selectedNode))
            unregisterDynamic(dynamic_cast<DynamicObject*>(m_selectedNode));

        if(tools::find(map.items, m_selectedNode))
            unregisterItem(dynamic_cast<Item*>(m_selectedNode));

        if(tools::find(m_visualSpawnPoints, m_selectedNode))
            tools::erase(m_visualSpawnPoints, m_selectedNode);

        tools::erase(m_allEntity, m_selectedNode);

        delete m_selectedNode, m_selectedNode = NULL;

        return true;
    }

    return false;
}

void EditorManager::newEntity(Object* ent)
{
    m_allEntity.push_back(ent);

    manager.scene->getRootNode()->addChild(ent);

    Vector3f pos = ent->getPos();
    m_axes->setPos(pos);
    m_OCamera->setCenter(pos);
}

bool EditorManager::allocEntityEvent(EventManager* event)
{
    Object* newObj = NULL;

    int c = event->lastActiveKey.first;

    if(c == 'c' && event->notify == EventManager::EVENT_KEY_DOWN && m_selectedNode)
    {
        Object* clone = NULL;
        if(tools::find(m_visualSpawnPoints, m_selectedNode))
        {
            clone = new StaticObject(this, OBJ_SPAWN, m_axes->getPos());

            m_visualSpawnPoints.push_back(clone);
        }

        else
        {
            clone = m_selectedNode->cloneToObject();
        }

        newEntity(clone);
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
                dy = new DYJumper(this, m_axes->getPos());
                break;
            case 't':
                dy = new DYTeleporter(this, m_axes->getPos());
                break;
        }

        if(dy)
        {
            registerDynamic(dy);
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
                it = new ItemAddLife(this, m_axes->getPos());
                break;
            case 'a':
                it = new ItemAddAmmo(this, m_axes->getPos());
                break;
            case 's':
                it = new ItemSuperLife(this, m_axes->getPos());
                break;
            case 'f':
                it = new ItemFatalShot(this, m_axes->getPos());
                break;
            case 'i':
                it = new ItemAddFinder(this, m_axes->getPos());
                break;
            case 'h':
                it = new ItemAddShotgun(this, m_axes->getPos());
                break;
            case 'b':
                it = new ItemAddBomb(this, m_axes->getPos());
                break;
        }

        if(it)
        {
            registerItem(it);
            newObj = it;
        }
    }

    else if(event->keyState['p'] && event->notify == EventManager::EVENT_KEY_DOWN)
    {
        StaticObject* visualSpawn = new StaticObject(this, OBJ_SPAWN, m_axes->getPos());

        m_visualSpawnPoints.push_back(visualSpawn);

        newObj = visualSpawn;
    }

        // Static
    else
    {
        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(event->lastActiveKey.first == 's')
                hud.entity.list->setEnable(true);

            else if(event->lastActiveKey.first == EventManager::KEY_ESCAPE)
                hud.entity.list->cancelSelection();
        }

        else if(event->notify == EventManager::EVENT_KEY_UP)
        {
            if(event->lastActiveKey.first == 's')
            {
                if(hud.entity.list->isSelection())
                {
                    string filename = hud.entity.list->getCurrentData().getValue<string > ();

                    StaticObject* so = new StaticObject(this, filename, m_axes->getPos());
                    registerStatic(so);
                    newObj = so;

                    parallelscene.newton->setWorldSize(map.aabb);

                    hud.entity.list->cancelSelection();
                }

                hud.entity.list->setEnable(false);
            }
        }
    }

    if(newObj)
    {
        newEntity(newObj);
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
            return (node1->getPos() - cameraPos) < (node2->getPos() - cameraPos);
        else
            return node1->getAabb().getSize() < node2->getAabb().getSize();
    }
};

bool EditorManager::selectEntityEvent(tbe::EventManager * event)
{
    if(event->notify != EventManager::EVENT_MOUSE_DOWN)
        return false;

    if(event->mouseState[EventManager::MOUSE_BUTTON_RIGHT])
    {
        m_selectedNode = NULL;

        SelectionSort pred;

        pred.cameraPos = m_camera->getPos();
        pred.byPos = true;
        std::sort(m_allEntity.begin(), m_allEntity.end(), pred);

        pred.byPos = false;
        std::sort(m_allEntity.begin(), m_allEntity.end(), pred);

        for(unsigned i = 0; i < m_allEntity.size(); i++)
        {
            if(m_allEntity[i]->getAbsolutAabb().add(AABB(0.5f)).isInner(m_3dSelect))
            {
                m_selectedNode = m_allEntity[i];
                break;
            }
        }

        if(m_selectedNode)
        {
            m_axes->setPos(m_selectedNode->getPos());
            m_OCamera->setCenter(m_selectedNode->getPos());

            return true;
        }
    }

    return false;
}

bool EditorManager::settingEntityEvent(EventManager* event)
{
    if(!m_selectedNode)
        return false;

    Vector3f grid(1);

    float moveSpeed = 0.05;
    float rotateSpeed = 15 * M_PI / 180;

    if(event->keyState[EventManager::KEY_LSHIFT])
    {
        manager.gameEngine->setMouseVisible(false);
        manager.gameEngine->setGrabInput(true);

        if(event->notify == EventManager::EVENT_MOUSE_MOVE)
        {
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

            transform += m_selectedNode->getPos();

            m_selectedNode->getPhysicBody()->setPos(transform);
            m_axes->setPos(transform);
        }

        else if(event->notify == EventManager::EVENT_MOUSE_DOWN)
        {
            Vector3f transform;

            if(event->lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_UP)
                transform.y++;
            else if(event->lastActiveMouse.first == EventManager::MOUSE_BUTTON_WHEEL_DOWN)
                transform.y--;

            transform += m_selectedNode->getPos();


            m_selectedNode->setPos(transform);
            m_axes->setPos(transform);
        }

        else if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            Vector3f pos = m_selectedNode->getPos();
            Matrix4f matrix = m_selectedNode->getMatrix();

            // Rotation
            if(event->keyState['A'])
                matrix.setRotateX(-rotateSpeed);
            if(event->keyState['Z'])
                matrix.setRotateX(rotateSpeed);
            if(event->keyState['Q'])
                matrix.setRotateY(-rotateSpeed);
            if(event->keyState['S'])
                matrix.setRotateY(rotateSpeed);
            if(event->keyState['W'])
                matrix.setRotateZ(-rotateSpeed);
            if(event->keyState['X'])
                matrix.setRotateZ(rotateSpeed);

            // In the floor
            if(event->keyState['F'])
            {
                Vector3f start = pos;
                start.y += m_selectedNode->getAabb().max.y;

                Vector3f end = map.aabb.min;
                end.x = pos.x;
                end.z = pos.z;

                pos = parallelscene.newton->findZeroMassBody(start, end);

                if(event->keyState[EventManager::KEY_LALT])
                    pos.y += -m_selectedNode->getAabb().min.y;
            }

            // In the gride
            if(event->keyState['G'])
                tools::round(pos, grid);

            // Apply
            matrix.setPos(pos);

            m_selectedNode->getPhysicBody()->setMatrix(matrix);
            m_axes->setPos(pos);
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
