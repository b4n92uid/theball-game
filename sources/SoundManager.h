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
#include <boost/signal.hpp>

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
    void playSound(std::string soundName, MapElement* object = NULL, int loop = 0, float vol = 1.0);
    void stopSound(std::string soundName);

    void playMusic(std::string soundName);
    void pauseMusic(std::string soundName);
    void stopMusic(std::string soundName);


    boost::signal<void (FMOD_CHANNEL*) > processSoundEffect;

    void syncronizeSoundsPosition();

protected:
    typedef std::map<MapElement*, FMOD_CHANNEL*> SoundPosMap;
    typedef std::map<std::string, std::pair<FMOD_SOUND*, FMOD_CHANNEL*> > SfxMap;

    SoundPosMap m_soundPosMap;
    GameManager* m_gameManager;
    FMOD_SYSTEM* m_fmodsys;
    FMOD_CHANNELGROUP* m_musicGroupe;
    SfxMap m_sounds;
    SfxMap m_musics;
};

#endif	/* _SOUNDMANAGER_H */

