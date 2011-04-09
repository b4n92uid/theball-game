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
        setBlendEq(ParticlesEmiter::MODULAR);
        setDepthTest(true);
    }

    tbe::scene::Node* clone()
    {
        return new AloneBillboardIcon(*this);
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

PlayAloneManager::PlayAloneManager(AppManager* appManager) : PlayManager(appManager)
{
    m_playersLabel = new AloneBillboardIcon(this);
    m_playersLabel->setTexture(Texture(PARTICLE_PLAYER, true));
    m_playersLabel->build();
    manager.scene->getRootNode()->addChild(m_playersLabel);
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

    bool onTakeItems(Player* player, Item* item)
    {
        return false;
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

    bool onKilled(Player* player)
    {
        m_playManager->setGameOver();
        return true;
    }

    bool onTakeDammage(Player* player, Bullet* ammo)
    {
        ammo->setDammage(ammo->getDammage() / 2);
        return true;
    }

private:
    PlayAloneManager* m_playManager;
};

void PlayAloneManager::modSetupUser(Player* userPlayer)
{
    userPlayer->addCheckMe(new AloneModeCheckUserPlayer(this));
}

void PlayAloneManager::modSetupAi(Player* player)
{
    m_playersLabel->setNumber(m_players.size());
    m_playersLabel->build();

    player->attachController(new AloneModeAi(this));
    player->addCheckMe(new AloneModeCheckPlayers(this));
    player->setLife(1);
}

void PlayAloneManager::modUpdateStateText(std::ostringstream& ss)
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

void PlayAloneManager::modUpdateScoreListText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Carte : " << map.name << endl;
    ss << "Type : Alone" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    ss << "[" << m_userPlayer->getScore() << "] " << m_userPlayer->getName() << endl;
}

void PlayAloneManager::modUpdateGameOverText(std::ostringstream& ss)
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
    ss << "Type : Alone" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    ss << "[" << m_userPlayer->getScore() << "] " << m_userPlayer->getName() << endl;

    ss << endl;

    ss << "Espace pour continuer..." << endl;
}
