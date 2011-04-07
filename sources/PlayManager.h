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
    void SetupMap(const Settings::PartySetting& playSetting);

    /// Construction de l'ETH
    void SetupGui();

    /// Appler avant le lancement du jeu
    void OnStartGame();

    /// Traitement de l'entrer
    void EventProcess();

    /// Traitement ETH
    void HudProcess();

    /// Traitement Jeu
    void GameProcess();

    /// Rendue
    void Render();

    /// Rotine d'affichage de notification
    void Log(std::string msg);

    void SetGameOver();
    bool IsGameOver() const;

    void RegisterPlayer(Player* player);
    void UnRegisterPlayer(Player* player);

    void HudItem(bool status);
    void HudBullettime(bool status);
    void HudBoost(bool status);
    void HudDammage(bool status);

    virtual BulletTime* GetBullettime() const;

    virtual tbe::Vector3f GetShootTarget() const;

    virtual Player* GetUserPlayer() const;

    virtual const Player::Array& GetPlayers() const;

    virtual const Player::Array GetTargetsOf(Player* player) const;

    int ModulatScore(int score);

protected:

    virtual void ModSetupAi(Player* player) = 0;
    virtual void ModSetupUser(Player* player) = 0;

    virtual void ModUpdateStateText(std::ostringstream& ss) = 0;
    virtual void ModUpdateScoreListText(std::ostringstream& ss) = 0;
    virtual void ModUpdateGameOverText(std::ostringstream& ss) = 0;

    static bool PlayerScoreSortProcess(Player* p1, Player* p2)
    {
        if(p1->GetScore() == p2->GetScore())
            return (p1->GetName() > p2->GetName());
        else
            return (p1->GetScore() > p2->GetScore());
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

    void ProcessDevelopperCodeEvent();
};

#endif	/* _PLAYMODEMANAGER_H */
