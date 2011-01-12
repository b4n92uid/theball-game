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
class PlayManager;

class FragModeAi : public AIControl
{
public:
    FragModeAi(PlayManager* playManager);
    ~FragModeAi();

    void Process(Player* player);

private:
    tbe::ticks::Clock m_gustClock;
    unsigned m_gustCount;
};

#endif	/* _FRAGMODEAI_H */
