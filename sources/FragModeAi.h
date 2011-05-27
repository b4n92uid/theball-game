/* 
 * File:   FragModeAi.h
 * Author: b4n92uid
 *
 * Created on 15 juin 2010, 17:23
 */

#ifndef _FRAGMODEAI_H
#define	_FRAGMODEAI_H

#include "AIControl.h"

class Player;
class GameManager;

class FragModeAi : public AIControl
{
public:
    FragModeAi(GameManager* playManager);
    ~FragModeAi();

    void process(Player* player);
};

#endif	/* _FRAGMODEAI_H */
