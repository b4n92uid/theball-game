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

    void saveLevel();
    void saveLevel(const std::string& filepath);
    void loadLevel(const std::string& filepath);

    void setOpenFileName(std::string openFileName);
    std::string getOpenFileName() const;

    bool isChanged();

protected:
    void onLoadMap(AttribMap& att);
    void onLoadMusic(AttribMap& att);
    void onLoadFog(AttribMap& att);
    void onLoadSkyBox(AttribMap& att);
    void onLoadNode(AttribMap& att);
    void onLoadLight(AttribMap& att);

    Item* createItem(std::string add, tbe::Matrix4f pos);
    StaticObject* createStatic(std::string modelPath, tbe::Matrix4f pos);
    DynamicObject* createDynamic(std::string action, tbe::Matrix4f pos);
    Object* createSpawn(tbe::Vector3f pos);

protected:
    GameManager* m_gameManager;
    MaterialManager* m_materialManager;

    std::string m_openFileName;

    FMOD_SYSTEM* m_fmodsys;

    tbe::scene::SceneManager* m_sceneManager;
    tbe::scene::MeshParallelScene* m_meshScene;
    tbe::scene::LightParallelScene* m_lightScene;
    tbe::scene::NewtonParallelScene* m_newtonScene;

private:
    void writeMap(std::iostream& stream);

    static BldParser::AttribMap getAttributs(std::ifstream& file);
};

#endif	/* _BLDPARSER_H */
