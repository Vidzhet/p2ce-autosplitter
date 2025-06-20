#include "readmem.h"
#include "panorama.h"

bool speedrun_restarted = false;
bool speedrun_finished = false;

bool ent_cvar_protect = false;

bool timer_resume = false;
bool monitorStatusRunning = false;
void monitorStatus(ServerSplitter::Timer& timer)
{
    monitorStatusRunning = true;
    std::cout << "statusThread started.\n";
    bool timerOn = true;
    for (bool IsDrawingLoadingImage, IsInGame; true;) {
        IsDrawingLoadingImage = engine->IsDrawingLoadingImage();
        IsInGame = engine->IsInGame();
        if (speedrun_restarted || speedrun_finished || !livesplit_connected) { break; }
        //std::cout << "timerOn? " << timerOn << " status: " << value << " working...\n";
        if ((IsDrawingLoadingImage || !IsInGame || engine->IsPaused()) && timerOn) {
            timer.pauseGametime();
            timerOn = false;
            while (!timer.timerPaused()) { // awful fix yeah
#ifdef DEBUG
                std::thread(MessageBoxW, nullptr, L"---EXTRA TIMER PAUSE---", L"OMG!!! WHAT HAPPENED??", MB_OK).detach(); // dirty
#endif
                timer.pauseGametime();
            }
            std::cout << "Timer paused.\n";
        }
        else if (!IsDrawingLoadingImage && IsInGame && !engine->IsLevelMainMenuBackground() && !engine->IsPaused() && !timerOn) {
            timer_resume = true;
            while (timer_resume) { Sleep(1); } // await for changing this flag on the next frame
            timer.resumeGametime();
            timerOn = true;
            while (timer.timerPaused()) {
#ifdef DEBUG
                std::thread(MessageBoxW, nullptr, L"---EXTRA TIMER RESUME---", L"OMG!!! WHAT HAPPENED??", MB_OK).detach(); // dirty
#endif
                timer.resumeGametime();
            }
            std::cout << "Timer resumed.\n";

            // disable flashbang on sp_a4_finale
            /*if (map == MAP_FINALE) {
                MessageBoxA(nullptr, "Flashbang disabled!", "p2ce-autosplitter", MB_OK);
                engine->ClientCmd_Unrestricted("sv_cheats 1");
                engine->ClientCmd_Unrestricted("ent_remove fin_whitefade");
                engine->ClientCmd_Unrestricted("ent_remove credits_teleport");
                engine->ClientCmd_Unrestricted("ent_remove credits_camera");
                engine->ClientCmd_Unrestricted("sv_cheats 0");
            }*/
        }
        Sleep(1);
    }
    std::cout << "statusThread done working!\n";
    monitorStatusRunning = false;
}

bool monitorBspRunning = false;
// rewrite of monitorMap, now it uses exactly map bsp names instead of map ids
void monitorBsp(ServerSplitter::Timer& timer) {
    monitorBspRunning = true;
    std::cout << "bspThread started.\n";
    int splitIndexBuf;
    bool nextmap = false;
    std::string mapname_buffer;
    std::string mapname;
    std::vector<std::string> mapsbuffer = { "sp_a1_begin" };
    while (true) {
        if (engine->IsInGame()) { mapname = engine->GetLevelNameShort(); } // when next map is loading, GetLevelName() may not return any value and it will be unknown ptr so thats fix
        if (speedrun_restarted || speedrun_finished || !livesplit_connected) { break; }
        // if current map not in mapsbuffer
        if (std::find(mapsbuffer.begin(), mapsbuffer.end(), mapname) == mapsbuffer.end() && !engine->IsLevelMainMenuBackground() && !nextmap) {
            //std::cout << "split!\n";
            splitIndexBuf = timer.getSplitIndex();
            timer.split();
            while (splitIndexBuf == timer.getSplitIndex()) {
#ifdef DEBUG
                std::thread(MessageBoxW, nullptr, L"---EXTRA SPLIT---", L"OMG!!! WHAT HAPPENED???", MB_OK).detach();
#endif
                timer.split();
            }
            nextmap = true;
        }
        else if (mapname == mapname_buffer && nextmap) {
            nextmap = false;
        }
        // todo: fix the code below
        //if (bsp == "sp_a4_finale.bsp" && *reinterpret_cast<int*>(endAddress) == 255) {
        //    timer.split(); // final stop timer
        //    engine->ClientCmd_Unrestricted("play playonce\\level\\sad_party_horn_01.wav");
        //    std::string finaltime = "Unknown";
        //    if (livesplit_connected) {
        //        finaltime = timer.getGametime();
        //    }
        //    Sleep(2000);
        //    if (MessageBoxA(nullptr, std::string("Speedrun finished! Final time is: " + finaltime + "\nDo you want to skip the cutscene?").c_str(), "Speedrun finished!", MB_YESNO) == IDYES) {
        //        engine->ClientCmd_Unrestricted("disconnect");
        //    }
        //    speedrun_finished = true;
        //    break;
        //}
        mapname_buffer = mapname;
        // add only unique maps to mapsbuffer
        if (std::find(mapsbuffer.begin(), mapsbuffer.end(), mapname) == mapsbuffer.end()) {
            mapsbuffer.push_back(mapname);
        }
        Sleep(10);
    }
    std::cout << "bspThread done working!\n";
    monitorBspRunning = false;
}

