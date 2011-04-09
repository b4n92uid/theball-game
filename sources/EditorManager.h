/*
 * File:   EditorManager.h
 * Author: b4n92uid
 *
 * Created on 30 décembre 2009, 20:26
 */

#ifndef _EDITORMANAGER_H
#define	_EDITORMANAGER_H

#include <iostream>

#include "GameManager.h"

class AppManager;
class BldParser;

class EditorManager : public GameManager
{
public:
    EditorManager(AppManager* appManager);
    virtual ~EditorManager();

    /// Construit la map et les entités
    void setupMap(const Settings::EditSetting& editSetting);

    /// Construction de l'ETH
    void setupGui();

    /// Traitement de l'entrer
    void eventProcess();

    /// Traitement ETH
    void hudProcess();

    /// Rendue
    void render();

protected:
    bool allocEntityEvent(tbe::EventManager* event);
    bool deleteEntityEvent(tbe::EventManager* event);
    bool settingEntityEvent(tbe::EventManager* event);
    bool selectEntityEvent(tbe::EventManager* event);

    void newEntity(Object* ent);

    bool allocLightEvent(tbe::EventManager* event);
    bool deleteLightEvent(tbe::EventManager* event);
    bool settingLightEvent(tbe::EventManager* event);

    void newLight(tbe::scene::Light* light);
    void selectLight(unsigned index);
    void deleteLight(unsigned index);

    bool settingSkyEvent(tbe::EventManager* event);
    bool settingFogEvent(tbe::EventManager* event);
    bool settingMusicEvent(tbe::EventManager* event);

    void viewEventPorcess(tbe::EventManager* event);
    void editEventPorcess(tbe::EventManager* event);
    void guiEventPorcess(tbe::EventManager* event);

protected:

    enum Navigation
    {
        ENTITY,
        LIGHT,
        SKYBOX,
        FOG,
        MUSIC,

    } m_navigation;

    enum TimeTo
    {
        VIEW,
        EDIT,
        PAUSE,

    } m_timeTo;

    struct
    {
        tbe::ppe::BloomEffect* bloom;
        //tbe::ppe::BlurEffect* blur;

    } ppe;

    struct
    {

        struct
        {
            tbe::gui::Button* quit;
            tbe::gui::Button* save;
            tbe::gui::Button* ret;
            tbe::gui::EditBox* name;
            tbe::gui::TextBox* status;
            tbe::ticks::Clock statusClock;
            bool confirm;

        } pause;

        struct
        {
            tbe::gui::ListBox* list;
            tbe::gui::TextBox* info;

        } entity;

        struct
        {
            tbe::gui::ListBox* list;
            tbe::gui::TextBox* info;

        } music;

        struct
        {
            tbe::gui::VectorBox* amibent;
            tbe::gui::VectorBox* diffuse;
            tbe::gui::VectorBox* specular;
            tbe::gui::SwitchNumeric<float>* radius;
            tbe::gui::SwitchString* slector;
            tbe::gui::SwitchString* type;
            tbe::gui::TextBox* info;

        } light;

        struct
        {
            tbe::gui::VectorBox* color;
            tbe::gui::SwitchNumeric<float>* start;
            tbe::gui::SwitchNumeric<float>* end;
            tbe::gui::SwitchString* enable;
            tbe::gui::TextBox* info;

        } fog;

        struct
        {
            tbe::gui::ListBox* list;
            tbe::gui::SwitchString* face;
            tbe::gui::Button* apply;
            tbe::gui::Button* clear;
            tbe::gui::SwitchString* enable;
            tbe::gui::TextBox* info;

        } sky;

    } hud;

    Object::Array m_visualSpawnPoints;

    tbe::scene::FreeFlyCamera* m_FFCamera;
    tbe::scene::OrbitalCamera* m_OCamera;
    tbe::scene::UserCamera* m_camera;

    tbe::Vector3f m_3dSelect;
    tbe::scene::Mesh* m_axes;
    Object* m_selectedNode;
    tbe::scene::Light* m_selectedLight;

    Object::Array m_allEntity;

    tbe::scene::SkyBox* m_sky;
    tbe::scene::Fog* m_fog;

    Settings::EditSetting m_editSetting;

    bool m_completeHelp;
};

#endif	/* _EDITORMANAGER_H */

