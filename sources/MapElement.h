/*
 * File:   MapElement.h
 * Author: b4n92uid
 *
 * Created on 26 mai 2011, 22:53
 */

#ifndef MAPELEMENT_H
#define	MAPELEMENT_H

#include <map>
#include <vector>
#include <string>

#include <NewtonBall/NewtonBall.h>

#include "Settings.h"

class SoundManager;
class GameManager;

class MapElement
{
public:
    MapElement(GameManager* gameManager);
    virtual ~MapElement();

    void setVisualBody(tbe::scene::Node* visualBody);
    tbe::scene::Node* getVisualBody() const;

    void setPhysicBody(tbe::scene::NewtonNode* physicBody);
    tbe::scene::NewtonNode* getPhysicBody() const;

    void setId(std::string id);
    std::string getId() const;

    void resetInitialMatrix();

    void stopMotion();

    GameManager* getGameManager() const;

    typedef std::map<std::string, MapElement*> Map;
    typedef std::vector<MapElement*> Array;

    static void applyForceAndTorqueCallback(const NewtonBody* body, float, int);
    static void applyTransformCallback(const NewtonBody* body, const float*, int);

protected:
    GameManager* m_gameManager;
    SoundManager* m_soundManager;
    Settings::World m_worldSettings;

    tbe::scene::NewtonNode* m_physicBody;
    tbe::scene::Node* m_visualBody;

    tbe::Matrix4 m_initialMatrix;

    std::string m_id;
};

class AreaElement : public MapElement
{
public:
    AreaElement(GameManager*, std::string, tbe::Vector3f, float);

    void setRadius(float radius);
    float getRadius() const;

    typedef std::vector<AreaElement*> Array;

protected:
    float m_radius;
};

#endif	/* MAPELEMENT_H */

