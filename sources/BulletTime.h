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

class BulletTime
{
public:
    BulletTime(PlayManager* playManager);
    virtual ~BulletTime();

    void Process();

    void SetActive(bool active);
    bool IsActive() const;

    void SetValue(float value);
    float GetValue() const;

private:
    float m_value;
    bool m_active;
    Player* m_userPlayer;
    PlayManager* m_playManager;
};

#endif	/* BULLETTIME_H */

