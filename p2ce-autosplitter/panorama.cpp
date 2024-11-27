#include "panorama.h"
#include "externs.h"

#define IDINAHUI_CONSOLELOG

uintptr_t consolelogAddress; // global var for console_log()
HANDLE processHandleBuffer; // this aswell
bool livesplit_connected = false;

void editMainMenu(HANDLE processHandle, uintptr_t panoramaBaseAddress, uintptr_t gameoverlayrenderer64BaseAddress)
{
    //uintptr_t scriptAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4E58, scriptOffsets);
    //WriteProcessMemory(processHandle, (LPVOID)scriptAddress, "map wakeup_trigger", 32, NULL);

    //uintptr_t labelAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4B00, labelOffsets);
    //std::cout << std::hex << labelAddress << std::dec << std::endl;
    //WriteProcessMemory(processHandle, (LPVOID)labelAddress, "START SPEEDRUN", 32, NULL);
}

void start_loglabel(ServerSplitter::Timer& timer) {
    uintptr_t loglabelRedAddress = ptrOffset(panoramaBaseAddress + 0x004F4E58, loglabelRedOffsets);
    uintptr_t loglabelGreenAddress = ptrOffset(panoramaBaseAddress + 0x004F4E58, loglabelGreenOffsets);
    //std::cout << "green address: " << std::hex << loglabelGreenAddress << std::dec << std::endl;
    while (std::string(reinterpret_cast<char*>(loglabelGreenAddress)) != "    LiveSplit connected!  ") {}

    //processHandleBuffer = processHandle; // init for consolelog
    //consolelogAddress = ptrOffset(panoramaBaseAddress + 0x004F4E58, consolelogOffsets); // init consolelog
    //strcpy_s(reinterpret_cast<char*>(consolelogAddress), 59, "                                                          "); // idi nahui consolelog
    strcpy_s(reinterpret_cast<char*>(loglabelGreenAddress), 31, "                              "); // write str to memory
    strcpy_s(reinterpret_cast<char*>(loglabelRedAddress), 40, "                                       "); // makes loglabel invisible

    //if (std::string(reinterpret_cast<char*>(consolelogAddress)) != "                                                          ") { // check if pizdec
    //    MessageBoxA(nullptr, "FATAL ERROR\np2ce-autosplitter just died, please restart the game.", "PIZDEC", MB_OK | MB_ICONERROR);
    //    //throw std::exception("pizdec");
    //    TerminateProcess(GetCurrentProcess(), 0); // UBIT!!! (https://youtu.be/XzzMS38gYtM?si=_als787yUUnv7yj4)
    //}

    // wait till panorama loads.
    while (*reinterpret_cast<int*>(onGameLoadAddress) != GAME_LOADED) { Sleep(100); }

    std::cout << "game loaded panorama\n";

    while (true) {
        try {
            timer.sendCommand("ping");
            //console_log("send err. Connection restored");
            strcpy_s(reinterpret_cast<char*>(loglabelRedAddress), 40, "                                       ");
            strcpy_s(reinterpret_cast<char*>(loglabelGreenAddress), 22, "LiveSplit connected! ");
            livesplit_connected = true;
        }
        catch (const std::runtime_error&) {
            //console_log("priv");
            //console_log("LiveSplit is not connected!");
            strcpy_s(reinterpret_cast<char*>(loglabelGreenAddress), 30, "                             ");
            strcpy_s(reinterpret_cast<char*>(loglabelRedAddress), 29, "LiveSplit is not connected! ");
            //std::cout << ex.what() << std::endl;
            livesplit_connected = false;
            timer = ServerSplitter::createTimer();
        }
        Sleep(2000);
    }
}

#ifndef IDINAHUI_CONSOLELOG

// string arg must be 30 symbols or lower!
void console_log(std::string str) { // FUCK THIS SHIT
    std::string temp(45 - str.size()*1.5, ' ');
    temp = temp + "...console-log: " + str + "...";
    WriteProcessMemory(processHandleBuffer, (LPVOID)consolelogAddress, temp.c_str(), 59, NULL);
}
#endif