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

class TeamBillboardIcon : public tbe::scene::ParticlesEmiter
{
public:

    TeamBillboardIcon(GameManager* playManager, Player::Array& teamPlayers)
    : tbe::scene::ParticlesEmiter(playManager->parallelscene.particles)
    , m_playManager(playManager), m_teamPlayers(teamPlayers)
    {
        setBlendEq(ParticlesEmiter::MODULAR);
        setDepthTest(true);
    }

    tbe::scene::Node* clone()
    {
        return new TeamBillboardIcon(*this);
    }

    void setupBullet(tbe::scene::Particle& p)
    {
        p.color(1, 1, 1, 0.5);
        p.life = 1.0f;
    }

    void process()
    {
        scene::Particle* particles = beginParticlesPosProcess();

        unsigned show = 0;
        for(unsigned i = 0; i < m_teamPlayers.size(); i++)
            if(m_teamPlayers[i] != m_playManager->getUserPlayer()
               && !m_teamPlayers[i]->isKilled())
            {
                particles[show].pos = m_teamPlayers[i]->getVisualBody()->getPos() + Vector3f(0, 1, 0);
                show++;
            }

        setDrawNumber(show);

        endParticlesPosProcess();
    }

protected:
    GameManager* m_playManager;
    Player::Array& m_teamPlayers;
};

PlayTeamManager::PlayTeamManager(AppManager* appManager) : GameManager(appManager)
{
    m_teamBleuIcon = new TeamBillboardIcon(this, blueTeamPlayers);
    m_teamBleuIcon->setTexture(Texture(PARTICLE_BLEUTEAM, true));
    m_teamBleuIcon->build();
    manager.scene->getRootNode()->addChild(m_teamBleuIcon);

    m_teamRedIcon = new TeamBillboardIcon(this, redTeamPlayers);
    m_teamRedIcon->setTexture(Texture(PARTICLE_REDTEAM, true));
    m_teamRedIcon->build();
    manager.scene->getRootNode()->addChild(m_teamRedIcon);
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

    virtual bool onTakeDammage(Player* player, Bullet* ammo)
    {
        return (m_playManager->isInBleuTeam(player)
                != m_playManager->isInBleuTeam(ammo->getWeapon()->getShooter())
                && m_playManager->isInRedTeam(player)
                != m_playManager->isInRedTeam(ammo->getWeapon()->getShooter()));
    }

private:
    PlayTeamManager* m_playManager;
};

void PlayTeamManager::modSetupUser(Player* userPlayer)
{
    m_userBleuTeam = tools::rand(0, 2);

    if(m_userBleuTeam)
        blueTeamPlayers.push_back(userPlayer);
    else
        redTeamPlayers.push_back(userPlayer);
}

void PlayTeamManager::modSetupAi(Player* player)
{
    using namespace scene;

    player->attachController(new TeamModeAi(this));
    player->addCheckMe(new TeamModeCheckPlayers(this));

    if(redTeamPlayers.size() > blueTeamPlayers.size())
    {
        blueTeamPlayers.push_back(player);
        m_teamBleuIcon->setNumber(blueTeamPlayers.size());
        m_teamBleuIcon->build();
    }
    else
    {
        redTeamPlayers.push_back(player);
        m_teamRedIcon->setNumber(redTeamPlayers.size());
        m_teamRedIcon->build();
    }

}

void PlayTeamManager::modUpdateStateText(std::ostringstream& ss)
{
    using namespace gui;

    ss << "Score : " << m_userPlayer->getScore() << " Point(s)" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.curChrono << "/" << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : Infinie" << endl;

    ss << "Equipe : " << (m_userBleuTeam ? "Bleu" : "Rouge") << endl;

    if(m_playSetting.winCond > 0)
        ss << "Objectif : " << m_playSetting.winCond << endl;
}

