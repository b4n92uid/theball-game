/* 
 * File:   PlayFragManager.cpp
 * Author: b4n92uid
 * 
 * Created on 13 février 2010, 17:16
 */

#include "PlayFragManager.h"

#include "FragModeAi.h"
#include "Define.h"

using namespace std;
using namespace tbe;

PlayFragManager::PlayFragManager(AppManager* appManager) : PlayManager(appManager)
{
}

PlayFragManager::~PlayFragManager()
{
}

void PlayFragManager::ModSetupAi()
{
    for(unsigned i = 0; i < players.size(); i++)
        if(players[i] != m_userPlayer)
            players[i]->AttachController(new FragModeAi(this));
}

void PlayFragManager::ModUpdateStateText()
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

void PlayFragManager::ModUpdateScoreText()
{
    using namespace gui;

    sort(players.begin(), players.end(), PlayerScoreSortProcess);

    stringstream str;

    str << "Carte : " << map.name << endl;
    str << "Type : Frag" << endl;

    if(m_playTimeManager.startChrono > 0)
        str << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        str << "Temps : " << "Infinie" << endl;

    str << endl;

    for(unsigned i = 0; i < players.size(); i++)
        str << players[i]->GetName() << " [" << players[i]->GetScore() << "] point(s)" << endl;

    hud.scorelist->Write(str.str());
}

void PlayFragManager::ModUpdateGameOverText()
{
    using namespace gui;

    sort(players.begin(), players.end(), PlayerScoreSortProcess);

    stringstream str;

    str << "Carte : " << map.name << endl;
    str << "Type : Frag" << endl;

    if(m_playTimeManager.startChrono > 0)
        str << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        str << "Temps : " << "Infinie" << endl;

    str << endl;

    for(unsigned i = 0; i < players.size(); i++)
        str << players[i]->GetName() << " [" << players[i]->GetScore() << "] point(s)" << endl;

    str << "Appuyez sur espace pour continuer..." << endl;

    hud.gameover->Write(str.str());
}
