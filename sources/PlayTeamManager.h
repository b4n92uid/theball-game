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

    bool isInBleuTeam(Player* player) const;
    bool isInRedTeam(Player* player) const;

    const Player::Array getTargetsOf(Player* player) const;

protected:
    void modSetupUser(Player* userPlayer);
    void modSetupAi(Player* player);
    void modUpdateStateText(std::ostringstream& ss);
    void modUpdateScoreListText(std::ostringstream& ss);
    void modUpdateGameOverText(std::ostringstream& ss);

protected:
    Player::Array blueTeamPlayers;
    Player::Array redTeamPlayers;
    tbe::scene::ParticlesEmiter* m_teamBleuIcon;
    tbe::scene::ParticlesEmiter* m_teamRedIcon;
    bool m_userBleuTeam;
};

#endif	/* _PLAYTEAMMANAGER_H */

