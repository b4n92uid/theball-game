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
#include <boost/signals2/signal.hpp>

#include "Settings.h"

class GameManager;
class Player;

class Controller
{
public:
    Controller(GameManager* playManager);
    virtual ~Controller();

    virtual void process(Player* player) = 0;

protected:
    GameManager* m_playManager;
    Settings::World& m_worldSettings;
};

#endif	/* _CONTROLLER_H */
