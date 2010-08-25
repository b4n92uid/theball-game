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
                // NOTE play <name> <player> <level> <playerCount> <playMod> <playtime>

                AppManager::PlaySetting ps;
                ps.playerName = argv[2];
                ps.playerModel = argv[3];
                ps.playMap = argv[4];
                ps.playerCount = tools::StrToNum<unsigned>(argv[5]);
                ps.playMod = tools::StrToNum<unsigned> (argv[6]);
                ps.playTime = tools::StrToNum<int>(argv[7]);

                theBall.ExecuteGame(ps);
            }

            else if(string(argv[1]) == "edit")
            {
                // NOTE edit <level>

                AppManager::EditSetting es;
                es.editMap = argv[2];
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
