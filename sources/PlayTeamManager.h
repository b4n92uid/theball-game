/* 
 * File:   PlayTeamManager.h
 * Author: b4n92uid
 *
 * Created on 13 février 2010, 18:42
 */

#ifndef _PLAYTEAMMANAGER_H
#define	_PLAYTEAMMANAGER_H

#include "PlayManager.h"

class PlayTeamManager : public PlayManager
{
public:
    PlayTeamManager(AppManager* appManager);
    virtual ~PlayTeamManager();

    bool IsInBleuTeam(Player* player) const;
    bool IsInRedTeam(Player* player) const;

    const Player::Array GetTargetsOf(Player* player) const;

protected:
    virtual void ModSetupAi();
    virtual void ModUpdateStateText(std::ostringstream& ss);
    virtual void ModUpdateScoreListText(std::ostringstream& ss);
    virtual void ModUpdateGameOverText(std::ostringstream& ss);

protected:
    Player::Array blueTeamPlayers;
    Player::Array redTeamPlayers;

    tbe::scene::ParticlesEmiter* m_teamBleuIcon;
    tbe::scene::ParticlesEmiter* m_teamRedIcon;
};

#endif	/* _PLAYTEAMMANAGER_H */

