/*
 * File:   AIManager.h
 * Author: b4n92uid
 *
 * Created on 16 décembre 2009, 18:45
 */

#ifndef _AIMANAGER_H
#define	_AIMANAGER_H

#include <Tbe.h>

#include "Settings.h"
#include "Controller.h"
#include "Player.h"

class GameManager;
class Player;
class Item;

class AIControl : public Controller
{
public:
    AIControl(GameManager* playManager);
    ~AIControl();

    void process(Player* player);

    typedef std::map<Player*, AIControl*> Map;
};

#endif	/* _AIMANAGER_H */

