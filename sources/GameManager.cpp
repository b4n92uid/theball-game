/*
 * File:   GameManager.cpp
 * Author: b4n92uid
 *
 * Created on 01 janvier 2010, 20:34
 */

#include "GameManager.h"

#include "AppManager.h"
#include "SoundManager.h"
#include "MaterialManager.h"
#include "UserControl.h"

#include "StaticElement.h"
#include "Player.h"

#include "Define.h"
#include "Tools.h"

#include "Weapon.h"
#include "Bullet.h"

#include "Power.h"
#include "BulletTime.h"
#include "Boost.h"

#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <cmath>

using namespace std;
using namespace tbe;

GameManager::GameManager(AppManager* appManager) :
worldSettings(appManager->globalSettings.world)
{
    using namespace scene;

    cout << "> Initing game manager" << endl;

    manager.app = appManager;

    m_gameRunning = true;
    m_pauseRunning = false;

    manager.gameEngine = manager.app->getGameEngine();
    manager.gameEngine->setGrabInput(true);
    manager.gameEngine->setMouseVisible(false);

    manager.fps = manager.gameEngine->getFpsManager();
    manager.scene = manager.gameEngine->getSceneManager();
    manager.ppe = manager.gameEngine->getPostProcessManager();

    manager.gui = manager.app->getGuiMng();

    parallelscene.meshs = new scene::MeshParallelScene;
    parallelscene.meshs->setEnableFrustumTest(true);
    parallelscene.meshs->setTransparencySort(true);
    manager.scene->addParallelScene(parallelscene.meshs);

    parallelscene.particles = new scene::ParticlesParallelScene;
    manager.scene->addParallelScene(parallelscene.particles);

    parallelscene.marks = new scene::MapMarkParallelScene;
    manager.scene->addParallelScene(parallelscene.marks);

    parallelscene.newton = new scene::NewtonParallelScene;
    parallelscene.newton->setGravity(worldSettings.gravity);
    NewtonSetSolverModel(parallelscene.newton->getNewtonWorld(), 8);
    NewtonSetFrictionModel(parallelscene.newton->getNewtonWorld(), 1);
    manager.scene->addParallelScene(parallelscene.newton);

    manager.fmodsys = appManager->getFmodSystem();

    manager.material = new BodyMaterialManager(this);
    manager.sound = new SoundManager(this);
    manager.script = new ScriptManager(this);

    //    m_shootTargetAxes = new Axes(parallelscene.meshs, 2, 2);
    //    m_shootTargetAxes->setName("shootAxes");
    //    manager.scene->getRootNode()->addChild(m_shootTargetAxes);

    m_gameOver = false;

    m_cursorOnPlayer = false;

    m_winnerPlayer = NULL;

    m_camera = new scene::Camera(scene::Camera::TARGET_RELATIVE);
    m_camera->rotate(0);
    manager.scene->addCamera(m_camera);

    m_weaponSlot[38] = 1;
    m_weaponSlot[233] = 2;
    m_weaponSlot[34] = 3;
    m_weaponSlot[39] = 4;

    m_powerSlot[40] = 1;
    m_powerSlot[45] = 2;
    m_powerSlot[232] = 3;
    m_powerSlot[95] = 4;

    m_earthquake.intensity = 0;
    m_earthquake.physical = false;
}

GameManager::~GameManager()
{
    cout << "> Cleaning game manager" << endl;

    manager.gameEngine->setGrabInput(false);
    manager.gameEngine->setMouseVisible(true);

    Boost::clearSingleTone(this);
    BulletTime::clearSingleTone(this);

    BOOST_FOREACH(StaticElement* st, map.staticElements) delete st;
    BOOST_FOREACH(MapElement* st, map.mapElements) delete st;

    delete manager.sound;
    delete manager.material;
    delete manager.script;

    manager.scene->clearAll();

    manager.gui->getContext()->UnloadDocument(m_gui.gameover);
    manager.gui->getContext()->UnloadDocument(m_gui.hud);
}

