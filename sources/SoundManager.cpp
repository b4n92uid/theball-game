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

    const Settings& gs = gameManager->manager.app->globalSettings;

    soundPaths["hit"] = gs("audio.hit");
    soundPaths["kill"] = gs("audio.kill");
    soundPaths["noAvailable"] = gs("audio.noAvailable");
    soundPaths["flash"] = gs("audio.flash");

    for(map<string, string>::iterator it = soundPaths.begin(); it != soundPaths.end(); it++)
        registerSound(it->first, it->second);

    if(!m_gameManager->manager.app->globalSettings.noaudio)
    {
        FMOD_CHANNELGROUP* masterGroupe;
        FMOD_System_GetMasterChannelGroup(m_fmodsys, &masterGroupe);
        FMOD_ChannelGroup_SetVolume(masterGroupe, 1.00);

        FMOD_System_CreateChannelGroup(m_fmodsys, "musicGroupe", &m_musicGroupe);
        FMOD_ChannelGroup_SetVolume(m_musicGroupe, 0.60);
    }
}

SoundManager::~SoundManager()
{

    foreach(SfxMap::value_type it, m_musics)
    {
        FMOD_Channel_Stop(it.second.second);
        FMOD_Sound_Release(it.second.first);
    }

    foreach(SfxMap::value_type it, m_sounds)
    {
        FMOD_Channel_Stop(it.second.second);
        FMOD_Sound_Release(it.second.first);
    }
}

void SoundManager::registerSound(std::string name, std::string filename)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return;

    if(m_sounds.count(name))
        return;

    cout << "[Sound] " << filename << endl;

    FMOD_RESULT res = FMOD_System_CreateSound(m_fmodsys, filename.c_str(),
                                              FMOD_LOOP_NORMAL | FMOD_3D | FMOD_HARDWARE,
                                              0, &m_sounds[name].first);

    if(res != FMOD_OK)
        throw tbe::Exception("SoundManager::registerSound; %1% (%2%)")
        % FMOD_ErrorString(res)
        % filename.c_str();

    FMOD_Sound_Set3DMinMaxDistance(m_sounds[name].first, 8, 128);
}

void SoundManager::stopSound(std::string soundName)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return;

    FMOD_Channel_Stop(m_sounds[soundName].second);
}

void SoundManager::syncronizeSoundsPosition()
{

    foreach(SoundPosMap::value_type & i, m_soundPosMap)
    {
        MapElement* object = i.first;

        FMOD_Channel_Set3DAttributes(i.second,
                                     (FMOD_VECTOR*) (float*) object->getPhysicBody()->getPos(),
                                     (FMOD_VECTOR*) (float*) object->getPhysicBody()->getVelocity());
    }
}

void SoundManager::volSound(std::string soundName, float vol)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return;

    FMOD_Channel_SetVolume(m_sounds[soundName].second, vol);
}

bool SoundManager::isPlaySound(std::string soundName)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return false;

    int isplay = false;
    FMOD_Channel_IsPlaying(m_sounds[soundName].second, &isplay);

    return isplay;
}

void SoundManager::playSound(std::string soundName, MapElement* object, int loop, float vol)
{
    if(m_gameManager->manager.app->globalSettings.noaudio)
        return;

    FMOD_Sound_SetLoopCount(m_sounds[soundName].first, loop);

    FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE,
                          m_sounds[soundName].first, true,
                          &m_sounds[soundName].second);

    if(object)
        m_soundPosMap[object] = m_sounds[soundName].second;

    processSoundEffect(m_sounds[soundName].second);

    FMOD_Channel_SetPaused(m_sounds[soundName].second, false);
    FMOD_Channel_SetVolume(m_sounds[soundName].second, vol);
}

void SoundManager::unregisterElement(MapElement* object)
{
    m_soundPosMap.erase(object);
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
        throw tbe::Exception("SoundManager::registerMusic; %1% (%2%)")
        % FMOD_ErrorString(res)
        % filename.c_str();
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
