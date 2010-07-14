/* 
 * File:   Controller.h
 * Author: b4n92uid
 *
 * Created on 15 juin 2010, 14:54
 */

#ifndef _CONTROLLER_H
#define	_CONTROLLER_H

#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

#include "AppManager.h"

class PlayManager;
class Player;

class Controller
{
public:
    Controller(PlayManager* playManager);
    virtual ~Controller();

    virtual void Process(Player* player) = 0;

protected:
    PlayManager* m_playManager;
    Settings::Physics m_worldSettings;
};

#endif	/* _CONTROLLER_H */
