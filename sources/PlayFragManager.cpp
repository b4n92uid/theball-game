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
    for(unsigned i = 0; i < m_players.size(); i++)
        if(m_players[i] != m_userPlayer)
            m_players[i]->AttachController(new FragModeAi(this));
}

void PlayFragManager::ModUpdateStateText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Score : " << m_userPlayer->GetScore() << " Point(s)" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.curChrono << "/" << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : Infinie" << endl;
}

void PlayFragManager::ModUpdateScoreListText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Carte : " << map.name << endl;
    ss << "Type : Frag" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    for(unsigned i = 0; i < m_players.size(); i++)
        ss << " [" << m_players[i]->GetScore() << "] " << m_players[i]->GetName() << endl;
}

void PlayFragManager::ModUpdateGameOverText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Carte : " << map.name << endl;
    ss << "Type : Frag" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    for(unsigned i = 0; i < m_players.size(); i++)
        ss << " [" << m_players[i]->GetScore() << "] " << m_players[i]->GetName() << endl;

    ss << "Espace pour continuer..." << endl;
}
