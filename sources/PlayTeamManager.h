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
    void ModSetupUser(Player* userPlayer);
    void ModSetupAi(Player* player);
    void ModUpdateStateText(std::ostringstream& ss);
    void ModUpdateScoreListText(std::ostringstream& ss);
    void ModUpdateGameOverText(std::ostringstream& ss);

protected:
    Player::Array blueTeamPlayers;
    Player::Array redTeamPlayers;
    tbe::scene::ParticlesEmiter* m_teamBleuIcon;
    tbe::scene::ParticlesEmiter* m_teamRedIcon;
    bool m_userBleuTeam;
};

#endif	/* _PLAYTEAMMANAGER_H */

