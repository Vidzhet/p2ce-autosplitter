#include "includes.h"
#include "readmem.h"
#include "timercontrol.h"
#include <intrin.h>
#include "misc.h"
#include "offsets.h"
#include "panorama.h"

int main() {
    setlocale(LC_ALL, "ru");

    //getDemoPath();
    const std::wstring processName = L"revolution.exe";

    auto processID = GetProcessID(processName);
    std::cout << "Opening process handle...";
    while (processID == 0) {
        processID = GetProcessID(processName);
    }

    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    while (!processHandle) {
        processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    }
    std::cout << "\tDone!\n";

    
    std::cout << "Initializing base address...";
    auto moduleBaseAddress = GetModuleBaseAddress(processID, L"materialsystem.dll");
    while (moduleBaseAddress == 0) {
        moduleBaseAddress = GetModuleBaseAddress(processID, L"materialsystem.dll");
    }
    auto engineBaseAddress = GetModuleBaseAddress(processID, L"engine.dll");
    while (engineBaseAddress == 0) {
        engineBaseAddress = GetModuleBaseAddress(processID, L"engine.dll");
    }
    auto clientBaseAddress = GetModuleBaseAddress(processID, L"client.dll");
    while (clientBaseAddress == 0) {
        clientBaseAddress = GetModuleBaseAddress(processID, L"client.dll");
    }
    auto panoramaBaseAddress = GetModuleBaseAddress(processID, L"panorama.dll");
    while (panoramaBaseAddress == 0) {
        panoramaBaseAddress = GetModuleBaseAddress(processID, L"panorama.dll");
    }
    auto gameoverlayrenderer64BaseAddress = GetModuleBaseAddress(processID, L"gameoverlayrenderer64.dll");
    while (gameoverlayrenderer64BaseAddress == 0) {
        gameoverlayrenderer64BaseAddress = GetModuleBaseAddress(processID, L"gameoverlayrenderer64.dll");
    }
    std::cout << "\tDone!\n";

    std::cout << "Offset address...";
    auto statusAddress = ResolvePointerChain(processHandle, moduleBaseAddress + 0x002DD420, statusOffsets);
    while (statusAddress == 0) {
        statusAddress = ResolvePointerChain(processHandle, moduleBaseAddress + 0x002DD420, statusOffsets);
    }
    uintptr_t mapAddress = engineBaseAddress + 0x01BEF900;
    ReadProcessMemory(processHandle, (LPCVOID)mapAddress, &mapAddress, sizeof(mapAddress), nullptr);
    mapAddress += 0x0;
    uintptr_t bspAddress = engineBaseAddress + 0x1AD7E05; // bsp map string type adress
    uintptr_t endAddress = clientBaseAddress + 0x1A4BDA2C; // or +0x1A4BDA30 or  +0x1A4BDA34 (they are the same)
    std::cout << "\t\tDone!\n";

    ServerSplitter::Timer timer(TIMER_NOINIT);

    std::cout << "Connecting to LiveSplit...";
    //timer = ServerSplitter::createTimer(CATCH_CONSOLE_DEBUG); // CATCH_CONSOLE_DEBUG if wanna see console output
    std::cout << "\tDone!\n";

    std::cout << "*Status address: " << std::hex << statusAddress << std::dec << std::endl;
    std::cout << "*Map address: " << std::hex << mapAddress << std::dec << std::endl;

    std::thread loglabelThread(start_loglabel, processHandle, panoramaBaseAddress, (LPCVOID)mapAddress, std::ref(timer));
    loglabelThread.detach();

    //editMainMenu(processHandle, panoramaBaseAddress , gameoverlayrenderer64BaseAddress);
    monitorReset(processHandle, (LPCVOID)statusAddress, (LPCVOID)mapAddress, (LPCVOID)bspAddress, (LPCVOID)endAddress, timer);

    CloseHandle(processHandle);

    return 0;
}
