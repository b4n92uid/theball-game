/* 
 * File:   BulletTime.h
 * Author: b4n92uid
 *
 * Created on 27 juillet 2010, 13:48
 */

#ifndef BULLETTIME_H
#define	BULLETTIME_H

class Player;
class PlayManager;
class SoundManager;

class BulletTime
{
public:
    BulletTime(PlayManager* playManager);
    virtual ~BulletTime();

    void process();

    void setActive(bool active);
    bool isActive() const;

    void setValue(float value);
    float getValue() const;

private:
    float m_value;
    bool m_active;
    Player* m_userPlayer;
    PlayManager* m_playManager;
    SoundManager* m_soundManager;
};

#endif	/* BULLETTIME_H */

