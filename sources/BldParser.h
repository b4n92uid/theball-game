/* 
 * File:   BldParser.h
 * Author: b4n92uid
 *
 * Created on 2 décembre 2009, 21:46
 */

#ifndef _BLDPARSER_H
#define	_BLDPARSER_H

#include <fmod.h>
#include <fstream>
#include <Tbe.h>

#include "Item.h"
#include "Player.h"
#include "StaticObject.h"
#include "DynamicObject.h"

class MaterialManager;
class PlayManager;

class BldParser
{
public:
    BldParser(GameManager* gameManager);
    virtual ~BldParser();

    typedef std::map<std::string, std::string> AttribMap;

    void SaveLevel();
    void SaveLevel(const std::string& filepath);
    void LoadLevel(const std::string& filepath);

    void SetOpenFileName(std::string openFileName);
    std::string GetOpenFileName() const;

protected:
    void OnLoadMap(AttribMap& att);
    void OnLoadMusic(AttribMap& att);
    void OnLoadFog(AttribMap& att);
    void OnLoadSkyBox(AttribMap& att);
    void OnLoadNode(AttribMap& att);
    void OnLoadLight(AttribMap& att);

    Item* CreateItem(std::string add, tbe::Matrix4f pos);
    StaticObject* CreateStatic(std::string modelPath, tbe::Matrix4f pos);
    DynamicObject* CreateDynamic(std::string action, tbe::Matrix4f pos);
    Object* CreateSpawn(tbe::Vector3f pos);

protected:
    GameManager* m_gameManager;
    MaterialManager* m_materialManager;

    std::string m_openFileName;

    FMOD_SYSTEM* m_fmodsys;

    tbe::scene::MeshParallelScene* m_meshScene;
    tbe::scene::SceneManager* m_sceneManager;
    tbe::scene::NewtonParallelScene* m_newtonScene;

private:
    static BldParser::AttribMap GetAttributs(std::ifstream& file);
};

#endif	/* _BLDPARSER_H */
