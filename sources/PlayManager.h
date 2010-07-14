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

class BldPlayModeParser;
class UserControl;
class MaterialManager;

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
    void SetupMap(const AppManager::PlaySetting& playSetting);

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

    void ActivateBullttime();
    void DeactivateBullttime();

    void SetGameOver();
    bool IsGameOver() const;

    void RegisterPlayer(Player* player);
    void UnRegisterPlayer(Player* player, bool keep = false);

    tbe::Vector3f GetShootTarget() const;

    Player* GetUserPlayer() const;

    struct
    {
        tbe::ppe::BloomEffect* bloom;
        tbe::ppe::BlurEffect* gameover;
        tbe::ppe::ColorEffect* bullettime;
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

    Player::Array players;

protected:
    virtual void ModSetupAi() = 0;
    virtual void ModUpdateStateText() = 0;
    virtual void ModUpdateScoreText() = 0;
    virtual void ModUpdateGameOverText() = 0;

    void ProcessDevelopperCodeEvent();

    static bool PlayerScoreSortProcess(Player* p1, Player* p2)
    {
        return (p1->GetScore() > p2->GetScore());
    }


protected:

    enum TimtTo
    {
        TIME_TO_VIEWSCORE,
        TIME_TO_PLAY,
        TIME_TO_PAUSE,
        TIME_TO_GAMEOVER,
    };

    Player* m_userPlayer;

    AppManager::PlaySetting m_playSetting;

    struct
    {
        int startChrono;
        int curChrono;
        tbe::ticks::Clock clock;
        time_t startTimestamp;

    } m_playTimeManager;

    tbe::ticks::Clock m_logClock;
    tbe::ticks::Clock m_newtonClock;

    tbe::scene::Camera* m_camera;

    tbe::Vector3f m_shootTarget;
    std::list<tbe::Vector3f> m_playerPosRec;

    TimtTo m_timeTo;
    bool m_gameOver;
};

#endif	/* _PLAYMODEMANAGER_H */