void GameManager::setupMap(const Content::PartySetting& playSetting)
{
    cout << "> Loading level" << endl;

    map.settings = playSetting;

    // SCENE -------------------------------------------------------------------

    map.aabb.clear();

    scene::SceneParser* loader = manager.app->getSceneParser();
    loader->load(playSetting.map->filepath);
    loader->build();

    for(Iterator<scene::Mesh*> it = parallelscene.meshs->iterator(); it; it++)
    {
        StaticElement* elem = new StaticElement(this, *it);

        registerElement(elem);
    }

    for(Iterator<scene::ParticlesEmiter*> it = parallelscene.particles->iterator(); it; it++)
    {
        MapElement* elem = new MapElement(this);
        elem->setVisualBody(*it);
        elem->setId(it->getName());

        registerElement(elem);
    }

    for(Iterator<scene::MapMark*> it = parallelscene.marks->iterator(); it; it++)
    {
        MapElement* elem = new MapElement(this);
        elem->setVisualBody(*it);
        elem->setId(it->getName());

        registerElement(elem);
    }

    //    for(Iterator<scene::Light*> it = parallelscene.light->iterator(); it; it++)
    //    {
    //        MapElement* elem = new MapElement(this);
    //        elem->setVisualBody(*it);
    //        elem->setId(it->getName());
    //
    //        registerElement(elem);
    //    }

    map.aabb.add(AABB(Vector3f(-8, -8, -8), Vector3f(8, 64, 8)));

    AABB newtonWordSize = map.aabb;
    newtonWordSize.add(Vector3f(32.0f));
    parallelscene.newton->setWorldSize(newtonWordSize);

    // PPE ---------------------------------------------------------------------

    using namespace tbe::ppe;

    m_gui.background.flash = new ColorEffect;
    m_gui.background.flash->setFusionMode(ColorEffect::ADDITIVE_COLOR);
    m_gui.background.flash->setColor(Vector4f(1, 1, 1, 1));
    m_gui.background.flash->setEnable(false);
    manager.ppe->addPostEffect("flashEffect", m_gui.background.flash);

    m_gui.background.dammage = new ColorEffect;
    m_gui.background.dammage->setFusionMode(ColorEffect::MULTIPLICATION_COLOR);
    m_gui.background.dammage->setColor(Vector4f(1, 0, 0, 1));
    m_gui.background.dammage->setEnable(false);
    manager.ppe->addPostEffect("dammageEffect", m_gui.background.dammage);

    // -- Shader PPE

    const Settings::Video& vidSets = manager.app->globalSettings.video;

    if(vidSets.ppeUse)
    {
        m_gui.background.gameover = new BlurEffect;
        m_gui.background.gameover->setPasse(1);
        m_gui.background.gameover->setEnable(false);
        manager.ppe->addPostEffect("gameoverEffect", m_gui.background.gameover);

        m_gui.background.bloom = new BloomEffect;
        m_gui.background.bloom->setRttFrameSize(vidSets.ppeSize);
        m_gui.background.bloom->setEnable(false);
        manager.ppe->addPostEffect("bloomEffect", m_gui.background.bloom);
    }

    // PLAYERS -----------------------------------------------------------------

    m_userPlayer = new Player(this, map.settings.nickname, map.settings.player);
    m_userPlayer->attachController(new UserControl(this));

    manager.scene->getRootNode()->addChild(m_userPlayer->getVisualBody());

    registerPlayer(m_userPlayer);

    // SCRIPT ------------------------------------------------------------------

    if(!map.settings.map->script.empty())
        manager.script->load(map.settings.map->script);
    else
        cout << "***Warning*** their is no script attached to this map !" << endl;
}

typedef void (GameManager::*ActionMethod)();

class EventListner :
public Rocket::Core::EventListener,
public std::map<Rocket::Core::String, ActionMethod>
{
public:

    EventListner(GameManager* appm) : m_gamem(appm) { }

    void ProcessEvent(Rocket::Core::Event& e)
    {
        if(!this->count(e.GetCurrentElement()->GetId()))
            return;

        ActionMethod f = this->at(e.GetCurrentElement()->GetId());
        (m_gamem->*f)();
    }

private:
    GameManager* m_gamem;
};

