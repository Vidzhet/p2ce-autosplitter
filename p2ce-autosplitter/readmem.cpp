#include "readmem.h"

void monitorStatus(const HANDLE &processHandle, const LPCVOID &targetAddress, const LPCVOID& mapAddress, ServerSplitter::Timer& timer)
{
    //std::cout << "statusThread started.\n";
    bool timerOn = true;
    for (int value = 0, map = 0; true; ReadProcessMemory(processHandle, targetAddress, &value, sizeof(value), nullptr), ReadProcessMemory(processHandle, mapAddress, &map, sizeof(map), nullptr)) {
        //std::cout << "Status value:\t" << value << std::endl;
        //std::cout << "Status value:\t" << *timerOnPtr << std::endl;
        if (value == 0 && timerOn) {
            timer.pause();
            timerOn = false;
        }
        else if (value == 1 && !timerOn) {
            timer.resume();
            timerOn = true;
        }
        if (map == MAP_WAKEUP_TRIGGER) {
            break;
        }
        Sleep(1);
    }
    //std::cout << "statusThread done working!\n";
}

void monitorMap(const HANDLE &processHandle, const LPCVOID &mapAddress, ServerSplitter::Timer& timer)
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
#ifdef oldsplitting
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
// rewrite of monitorMap, now it uses exactly map bsp names instead of map ids
void monitorBsp(const HANDLE& processHandle, const LPCVOID& bspAddress, const LPCVOID& endAddress, const LPCVOID& statusAddress, ServerSplitter::Timer& timer) {
    //std::cout << "bspThread started.\n";
    bool nextmap = false;
    bool split = false;
    int status = 0;
    int end = 0;
    char* buffer = nullptr;
    std::string bsp;
    std::vector<std::string> mapsbuffer = { "sp_a1_begin.bsp", "menu_post.bsp" };
    for (char value[64] = "sp_a1_begin.bsp"; true; ReadProcessMemory(processHandle, bspAddress, &value, sizeof(value) - 1, nullptr)) {
        value[sizeof(value) - 1] = '\0';
        bsp = value;
        //std::cout << "Map value:\t" << value << std::endl;
        if (bsp == "wakeup_trigger.bsp") {
            break;
        }
        // if current map not in mapsbuffer
        if (std::find(mapsbuffer.begin(), mapsbuffer.end(), value) == mapsbuffer.end() && !nextmap) {
            split = true;
            nextmap = true;
        }
        else if (value == buffer && nextmap) {
            nextmap = false;
        }
        ReadProcessMemory(processHandle, statusAddress, &status, sizeof(status), nullptr);
        if (split && status == 1) {
            //std::cout << "split!\n";
            timer.resume();
            timer.split();
            timer.setgametime(timer.gettime()); // sets segment time to gametime
            split = false;
        }
        ReadProcessMemory(processHandle, endAddress, &end, sizeof(end), nullptr);
        //std::cout << end << std::endl;
        if (bsp == "sp_a4_finale.bsp" && end == 255) {
            timer.split(); // final stop timer
            break;
        }
        buffer = value;
        // add only unique maps to mapsbuffer
        if (std::find(mapsbuffer.begin(), mapsbuffer.end(), bsp) == mapsbuffer.end()) {
            mapsbuffer.push_back(value);
        }
        Sleep(1);
    }
    //std::cout << "bspThread done working!\n";
}
#endif
void monitorReset(const HANDLE& processHandle, const LPCVOID& statusAddress, const LPCVOID& mapAddress, const LPCVOID& bspAddress, const LPCVOID& endAddress, ServerSplitter::Timer& timer)
{
    while (true) {
        try
        {
            timer.sendCommand("switchto gametime");
            bool restarted = false;
            for (int status = 0, map = 0; true; ReadProcessMemory(processHandle, statusAddress, &status, sizeof(status), nullptr), ReadProcessMemory(processHandle, mapAddress, &map, sizeof(map), nullptr)) {
                if (status == 1 && map == MAP_WAKEUP_SAVE && restarted) {
                    timer.start();
                    timer.setgametime("0.00");
                    restarted = false;
                    std::thread statusThread(monitorStatus, processHandle, (LPCVOID)statusAddress, (LPCVOID)mapAddress, std::ref(timer));
                    std::thread bspThread(monitorBsp, processHandle, (LPCVOID)bspAddress, (LPCVOID)endAddress, (LPCVOID)statusAddress, std::ref(timer)); // using bsp monitoring method instead of id monitoring
                    //std::thread mapThread(monitorMap, processHandle, (LPCVOID)mapAddress, std::ref(timer));
                    statusThread.detach();
                    bspThread.detach();
                    //mapThread.detach();
                }
                else if (map == MAP_WAKEUP_TRIGGER && !restarted) {
                    timer.reset();
                    restarted = true;
                }
                Sleep(1);
            }
        }
        catch (const std::runtime_error& ex)
        {
            Sleep(2000);
            continue;
        }
    } 
}