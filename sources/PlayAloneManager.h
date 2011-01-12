/* 
 * File:   PlayAloneManager.h
 * Author: b4n92uid
 *
 * Created on 13 février 2010, 18:42
 */

#ifndef _PLAYALONEMANAGER_H
#define	_PLAYALONEMANAGER_H

#include "PlayManager.h"

class PlayAloneManager : public PlayManager
{
public:
    PlayAloneManager(AppManager* appManager);
    virtual ~PlayAloneManager();

protected:
    void ModSetupUser(Player* userPlayer);
    void ModSetupAi(Player* player);
    void ModUpdateStateText(std::ostringstream& ss);
    void ModUpdateScoreListText(std::ostringstream& ss);
    void ModUpdateGameOverText(std::ostringstream& ss);

    tbe::scene::ParticlesEmiter* m_playersLabel;
};

#endif	/* _PLAYALONEMANAGER_H */

