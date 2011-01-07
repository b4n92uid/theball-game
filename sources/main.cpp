/*
 * File:   main.cpp
 * Author: b4n92uid
 *
 * Created on 13 août 2010, 23:34
 */

#include "AppManager.h"

using namespace std;
using namespace tbe;

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

        if(argc > 1)
        {
            if(string(argv[1]) == "play")
            {
                // NOTE play <player> <level> <playerCount> <playMod> <playtime>

                Settings::PartySetting ps;
                ps.playerName = Settings::PlayerInfo(argv[2]);
                ps.playMap = Settings::MapInfo(argv[3]);
                ps.playerCount = tools::StrToNum<unsigned>(argv[4]);
                ps.playMod = tools::StrToNum<unsigned> (argv[5]);
                ps.playTime = tools::StrToNum<int>(argv[6]);

                theBall.ExecuteGame(ps);
            }

            else if(string(argv[1]) == "edit")
            {
                // NOTE edit <level>

                Settings::EditSetting es;
                es.editMap = Settings::MapInfo(argv[2]);
                es.createNew = false;

                theBall.ExecuteEditor(es);
            }
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
