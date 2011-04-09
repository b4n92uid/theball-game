/* 
 * File:   PlaySoundManager.h
 * Author: b4n92uid
 *
 * Created on 31 octobre 2010, 13:08
 */

#ifndef PLAYSOUNDMANAGER_H
#define	PLAYSOUNDMANAGER_H

#include "SoundManager.h"

class PlayManager;

class PlaySoundManager : public SoundManager
{
public:
    PlaySoundManager(PlayManager* playManager);
    virtual ~PlaySoundManager();

private:
    void processEffect(std::string soundName, FMOD_CHANNEL* channel);

private:
    PlayManager* m_playManager;
};

#endif	/* PLAYSOUNDMANAGER_H */

