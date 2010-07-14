/* 
 * File:   BLDLoader.cpp
 * Author: b4n92uid
 * 
 * Created on 2 décembre 2009, 21:46
 */

#include <bits/stringfwd.h>

#include "BldParser.h"

#include "PlayManager.h"
#include "MaterialManager.h"
#include "UserControl.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

BldParser::BldParser(GameManager* gameManager)
{
    m_gameManager = gameManager;
    m_sceneManager = gameManager->manager.scene;
    m_materialManager = gameManager->manager.material;

    m_meshScene = gameManager->parallelscene.meshs;
    m_newtonScene = gameManager->parallelscene.newton;
}

BldParser::~BldParser()
{
}

void BldParser::SaveLevel()
{
    SaveLevel(m_openFileName);
}

void BldParser::SaveLevel(const std::string& filepath)
{
    ofstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("BLDLoader::SaveLevel; Open file error (%s)", filepath.c_str());

    m_openFileName = filepath;

    file << "# Ball level descriptor" << endl;
    file << "# Generated " << ticks::Clock::Date("%x - %X") << endl;
    file << endl;


    file << ".map" << endl;
    file << "name=" << m_gameManager->map.name << endl;
    file << "ambient=" << m_sceneManager->GetAmbientLight() << endl;
    file << endl;


    if(m_gameManager->map.musicStream)
    {
        file << ".music" << endl;
        file << "filePath=" << m_gameManager->map.musicPath << endl;
        file << endl;
    }

    Fog* fog = m_sceneManager->GetFog();

    if(fog->IsEnable())
    {
        file << ".fog" << endl;
        file << "color=" << fog->GetColor() << endl;
        file << "start=" << fog->GetStart() << endl;
        file << "end=" << fog->GetEnd() << endl;
        file << endl;
    }

    SkyBox* sky = m_sceneManager->GetSkybox();

    if(sky->IsEnable())
    {
        Texture* skyTexs = sky->GetTextures();

        file << ".skybox" << endl;
        file << "front=" << skyTexs[0].GetFilename() << endl;
        file << "back=" << skyTexs[1].GetFilename() << endl;
        file << "top=" << skyTexs[2].GetFilename() << endl;
        file << "bottom=" << skyTexs[3].GetFilename() << endl;
        file << "left=" << skyTexs[4].GetFilename() << endl;
        file << "right=" << skyTexs[5].GetFilename() << endl;
        file << endl;
    }

    file << "# Light" << endl;
    file << endl;

    for(unsigned i = 0; i < m_gameManager->map.lights.size(); i++)
    {
        Light* light = m_gameManager->map.lights[i];

        bool pointType = (light->GetType() == Light::POINT);

        file << "+light" << endl;
        file << "type=" << (pointType ? "POINT" : "DIRI") << endl;
        file << "pos=" << light->GetPos() << endl;
        file << "ambient=" << light->GetAmbient() << endl;
        file << "diffuse=" << light->GetDiffuse() << endl;
        file << "specular=" << light->GetSpecular() << endl;
        if(pointType)
            file << "radius=" << light->GetRadius() << endl;
        file << endl;
    }

    file << "# Static" << endl;
    file << endl;

    for(unsigned i = 0; i < m_gameManager->map.staticObjects.size(); i++)
        m_gameManager->map.staticObjects[i]->OutputConstruction(file);

    file << "# Dynamic" << endl;
    file << endl;

    for(unsigned i = 0; i < m_gameManager->map.dynamicObjects.size(); i++)
        m_gameManager->map.dynamicObjects[i]->OutputConstruction(file);

    file << "# Item" << endl;
    file << endl;

    for(unsigned i = 0; i < m_gameManager->map.items.size(); i++)
        m_gameManager->map.items[i]->OutputConstruction(file);

    file.close();
}

void BldParser::LoadLevel(const std::string& filepath)
{
    ifstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("BLDLoader::LoadScene\nOpen file error (%s)\n\n", filepath.c_str());

    m_openFileName = filepath;

    string buffer;
    while(getline(file, buffer))
    {
        if(buffer.empty() || buffer[0] == '#')
            continue;

        while(buffer[buffer.size() - 1] == ' ')
            buffer.erase(buffer.size() - 1);

        if(buffer == ".map")
        {
            AttribMap att = GetAttributs(file);
            OnLoadMap(att);
        }

        else if(buffer == ".music")
        {
            AttribMap att = GetAttributs(file);
            OnLoadMusic(att);
        }

        else if(buffer == ".fog")
        {
            AttribMap att = GetAttributs(file);
            OnLoadFog(att);
        }


        else if(buffer == ".skybox")
        {
            AttribMap att = GetAttributs(file);
            OnLoadSkyBox(att);
        }

        else if(buffer == "+node")
        {
            AttribMap att = GetAttributs(file);
            OnLoadNode(att);
        }

        else if(buffer == "+light")
        {
            AttribMap att = GetAttributs(file);
            OnLoadLight(att);
        }

        else
            throw tbe::Exception("BLDLoader::LoadScene\nParse error (%s)", buffer.c_str());
    }

    file.close();
}

