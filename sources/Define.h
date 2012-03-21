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
    MENU_SETTING_KEYS,
    MENU_LOAD,

    SCREEN_HUD,
    SCREEN_GAMEOVER,
    SCREEN_PAUSEMENU,
};

struct alltrue
{
    typedef bool result_type;

    template<typename InputIterator>
            bool operator()(InputIterator first, InputIterator last) const
    {
        if(first == last)
            return false;

        for(; first != last; ++first)
        {
            if(!*first)
                return false;
        }

        return true;
    }
};

#endif	/* _DEFINE_H */

