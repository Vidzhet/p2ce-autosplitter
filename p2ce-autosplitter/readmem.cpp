#include "readmem.h"
#include "panorama.h"

bool speedrun_restarted = false;
bool speedrun_finished = false;

bool monitorStatusRunning = false;
void monitorStatus(ServerSplitter::Timer& timer)
{
    monitorStatusRunning = true;
    std::cout << "statusThread started.\n";
    bool timerOn = true;
    for (int status, statusExtra = 0; true;) {
        status = *reinterpret_cast<int*>(statusAddress);
        statusExtra = *reinterpret_cast<int*>(statusExtraAddress);
        if (speedrun_restarted || speedrun_finished || !livesplit_connected) { break; }
        //std::cout << "Status value:\t" << status << std::endl;
        //std::cout << "timerOn? " << timerOn << " status: " << value << " working...\n";
        if ((status == 0 || statusExtra == 0)  && timerOn) {
            timer.pauseGametime();
            timerOn = false;
            while (!timer.timerPaused()) { // awful fix yeah
#ifdef DEBUG
                std::thread MessageBoxThread(MessageBoxW, nullptr, L"---EXTRA TIMER PAUSE---", L"OMG!!! WHAT HAPPENED??", MB_OK); // dirty
                MessageBoxThread.detach();
#endif
                timer.pauseGametime();
            }
            std::cout << "Timer paused.\n";
        }
        else if (status == 1 && statusExtra == 1 && !timerOn) {
            timer.resumeGametime();
            timerOn = true;
            while (timer.timerPaused()) {
#ifdef DEBUG
                std::thread MessageBoxThread(MessageBoxW, nullptr, L"---EXTRA TIMER RESUME---", L"OMG!!! WHAT HAPPENED??", MB_OK); // dirty
                MessageBoxThread.detach();
#endif
                timer.resumeGametime();
            }
            std::cout << "Timer resumed.\n";

            // disable flashbang on sp_a4_finale
            /*if (map == MAP_FINALE) {
                MessageBoxA(nullptr, "Flashbang disabled!", "p2ce-autosplitter", MB_OK);
                engine->ConsoleCommand("sv_cheats 1");
                engine->ConsoleCommand("ent_remove fin_whitefade");
                engine->ConsoleCommand("ent_remove credits_teleport");
                engine->ConsoleCommand("ent_remove credits_camera");
                engine->ConsoleCommand("sv_cheats 0");
            }*/
        }
        Sleep(1);
    }
    std::cout << "statusThread done working!\n";
    monitorStatusRunning = false;
}
#ifdef oldsplitting
void monitorMap(const HANDLE &processHandle, const LPCVOID &mapAddress, ServerSplitter::Timer& timer) // obsolete
{
    std::cout << "mapThread started.\n";
    bool nextmap = false;
    int buffer = 0;
    std::set<int> mapsbuffer = {0 , MAP_WAKEUP_SAVE, MAP_MAIN_MENU};
    for (int value = MAP_WAKEUP_SAVE; true; ReadProcessMemory(processHandle, mapAddress, &value, sizeof(value), nullptr)) {
        //std::cout << "Map value:\t" << value << std::endl;
        //std::cout << "Map value:\t" << *nextMapPtr << std::endl;
        if (value == MAP_WAKEUP_TRIGGER) {
            break;
        }
        // if current map not in mapsbuffer
        if (mapsbuffer.find(value) == mapsbuffer.end() && !nextmap) {
            //std::cout << "split!\n";
            timer.resume();
            timer.split();
            timer.pause();
            timer.setgametime(timer.gettime()); // sets segment time to gametime
            nextmap = true;
        }
        else if (value == buffer && nextmap) {
            nextmap = false;
            // 1135606784 points to 2 (~too-too!!) different maps. can cause bugs with splitting
            if (mapsbuffer.find(1135606784) != mapsbuffer.end() && value == 1151197411) {
                mapsbuffer.erase(1135606784); // deleting it from mapsbuffer when player completed this map.to avoid inability to split when another map loads with this id
            }
            //std::cout << "nextmap=false\n";
        }
        buffer = value;
        mapsbuffer.insert(buffer);
        Sleep(1);
    }
    std::cout << "mapThread done working!\n";
}

