/* 
 * File:   PlaySoundManager.cpp
 * Author: b4n92uid
 * 
 * Created on 31 octobre 2010, 13:08
 */

#include "PlaySoundManager.h"
#include "PlayManager.h"

PlaySoundManager::PlaySoundManager(PlayManager* playManager) : SoundManager(playManager)
{
    m_playManager = playManager;
}

PlaySoundManager::~PlaySoundManager()
{
}

void PlaySoundManager::ProcessEffect(std::string soundName, FMOD_CHANNEL* channel)
{
    #ifndef THEBALL_NO_AUDIO
    if(m_playManager->GetBullettime()->IsActive())
        FMOD_Channel_SetFrequency(channel, 22050);
    #endif
}
