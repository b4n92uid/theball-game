/* 
 * File:   PlayAloneManager.cpp
 * Author: b4n92uid
 * 
 * Created on 13 février 2010, 18:42
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
    for(unsigned i = 0; i < players.size(); i++)
        if(players[i] != m_userPlayer)
        {
            players[i]->AttachController(new AloneModeAi(this));
            players[i]->AddCheckPoint(new AloneModeCheckPlayers(this));
            players[i]->SetLife(1);
        }

    m_userPlayer->AddCheckPoint(new AloneModeCheckUserPlayer(this));
}

void PlayAloneManager::ModUpdateStateText()
{
    using namespace gui;

    stringstream str;

    str << "Score : " << m_userPlayer->GetScore() << " Point(s)" << endl;

    if(m_playTimeManager.startChrono > 0)
        str << "Temps : " << m_playTimeManager.curChrono << "/" << m_playTimeManager.startChrono << endl;
    else
        str << "Temps : Infinie" << endl;

    hud.state->Write(str.str());
}

void PlayAloneManager::ModUpdateScoreText()
{
    using namespace gui;

    stringstream str;

    str << "Carte : " << map.name << endl;
    str << "Type : Alone" << endl;

    if(m_playTimeManager.startChrono > 0)
        str << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        str << "Temps : " << "Infinie" << endl;

    str << endl;

    str << m_userPlayer->GetName() << " : " << m_userPlayer->GetScore() << " point(s)" << endl;

    hud.scorelist->Write(str.str());
}

void PlayAloneManager::ModUpdateGameOverText()
{
    using namespace gui;

    stringstream str;

    str << "Carte : " << map.name << endl;
    str << "Type : Alone" << endl;

    if(m_playTimeManager.startChrono > 0)
        str << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        str << "Temps : " << "Infinie" << endl;

    str << endl;

    str << m_userPlayer->GetName() << " : " << m_userPlayer->GetScore() << " point(s)" << endl;

    str << "Appuyez sur espace pour continuer..." << endl;

    hud.gameover->Write(str.str());
}
