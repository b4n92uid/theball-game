/* 
 * File:   PlayAloneManager.h
 * Author: b4n92uid
 *
 * Created on 13 février 2010, 18:42
 */

#ifndef _PLAYALONEMANAGER_H
#define	_PLAYALONEMANAGER_H

#include "GameManager.h"

class PlayAloneManager : public GameManager
{
public:
    PlayAloneManager(AppManager* appManager);
    virtual ~PlayAloneManager();

protected:
    void modSetupUser(Player* userPlayer);
    void modSetupAi(Player* player);
    void modUpdateStateText(std::ostringstream& ss);
    void modUpdateScoreListText(std::ostringstream& ss);
    void modUpdateGameOverText(std::ostringstream& ss);

    tbe::scene::ParticlesEmiter* m_playersLabel;
};

#endif	/* _PLAYALONEMANAGER_H */

