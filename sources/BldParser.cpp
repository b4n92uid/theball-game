/* 
 * File:   BldParser.cpp
 * Author: b4n92uid
 * 
 * created on 2 décembre 2009, 21:46
 */

#include "BldParser.h"

#include "PlayManager.h"
#include "MaterialManager.h"
#include "UserControl.h"

#include <fmod_errors.h>

using namespace std;
using namespace tbe;
using namespace tbe::scene;

BldParser::BldParser(GameManager* gameManager)
{
    m_gameManager = gameManager;
    m_sceneManager = m_gameManager->manager.scene;
    m_materialManager = m_gameManager->manager.material;

    m_meshScene = m_gameManager->parallelscene.meshs;
    m_lightScene = m_gameManager->parallelscene.light;
    m_newtonScene = m_gameManager->parallelscene.newton;

    m_fmodsys = m_gameManager->manager.fmodsys;
}

BldParser::~BldParser()
{
}

inline std::string stripComments(std::string content)
{
    for(unsigned pos = 0; (pos = content.find('#', pos)) != string::npos;)
        content.replace(pos, content.find('\n', pos) - pos + 1, "");

    return content;
}

bool BldParser::isChanged()
{
    std::stringstream newmap, oldmap;

    std::ifstream file(m_openFileName.c_str());
    oldmap << file.rdbuf();

    writeMap(newmap);

    return stripComments(newmap.str()) != stripComments(oldmap.str());
}

void BldParser::saveLevel()
{
    saveLevel(m_openFileName);
}

void BldParser::writeMap(std::iostream& stream)
{
    using namespace boost::posix_time;

    stream << "# Ball level descriptor" << endl;
    stream << "# Generated " << second_clock::local_time() << endl;
    stream << endl;


    stream << ".map" << endl;
    stream << "name=" << m_gameManager->map.name << endl;
    stream << "ambient=" << m_sceneManager->getAmbientLight() << endl;
    stream << endl;


    if(!m_gameManager->map.musicPath.empty())
    {
        stream << ".music" << endl;
        stream << "filePath=" << m_gameManager->map.musicPath << endl;
        stream << endl;
    }

    Fog* fog = m_sceneManager->getFog();

    if(fog->isEnable())
    {
        stream << ".fog" << endl;
        stream << "color=" << fog->getColor() << endl;
        stream << "start=" << fog->getStart() << endl;
        stream << "end=" << fog->getEnd() << endl;
        stream << endl;
    }

    SkyBox* sky = m_sceneManager->getSkybox();

    if(sky->isEnable())
    {
        Texture* skyTexs = sky->getTextures();

        stream << ".skybox" << endl;
        stream << "front=" << skyTexs[0].getFilename() << endl;
        stream << "back=" << skyTexs[1].getFilename() << endl;
        stream << "top=" << skyTexs[2].getFilename() << endl;
        stream << "bottom=" << skyTexs[3].getFilename() << endl;
        stream << "left=" << skyTexs[4].getFilename() << endl;
        stream << "right=" << skyTexs[5].getFilename() << endl;
        stream << endl;
    }

    stream << "# Light" << endl;
    stream << endl;

    for(unsigned i = 0; i < m_gameManager->map.lights.size(); i++)
    {
        Light* light = m_gameManager->map.lights[i];

        bool pointType = (light->getType() == Light::POINT);

        stream << "+light" << endl;
        stream << "type=" << (pointType ? "POINT" : "DIRI") << endl;
        stream << "pos=" << light->getPos() << endl;
        stream << "ambient=" << light->getAmbient() << endl;
        stream << "diffuse=" << light->getDiffuse() << endl;
        stream << "specular=" << light->getSpecular() << endl;
        if(pointType)
            stream << "radius=" << light->getRadius() << endl;
        stream << endl;
    }

    stream << "# Static" << endl;
    stream << endl;

    for(unsigned i = 0; i < m_gameManager->map.staticObjects.size(); i++)
        m_gameManager->map.staticObjects[i]->outputConstruction(stream);

    stream << "# Dynamic" << endl;
    stream << endl;

    for(unsigned i = 0; i < m_gameManager->map.dynamicObjects.size(); i++)
        m_gameManager->map.dynamicObjects[i]->outputConstruction(stream);

    stream << "# Item" << endl;
    stream << endl;

    for(unsigned i = 0; i < m_gameManager->map.items.size(); i++)
        m_gameManager->map.items[i]->outputConstruction(stream);

    stream << "# Spawn" << endl;
    stream << endl;

    for(unsigned i = 0; i < m_gameManager->map.spawnPoints.size(); i++)
    {
        stream << "+node" << endl;
        stream << "type=SPAWN" << endl;
        stream << "pos=" << m_gameManager->map.spawnPoints[i] << endl;
        stream << endl;
    }
}

