/* 
 * File:   PlayFragManager.h
 * Author: b4n92uid
 *
 * Created on 13 février 2010, 17:16
 */

#ifndef _PLAYFRAGMANAGER_H
#define	_PLAYFRAGMANAGER_H

#include "GameManager.h"

class PlayFragManager : public GameManager
{
public:
    PlayFragManager(AppManager* appManager);
    virtual ~PlayFragManager();

protected:
    void modSetupUser(Player* userPlayer);
    void modSetupAi(Player* player);
    void modUpdateStateText(std::ostringstream& ss);
    void modUpdateScoreListText(std::ostringstream& ss);
    void modUpdateGameOverText(std::ostringstream& ss);

    tbe::scene::ParticlesEmiter* m_playersLabel;
};

#endif	/* _PLAYFRAGMANAGER_H */