void GameManager::setupGui()
{
    using namespace boost;
    using namespace tbe::gui;
    using namespace Rocket::Core;

    cout << "> Loading GUI" << endl;

    Context* context = manager.gui->getContext();

    #define guidir(file) (manager.app->globalSettings.paths.get<string > \
                            ("dirs.gui") + "/" + file).c_str()

    m_gui.hud = context->LoadDocument(guidir("hud.rml"));
    m_gui.pause = context->LoadDocument(guidir("pausemenu.rml"));
    m_gui.gameover = context->LoadDocument(guidir("gameover.rml"));

    #undef guidir

    m_gui.objective = m_gui.hud->GetElementById("objective");
    m_gui.message = m_gui.hud->GetElementById("message");

    m_gui.life = m_gui.hud->GetElementById("health");
    m_gui.ammo = m_gui.hud->GetElementById("ammo");

    m_gui.powerIcon = m_gui.hud->GetElementById("power-icon");
    m_gui.weaponIcon = m_gui.hud->GetElementById("weapon-icon");

    m_gui.croshair = m_gui.hud->GetElementById("croshair");

    EventListner* elister = new EventListner(this);
    (*elister)["return"] = &GameManager::onPauseMenuReturn;
    (*elister)["quit"] = &GameManager::onPauseMenuQuit;

    m_gui.pause->AddEventListener("click", elister);
}

void GameManager::startGameProcess()
{
    m_gui.hud->Show();

    onStartGame(m_userPlayer);
}

tbe::Vector3f GameManager::getRandomPosOnTheFloor()
{
    Vector3f randPos;

    // Réduit le champs d'application de 5%
    float factor = 5 * map.aabb.getLength() / 100;

    do
    {
        randPos = (map.aabb - Vector3f(factor)).randPos();
        randPos.y = 1;
        randPos = parallelscene.newton->findFloor(randPos);
    }
    while(randPos.y < map.aabb.min.y);

    return randPos;
}

MapElement* GameManager::getInterface(tbe::scene::Node* node)
{

    BOOST_FOREACH(StaticElement* st, map.staticElements)
    {
        if(st->getVisualBody() == node)
            return st;
    }

    BOOST_FOREACH(MapElement* st, map.mapElements)
    {
        if(st->getVisualBody() == node)
            return st;
    }

    return NULL;
}

tbe::Vector3f GameManager::getRandomPosOnTheFloor(Vector3f pos, float radius)
{
    Vector3f randPos;

    do
    {
        randPos = pos + AABB(radius).randPos();
        randPos.y = 1;
        randPos = parallelscene.newton->findFloor(randPos);
    }
    while(randPos.y < map.aabb.min.y);

    return randPos;
}

void GameManager::display(std::string msg, unsigned duration)
{
    m_gui.message->SetInnerRML(msg.c_str());
    m_gui.message->SetProperty("visibility", "visible");

    m_logClock.snapShoot();
    m_logClockOff = duration;
}

void GameManager::status(std::string msg)
{
    m_gui.objective->SetInnerRML(msg.c_str());
    m_gui.objective->SetProperty("visibility", "visible");
}

void GameManager::processDevelopperCodeEvent()
{
    EventManager* event = manager.gameEngine->getEventManager();

    if(event->notify == EventManager::EVENT_KEY_DOWN)
    {
    }
}

void GameManager::eventProcess()
{
    using namespace tbe::scene;

    manager.gameEngine->pollEvent();

    EventManager* event = manager.gameEngine->getEventManager();

    if(m_gameOver)
    {
        if(event->keyState[EventManager::KEY_SPACE]
           && m_validGameOver.isEsplanedTime(2000))
            m_gameRunning = false;
    }

    else
    {
        if(m_userPlayer->isKilled())
            return;

        // Vue
        if(event->notify == EventManager::EVENT_MOUSE_MOVE)
            manager.scene->getCurCamera()->rotate(event->mousePosRel);

        // Séléction d'arme
        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(m_weaponSlot.count(event->lastActiveKey.first))
                m_userPlayer->slotWeapon(m_weaponSlot[event->lastActiveKey.first]);

            if(m_powerSlot.count(event->lastActiveKey.first))
                m_userPlayer->slotPower(m_powerSlot[event->lastActiveKey.first]);
        }

        // Touche de pause (Esc)
        if(event->keyState[EventManager::KEY_ESCAPE])
        {
            if(m_userPlayer->getCurPower())
                m_userPlayer->getCurPower()->diactivate();

            onPauseMenuShow();
        }

        #ifndef NDEBUG
        // Code developper
        processDevelopperCodeEvent();
        #endif
    }
}

