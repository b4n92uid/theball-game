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

class Object;
class GameManager;

class SoundManager
{
public:

    SoundManager(GameManager* gameManager);
    virtual ~SoundManager();

    /// Chargement d'un son par le gestionnaire audio
    void registerSound(std::string name, std::string filename);

    /// Jouer un son
    void play(std::string soundName, Object* object);

    typedef std::map<std::string, FMOD_SOUND*> SoundMap;

protected:
    virtual void processEffect(std::string soundName, FMOD_CHANNEL* channel);

protected:
    GameManager* m_gameManager;
    FMOD_SYSTEM* m_fmodsys;
    SoundMap m_sounds;
};

#endif	/* _SOUNDMANAGER_H */

