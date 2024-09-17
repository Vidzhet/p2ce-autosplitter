#include "includes.h"
#include "readmem.h"
#include "timercontrol.h"
#include <intrin.h>

int main() {
    setlocale(LC_ALL, "ru");
    const std::wstring processName = L"revolution.exe";
    const uintptr_t baseOffset = 0x002DD420;
    const std::vector<uintptr_t> offsets = { 0xA0, 0xA0, 0x0, 0xA0, 0x0, 0x50, 0x5C8 };

    auto processID = GetProcessID(processName);
    std::cout << "Opening process handle...";
    while (processID == 0) {
        processID = GetProcessID(processName);
    }

    HANDLE processHandle = OpenProcess(PROCESS_VM_READ, FALSE, processID);
    while (!processHandle) {
        processHandle = OpenProcess(PROCESS_VM_READ, FALSE, processID);
    }
    std::cout << "\tDone!\n";

    auto moduleBaseAddress = GetModuleBaseAddress(processID, L"materialsystem.dll");
    std::cout << "Initializing base address...";
    while (moduleBaseAddress == 0) {
        moduleBaseAddress = GetModuleBaseAddress(processID, L"materialsystem.dll");
    }
    auto engineBaseAddress = GetModuleBaseAddress(processID, L"engine.dll");
    while (engineBaseAddress == 0) {
        engineBaseAddress = GetModuleBaseAddress(processID, L"engine.dll");
    }
    std::cout << "\tDone!\n";

    auto statusAddress = ResolvePointerChain(processHandle, moduleBaseAddress + baseOffset, offsets);
    std::cout << "Offset address...";
    while (statusAddress == 0) {
        statusAddress = ResolvePointerChain(processHandle, moduleBaseAddress + baseOffset, offsets);
    }
    uintptr_t mapAddress = engineBaseAddress + 0x01BEF900;
    ReadProcessMemory(processHandle, (LPCVOID)mapAddress, &mapAddress, sizeof(mapAddress), nullptr);
    mapAddress += 0x0;
    std::cout << "\t\tDone!\n";

    std::cout << "Connecting to LiveSplit...";
    ServerSplitter::Timer timer = ServerSplitter::createTimer(CATCH_CONSOLE_DEBUG);
    std::cout << "\tDone!\n";

    std::cout << "*Status address: " << std::hex << statusAddress << std::dec << std::endl;
    std::cout << "*Map address: " << std::hex << mapAddress << std::dec << std::endl;

    monitorReset(processHandle, (LPCVOID)statusAddress, (LPCVOID)mapAddress, timer);

    CloseHandle(processHandle);

    return 0;
}
