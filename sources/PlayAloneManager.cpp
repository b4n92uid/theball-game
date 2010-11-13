/* 
 * File:   PlayAloneManager.cpp
 * Author: b4n92uid
 * 
 * Created on 13 f�vrier 2010, 18:42
 */

#include "PlayAloneManager.h"

#include "AloneModeAi.h"
#include "Define.h"

using namespace std;
using namespace tbe;

PlayAloneManager::PlayAloneManager(AppManager* appManager) : PlayManager(appManager)
{
}

PlayAloneManager::~PlayAloneManager()
{
}

class AloneModeCheckPlayers : public Player::CheckMe
{
public:

    AloneModeCheckPlayers(PlayAloneManager* playManager)
    {
        m_playManager = playManager;
    }

    bool OnTakeItems(Player* player, Item* item)
    {
        return false;
    }

    void AfterReborn(Player* player)
    {
        player->SetLife(1);
    }

private:
    PlayAloneManager* m_playManager;
};

class AloneModeCheckUserPlayer : public Player::CheckMe
{
public:

    AloneModeCheckUserPlayer(PlayAloneManager* playManager)
    {
        m_playManager = playManager;
    }

    bool OnKilled(Player* player)
    {
        m_playManager->SetGameOver();
        return true;
    }

    bool OnTakeDammage(Player* player, Bullet* ammo)
    {
        ammo->SetDammage(ammo->GetDammage() / 2);
        return true;
    }

private:
    PlayAloneManager* m_playManager;
};

void PlayAloneManager::ModSetupAi()
{
    for(unsigned i = 0; i < m_players.size(); i++)
        if(m_players[i] != m_userPlayer)
        {
            m_players[i]->AttachController(new AloneModeAi(this));
            m_players[i]->AddCheckPoint(new AloneModeCheckPlayers(this));
            m_players[i]->SetLife(1);
        }

    m_userPlayer->AddCheckPoint(new AloneModeCheckUserPlayer(this));
}

void PlayAloneManager::ModUpdateStateText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Score : " << m_userPlayer->GetScore() << " Point(s)" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.curChrono << "/" << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : Infinie" << endl;
}

void PlayAloneManager::ModUpdateScoreListText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Carte : " << map.name << endl;
    ss << "Type : Alone" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    ss << "[" << m_userPlayer->GetScore() << "] " << m_userPlayer->GetName() << endl;
}

void PlayAloneManager::ModUpdateGameOverText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Carte : " << map.name << endl;
    ss << "Type : Alone" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    ss << "[" << m_userPlayer->GetScore() << "] " << m_userPlayer->GetName() << endl;

    ss << "Appuyez sur espace pour continuer..." << endl;
}