void monitorBsp(const HANDLE& processHandle, const LPCVOID& bspAddress, const LPCVOID& endAddress, ServerSplitter::Timer& timer) {
    std::cout << "bspThread started.\n";
    bool nextmap = false;
    int end = 0;
    char* buffer = nullptr;
    std::string bsp;
    std::vector<std::string> mapsbuffer = { "sp_a1_begin.bsp", "menu_post.bsp" };
    for (char value[64] = "sp_a1_begin.bsp"; true; ReadProcessMemory(processHandle, bspAddress, &value, sizeof(value)-1, nullptr)) {
        value[sizeof(value) - 1] = '\0';
        bsp = value;
        //std::cout << "Map value:\t" << value << std::endl;
        if (bsp == "wakeup_trigger.bsp") {
            break;
        }
        // if current map not in mapsbuffer
        if (std::find(mapsbuffer.begin(), mapsbuffer.end(), value) == mapsbuffer.end() && !nextmap) {
            //std::cout << "split!\n";
            timer.resume();
            if (value == "sp_a1_begin.bsp") {
                Sleep(1);
            }
            timer.split();
            timer.pause();
            timer.setgametime(timer.gettime()); // sets segment time to gametime
            nextmap = true;
        }
        else if (value == buffer && nextmap) {
            nextmap = false;
        }
        ReadProcessMemory(processHandle, endAddress, &end, sizeof(end), nullptr);
        if (value == "sp_a4_finale.bsp" && end == 255) {
            timer.split(); // final stop timer
        }
        buffer = value;
        // add only unique maps to mapsbuffer
        if (std::find(mapsbuffer.begin(), mapsbuffer.end(), bsp) == mapsbuffer.end()) {
            mapsbuffer.push_back(value);
        }
        Sleep(1);
    }
    std::cout << "bspThread done working!\n";
}
#else
bool monitorBspRunning = false;
// rewrite of monitorMap, now it uses exactly map bsp names instead of map ids
void monitorBsp(ServerSplitter::Timer& timer) {
    monitorBspRunning = true;
    std::cout << "bspThread started.\n";
    int splitIndexBuf;
    bool nextmap = false;
    std::string buffer;
    std::string bsp;
    std::vector<std::string> mapsbuffer = { "sp_a1_begin.bsp", "menu_post.bsp" };
    while (true) {
        //value[sizeof(value) - 1] = '\0';
        bsp = reinterpret_cast<char*>(bspAddress);
        //std::cout << "Map value:\t" << value << std::endl;
        if (speedrun_restarted || speedrun_finished || !livesplit_connected) { break; }
        // if current map not in mapsbuffer
        if (std::find(mapsbuffer.begin(), mapsbuffer.end(), bsp) == mapsbuffer.end() && !nextmap) {
            //std::cout << "split!\n";
            splitIndexBuf = timer.getSplitIndex();
            timer.split();
            while (splitIndexBuf == timer.getSplitIndex()) {
#ifdef DEBUG
                std::thread MessageBoxThread(MessageBoxW, nullptr, L"---EXTRA SPLIT---", L"OMG!!! WHAT HAPPENED???", MB_OK);
                MessageBoxThread.detach();
#endif
                timer.split();
            }
            nextmap = true;
        }
        else if (bsp == buffer && nextmap) {
            nextmap = false;
        }
        if (bsp == "sp_a4_finale.bsp" && *reinterpret_cast<int*>(endAddress) == 255) {
            timer.split(); // final stop timer
            engine->ConsoleCommand("play playonce\\level\\sad_party_horn_01.wav");
            std::string finaltime = "Unknown";
            if (livesplit_connected) {
                finaltime = timer.getGametime();
            }
            Sleep(2000);
            if (MessageBoxA(nullptr, std::string("Speedrun finished! Final time is: " + finaltime + "\nDo you want to skip the cutscene?").c_str(), "Speedrun finished!", MB_YESNO) == IDYES) {
                engine->ConsoleCommand("disconnect");
            }
            speedrun_finished = true;
            break;
        }
        buffer = bsp;
        // add only unique maps to mapsbuffer
        if (std::find(mapsbuffer.begin(), mapsbuffer.end(), bsp) == mapsbuffer.end()) {
            mapsbuffer.push_back(bsp);
        }
        Sleep(10);
    }
    std::cout << "bspThread done working!\n";
    monitorBspRunning = false;
}
#endif

