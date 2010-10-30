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

    #ifndef THEBALL_DISABLE_SOUND
    map<string, string> soundPaths;

    soundPaths["appear"] = "data/sfxart/appear.wav";

    soundPaths["boost"] = "data/sfxart/boost.wav";
    soundPaths["bullettime"] = "data/sfxart/bullettime.wav";

    soundPaths["hit"] = "data/sfxart/hit.wav";
    soundPaths["kill"] = "data/sfxart/kill.wav";

    soundPaths["land"] = "data/sfxart/land.wav";
    soundPaths["contact"] = "data/sfxart/contact.wav";

    soundPaths["takeammo"] = "data/sfxart/takeammo.wav";
    soundPaths["takelife"] = "data/sfxart/takelife.wav";
    soundPaths["takefatalshot"] = "data/sfxart/takefatalshot.wav";
    soundPaths["takesuperlife"] = "data/sfxart/takesuperlife.wav";

    soundPaths["jumper"] = "data/sfxart/jumper.wav";
    soundPaths["teleporter"] = "data/sfxart/teleporter.wav";

    soundPaths["notime"] = "data/sfxart/notime.wav";
    soundPaths["noAvailable"] = "data/sfxart/noAvailable.wav";

    for(map<string, string>::iterator it = soundPaths.begin(); it != soundPaths.end(); it++)
    {
        FMOD_RESULT res = FMOD_System_CreateSound(m_fmodsys, it->second.c_str(),
                                                  FMOD_LOOP_OFF | FMOD_3D | FMOD_HARDWARE,
                                                  0, &m_sounds[it->first]);

        if(res != FMOD_OK)
            throw tbe::Exception("SoundManager::SoundManager; %s (%s)",
                                 FMOD_ErrorString(res), it->second.c_str());
        else
            FMOD_Sound_Set3DMinMaxDistance(m_sounds[it->first], SOUND_MIN_DIST, SOUND_MAX_DIST);
    }
    #endif
}

void SoundManager::Play(std::string soundName, Object* node)
{
    #ifndef THEBALL_DISABLE_SOUND
    FMOD_CHANNEL* channel;

    FMOD_System_PlaySound(m_fmodsys, FMOD_CHANNEL_FREE, m_sounds[soundName], true, &channel);

    FMOD_Channel_Set3DAttributes(channel,
                                 (FMOD_VECTOR*)(float*)node->NewtonNode::GetPos(),
                                 (FMOD_VECTOR*)(float*)node->NewtonNode::GetVelocity());

    FMOD_Channel_SetPaused(channel, false);
    #endif
}
