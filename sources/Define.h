#ifndef _DEFINE_H
#define	_DEFINE_H

#define CAPTION_TITLE "theBall ("__DATE__")"

#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

enum GuiSess
{
    MENU_MAIN,
    MENU_ABOUT,
    MENU_SETTING,
    MENU_MAPCHOOSE,
    MENU_PLAYERCHOOSE,
    MENU_CAMPAIGN,
    MENU_SETTING_KEYS,
    MENU_LOAD,

    SCREEN_HUD,
    SCREEN_GAMEOVER,
    SCREEN_PAUSEMENU,
};

#endif	/* _DEFINE_H */