bool recordDemoRunning = false;
void recordDemo() {
    recordDemoRunning = true;
    int i = 1;
    int status = 0;
    bool recording = false;
    std::string bsp;
    std::wstring currentTime = getCurrentTimeW();
    //MessageBoxW(nullptr, std::wstring(gamePath + L"\\revolution\\demos\\" + currentTime).c_str(), L"current time:", MB_OK);
    CreateDirectoryW(std::wstring(gamePath + L"\\" + gameName + L"\\demos").c_str(), nullptr);
    CreateDirectoryW(std::wstring(gamePath + L"\\" + gameName + L"\\demos\\" + currentTime).c_str(), nullptr);
    while (true) {
        //value[sizeof(value) - 1] = '\0';
        status = *reinterpret_cast<int*>(statusAddress);
        bsp = reinterpret_cast<char*>(bspAddress);
        //std::cout << status << std::endl;
        if (speedrun_restarted || speedrun_finished) { break; }
        if (status == 0 && *reinterpret_cast<int*>(preloadAddress) == 1 && recording) { // preload = 0 when map loaded, != 0 when game initializes world
            engine->ConsoleCommand("stop"); // stop recording demo
            std::cout << "stop\n";
            recording = false;
        }
        else if (status == 0 && *reinterpret_cast<int*>(preloadAddress) != 1 && !recording && std::string(reinterpret_cast<char*>(bspAddress), 5) != "menu_") { // check if map name starts with menu_
            engine->ConsoleCommand("stop");
            while (*reinterpret_cast<int*>(statusAddress) != 1) {
                engine->ConsoleCommand(std::string("record demos\\" + std::string(currentTime.begin(), currentTime.end()) + "\\" + std::to_string(i) + "-" + bsp.substr(0, bsp.size() - 4)).c_str());
                Sleep(10);
                std::cout << "trying to record...\n";
                //std::cout << bsp.c_str() << "\t" << std::string("record demos\\" + std::string(currentTime.begin(), currentTime.end()) + "\\" + std::to_string(i) + "-" + bsp.substr(0, bsp.size() - 4)).c_str() << std::endl;
            }
            std::cout << "recording: " << bsp.c_str() << "\t" << i <<std::endl;
            recording = true;
            i++;
        }
        else if (status == 1 && !recording) {
            engine->ConsoleCommand("stop");
            engine->ConsoleCommand(std::string("record demos\\" + std::string(currentTime.begin(), currentTime.end()) + "\\" + std::to_string(i) + "-" + bsp.substr(0, bsp.size() - 4)).c_str());
            std::cout << "else\n";
            recording = true;
            i++;
        }
        if (!monitorBspRunning && bsp == "sp_a4_finale.bsp" && *reinterpret_cast<int*>(endAddress) == 255) {
            engine->ConsoleCommand("play playonce\\level\\sad_party_horn_01.wav");
            std::string finaltime = "Unknown";
            Sleep(2000);
            if (MessageBoxA(nullptr, std::string("Speedrun finished! Final time is: " + finaltime + "\nDo you want to skip the cutscene?").c_str(), "Speedrun finished!", MB_YESNO) == IDYES) {
                engine->ConsoleCommand("disconnect");
            }
            speedrun_finished = true;
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
            std::string bsp;
            for (int status = 0; true;) {
                status = *reinterpret_cast<int*>(statusAddress);
                bsp = reinterpret_cast<char*>(bspAddress);
                if (status == 1 && bsp == "sp_a1_begin.bsp" && restarted) {
                    while (monitorStatusRunning || monitorBspRunning || recordDemoRunning) { Sleep(100); } // wait till threads done working
                    speedrun_restarted = false;
                    speedrun_finished = false;
                    restarted = false;
                    std::thread demoThread(recordDemo);
                    if (livesplit_connected) {
                        timer.start();
                        while (timer.timerPaused()) {
                            timer.start();
                        }
                        timer.setgametime("0.00");
                        std::thread statusThread(monitorStatus, std::ref(timer));
                        std::thread bspThread(monitorBsp, std::ref(timer)); // using bsp monitoring method instead of id monitoring
                        statusThread.detach();
                        bspThread.detach();
                    }
                    demoThread.detach();
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