void GameManager::onPauseMenuShow()
{
    manager.gameEngine->setGrabInput(false);
    manager.gameEngine->setMouseVisible(true);

    m_gui.hud->Hide();
    m_gui.pause->Show();

    m_pauseRunning = true;
    while(m_pauseRunning)
    {
        manager.gameEngine->pollEvent();
        manager.gui->trasmitEvent(*manager.gameEngine->getEventManager());

        manager.gameEngine->beginScene();
        manager.gui->render();
        manager.gameEngine->endScene();
    }

    manager.gameEngine->setGrabInput(true);
    manager.gameEngine->setMouseVisible(false);

    m_gui.pause->Hide();
    m_gui.hud->Show();
}

void GameManager::onPauseMenuQuit()
{
    manager.gameEngine->setGrabInput(true);
    manager.gameEngine->setMouseVisible(false);

    m_pauseRunning = false;
    m_gameRunning = false;
}

void GameManager::onPauseMenuReturn()
{
    m_pauseRunning = false;
}

int EarthQuakeProcess(const NewtonBody* body, void* userData)
{
    MapElement* elem = (MapElement*) NewtonBodyGetUserData(body);

    scene::NewtonNode* pbody = elem->getPhysicBody();

    float intensity = *(float*) userData;

    Vector3f pointPos = pbody->getPos() + AABB(1).randPos();

    NewtonBodyAddImpulse(body, AABB(intensity * 4).randPos().normalize(), pointPos);

    return 0;
}

void GameManager::gameProcess()
{
    using namespace tbe;
    using namespace tbe::scene;

    if(m_gameOver)
        return;

    EventManager* eventmng = manager.gameEngine->getEventManager();

    for(unsigned i = 0; i < map.staticElements.size(); i++)
    {
        StaticElement* elem = map.staticElements[i];

        if(!map.aabb.isInner(elem->getVisualBody()))
            elem->resetInitialMatrix();
    }

    if(m_backImpulse.intensity > 0)
    {

        foreach(Vector3f & pos, m_playerPosRec)
        {
            pos += -m_camera->getTarget() * m_backImpulse.intensity;
        }

        m_backImpulse.intensity -= m_backImpulse.push * eventmng->lastPollTimestamp * 0.5f;
    }

    /*
     * Gestion de l'effet tremblement de terre :(
     */
    if(m_earthquake.intensity > 0)
    {

        foreach(Vector3f& pos, m_playerPosRec)
        {
            Vector3f quake = AABB(0.5).randPos();

            pos += quake * m_earthquake.intensity;
        }

        if(m_earthquake.physical)
        {
            NewtonWorldForEachBodyInAABBDo(parallelscene.newton->getNewtonWorld(),
                                           map.aabb.min, map.aabb.max,
                                           EarthQuakeProcess, &m_earthquake.intensity);
        }
        m_earthquake.intensity -= 0.01;
    }

    /*
     * Pour chaque joueurs
     *  - les joueurs mort pour les reconstruires
     *  - les joueurs en fin de vie pour la préparation a la mort ;)
     *  - les joueurs hors de l'arene pour les remetr en place
     */
    for(unsigned i = 0; i < m_players.size(); i++)
    {
        Player* player = m_players[i];

        player->process();

        if(player->isKilled())
        {
            if(player->clocks.toRespawn.isEsplanedTime(1000))
            {
                player->reBorn();

                manager.sound->playSound("respawn", player);
            }
        }

        else
        {

            if(!map.aabb.isInner(player->getVisualBody()))
            {
                if(!onOutOfArena(player))
                    player->setInLastSpawnPoint();
            }
        }
    }

    if(m_gui.message->IsVisible() && m_logClock.isEsplanedTime(m_logClockOff))
        m_gui.message->SetProperty("visibility", "hidden");
}

