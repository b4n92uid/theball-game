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

class MapElement;
class GameManager;

class SoundManager
{
public:

    SoundManager(GameManager* gameManager);
    virtual ~SoundManager();

    /// Chargement d'une musique par le gestionnaire audio
    void registerMusic(std::string name, std::string filename);

    /// Chargement d'un son par le gestionnaire audio
    void registerSound(std::string name, std::string filename);

    /// Jouer un son
    void playSound(std::string soundName, MapElement* object);

    void playMusic(std::string soundName);
    void pauseMusic(std::string soundName);
    void stopMusic(std::string soundName);

    typedef std::map<std::string, FMOD_SOUND*> SoundMap;

    typedef std::map<std::string, std::pair<FMOD_SOUND*, FMOD_CHANNEL*> > MusicMap;

protected:
    virtual void processEffect(std::string soundName, FMOD_CHANNEL* channel);

protected:
    GameManager* m_gameManager;
    FMOD_SYSTEM* m_fmodsys;
    FMOD_CHANNELGROUP* m_musicGroupe;
    SoundMap m_sounds;
    MusicMap m_musics;
};

#endif	/* _SOUNDMANAGER_H */