void BldParser::saveLevel(const std::string& filepath)
{
    fstream file(filepath.c_str(), ios::out);

    if(!file)
        throw tbe::Exception("BldParser::SaveLevel; Open file error (%s)", filepath.c_str());

    m_openFileName = filepath;

    writeMap(file);

    file.close();
}

void BldParser::loadLevel(const std::string& filepath)
{
    ifstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("BldParser::LoadScene; Open file error (%s)", filepath.c_str());

    m_openFileName = filepath;

    string buffer;
    while(tools::getline(file, buffer))
    {
        if(buffer.empty() || buffer[0] == '#')
            continue;

        if(buffer == ".map")
        {
            AttribMap att = getAttributs(file);
            onLoadMap(att);
        }

        else if(buffer == ".music")
        {
            AttribMap att = getAttributs(file);
            onLoadMusic(att);
        }

        else if(buffer == ".fog")
        {
            AttribMap att = getAttributs(file);
            onLoadFog(att);
        }


        else if(buffer == ".skybox")
        {
            AttribMap att = getAttributs(file);
            onLoadSkyBox(att);
        }

        else if(buffer == "+node")
        {
            AttribMap att = getAttributs(file);
            onLoadNode(att);
        }

        else if(buffer == "+light")
        {
            AttribMap att = getAttributs(file);
            onLoadLight(att);
        }

        else
            throw tbe::Exception("BldParser::LoadScene; Parse error (%s)", buffer.c_str());
    }

    file.close();
}

void BldParser::setOpenFileName(std::string openFileName)
{
    this->m_openFileName = openFileName;
}

std::string BldParser::getOpenFileName() const
{
    return m_openFileName;
}

BldParser::AttribMap BldParser::getAttributs(std::ifstream& file)
{
    BldParser::AttribMap fileMap;
    string buffer;

    while(tools::getline(file, buffer))
    {
        if(buffer.empty())
            break;

        if(buffer[0] == '#')
            continue;

        string key(buffer, 0, buffer.find_first_of('=')),
                value(buffer, buffer.find_first_of('=') + 1);

        fileMap[key] = value;
    }

    return fileMap;
}

void BldParser::onLoadMap(AttribMap& att)
{
    m_gameManager->map.name = att["name"];
    m_sceneManager->setAmbientLight(tools::strToVec4<float>(att["ambient"], true));
}

void BldParser::onLoadMusic(AttribMap& att)
{
    string& filePath = att["filePath"];
    m_gameManager->map.musicPath = filePath;

    if(!m_gameManager->manager.app->globalSettings.noaudio
       && !m_gameManager->manager.app->globalSettings.nomusic)
    {
        FMOD_RESULT res = FMOD_System_CreateStream(m_fmodsys, filePath.c_str(),
                                                   FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE,
                                                   0, &m_gameManager->map.musicStream);

        if(res != FMOD_OK)
            throw Exception("BldParser::onLoadMusic; %s (%s)",
                            FMOD_ErrorString(res), filePath.c_str());
    }
}

void BldParser::onLoadFog(AttribMap& att)
{
    Vector4f color = tools::strToVec4<float>(att["color"], true);
    float start = tools::strToNum<float>(att["start"]);
    float end = tools::strToNum<float>(att["end"]);

    scene::Fog* fog = m_sceneManager->getFog();
    fog->setColor(color);
    fog->setStart(start);
    fog->setEnd(end);
    fog->setEnable(true);
}

