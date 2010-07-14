/* 
 * File:   SoundManager.cpp
 * Author: b4n92uid
 * 
 * Created on 10 novembre 2009, 15:04
 */

#include "SoundManager.h"
#include "Player.h"
#include "Define.h"
#include "GameManager.h"
#include <Tbe.h>

using namespace tbe;
using namespace tbe::scene;

SoundManager::SoundManager(GameManager* gameManager)
{
    m_gameManager = gameManager;

    m_sounds["appear"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/appear.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);

    m_sounds["boost"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/boost.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);
    m_sounds["bullettime"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/bullettime.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);

    m_sounds["hit"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/hit.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);
    m_sounds["kill"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/kill.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);

    m_sounds["land"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/land.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);
    m_sounds["contact"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/contact.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);

    m_sounds["takeammo"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/takeammo.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);
    m_sounds["takelife"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/takelife.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);
    m_sounds["takefatalshot"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/takefatalshot.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);
    m_sounds["takesuperlife"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/takesuperlife.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);

    m_sounds["jumper"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/jumper.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);
    m_sounds["teleporter"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/teleporter.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);

    m_sounds["notime"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/notime.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);
    m_sounds["noAvailable"] = FSOUND_Sample_Load(FSOUND_FREE, "data/sfxart/noAvailable.wav", FSOUND_HW3D | FSOUND_LOOP_OFF, 0, 0);

    for(std::map<std::string, FSOUND_SAMPLE*>::iterator i = m_sounds.begin(); i != m_sounds.end(); i++)
        if(!i->second)
            throw tbe::Exception("SoundManager::SoundManager\nSound load error (%s)\n\n", i->first.c_str());
        else
            FSOUND_Sample_SetMinMaxDistance(i->second, SOUND_MIN_DIST, SOUND_MAX_DIST);
}

void SoundManager::Play(std::string soundName, Node* node)
{
    #ifndef DISABLE_SOUND
    int channel = FSOUND_PlaySound(FSOUND_FREE, m_sounds[soundName]);
    FSOUND_3D_SetAttributes(channel, node->GetMatrix().GetPos(), 0);
    #endif
}
