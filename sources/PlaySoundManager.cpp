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

void PlaySoundManager::processEffect(std::string soundName, FMOD_CHANNEL* channel)
{
    if(!m_gameManager->manager.app->globalSettings.noaudio)
        if(m_playManager->getBullettime()->isActive())
            FMOD_Channel_SetFrequency(channel, 22050);
}
