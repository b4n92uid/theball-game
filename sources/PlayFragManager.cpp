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
        setBlendEq(ParticlesEmiter::MODULAR);
        setDepthTest(true);
    }

    tbe::scene::Node* clone()
    {
        return new FragBillboardIcon(*this);
    }

    void setupBullet(tbe::scene::Particle& p)
    {
        p.color(1, 1, 1, 0.5);
        p.life = 1.0f;
    }

    void process()
    {
        const Player::Array& players = m_playManager->getPlayers();

        scene::Particle* particles = beginParticlesPosProcess();

        unsigned show = 0;
        for(unsigned i = 0; i < players.size(); i++)
            if(players[i] != m_playManager->getUserPlayer()
               && !players[i]->isKilled())
            {
                particles[show].pos = players[i]->getPos() + Vector3f(0, 1, 0);
                show++;
            }

        setDrawNumber(show);

        endParticlesPosProcess();
    }

protected:
    PlayManager* m_playManager;
};

PlayFragManager::PlayFragManager(AppManager* appManager) : PlayManager(appManager)
{
    m_playersLabel = new FragBillboardIcon(this);
    m_playersLabel->setTexture(Texture(PARTICLE_PLAYER, true));
    m_playersLabel->build();
    manager.scene->getRootNode()->addChild(m_playersLabel);
}

PlayFragManager::~PlayFragManager()
{
}

void PlayFragManager::modSetupUser(Player* userPlayer)
{

}

void PlayFragManager::modSetupAi(Player* player)
{
    m_playersLabel->setNumber(m_players.size());
    m_playersLabel->build();

    player->attachController(new FragModeAi(this));
}

void PlayFragManager::modUpdateStateText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Score : " << m_userPlayer->getScore() << " Point(s)" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.curChrono << "/" << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : Infinie" << endl;

    if(m_playSetting.winCond > 0)
        ss << "Objectif : " << m_playSetting.winCond << endl;
}

void PlayFragManager::modUpdateScoreListText(std::ostringstream& ss)
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
        ss << " [" << m_players[i]->getScore() << "] " << m_players[i]->getName() << endl;
}

void PlayFragManager::modUpdateGameOverText(std::ostringstream& ss)
{
    using namespace gui;

    if(m_playSetting.winCond > 0)
    {
        if(m_userPlayer->getScore() >= (int)m_playSetting.winCond)
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
        ss << " [" << m_players[i]->getScore() << "] " << m_players[i]->getName() << endl;

    ss << endl;

    ss << "Espace pour continuer..." << endl;
}
