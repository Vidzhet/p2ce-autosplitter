#include "readmem.h"

DWORD GetProcessID(const std::wstring& processName) {
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &processEntry)) {
        do {
            if (processName == processEntry.szExeFile) {
                DWORD processID = processEntry.th32ProcessID;
                CloseHandle(snapshot);
                return processID;
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return 0;
}

uintptr_t GetModuleBaseAddress(DWORD processID, const std::wstring& moduleName) {
    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    if (Module32First(snapshot, &moduleEntry)) {
        do {
            if (moduleName == moduleEntry.szModule) {
                uintptr_t baseAddress = (uintptr_t)moduleEntry.modBaseAddr;
                CloseHandle(snapshot);
                return baseAddress;
            }
        } while (Module32Next(snapshot, &moduleEntry));
    }

    CloseHandle(snapshot);
    return 0;
}

uintptr_t ResolvePointerChain(HANDLE processHandle, uintptr_t baseAddress, const std::vector<uintptr_t>& offsets) {
    uintptr_t currentAddress = baseAddress;
    for (const auto& offset : offsets) {
        if (!ReadProcessMemory(processHandle, (LPCVOID)currentAddress, &currentAddress, sizeof(currentAddress), nullptr)) {
            return 0;
        }
        currentAddress += offset;
    }
    return currentAddress;
}

void monitorStatus(const HANDLE &processHandle, const LPCVOID &targetAddress, const LPCVOID& mapAddress, ServerSplitter::Timer& timer)
{
    std::cout << "statusThread started.\n";
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
    std::cout << "statusThread done working!\n";
}

void monitorMap(const HANDLE &processHandle, const LPCVOID &targetAddress, ServerSplitter::Timer& timer)
{
    std::cout << "mapThread started.\n";
    bool nextmap = false;
    bool wakeupmap = true;
    int buffer = 0;
    std::set<int> mapsbuffer;
    for (int value = 0; true; ReadProcessMemory(processHandle, targetAddress, &value, sizeof(value), nullptr)) {
        //std::cout << "Map value:\t" << value << std::endl;
        //std::cout << "Map value:\t" << *nextMapPtr << std::endl;
        if (value == MAP_WAKEUP_TRIGGER) {
            break;
        }
        // if current map not in mapsbuffer
        if (mapsbuffer.find(value) == mapsbuffer.end() && buffer != 0 && !nextmap) {
            std::cout << "split!\n";
            //basically split() but works if timer is paused
            timer.skip();
            nextmap = true;
        }
        else if (value == buffer && nextmap) {
            nextmap = false;
        }
        buffer = value;
        mapsbuffer.insert(buffer);
        Sleep(1);
    }
    std::cout << "mapThread done working!\n";
}

void monitorReset(const HANDLE& processHandle, const LPCVOID& statusAddress, const LPCVOID& mapAddress, ServerSplitter::Timer& timer)
{
    bool restarted = false;
    for (int status = 0, map = 0; true; ReadProcessMemory(processHandle, statusAddress, &status, sizeof(status), nullptr), ReadProcessMemory(processHandle, mapAddress, &map, sizeof(map), nullptr)) {
        if (status == 1 && map == MAP_WAKEUP_SAVE && restarted) {
            timer.start();
            restarted = false;
            std::thread statusThread(monitorStatus, processHandle, (LPCVOID)statusAddress, (LPCVOID)mapAddress, std::ref(timer));
            std::thread mapThread(monitorMap, processHandle, (LPCVOID)mapAddress, std::ref(timer));
            statusThread.detach();
            mapThread.detach();
        }
        else if (map == MAP_WAKEUP_TRIGGER && !restarted) {
            timer.reset();
            restarted = true;
        }
        Sleep(1);
    }
}