void BldParser::onLoadSkyBox(AttribMap& att)
{
    string skyPath[6] = {
        att["front"], att["back"],
        att["top"], att["bottom"],
        att["left"], att["right"]
    };

    Texture skyTex[6];

    for(unsigned i = 0; i < 6; i++)
        if(!skyPath[i].empty())
            skyTex[i] = skyPath[i];

    scene::SkyBox * sky = m_sceneManager->getSkybox();
    sky->setTextures(skyTex);
    sky->setEnable(true);
}

void BldParser::onLoadNode(AttribMap& att)
{
    Node* node = 0;

    if(att["type"] == "ITEM")
        node = createItem(att["add"], att["matrix"]);

    else if(att["type"] == "DYNAMIC")
        node = createDynamic(att["action"], att["matrix"]);

    else if(att["type"] == "STATIC")
        node = createStatic(att["modelPath"], att["matrix"]);

    else if(att["type"] == "SPAWN")
        m_gameManager->map.spawnPoints.push_back(tools::strToVec3<float>(att["pos"], true));

    else
        throw tbe::Exception("BldParser::onLoadNode; Unknown node type (%s)", att["type"].c_str());

    if(node)
        m_gameManager->manager.scene->getRootNode()->addChild(node);
}

void BldParser::onLoadLight(AttribMap& att)
{
    Light* light = NULL;

    if(att["type"] == "DIRI")
    {
        light = new DiriLight(m_lightScene);
    }

    else if(att["type"] == "POINT")
    {
        float radius = tools::strToNum<float>(att["radius"]);

        light = new PointLight(m_lightScene);
        light->setRadius(radius);
    }

    else
        throw tbe::Exception("BldParser::LoadScene; Unknown light type (%s)", att["type"].c_str());

    light->setPos(tools::strToVec3<float>(att["pos"], true));

    light->setAmbient(tools::strToVec4<float>(att["ambient"], true));
    light->setDiffuse(tools::strToVec4<float>(att["diffuse"], true));
    light->setSpecular(tools::strToVec4<float>(att["specular"], true));

    m_gameManager->manager.scene->getRootNode()->addChild(light);
    m_gameManager->map.lights.push_back(light);
}

Item* BldParser::createItem(std::string add, tbe::Matrix4f pos)
{
    Item* item;

    if(add == "AMMO")
        item = new ItemAddAmmo(m_gameManager, pos);

    else if(add == "LIFE")
        item = new ItemAddLife(m_gameManager, pos);

    else if(add == "FATALSHOT")
        item = new ItemFatalShot(m_gameManager, pos);

    else if(add == "SUPERLIFE")
        item = new ItemSuperLife(m_gameManager, pos);

    else if(add == "FINDER")
        item = new ItemAddFinder(m_gameManager, pos);

    else if(add == "BOMB")
        item = new ItemAddBomb(m_gameManager, pos);

    else if(add == "SHOTGUN")
        item = new ItemAddShotgun(m_gameManager, pos);

    else
        throw tbe::Exception("LevelLoader::onLoadNode; Unknown item add (%s)", add.c_str());

    m_gameManager->registerItem(item);

    return item;
}

DynamicObject* BldParser::createDynamic(std::string action, tbe::Matrix4f pos)
{
    DynamicObject* dyo;

    if(action == "JUMPER")
        dyo = new DYJumper(m_gameManager, pos);

    else if(action == "TELEPORTER")
        dyo = new DYTeleporter(m_gameManager, pos);

    else
        throw tbe::Exception("LevelLoader::onLoadNode; Unknown dynamic action (%s)", action.c_str());

    m_gameManager->registerDynamic(dyo);

    return dyo;
}

StaticObject* BldParser::createStatic(std::string modelPath, tbe::Matrix4f pos)
{
    StaticObject* node = new StaticObject(m_gameManager, modelPath, pos);

    m_gameManager->registerStatic(node);

    return node;
}
