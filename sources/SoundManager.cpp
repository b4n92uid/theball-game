/* 
 * File:   SoundManager.cpp
 * Author: b4n92uid
 * 
 * Created on 10 novembre 2009, 15:04
 */

#include "SoundManager.h"

#include "GameManager.h"
#include "Player.h"
#include "Define.h"
#include "StaticObject.h"

#include <fmod_errors.h>

using namespace std;
using namespace tbe;
using namespace tbe::scene;

SoundManager::SoundManager(GameManager* gameManager)
{
    m_gameManager = gameManager;

    m_fmodsys = m_gameManager->manager.fmodsys;

    map<string, string> soundPaths;

    soundPaths["boost"] = "data/sfxart/boost.wav";
    soundPaths["stop"] = "data/sfxart/stop.wav";
    soundPaths["bullettime"] = "data/sfxart/bullettime.wav";
    soundPaths["hit"] = "data/sfxart/hit.wav";
    soundPaths["jumper"] = "data/sfxart/jumper.wav";
    soundPaths["kill"] = "data/sfxart/kill.wav";
    soundPaths["noAvailable"] = "data/sfxart/noAvailable.wav";
    soundPaths["notime"] = "data/sfxart/notime.wav";
    soundPaths["respawn"] = "data/sfxart/respawn.wav";
    soundPaths["takeammo"] = "data/sfxart/takeammo.wav";
    soundPaths["takefatalshot"] = "data/sfxart/takefatalshot.wav";
    soundPaths["takelife"] = "data/sfxart/takelife.wav";
    soundPaths["takesuperlife"] = "data/sfxart/takesuperlife.wav";
    soundPaths["teleport"] = "data/sfxart/teleport.wav";

    for(map<string, string>::iterator it = soundPaths.begin(); it != soundPaths.end(); it++)
        RegisterSound(it->first, it->second);
}

SoundManager::~SoundManager()
{
}

void SoundManager::RegisterSound(std::string name, std::string filename)
{
    if(!m_gameManager->manager.app->globalSettings.noaudio)
    {
        if(m_sounds.count(name))
        {
            cout << "Load shared sound : " << filename << endl;
            return;
        }

        cout << "Load sound : " << filename << endl;

        FMOD_RESULT res = FMOD_System_CreateSound(m_fmodsys, filename.c_str(),
                                                  FMOD_LOOP_OFF | FMOD_3D | FMOD_HARDWARE,
                                                  0, &m_sounds[name]);

        if(res != FMOD_OK)
            throw tbe::Exception("SoundManager::RegisterSound; %s (%s)",
                                 FMOD_ErrorString(res), filename.c_str());
        else
            FMOD_Sound_Set3DMinMaxDistance(m_sounds[name], 8, 128);
    }
}

void SoundManager::Play(std::string soundName, Object* object)
{
    #if !defined(THEBALL_DISABLE_SOUND) && !defined(THEBALL_NO_AUDIO)
    FMOD_CHANNEL* channel;

    FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_sounds[soundName], true, &channel);

    FMOD_Channel_Set3DAttributes(channel,
                                 (FMOD_VECTOR*)(float*)object->GetPos(),
                                 (FMOD_VECTOR*)(float*)object->GetPhysicBody()->GetVelocity());

    ProcessEffect(soundName, channel);

    FMOD_Channel_SetPaused(channel, false);
    #endif
}

void SoundManager::ProcessEffect(std::string soundName, FMOD_CHANNEL* channel)
{

}
