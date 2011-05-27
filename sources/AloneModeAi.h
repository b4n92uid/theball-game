/* 
 * File:   AloneModeAi.h
 * Author: b4n92uid
 *
 * Created on 15 juin 2010, 17:25
 */

#ifndef _ALONEMODEAI_H
#define	_ALONEMODEAI_H

#include "AIControl.h"

class Player;
class GameManager;

class AloneModeAi : public AIControl
{
public:
    AloneModeAi(GameManager* playManager);
    ~AloneModeAi();

    void process(Player* player);
};

#endif	/* _ALONEMODEAI_H */