void GameManager::hudProcess()
{
    using namespace tbe::gui;
    using namespace Rocket::Core;
    using boost::format;

    // Gestion de l'ETH en gameover
    if(m_gameOver)
        return;

    // Mise a jour des bar de progression (Vie, Muinition, Bullettime)

    if(m_userPlayer->isKilled())
    {
        m_gui.life->SetInnerRML("Mort !");
        //hud.life->setValue(0);
        return;
    }

    // -- Arme en cours

    const Weapon* curWeapon = m_userPlayer->getCurWeapon();

    if(curWeapon)
    {
        m_gui.croshair->SetProperty("visibility", "visible");
        m_gui.weaponIcon->SetProperty("visibility", "visible");
        m_gui.weaponIcon->SetClassNames(curWeapon->getWeaponName().c_str());

        const int &ammoCount = curWeapon->getAmmoCount(),
                &ammoCountMax = curWeapon->getMaxAmmoCount();

        format weaponGaugeFormat = format("%1%/%2%")
                % ammoCount % ammoCountMax;

        m_gui.ammo->SetInnerRML(weaponGaugeFormat.str().c_str());
        // hud.ammoGauge->setValue(ammoCount * 100 / ammoCountMax);
    }
    else
    {
        m_gui.croshair->SetProperty("visibility", "hidden");
        m_gui.weaponIcon->SetProperty("visibility", "hidden");
        m_gui.ammo->SetInnerRML("n/a");
        //hud.ammoGauge->setValue(0);
    }

    // -- Pouvoire en cour

    const Power* curPower = m_userPlayer->getCurPower();

    if(curPower)
    {
        m_gui.powerIcon->SetProperty("visibility", "visible");
        m_gui.powerIcon->SetClassNames(curPower->getName().c_str());

        // const int &energy = m_userPlayer->getEnergy();

        // format powerGaugeFormat = format("%1% %2%/100") % curPower->getName() % energy;

        // hud.energy->write(powerGaugeFormat.str());
        // hud.energyGauge->setValue(energy);
    }
    else
    {
        m_gui.powerIcon->SetProperty("visibility", "hidden");
        // hud.energy->write("Pas de pouvoir :(");
        // hud.energyGauge->setValue(0);
    }

    // -- Etat de santé

    const int &life = m_userPlayer->getLife();
    m_gui.life->SetInnerRML((format("%1%/100") % life).str().c_str());
    // hud.life->setValue(life);

    // ---- Effet de dommage

    if(m_gui.background.dammage->isEnable())
    {
        float alpha = m_gui.background.dammage->getAlpha();

        if(alpha > 0)
        {
            alpha -= 0.01f;
            m_gui.background.dammage->setAlpha(alpha);
        }
        else
            m_gui.background.dammage->setEnable(false);
    }

    // ---- Flash effect

    if(m_flasheffect.initOpacity > 0)
    {
        m_flasheffect.initOpacity -= m_flasheffect.downOpacity;
        m_gui.background.flash->setAlpha(m_flasheffect.initOpacity);
    }
    else
        m_gui.background.flash->setEnable(false);
}

float cameraRayFilter(const NewtonBody* body, const NewtonCollision*, const float*, const float*, dLong, void* userData, float intersectParam)
{
    MapElement* elem = (MapElement*) NewtonBodyGetUserData(body);

    if(elem == elem->getGameManager()->getUserPlayer())
        return intersectParam;

    float& hit = *static_cast<float*> (userData);

    if(intersectParam > 0.2 && intersectParam < hit)
        hit = intersectParam;

    return intersectParam;
}

