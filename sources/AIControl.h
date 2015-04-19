/*
 * File:   AIManager.h
 * Author: b4n92uid
 *
 * Created on 16 d�cembre 2009, 18:45
 */

#ifndef _AIMANAGER_H
#define	_AIMANAGER_H

#include <Tbe.h>

#include "Controller.h"

class GameManager;
class Player;

class AIControl : public Controller
{
public:
    AIControl(GameManager* playManager);
    ~AIControl();

    void process(Player* player);

    boost::signals2::signal<void(Player*) > onAi;
};

#endif	/* _AIMANAGER_H */