void PlayTeamManager::modUpdateScoreListText(std::ostringstream& ss)
{
    using namespace gui;

    sort(blueTeamPlayers.begin(), blueTeamPlayers.end(), playerScoreSortProcess);
    sort(redTeamPlayers.begin(), redTeamPlayers.end(), playerScoreSortProcess);

    unsigned bleuScore = 0, redScore = 0;

    if(blueTeamPlayers.size())
    {
        for(unsigned i = 0; i < blueTeamPlayers.size(); i++)
            bleuScore += blueTeamPlayers[i]->getScore();
        bleuScore /= blueTeamPlayers.size();
    }

    if(redTeamPlayers.size())
    {
        for(unsigned i = 0; i < redTeamPlayers.size(); i++)
            redScore += redTeamPlayers[i]->getScore();
        redScore /= redTeamPlayers.size();
    }

    ss << "Carte : " << map.name << endl;
    ss << "Type : Team" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    ss << "Equipe bleu [" << bleuScore << "]" << endl;
    for(unsigned i = 0; i < blueTeamPlayers.size(); i++)
        ss << " [" << blueTeamPlayers[i]->getScore() << "] " << blueTeamPlayers[i]->getName() << endl;
    ss << endl;

    ss << "Equipe rouge [" << redScore << "]" << endl;
    for(unsigned i = 0; i < redTeamPlayers.size(); i++)
        ss << " [" << redTeamPlayers[i]->getScore() << "] " << redTeamPlayers[i]->getName() << endl;
    ss << endl;
}

void PlayTeamManager::modUpdateGameOverText(std::ostringstream& ss)
{
    using namespace gui;

    sort(blueTeamPlayers.begin(), blueTeamPlayers.end(), playerScoreSortProcess);
    sort(redTeamPlayers.begin(), redTeamPlayers.end(), playerScoreSortProcess);

    unsigned blueScore = 0, redScore = 0;

    for(unsigned i = 0; i < blueTeamPlayers.size(); i++)
        blueScore += blueTeamPlayers[i]->getScore();
    blueScore /= blueTeamPlayers.size() ? blueTeamPlayers.size() : 1;

    for(unsigned i = 0; i < redTeamPlayers.size(); i++)
        redScore += redTeamPlayers[i]->getScore();
    redScore /= redTeamPlayers.size() ? redTeamPlayers.size() : 1;

    if(m_playSetting.winCond > 0)
    {
        if(m_userPlayer->getScore() >= (int)m_playSetting.winCond)
            ss << "<< VOUS AVEZ GANGEZ !!! >>" << endl;
        else
            ss << "<< VOUS AVEZ PERDU !!! >>" << endl;

        ss << endl;
    }

    ss << "Carte : " << map.name << endl;
    ss << "Type : Team" << endl;

    if(m_playTimeManager.startChrono > 0)
        ss << "Temps : " << m_playTimeManager.startChrono << endl;
    else
        ss << "Temps : " << "Infinie" << endl;

    ss << endl;

    ss << "Equipe bleu [" << blueScore << "]" << endl;
    for(unsigned i = 0; i < blueTeamPlayers.size(); i++)
        ss << " [" << blueTeamPlayers[i]->getScore() << "] " << blueTeamPlayers[i]->getName() << endl;
    ss << endl;

    ss << "Equipe rouge [" << redScore << "]" << endl;
    for(unsigned i = 0; i < redTeamPlayers.size(); i++)
        ss << " [" << redTeamPlayers[i]->getScore() << "] " << redTeamPlayers[i]->getName() << endl;
    ss << endl;

    ss << "Espace pour continuer..." << endl;
}

bool PlayTeamManager::isInBleuTeam(Player* player) const
{
    return find(blueTeamPlayers.begin(), blueTeamPlayers.end(), player) != blueTeamPlayers.end();
}

bool PlayTeamManager::isInRedTeam(Player* player) const
{
    return find(redTeamPlayers.begin(), redTeamPlayers.end(), player) != redTeamPlayers.end();
}

const Player::Array PlayTeamManager::getTargetsOf(Player* player) const
{
    return isInRedTeam(player) ? blueTeamPlayers : redTeamPlayers;
}
