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
    void ModSetupAi();
    void ModUpdateStateText();
    void ModUpdateScoreText();
    void ModUpdateGameOverText();
};

#endif	/* _PLAYALONEMANAGER_H */

