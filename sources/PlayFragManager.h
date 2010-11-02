/* 
 * File:   PlayFragManager.h
 * Author: b4n92uid
 *
 * Created on 13 février 2010, 17:16
 */

#ifndef _PLAYFRAGMANAGER_H
#define	_PLAYFRAGMANAGER_H

#include "PlayManager.h"

class PlayFragManager : public PlayManager
{
public:
    PlayFragManager(AppManager* appManager);
    virtual ~PlayFragManager();

protected:
    void ModSetupAi();
    void ModUpdateStateText(std::ostringstream& ss);
    void ModUpdateScoreListText(std::ostringstream& ss);
    void ModUpdateGameOverText(std::ostringstream& ss);
};

#endif	/* _PLAYFRAGMANAGER_H */