bool recordDemoRunning = false;
void recordDemo() {
    recordDemoRunning = true;
    size_t i = 1;
    bool recording = false;
    std::wstring currentTimeW = getCurrentTimeW();
    std::string currentTimeA(currentTimeW.begin(), currentTimeW.end());
    //MessageBoxW(nullptr, std::wstring(gamePath + L"\\revolution\\demos\\" + currentTime).c_str(), L"current time:", MB_OK);
    CreateDirectoryW(std::wstring(gamePath + L"\\" + gameNameW + L"\\demos").c_str(), nullptr);
    CreateDirectoryW(std::wstring(gamePath + L"\\" + gameNameW + L"\\demos\\" + currentTimeW).c_str(), nullptr);
    while (true) {
        if (speedrun_restarted/* || speedrun_finished*/) { break; }
        if (!engine->IsInGame() && recording) { // preload = 0 when map loaded, != 0 when game initializes world
            engine->ClientCmd_Unrestricted("stop"); // stop recording demo
            std::cout << "stop\n";
            recording = false;
        }
        else if (engine->IsInGame() && !engine->IsDrawingLoadingImage() && !engine->IsLevelMainMenuBackground() && !recording) { // check if map name starts with menu_
            engine->ClientCmd_Unrestricted("stop");
            engine->ClientCmd_Unrestricted(std::string("record demos\\" + currentTimeA + '\\' + std::to_string(i) + '-' + engine->GetLevelNameShort()).c_str());
            //while (*reinterpret_cast<int*>(statusAddress) != 1) { // stupid code
            //    engine->ClientCmd_Unrestricted(std::string("record demos\\" + std::string(currentTime.begin(), currentTime.end()) + "\\" + std::to_string(i) + "-" + bsp.substr(0, bsp.size() - 4)).c_str());
            //    Sleep(10);
            //    std::cout << "trying to record...\n";
            //    //std::cout << bsp.c_str() << "\t" << std::string("record demos\\" + std::string(currentTime.begin(), currentTime.end()) + "\\" + std::to_string(i) + "-" + bsp.substr(0, bsp.size() - 4)).c_str() << std::endl;
            //}
            std::cout << "recording: " << engine->GetLevelNameShort() << '\t' << i << std::endl;
            recording = true;
            i++;
        }
        // todo: figure out what tf is that code?? or just remove it
        /*else if (status == 1 && !recording) {
            engine->ClientCmd_Unrestricted("stop");
            engine->ClientCmd_Unrestricted(std::string("record demos\\" + std::string(currentTime.begin(), currentTime.end()) + "\\" + std::to_string(i) + "-" + bsp.substr(0, bsp.size() - 4)).c_str());
            std::cout << "else\n";
            recording = true;
            i++;
        }*/
        // todo: fix the code below
        /*if (!monitorBspRunning && bsp == "sp_a4_finale.bsp" && *reinterpret_cast<int*>(endAddress) == 255) {
            engine->ClientCmd_Unrestricted("play playonce\\level\\sad_party_horn_01.wav");
            std::string finaltime = "Unknown";
            Sleep(2000);
            if (MessageBoxA(nullptr, std::string("Speedrun finished! Final time is: " + finaltime + "\nDo you want to skip the cutscene?").c_str(), "Speedrun finished!", MB_YESNO) == IDYES) {
                engine->ClientCmd_Unrestricted("disconnect");
            }
            speedrun_finished = true;
            break;
        }*/
        static bool createEndSignalEntity = false;
        if (createEndSignalEntity && speedrun_endMap != engine->GetLevelNameShort()) {
            createEndSignalEntity = false;
        }
        else if (!createEndSignalEntity && engine->IsInGame() && speedrun_endMap == engine->GetLevelNameShort()) {
            createEndSignalEntity = true;
            g_pCVar->FindVar("ent_fire")->RemoveFlags(FCVAR_CHEAT);
            g_pCVar->FindVar("ent_create")->RemoveFlags(FCVAR_CHEAT);
            engine->ClientCmd_Unrestricted("ent_create point_clientcommand targetname speedrun_endsignal");
            engine->ClientCmd_Unrestricted((std::string("ent_fire ") + speedrun_endInputEntity + " addoutput \"" + speedrun_endInput + " speedrun_endsignal:command:speedrun_signal_end:-1\"").c_str());
            //engine->ClientCmd_Unrestricted("ent_fire knopka1 addoutput \"OnPressed speedrun_endsignal:command:restart:-1\"");
            //MessageBoxA(nullptr, (std::string("ent_fire ") + speedrun_endInputEntity + " addoutput \"" + speedrun_endInput + " speedrun_endsignal:command:timer_split:-1\"").c_str(), "a", 0L);
            ent_cvar_protect = true;
        }

        if (speedrun_finished) {
            engine->ClientCmd_Unrestricted("stop");
            engine->ClientCmd_Unrestricted("play playonce\\level\\sad_party_horn_01.wav");
            std::string finaltime = "Unknown";
            if (g_pTimer && livesplit_connected) {
                finaltime = reinterpret_cast<ServerSplitter::Timer*>(g_pTimer)->getGametime();
            }
            Sleep(2000);
            if (MessageBoxA(nullptr, std::string("Speedrun finished! Final time is: " + finaltime + "\nDo you want to skip the cutscene?").c_str(), "Speedrun finished!", MB_YESNO) == IDYES) {
                engine->ClientCmd_Unrestricted("disconnect");
            }
            break;
        }

        Sleep(10);
    }
    std::cout << "demoThread done working!\n";
    recordDemoRunning = false;
}

void monitorReset(ServerSplitter::Timer& timer)
{
    while (true) {
        try
        {
            bool restarted = false;
            while (true) {
                if (engine->IsInGame() && !engine->IsDrawingLoadingImage() && restarted) {
                    while (monitorStatusRunning || monitorBspRunning || recordDemoRunning) { Sleep(100); } // wait till threads done working
                    speedrun_restarted = false;
                    speedrun_finished = false;
                    restarted = false;
                    std::thread(recordDemo).detach();
                    if (livesplit_connected) {
                        timer.start();
                        while (timer.timerPaused()) {
                            timer.start();
                        }
                        timer.setgametime("0.00");
                        std::thread(monitorStatus, std::ref(timer)).detach();
                        std::thread(monitorBsp, std::ref(timer)).detach(); // using bsp monitoring method instead of id monitoring
                    }
                    std::cout << "Speedrun started!\n";
                }
                else if (speedrun_restarted && !restarted) {
                    if (livesplit_connected) {
                        timer.reset();
                        timer.sendCommand("switchto gametime");
                    }
                    restarted = true;
                    std::cout << "Restarted...\n";
                }
                Sleep(100);
            }
        }
        catch (const std::runtime_error&)
        {
            Sleep(2000);
            continue;
        }
    } 
}