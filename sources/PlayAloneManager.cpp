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

class AloneBillboardIcon : public tbe::scene::ParticlesEmiter
{
public:

    AloneBillboardIcon(PlayManager* playManager)
    : tbe::scene::ParticlesEmiter(playManager->parallelscene.particles),
    m_playManager(playManager)
    {
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

PlayAloneManager::PlayAloneManager(AppManager* appManager) : PlayManager(appManager)
{
    m_playersLabel = new AloneBillboardIcon(this);
    m_playersLabel->SetTexture(Texture(PARTICLE_PLAYER, true));
    m_playersLabel->Build();
    manager.scene->GetRootNode()->AddChild(m_playersLabel);
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

void PlayAloneManager::ModSetupUser(Player* userPlayer)
{
    userPlayer->AddCheckPoint(new AloneModeCheckUserPlayer(this));
}

void PlayAloneManager::ModSetupAi(Player* player)
{
    m_playersLabel->SetNumber(m_players.size());
    m_playersLabel->Build();

    player->AttachController(new AloneModeAi(this));
    player->AddCheckPoint(new AloneModeCheckPlayers(this));
    player->SetLife(1);
}

void PlayAloneManager::ModUpdateStateText(std::ostringstream& ss)
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

    if(m_playSetting.winCond > 0)
    {
        if(m_userPlayer->GetScore() >= m_playSetting.winCond)
            ss << "<< VOUS AVEZ GANGEZ !!! >>" << endl;
        else
            ss << "<< VOUS AVEZ PERDU !!! >>" << endl;

        ss << endl;
    }

    ss << "Carte : " << map.name << endl;
    ss << "Type : Alone" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    ss << "[" << m_userPlayer->GetScore() << "] " << m_userPlayer->GetName() << endl;

    ss << endl;

    ss << "Espace pour continuer..." << endl;
}
