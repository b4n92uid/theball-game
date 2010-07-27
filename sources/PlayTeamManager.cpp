/* 
 * File:   PlayTeamManager.cpp
 * Author: b4n92uid
 * 
 * Created on 13 février 2010, 18:42
 */

#include "PlayTeamManager.h"

#include "Define.h"
#include "TeamModeAi.h"
#include "MaterialManager.h"

using namespace std;
using namespace tbe;

class BillboardIcon : public tbe::scene::ParticlesEmiter
{
public:

    BillboardIcon(Player::Array& teamPlayers) : m_teamPlayers(teamPlayers)
    {
        SetNumber(m_teamPlayers.size());
        SetBlendEq(ParticlesEmiter::MODULAR);
        SetDepthTest(true);
    }

    void Build(tbe::scene::Particle& p)
    {
        p.color(1, 1, 1, 0.5);
        p.life = 1.0f;
    }

    void Process()
    {
        scene::Particle* particles = BeginParticlesPosProcess();

        unsigned show = 0;
        for(unsigned i = 0; i < m_teamPlayers.size(); i++)
            if(!m_teamPlayers[i]->IsKilled())
            {
                particles[show].pos = m_teamPlayers[i]->NewtonNode::GetPos() + Vector3f(0, 1, 0);
                show++;
            }

        SetDrawNumber(show);

        EndParticlesPosProcess();
    }

protected:
    Player::Array& m_teamPlayers;
};

PlayTeamManager::PlayTeamManager(AppManager* appManager) : PlayManager(appManager)
{
}

PlayTeamManager::~PlayTeamManager()
{
}

class TeamModeCheckPlayers : public Player::CheckMe
{
public:

    TeamModeCheckPlayers(PlayTeamManager* playManager)
    {
        m_playManager = playManager;
    }

    virtual bool OnTakeDammage(Player* player, Ammo* ammo)
    {
        return(m_playManager->IsInBleuTeam(player)
                != m_playManager->IsInBleuTeam(ammo->GetParent()->GetShooter())
                && m_playManager->IsInRedTeam(player)
                != m_playManager->IsInRedTeam(ammo->GetParent()->GetShooter()));
    }

private:
    PlayTeamManager* m_playManager;
};

void PlayTeamManager::ModSetupAi()
{
    using namespace scene;

    unsigned middle = players.size() / 2;

    std::random_shuffle(players.begin(), players.end());

    for(unsigned i = 0; i < middle; i++)
    {
        if(players[i] != m_userPlayer)
            players[i]->AttachController(new TeamModeAi(this));

        players[i]->AddCheckPoint(new TeamModeCheckPlayers(this));

        blueTeamPlayers.push_back(players[i]);
    }

    for(unsigned i = middle; i < players.size(); i++)
    {
        if(players[i] != m_userPlayer)
            players[i]->AttachController(new TeamModeAi(this));

        players[i]->AddCheckPoint(new TeamModeCheckPlayers(this));

        redTeamPlayers.push_back(players[i]);
    }

    m_teamBleuIcon = new BillboardIcon(blueTeamPlayers);
    m_teamBleuIcon->SetTexture(Texture(PARTICLE_BLEUTEAM, true));
    m_teamBleuIcon->Build();
    parallelscene.particles->AddParticlesEmiter("", m_teamBleuIcon);

    m_teamRedIcon = new BillboardIcon(redTeamPlayers);
    m_teamRedIcon->SetTexture(Texture(PARTICLE_REDTEAM, true));
    m_teamRedIcon->Build();
    parallelscene.particles->AddParticlesEmiter("", m_teamRedIcon);
}

void PlayTeamManager::ModUpdateStateText()
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

void PlayTeamManager::ModUpdateScoreText()
{
    using namespace gui;

    sort(blueTeamPlayers.begin(), blueTeamPlayers.end(), PlayerScoreSortProcess);
    sort(redTeamPlayers.begin(), redTeamPlayers.end(), PlayerScoreSortProcess);

    unsigned bleuScore = 0, redScore = 0;

    for(unsigned i = 0; i < blueTeamPlayers.size(); i++)
        bleuScore += blueTeamPlayers[i]->GetScore();
    bleuScore /= blueTeamPlayers.size();

    for(unsigned i = 0; i < redTeamPlayers.size(); i++)
        redScore += redTeamPlayers[i]->GetScore();
    redScore /= redTeamPlayers.size();

    stringstream str;

    str << "Carte : " << map.name << endl;
    str << "Type : Team" << endl;

    if(m_playTimeManager.startChrono > 0)
        str << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        str << "Temps : " << "Infinie" << endl;

    str << endl;

    str << "Equipe bleu [" << bleuScore << "]" << endl;
    for(unsigned i = 0; i < blueTeamPlayers.size(); i++)
        str << blueTeamPlayers[i]->GetName() << " [" << blueTeamPlayers[i]->GetScore() << "] point(s)" << endl;
    str << endl;

    str << "Equipe rouge [" << redScore << "]" << endl;
    for(unsigned i = 0; i < redTeamPlayers.size(); i++)
        str << redTeamPlayers[i]->GetName() << " [" << redTeamPlayers[i]->GetScore() << "] point(s)" << endl;
    str << endl;

    hud.scorelist->Write(str.str());
}

void PlayTeamManager::ModUpdateGameOverText()
{
    using namespace gui;

    sort(blueTeamPlayers.begin(), blueTeamPlayers.end(), PlayerScoreSortProcess);
    sort(redTeamPlayers.begin(), redTeamPlayers.end(), PlayerScoreSortProcess);

    unsigned bleuScore = 0, redScore = 0;

    for(unsigned i = 0; i < blueTeamPlayers.size(); i++)
        bleuScore += blueTeamPlayers[i]->GetScore();
    bleuScore /= blueTeamPlayers.size();

    for(unsigned i = 0; i < redTeamPlayers.size(); i++)
        redScore += redTeamPlayers[i]->GetScore();
    redScore /= redTeamPlayers.size();

    stringstream str;

    str << "Carte : " << map.name << endl;
    str << "Type : Team" << endl;

    if(m_playTimeManager.startChrono > 0)
        str << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        str << "Temps : " << "Infinie" << endl;

    str << endl;

    str << "Equipe bleu [" << bleuScore << "]" << endl;
    for(unsigned i = 0; i < blueTeamPlayers.size(); i++)
        str << blueTeamPlayers[i]->GetName() << " [" << blueTeamPlayers[i]->GetScore() << "] point(s)" << endl;
    str << endl;

    str << "Equipe rouge [" << redScore << "]" << endl;
    for(unsigned i = 0; i < redTeamPlayers.size(); i++)
        str << redTeamPlayers[i]->GetName() << " [" << redTeamPlayers[i]->GetScore() << "] point(s)" << endl;
    str << endl;

    str << "Appuyez sur espace pour continuer..." << endl;

    hud.gameover->Write(str.str());
}

bool PlayTeamManager::IsInBleuTeam(Player* player)
{
    return find(blueTeamPlayers.begin(), blueTeamPlayers.end(), player) != blueTeamPlayers.end();
}

bool PlayTeamManager::IsInRedTeam(Player* player)
{
    return find(redTeamPlayers.begin(), redTeamPlayers.end(), player) != redTeamPlayers.end();
}
