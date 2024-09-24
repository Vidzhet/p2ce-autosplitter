#include "panorama.h"

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
        if (str == "    LiveSplit connected! ") {
            break;
        }
    }
    std::cout << "ready.\n";
    WriteProcessMemory(processHandle, (LPVOID)loglabelGreenAddress, "                              ", 31, NULL); // makes green loglabel invisible
    WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, "                                       ", 40, NULL);
    //WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, "LiveSplit is not connected! ", 29, NULL);
    //Sleep(5000);

    while (true) {
        try {
            timer.sendCommand("switchto gametime");
            //std::cout << "LiveSplit connected!\n";
            WriteProcessMemory(processHandle, (LPVOID)loglabelGreenAddress, "                              ", 31, NULL);
            WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, "                                       ", 40, NULL);
            WriteProcessMemory(processHandle, (LPVOID)loglabelGreenAddress, "LiveSplit connected! ", 31, NULL);
        }
        catch (std::runtime_error& ex) {
            //std::cout << "LiveSplit is not connected!\n";
            WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, "                                       ", 40, NULL);
            WriteProcessMemory(processHandle, (LPVOID)loglabelGreenAddress, "                             ", 30, NULL);
            WriteProcessMemory(processHandle, (LPVOID)loglabelRedAddress, "LiveSplit is not connected! ", 29, NULL);
            //std::cout << ex.what() << std::endl;
            timer = ServerSplitter::createTimer();
        }
        Sleep(2000);
    }
}