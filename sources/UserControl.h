/* 
 * File:   UserEventManager.h
 * Author: b4n92uid
 *
 * Created on 16 décembre 2009, 21:43
 */

#ifndef _USEREVENTMANAGER_H
#define	_USEREVENTMANAGER_H

#include "Controller.h"

class AppManager;
class PlayManager;
class Player;

class UserControl : public Controller
{
public:
    UserControl(PlayManager* playManager);
    ~UserControl();

    void Process(Player* player);

    bool IsActionJustDown(std::string name);
    bool IsActionJustUp(std::string name);

    void SetActionStateDown(std::string name, bool state);
    bool IsActionStateDown(std::string name);

private:
    Settings::Control m_control;
    tbe::EventManager* m_eventManager;
};

#endif	/* _USEREVENTMANAGER_H */

