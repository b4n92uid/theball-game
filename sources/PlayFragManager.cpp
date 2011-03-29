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

class FragBillboardIcon : public tbe::scene::ParticlesEmiter
{
public:

    FragBillboardIcon(PlayManager* playManager)
    : tbe::scene::ParticlesEmiter(playManager->parallelscene.particles),
    m_playManager(playManager)
    {
        SetBlendEq(ParticlesEmiter::MODULAR);
        SetDepthTest(true);
    }

    tbe::scene::Node* Clone()
    {
        return new FragBillboardIcon(*this);
    }

    void SetupBullet(tbe::scene::Particle& p)
    {
        p.color(1, 1, 1, 0.5);
        p.life = 1.0f;
    }

    void Process()
    {
        const Player::Array& players = m_playManager->GetPlayers();

        scene::Particle* particles = BeginParticlesPosProcess();

        unsigned show = 0;
        for(unsigned i = 0; i < players.size(); i++)
            if(players[i] != m_playManager->GetUserPlayer()
               && !players[i]->IsKilled())
            {
                particles[show].pos = players[i]->GetPos() + Vector3f(0, 1, 0);
                show++;
            }

        SetDrawNumber(show);

        EndParticlesPosProcess();
    }

protected:
    PlayManager* m_playManager;
};

PlayFragManager::PlayFragManager(AppManager* appManager) : PlayManager(appManager)
{
    m_playersLabel = new FragBillboardIcon(this);
    m_playersLabel->SetTexture(Texture(PARTICLE_PLAYER, true));
    m_playersLabel->Build();
    manager.scene->GetRootNode()->AddChild(m_playersLabel);
}

PlayFragManager::~PlayFragManager()
{
}

void PlayFragManager::ModSetupUser(Player* userPlayer)
{

}

void PlayFragManager::ModSetupAi(Player* player)
{
    m_playersLabel->SetNumber(m_players.size());
    m_playersLabel->Build();

    player->AttachController(new FragModeAi(this));
}

void PlayFragManager::ModUpdateStateText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Score : " << m_userPlayer->GetScore() << " Point(s)" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.curChrono << "/" << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : Infinie" << endl;

    if(m_playSetting.winCond > 0)
        ss << "Objectif : " << m_playSetting.winCond << endl;
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

    if(m_playSetting.winCond > 0)
    {
        if(m_userPlayer->GetScore() >= m_playSetting.winCond)
            ss << "<< VOUS AVEZ GANGEZ !!! >>" << endl;
        else
            ss << "<< VOUS AVEZ PERDU !!! >>" << endl;

        ss << endl;
    }

    ss << "Carte : " << map.name << endl;
    ss << "Type : Frag" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    for(unsigned i = 0; i < m_players.size(); i++)
        ss << " [" << m_players[i]->GetScore() << "] " << m_players[i]->GetName() << endl;

    ss << endl;

    ss << "Espace pour continuer..." << endl;
}
