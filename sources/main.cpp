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

    #ifndef THEBALL_COMPILE_DEBUG
    ofstream log("theball.log");
    streambuf* defaultCout = cout.rdbuf();
    cout.rdbuf(log.rdbuf());
    #endif

    try
    {
        cout
                << "theBall (b4n92uid)" << endl
                << "Start in " << second_clock::local_time() << endl
                << "Build in " << __DATE__ << endl
                << current_path() << endl;

        AppManager theBall;

        int index = -1;

        if((index = isarg(argc, argv, "nomusic")) != -1)
            theBall.globalSettings.nomusic = true;

        if((index = isarg(argc, argv, "noaudio")) != -1)
            theBall.globalSettings.noaudio = true;

        if((index = isarg(argc, argv, "play")) != -1)
        {
            // NOTE play <player> <level> <playerCount> <playMod> <playtime>

            Settings::PartySetting ps;
            ps.playerName = Settings::PlayerInfo(argv[index + 1]);
            ps.playMap = Settings::MapInfo(argv[index + 2]);
            ps.playerCount = tools::StrToNum<unsigned>(argv[index + 3]);
            ps.playMod = tools::StrToNum<unsigned> (argv[index + 4]);
            ps.playTime = tools::StrToNum<int>(argv[index + 5]);

            theBall.ExecuteGame(ps);
        }

        else if((index = isarg(argc, argv, "edit")) != -1)
        {
            // NOTE edit <level>

            Settings::EditSetting es;
            es.editMap = Settings::MapInfo(argv[index + 1]);
            es.createNew = false;

            theBall.ExecuteEditor(es);
        }

        else
        {
            theBall.ExecuteMenu();
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

    #ifndef THEBALL_COMPILE_DEBUG
    cout.rdbuf(defaultCout);
    log.close();
    #endif

    return 0;
}
