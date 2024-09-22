#include "panorama.h"

void editMainMenu(HANDLE processHandle, uintptr_t panoramaBaseAddress, uintptr_t gameoverlayrenderer64BaseAddress)
{
    //uintptr_t scriptAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4E58, scriptOffsets);
    //WriteProcessMemory(processHandle, (LPVOID)scriptAddress, "map wakeup_trigger", 32, NULL);

    //uintptr_t labelAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4B00, labelOffsets);
    //std::cout << std::hex << labelAddress << std::dec << std::endl;
    //WriteProcessMemory(processHandle, (LPVOID)labelAddress, "START SPEEDRUN", 32, NULL); // 32 bytes is limit for this type of strings for engine, higher value can cause many errors
}

void start_loglabel(HANDLE processHandle, uintptr_t panoramaBaseAddress, ServerSplitter::Timer& timer) {
    uintptr_t loglabelRedAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x00555F28, loglabelRedOffsets);
    uintptr_t loglabelGreenAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4E58, loglabelGreenOffsets);
    WriteProcessMemory(processHandle, (LPVOID)loglabelGreenAddress, " ", 22, NULL); // makes green loglabel invisible

    Sleep(1000);
    while (true) {
        try {
            timer.sendCommand("switchto gametime");
            std::cout << "LiveSplit connected!\n";
            WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, " ", 32, NULL);
            WriteProcessMemory(processHandle, (LPVOID)loglabelGreenAddress, "LiveSplit connected! ", 22, NULL);
        }
        catch (std::runtime_error& ex) {
            std::cout << "LiveSplit is not connected!\n";
            WriteProcessMemory(processHandle, (LPVOID)loglabelGreenAddress, " ", 22, NULL);
            WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, "LiveSplit is not connected! ", 32, NULL);
            //std::cout << ex.what() << std::endl;
            timer = ServerSplitter::createTimer();
        }
        Sleep(2000);
    }
}