void BldParser::SetOpenFileName(std::string openFileName)
{
    this->m_openFileName = openFileName;
}

std::string BldParser::GetOpenFileName() const
{
    return m_openFileName;
}

BldParser::AttribMap BldParser::GetAttributs(std::ifstream& file)
{
    BldParser::AttribMap fileMap;
    string buffer;

    while(getline(file, buffer))
    {
        if(buffer.empty())
            break;

        if(buffer[0] == '#')
            continue;

        while(buffer[buffer.size() - 1] == ' ')
            buffer.erase(buffer.size() - 1);

        string key(buffer, 0, buffer.find_first_of('=')),
                value(buffer, buffer.find_first_of('=') + 1);

        fileMap[key] = value;
    }

    return fileMap;
}

void BldParser::OnLoadMap(AttribMap& att)
{
    m_gameManager->map.name = att["name"];
    m_sceneManager->SetAmbientLight(vec34(Vector3f(att["ambient"])));
}

void BldParser::OnLoadMusic(AttribMap& att)
{
    string& filePath = att["filePath"];
    m_gameManager->map.musicPath = filePath;
    m_gameManager->map.musicStream = FSOUND_Stream_Open(filePath.c_str(), FSOUND_LOOP_NORMAL, 0, 0);

    if(!m_gameManager->map.musicStream)
        throw Exception("BldParser::OnLoadMusic; Open file error (%s)", filePath.c_str());
}

void BldParser::OnLoadFog(AttribMap& att)
{
    Vector4f color = att["color"];
    float start = tools::StrToNum<float>(att["start"]);
    float end = tools::StrToNum<float>(att["end"]);

    scene::Fog* fog = m_sceneManager->GetFog();
    fog->SetColor(color);
    fog->SetStart(start);
    fog->SetEnd(end);
    fog->SetEnable(true);
}

void BldParser::OnLoadSkyBox(AttribMap& att)
{
    Texture skyTex[6] = {
        att["front"],
        att["back"],
        att["top"],
        att["bottom"],
        att["left"],
        att["right"]
    };

    scene::SkyBox* sky = m_sceneManager->GetSkybox();
    sky->SetTextures(skyTex);
    sky->SetEnable(true);
}

void BldParser::OnLoadNode(AttribMap& att)
{
    if(att["type"] == "ITEM")
        CreateItem(att["add"], att["matrix"]);

    else if(att["type"] == "DYNAMIC")
        CreateDynamic(att["action"], att["matrix"]);

    else if(att["type"] == "STATIC")
        CreateStatic(att["modelPath"], att["matrix"]);

    else
        throw tbe::Exception("BLDLoader::OnLoadNode; Unknown node type (%s)", att["type"].c_str());
}

void BldParser::OnLoadLight(AttribMap& att)
{
    Light* light = NULL;

    if(att["type"] == "DIRI")
    {
        light = new DiriLight;
    }

    else if(att["type"] == "POINT")
    {
        float radius = tools::StrToNum<float>(att["radius"]);

        light = new PointLight;
        light->SetRadius(radius);
    }

    else
        throw tbe::Exception("BLDLoader::LoadScene\nUnknown light type (%s)", att["type"].c_str());

    light->SetPos(att["pos"]);

    light->SetAmbient(att["ambient"]);
    light->SetDiffuse(att["diffuse"]);
    light->SetSpecular(att["specular"]);

    m_sceneManager->AddDynamicLight(tools::numToStr(light), light);
    m_gameManager->map.lights.push_back(light);
}

Item* BldParser::CreateItem(std::string add, tbe::Matrix4f pos)
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
        throw tbe::Exception("LevelLoader::OnLoadNode\nUnknown item add (%s)", add.c_str());

    m_gameManager->RegisterItem(item);

    return item;
}

DynamicObject* BldParser::CreateDynamic(std::string action, tbe::Matrix4f pos)
{
    DynamicObject* dyo;

    if(action == "JUMPER")
        dyo = new DYJumper(m_gameManager, pos);

    else if(action == "TELEPORTER")
        dyo = new DYTeleporter(m_gameManager, pos);

    else
        throw tbe::Exception("LevelLoader::OnLoadNode\nUnknown dynamic action (%s)", action.c_str());

    m_gameManager->RegisterDynamic(dyo);

    return dyo;
}

StaticObject* BldParser::CreateStatic(std::string modelPath, tbe::Matrix4f pos)
{
    StaticObject* node = new StaticObject(m_gameManager, modelPath, pos);

    m_gameManager->RegisterStatic(node);

    return node;
}
