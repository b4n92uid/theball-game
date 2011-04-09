/* 
 * File:   PlayManager.h
 * Author: b4n92uid
 *
 * Created on 2 janvier 2010, 20:24
 */

#ifndef _PLAYMANAGER_H
#define	_PLAYMANAGER_H

#include "GameManager.h"
#include "AppManager.h"
#include "AIControl.h"
#include "Player.h"

class BldPlayModeParser;
class UserControl;
class MaterialManager;

class BulletTime;

class StaticObject;
class DynamicObject;
class Player;
class Item;

class PlayManager : public GameManager
{
public:
    PlayManager(AppManager* appManager);
    virtual ~PlayManager();

    /// Construit la map et les entités
    void setupMap(const Settings::PartySetting& playSetting);

    /// Construction de l'ETH
    void setupGui();

    /// Appler avant le lancement du jeu
    void onStartGame();

    /// Traitement de l'entrer
    void eventProcess();

    /// Traitement ETH
    void hudProcess();

    /// Traitement Jeu
    void gameProcess();

    /// Rendue
    void render();

    /// Rotine d'affichage de notification
    void log(std::string msg);

    void setGameOver();
    bool isGameOver() const;

    void registerPlayer(Player* player);
    void unregisterPlayer(Player* player);

    void hudItem(bool status);
    void hudBullettime(bool status);
    void hudBoost(bool status);
    void hudDammage(bool status);

    virtual BulletTime* getBullettime() const;

    virtual tbe::Vector3f getShootTarget() const;

    virtual Player* getUserPlayer() const;

    virtual const Player::Array& getPlayers() const;

    virtual const Player::Array getTargetsOf(Player* player) const;

    int modulatScore(int score);

protected:

    virtual void modSetupAi(Player* player) = 0;
    virtual void modSetupUser(Player* player) = 0;

    virtual void modUpdateStateText(std::ostringstream& ss) = 0;
    virtual void modUpdateScoreListText(std::ostringstream& ss) = 0;
    virtual void modUpdateGameOverText(std::ostringstream& ss) = 0;

    static bool playerScoreSortProcess(Player* p1, Player* p2)
    {
        if(p1->getScore() == p2->getScore())
            return (p1->getName() > p2->getName());
        else
            return (p1->getScore() > p2->getScore());
    }

    enum TimtTo
    {
        TIME_TO_VIEWSCORE,
        TIME_TO_PLAY,
        TIME_TO_PAUSE,
        TIME_TO_GAMEOVER,
    };

    struct
    {
        int startChrono;
        int curChrono;
        tbe::ticks::Clock clock;
        time_t startTimestamp;

    } m_playTimeManager;

    Player* m_userPlayer;

    Player::Array m_players;
    Weapon::Array m_weapons;

    BulletTime* m_bullettime;

    Settings::PartySetting m_playSetting;

    tbe::ticks::Clock m_logClock;
    tbe::ticks::Clock m_newtonClock;
    tbe::ticks::Clock m_validGameOver;
    tbe::ticks::Clock m_spawnPlayer;

    tbe::scene::Camera* m_camera;
    tbe::scene::NewtonNode* m_cameraBody;
    tbe::Matrix4f m_cameratMat;

    tbe::Vector3f m_shootTarget;
    bool m_cursorOnPlayer;

    std::list<tbe::Vector3f> m_playerPosRec;

    std::vector<std::string> m_botNames;

    TimtTo m_timeTo;

    bool m_gameOver;

private:

    std::map<unsigned, unsigned> m_numslot;

    struct
    {
        tbe::ppe::BloomEffect* bloom;
        tbe::ppe::BlurEffect* gameover;
        tbe::ppe::ColorEffect* bullettime;
        tbe::ppe::ColorEffect* dammage;
        tbe::ppe::MotionBlurEffect* boost;

    } ppe;

    struct
    {

        struct
        {
            tbe::gui::Button* ret;
            tbe::gui::Button* quit;

        } playmenu;

        tbe::gui::TextBox* scorelist;
        tbe::gui::TextBox* state;
        tbe::gui::TextBox* gameover;
        tbe::gui::TextBox* log;

        tbe::gui::Gauge* ammo;
        tbe::gui::Gauge* life;
        tbe::gui::Gauge* bullettime;

        tbe::gui::StateShow* boost;
        tbe::gui::StateShow* item;

        struct
        {
            tbe::gui::Image* background;
            tbe::gui::Image* dammage;
            tbe::gui::Image* bullettime;
            tbe::gui::Image* gameover;

        } background;

    } hud;

    void processDevelopperCodeEvent();
};

#endif	/* _PLAYMODEMANAGER_H */
