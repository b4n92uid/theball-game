/* 
 * File:   TeamModeAi.h
 * Author: b4n92uid
 *
 * Created on 15 juin 2010, 17:25
 */

#ifndef _TEAMMODEAI_H
#define	_TEAMMODEAI_H

#include "AIControl.h"
#include "PlayTeamManager.h"

class Player;
class GameManager;

class TeamModeAi : public AIControl
{
public:
    TeamModeAi(PlayTeamManager* playManager);
    ~TeamModeAi();

    void process(Player* player);

private:
    PlayTeamManager* m_playManager;
};

#endif	/* _TEAMMODEAI_H */