void GameManager::render()
{
    if(!m_gameOver)
        onEachFrame(m_userPlayer);

    Vector3f campos = m_camera->getPos();
    Vector3f camtar = m_camera->getTarget();

    // Positionement camera ----------------------------------------------------

    /*
     * La position de la camera est caluclé a partir de la position du joueur,
     * et en reculant de quelques unités vers l'arrier.
     *
     * Un lancé de rayon est éffectuer pour savoir si la position de la camera
     * entre en collision avec un objet, si c'est le cas alors la position
     * arrier diminue en fonction du taux d'intersection.
     */

    Vector3f setPos = m_userPlayer->getVisualBody()->getPos();

    if(!m_playerPosRec.empty())
        setPos.y -= (setPos.y - m_playerPosRec.back().y) / 3 * 2;

    m_playerPosRec.push_back(setPos);

    Vector3f camzeropos = m_playerPosRec.front() + Vector3f(0, worldSettings.cameraUp, 0);
    Vector3f camendpos = camzeropos + (-camtar) * worldSettings.cameraBack;

    {
        float hit = 1;
        NewtonWorldRayCast(parallelscene.newton->getNewtonWorld(), camzeropos, camendpos, cameraRayFilter, &hit, NULL, 0);

        hit = hit * worldSettings.cameraBack;

        campos = camzeropos - camtar * min(hit, worldSettings.cameraBack);
    }

    m_camera->setPos(campos);

    if(m_playerPosRec.size() > 2)
        m_playerPosRec.pop_front();

    // Pick --------------------------------------------------------------------

    /*
     * Ici sera spécifier le vecteur de visé du joueur
     * en effectuant un lancé de rayon grace au moteur physique
     * depuis la position de la camera vers un vecteur aligé sur
     * le vecteur cible de la camera.
     *
     * Si le vecteur de visé se trouve a l'interieur du joueur
     * on relance un second rayon en ignorant le joueur et en affectant une
     * légere transparence a ce dernier.
     *
     * Si la position de la camera se trouve a l'interieur du joueur
     * on relance un second rayon en ignorant le joueur et en affectant une
     * forte transparence a ce dernier.
     */

    Vector3f::Array hitArray = parallelscene.meshs->rayCast(campos, camtar);

    if(!hitArray.empty())
        m_shootTarget = hitArray.front();
    else
        m_shootTarget = campos + camtar * 32;

    //    m_shootTargetAxes->setPos(m_shootTarget);

    if(m_userPlayer->getVisualBody()->getAbsolutAabb().isInner(campos))
        m_userPlayer->makeTransparent(true, 0.2);
    else
        m_userPlayer->makeTransparent(false);

    // Son 3D ------------------------------------------------------------------

    /*
     * Met a jour la position virtuelle de la camera du moteur audio pour
     * calculer les son 3D.
     */

    if(!manager.app->globalSettings.noaudio)
    {
        manager.sound->syncronizeSoundsPosition();

        FMOD_System_Set3DListenerAttributes(manager.fmodsys, 0, (FMOD_VECTOR*) (float*) campos, 0,
                                            (FMOD_VECTOR*) (float*) camtar,
                                            (FMOD_VECTOR*) (float*) m_camera->getUp());

        FMOD_System_Update(manager.fmodsys);
    }

    // Physique ----------------------------------------------------------------

    // parallelscene.newton->setWorldTimestep(manager.fps->getFromLastFrameMs());

    manager.material->process();

    // Rendue ------------------------------------------------------------------

    manager.gameEngine->beginScene();

    if(manager.app->globalSettings.video.ppeUse)
    {
        Rtt* rtt = manager.ppe->getRtt();

        rtt->use(true);
        rtt->clear();
        manager.scene->render();
        rtt->use(false);
        manager.ppe->render();
    }
    else
        manager.scene->render();

    manager.gui->render();
    //    manager.scene->getShadowMap()->renderDebug();
    manager.gameEngine->endScene();

    // Effacement programmé ----------------------------------------------------

    BOOST_FOREACH(MapElement* elem, m_shuduledDel)
    {
        elem->freeAttachedNode();
        delete elem;
    }

    m_shuduledDel.clear();
}

tbe::Vector3f GameManager::getShootTarget() const
{
    return m_shootTarget;
}

tbe::Vector3f GameManager::getViewDirection() const
{
    Vector3f dir = m_camera->getTarget();
    dir.y = 0;

    return dir.normalize();
}

Player* GameManager::getUserPlayer() const
{
    return m_userPlayer;
}

Player* GameManager::getWinnerPlayer() const
{
    return m_winnerPlayer;
}

const Player::Array& GameManager::getPlayers() const
{
    return m_players;
}

