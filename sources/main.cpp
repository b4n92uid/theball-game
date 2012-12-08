/*
 * File:   main.cpp
 * Author: b4n92uid
 *
 * Created on 13 août 2010, 23:34
 */

#include "AppManager.h"

using namespace std;
using namespace tbe;

inline int isarg(int argc, char** argv, const char* required)
{
    for(int i = 0; i < argc; i++)
        if(strcmp(argv[i], required) == 0)
            return i;

    return -1;
}

int main(int argc, char** argv)
{
    using namespace boost::posix_time;
    using namespace boost::filesystem;

    #ifdef NDEBUG
    ofstream log("theball.log");
    streambuf* defaultCout = cout.rdbuf();
    cout.rdbuf(log.rdbuf());
    #endif

    try
    {
        cout
                #ifdef NDEBUG
                << "theBall (" << __DATE__ << ") Release run" << endl
                #else
                << "theBall (" << __DATE__ << ") Debug run" << endl
                #endif
                << endl
                << second_clock::local_time() << endl
                << current_path() << endl
                << endl;

        AppManager theBall;

        int index = -1;

        if((index = isarg(argc, argv, "noaudio")) != -1)
            theBall.globalSettings.noaudio = true;

        if((index = isarg(argc, argv, "play")) != -1)
        {
            // NOTE play <level> <player>

            Content::PartySetting ps;

            ps.map = new Content::MapInfo(&theBall, argv[index + 1]);
            ps.player = new Content::PlayerInfo(&theBall, argv[index + 2]);

            theBall.executeGame(ps);
        }

        else
        {
            theBall.executeMenu();
        }

    }

    catch(std::exception& e)
    {
        #ifdef COMPILE_FOR_WINDOWS
        MessageBox(0, e.what(), "Fatal error", MB_ICONERROR);
        #endif
        cout << e.what() << endl;
    }

    catch(...)
    {
        cout << "Unhandled exception catch" << endl;
    }

    cout << "Exit success :)" << endl;

    #ifdef NDEBUG
    cout.rdbuf(defaultCout);
    log.close();
    #endif

    return 0;
}
