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
#include "MapElement.h"

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
        registerSound(it->first, it->second);

    if(!m_gameManager->manager.app->globalSettings.noaudio)
    {
        FMOD_CHANNELGROUP* masterGroupe;
        FMOD_System_GetMasterChannelGroup(m_fmodsys, &masterGroupe);
        FMOD_ChannelGroup_SetVolume(masterGroupe, 0.75);

        FMOD_System_CreateChannelGroup(m_fmodsys, "musicGroupe", &m_musicGroupe);
        FMOD_ChannelGroup_SetVolume(m_musicGroupe, 1.00);
    }
}

SoundManager::~SoundManager()
{
}

void SoundManager::registerSound(std::string name, std::string filename)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return;

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
        throw tbe::Exception("SoundManager::registerSound; %s (%s)",
                             FMOD_ErrorString(res), filename.c_str());

    FMOD_Sound_Set3DMinMaxDistance(m_sounds[name], 8, 128);
}

void SoundManager::playSound(std::string soundName, MapElement* object)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return;

    FMOD_CHANNEL* channel;

    FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_sounds[soundName], true, &channel);

    FMOD_Channel_Set3DAttributes(channel,
                                 (FMOD_VECTOR*)(float*)object->getPhysicBody()->getPos(),
                                 (FMOD_VECTOR*)(float*)object->getPhysicBody()->getVelocity());


    processSoundEffect(channel);

    FMOD_Channel_SetPaused(channel, false);
}

void SoundManager::registerMusic(std::string name, std::string filename)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return;

    if(m_sounds.count(name))
    {
        cout << "Load shared music : " << filename << endl;
        return;
    }

    cout << "Load music : " << filename << endl;

    FMOD_RESULT res = FMOD_System_CreateStream(m_fmodsys, filename.c_str(),
                                               FMOD_LOOP_NORMAL | FMOD_HARDWARE,
                                               0, &m_musics[name].first);

    if(res != FMOD_OK)
        throw tbe::Exception("SoundManager::registerMusic; %s (%s)",
                             FMOD_ErrorString(res), filename.c_str());
}

void SoundManager::playMusic(std::string soundName)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return;

    FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_musics[soundName].first,
                          false, &m_musics[soundName].second);

    FMOD_Channel_SetChannelGroup(m_musics[soundName].second, m_musicGroupe);
}

void SoundManager::pauseMusic(std::string soundName)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return;

    FMOD_Channel_SetPaused(m_musics[soundName].second, true);
}

void SoundManager::stopMusic(std::string soundName)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return;

    FMOD_Channel_Stop(m_musics[soundName].second);
}
