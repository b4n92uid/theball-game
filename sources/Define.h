#ifndef _DEFINE_H
#define	_DEFINE_H

#define CAPTION_TITLE "theBall (" __DATE__ ")"

#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

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

class Weapon;
class Power;
class AIControl;
class Player;

typedef std::vector<Weapon*> WeaponArray;

typedef std::vector<Power*> PowerArray;

typedef std::map<Player*, AIControl*> AIControlMap;

#endif	/* _DEFINE_H */

