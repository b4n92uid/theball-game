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

    BillboardIcon(PlayManager* playManager, Player::Array& teamPlayers)
    : m_playManager(playManager), m_teamPlayers(teamPlayers)
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
            if(m_teamPlayers[i] != m_playManager->GetUserPlayer()
               && !m_teamPlayers[i]->IsKilled())
            {
                particles[show].pos = m_teamPlayers[i]->GetPos() + Vector3f(0, 1, 0);
                show++;
            }

        SetDrawNumber(show);

        EndParticlesPosProcess();
    }

protected:
    PlayManager* m_playManager;
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

    virtual bool OnTakeDammage(Player* player, Bullet* ammo)
    {
        return(m_playManager->IsInBleuTeam(player)
                != m_playManager->IsInBleuTeam(ammo->GetWeapon()->GetShooter())
                && m_playManager->IsInRedTeam(player)
                != m_playManager->IsInRedTeam(ammo->GetWeapon()->GetShooter()));
    }

private:
    PlayTeamManager* m_playManager;
};

void PlayTeamManager::ModSetupAi()
{
    using namespace scene;

    unsigned middle = m_players.size() / 2;

    std::random_shuffle(m_players.begin(), m_players.end());

    for(unsigned i = 0; i < middle; i++)
    {
        if(m_players[i] != m_userPlayer)
            m_players[i]->AttachController(new TeamModeAi(this));

        m_players[i]->AddCheckPoint(new TeamModeCheckPlayers(this));

        blueTeamPlayers.push_back(m_players[i]);
    }

    for(unsigned i = middle; i < m_players.size(); i++)
    {
        if(m_players[i] != m_userPlayer)
            m_players[i]->AttachController(new TeamModeAi(this));

        m_players[i]->AddCheckPoint(new TeamModeCheckPlayers(this));

        redTeamPlayers.push_back(m_players[i]);
    }

    m_userBleuTeam = IsInBleuTeam(m_userPlayer);

    m_teamBleuIcon = new BillboardIcon(this, blueTeamPlayers);
    m_teamBleuIcon->SetTexture(Texture(PARTICLE_BLEUTEAM, true));
    m_teamBleuIcon->Build();
    parallelscene.particles->AddChild(m_teamBleuIcon);

    m_teamRedIcon = new BillboardIcon(this, redTeamPlayers);
    m_teamRedIcon->SetTexture(Texture(PARTICLE_REDTEAM, true));
    m_teamRedIcon->Build();
    parallelscene.particles->AddChild(m_teamRedIcon);
}

void PlayTeamManager::ModUpdateStateText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Score : " << m_userPlayer->GetScore() << " Point(s)" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.curChrono << "/" << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : Infinie" << endl;

    ss << "Equipe : " << (m_userBleuTeam ? "Bleu" : "Rouge") << endl;
}

void PlayTeamManager::ModUpdateScoreListText(std::ostringstream& ss)
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

    ss << "Carte : " << map.name << endl;
    ss << "Type : Team" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    ss << "Equipe bleu [" << bleuScore << "]" << endl;
    for(unsigned i = 0; i < blueTeamPlayers.size(); i++)
        ss << " [" << blueTeamPlayers[i]->GetScore() << "] " << blueTeamPlayers[i]->GetName() << endl;
    ss << endl;

    ss << "Equipe rouge [" << redScore << "]" << endl;
    for(unsigned i = 0; i < redTeamPlayers.size(); i++)
        ss << " [" << redTeamPlayers[i]->GetScore() << "] " << redTeamPlayers[i]->GetName() << endl;
    ss << endl;
}

void PlayTeamManager::ModUpdateGameOverText(std::ostringstream& ss)
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

    ss << "Carte : " << map.name << endl;
    ss << "Type : Team" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    ss << "Equipe bleu [" << bleuScore << "]" << endl;
    for(unsigned i = 0; i < blueTeamPlayers.size(); i++)
        ss << " [" << blueTeamPlayers[i]->GetScore() << "] " << blueTeamPlayers[i]->GetName() << endl;
    ss << endl;

    ss << "Equipe rouge [" << redScore << "]" << endl;
    for(unsigned i = 0; i < redTeamPlayers.size(); i++)
        ss << " [" << redTeamPlayers[i]->GetScore() << "] " << redTeamPlayers[i]->GetName() << endl;
    ss << endl;

    ss << "Appuyez sur espace pour continuer..." << endl;
}

bool PlayTeamManager::IsInBleuTeam(Player* player) const
{
    return find(blueTeamPlayers.begin(), blueTeamPlayers.end(), player) != blueTeamPlayers.end();
}

bool PlayTeamManager::IsInRedTeam(Player* player) const
{
    return find(redTeamPlayers.begin(), redTeamPlayers.end(), player) != redTeamPlayers.end();
}

const Player::Array PlayTeamManager::GetTargetsOf(Player* player) const
{
    return IsInRedTeam(player) ? blueTeamPlayers : redTeamPlayers;
}