void GameManager::setGameOver(Player* winner, std::string finalmsg)
{
    m_gameOver = true;

    m_winnerPlayer = winner;

    for(unsigned i = 0; i < m_players.size(); i++)
    {
        m_players[i]->getPhysicBody()->setApplyForce(0);
        m_players[i]->getPhysicBody()->setVelocity(0);

        NewtonBodySetFreezeState(m_players[i]->getPhysicBody()->getBody(), true);
    }

    manager.gameEngine->setMouseVisible(true);

    Rocket::Core::Element* core = m_gui.gameover->GetElementById("core");
    core->SetInnerRML(finalmsg.c_str());

    m_gui.hud->Hide();
    m_gui.gameover->Show();

    if(manager.app->globalSettings.video.ppeUse)
        m_gui.background.gameover->setEnable(true);
}

bool GameManager::isGameOver() const
{
    return m_gameOver;
}

bool GameManager::isRunning() const
{
    return m_gameRunning;
}

void GameManager::backImpulse(float intensity, float push)
{
    m_backImpulse.intensity = intensity;
    m_backImpulse.push = push;
}

void GameManager::earthQuake(float intensity, bool physical)
{
    m_earthquake.intensity = intensity;
    m_earthquake.physical = physical;

    //manager.sound->playSound("quake", m_userPlayer);
}

void GameManager::whiteFlash(float initOpacity, float downOpacity)
{
    m_flasheffect.initOpacity = initOpacity;
    m_flasheffect.downOpacity = downOpacity;

    m_gui.background.flash->setEnable(true);
    m_gui.background.flash->setAlpha(m_flasheffect.initOpacity);

    manager.sound->playSound("flash", m_userPlayer);
}

void GameManager::dammageScreen()
{
    m_dammageEffect.initOpacity = 0.75;
    m_dammageEffect.downOpacity = 0.01;

    m_gui.background.dammage->setEnable(true);
    m_gui.background.dammage->setColor(Vector4f(1, 0, 0, 0.5));
}

void GameManager::glowEnable(bool stat)
{
    m_gui.background.bloom->setEnable(stat);
}

void GameManager::glowSettings(float intensity, float thershold, float blurpass)
{
    m_gui.background.bloom->setIntensity(intensity);
    m_gui.background.bloom->setThreshold(thershold);
    m_gui.background.bloom->setBlurPass(blurpass);
}

const Player::Array GameManager::getTargetsOf(Player* player) const
{
    Player::Array array(m_players);
    array.erase(find(array.begin(), array.end(), player));

    return array;
}

void GameManager::registerElement(StaticElement* elem)
{
    if(elem->getPhysicBody())
        manager.material->addElement(elem);

    map.staticElements.push_back(elem);

    if(elem->getVisualBody())
        map.aabb.count(elem->getVisualBody());
}

void GameManager::unregisterElement(StaticElement* staticObject)
{
    StaticElement::Array::iterator it = find(map.staticElements.begin(),
                                             map.staticElements.end(), staticObject);

    map.staticElements.erase(it);
}

void GameManager::registerElement(MapElement* elem)
{
    if(elem->getPhysicBody())
        manager.material->addElement(elem);

    map.mapElements.push_back(elem);

    if(elem->getVisualBody())
        map.aabb.count(elem->getVisualBody());
}

void GameManager::unregisterElement(MapElement* staticObject)
{
    MapElement::Array::iterator it = find(map.mapElements.begin(),
                                          map.mapElements.end(), staticObject);

    map.mapElements.erase(it);
}

void GameManager::registerPlayer(Player* player)
{
    manager.material->addPlayer(player);

    m_players.push_back(player);

    onPlayerInit(player);
}

void GameManager::unregisterPlayer(Player* player, bool toDelete)
{
    Player::Array::iterator it = find(m_players.begin(), m_players.end(), player);

    m_players.erase(it);

    if(toDelete)
        m_shuduledDel.push_back(player);
}

void GameManager::registerArea(AreaElement* area)
{
    map.areaElements.push_back(area);
}

void GameManager::unregisterArea(AreaElement* area)
{
    AreaElement::Array::iterator it = find(map.areaElements.begin(), map.areaElements.end(), area);

    map.areaElements.erase(it);
}
