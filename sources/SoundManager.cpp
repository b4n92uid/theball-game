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

using namespace std;
using namespace tbe;
using namespace tbe::scene;

SoundManager::SoundManager(GameManager* gameManager)
{
    m_gameManager = gameManager;

    #ifndef THE_BALL_DISABLE_SOUND
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
        m_sounds[it->first] = FSOUND_Sample_Load(FSOUND_FREE, it->second.c_str(), FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);

        if(!m_sounds[it->first])
            throw tbe::Exception("SoundManager::SoundManager; %s (%s)",
                                 FMOD_ErrorString(FSOUND_GetError()), it->first.c_str());
        else
            FSOUND_Sample_SetMinMaxDistance(m_sounds[it->first], SOUND_MIN_DIST, SOUND_MAX_DIST);
    }
    #endif
}

void SoundManager::Play(std::string soundName, Node* node)
{
    #ifndef THE_BALL_DISABLE_SOUND
    int channel = FSOUND_PlaySound(FSOUND_FREE, m_sounds[soundName]);
    FSOUND_3D_SetAttributes(channel, node->GetMatrix().GetPos(), 0);
    #endif
}
