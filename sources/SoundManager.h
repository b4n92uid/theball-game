/* 
 * File:   SoundManager.h
 * Author: b4n92uid
 *
 * Created on 10 novembre 2009, 15:04
 */

#ifndef _SOUNDMANAGER_H
#define	_SOUNDMANAGER_H

#include <map>
#include <fmod.h>
#include <Tbe.h>

class GameManager;

class SoundManager
{
public:

    /// Constructeur
    SoundManager(GameManager* gameManager);

    /// Jouer un son
    void Play(std::string soundName, tbe::scene::Node* object);
    
protected:
    std::map<std::string, FSOUND_SAMPLE*> m_sounds;
    GameManager* m_gameManager;
};

#endif	/* _SOUNDMANAGER_H */

