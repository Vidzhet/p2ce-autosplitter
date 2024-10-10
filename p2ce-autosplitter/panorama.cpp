#include "panorama.h"

uintptr_t consolelogAddress; // global var for console_log()
HANDLE processHandleBuffer; // this aswell

void editMainMenu(HANDLE processHandle, uintptr_t panoramaBaseAddress, uintptr_t gameoverlayrenderer64BaseAddress)
{
    //uintptr_t scriptAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4E58, scriptOffsets);
    //WriteProcessMemory(processHandle, (LPVOID)scriptAddress, "map wakeup_trigger", 32, NULL);

    //uintptr_t labelAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4B00, labelOffsets);
    //std::cout << std::hex << labelAddress << std::dec << std::endl;
    //WriteProcessMemory(processHandle, (LPVOID)labelAddress, "START SPEEDRUN", 32, NULL); // 32 bytes is limit for this type of strings for engine, higher value can cause many errors
}

void start_loglabel(HANDLE processHandle, uintptr_t panoramaBaseAddress, LPCVOID mapAddress, ServerSplitter::Timer& timer) {
    // wait till game loads.
    for (int value = 0; value != MAP_MAIN_MENU; ReadProcessMemory(processHandle, mapAddress, &value, sizeof(value), nullptr)) {}
    uintptr_t loglabelRedAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4E58, loglabelRedOffsets);
    uintptr_t loglabelGreenAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4E58, loglabelGreenOffsets);
    std::cout << "green address: " << std::hex << loglabelGreenAddress << std::dec << std::endl;
    for (char value[64] = "null"; true; ReadProcessMemory(processHandle, (LPCVOID)loglabelGreenAddress, &value, sizeof(value) - 1, nullptr)) {
        std::string str = value;
        if (str == "    LiveSplit connected!  ") {
            break;
        }
    }
    processHandleBuffer = processHandle; // init for consolelog
    consolelogAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4E58, consolelogOffsets); // init consolelog
    //WriteProcessMemory(processHandle, (LPVOID)consolelogAddress, "                                         console-log: ... ", 59, NULL); // init consolelog
    WriteProcessMemory(processHandle, (LPVOID)consolelogAddress, "                                                          ", 59, NULL); // idi nahui consol-log
    //std::cout << "ready.\n";
    WriteProcessMemory(processHandle, (LPVOID)loglabelGreenAddress, "                              ", 31, NULL); // makes green loglabel invisible
    WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, "                                       ", 40, NULL);
    //WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, "LiveSplit is not connected! ", 29, NULL);
    //Sleep(5000);

    while (true) {
        try {
            timer.sendCommand("switchto gametime");
            //console_log("send err. Connection restored");
            WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, "                                       ", 40, NULL);
            WriteProcessMemory(processHandle, (LPVOID)loglabelGreenAddress, "LiveSplit connected! ", 30, NULL);
        }
        catch (std::runtime_error& ex) {
            //console_log("priv");
            //console_log("LiveSplit is not connected!");
            WriteProcessMemory(processHandle, (LPVOID)loglabelGreenAddress, "                             ", 30, NULL);
            WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, "LiveSplit is not connected! ", 40, NULL);
            //std::cout << ex.what() << std::endl;
            timer = ServerSplitter::createTimer();
        }
        Sleep(2000);
    }
}

// string arg must be 30 symbols or lower!
void console_log(std::string str) { // FUCK THIS SHIT
    std::string temp(45 - str.size()*1.5, ' ');
    temp = temp + "...console-log: " + str + "...";
    WriteProcessMemory(processHandleBuffer, (LPVOID)consolelogAddress, temp.c_str(), 59, NULL);
}