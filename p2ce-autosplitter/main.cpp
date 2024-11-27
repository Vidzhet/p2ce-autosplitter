#include "includes.h"
#include "readmem.h"
#include "timercontrol.h"
#include <intrin.h>
#include "misc.h"
#include "offsets.h"
#include "panorama.h"
#include "console.h"
#include "dx11hook/dx11hook.h"
#include "keybinds.h"
#include "demoui.h"
#include <fstream>

HANDLE processHandle = NULL;
uintptr_t engineBaseAddress = NULL;
uintptr_t materialsystemBaseAddress = NULL;
uintptr_t clientBaseAddress = NULL;
uintptr_t panoramaBaseAddress = NULL;
uintptr_t shaderapidx11BaseAddress = NULL;
uintptr_t soundemittersystemBaseAddress = NULL;
uintptr_t onGameLoadAddress = NULL;
uintptr_t onClientLoadAddress = NULL;
uintptr_t statusAddress = NULL;
uintptr_t statusAddressOld = NULL;
uintptr_t statusExtraAddress = NULL;
uintptr_t mapAddress = NULL;
uintptr_t bspAddress = NULL;
uintptr_t endAddress = NULL;
uintptr_t preloadAddress = NULL;
uintptr_t extra_demoMaxTickAddress = NULL;
uintptr_t clean_demoMaxTickAddress = NULL;
uintptr_t demoTickAddress = NULL;
uintptr_t demo_modeAddress = NULL;
uintptr_t errorsoundAddress = NULL;
std::wstring gamePath;
std::wstring rootName;
std::wstring gameName;
std::string appid;

static void InitConsole();

void mainThread() {
#ifdef DEBUG
    AllocConsole();
    FILE* NewStdOut = nullptr;
    freopen_s(&NewStdOut, "CONOUT$", "w", stdout);
    std::cout << "Debug mode. Waiting to attach debugger\n";
    //system("pause");
#endif

#ifdef EXTERNAL
    const std::string processName = "revolution.exe";
    auto processID = GetProcessID(processName);
    //std::cout << "Opening process handle...";
    while (processID == 0) {
        processID = GetProcessID(processName);
    }

    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    while (!processHandle) {
        processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    }
#else
    processHandle = GetCurrentProcess(); // useless tbh lol but ill keep it for some debug cases maybe
#endif // EXTERNAL
    //std::cout << "\tDone!\n";

    //std::cout << "Initializing base address...";
    while (materialsystemBaseAddress == NULL) {
        materialsystemBaseAddress = (uintptr_t)GetModuleHandleA("materialsystem.dll");
    }
    while (engineBaseAddress == NULL) {
        engineBaseAddress = (uintptr_t)GetModuleHandleA("engine.dll");
    }
    while (clientBaseAddress == NULL) {
        clientBaseAddress = (uintptr_t)GetModuleHandleA("client.dll");
    }
    while (panoramaBaseAddress == NULL) {
        panoramaBaseAddress = (uintptr_t)GetModuleHandleA("panorama.dll");
    }
    while (shaderapidx11BaseAddress == NULL) {
        shaderapidx11BaseAddress = (uintptr_t)GetModuleHandleA("shaderapidx11.dll");
    }
    while (soundemittersystemBaseAddress == NULL) {
        soundemittersystemBaseAddress = (uintptr_t)GetModuleHandleA("soundemittersystem.dll");
    }

    //std::cout << "\tDone!\n";

    //std::cout << "Offset address...";
    //statusAddressOld = ResolvePointerChain(processHandle, materialsystemBaseAddress + 0x002DD420, statusOffsetsOld); // obsolete
    //for (int statusValue = 1337; statusValue != 0; ReadProcessMemory(processHandle, (LPCVOID)statusAddressOld, &statusValue, sizeof(statusValue), nullptr)) {
    //    statusAddressOld = ResolvePointerChain(processHandle, materialsystemBaseAddress + 0x002DD420, statusOffsetsOld);
    //}
    //mapAddress = engineBaseAddress + 0x01BEF900;
    //ReadProcessMemory(processHandle, (LPCVOID)mapAddress, &mapAddress, sizeof(mapAddress), nullptr); // obsolete
    //mapAddress += 0x0;

    // init pointers
    while (onClientLoadAddress == NULL) { onClientLoadAddress = ptrOffset(clientBaseAddress + 0x00D7B4A0, onClientLoadOffsets); }
    while (*reinterpret_cast<int*>(onClientLoadAddress) != 17) { Sleep(100); } // wait till game loads.
    while (statusAddress == NULL) { statusAddress = ptrOffset(engineBaseAddress + 0x01C0D4F8, statusOffsets); }
    while (statusExtraAddress == NULL) { statusExtraAddress = ptrOffset(engineBaseAddress + 0x01BEF8F0, statusExtraOffsets); }
    bspAddress = engineBaseAddress + 0x1AD7E05; // bsp map str adress
    endAddress = clientBaseAddress + 0x1A4BDA2C; // or +0x1A4BDA30 or  +0x1A4BDA34 (they are the same)
    while (onGameLoadAddress == NULL) { onGameLoadAddress = ptrOffset(materialsystemBaseAddress + 0x002DD058, onGameLoadOffsets); }
    while (preloadAddress == NULL) { preloadAddress = ptrOffset(shaderapidx11BaseAddress + 0x0034B8D8, preloadOffsets); }
    while (errorsoundAddress == NULL) { errorsoundAddress = ptrOffset(soundemittersystemBaseAddress + 0x0005C118, errorsoundOffsets); }
    demoTickAddress = engineBaseAddress + 0x806B70;
    demo_modeAddress = engineBaseAddress + 0x1BE5E88;
    clean_demoMaxTickAddress = engineBaseAddress + 0x8069D8;
    extra_demoMaxTickAddress = clean_demoMaxTickAddress + 0x4;
    
    //std::cout << "\t\tDone!\n";

    std::setlocale(LC_NUMERIC, "C");

    gamePath = getGamePath();
    rootName = gamePath.substr(gamePath.rfind(L"\\") + 1);
    if (rootName == L"Portal Revolution") {
        gameName = L"revolution"; // this is game folder that contents game stuff (especially panorama)
    } // todo: add more games
    else { gameName = L"Game_Undefined"; }
    std::fstream steam_appid(gamePath + L"\\steam_appid.txt");
    if (steam_appid.is_open()) {
        std::getline(steam_appid, appid);
    }
    else { appid = "Undefined"; }
    steam_appid.close();
    /*std::wcout << gamePath << std::endl << rootName << std::endl << gameName << std::endl;
    std::cout << appid << std::endl;*/

    std::cout << std::hex << statusAddress << std::endl << endAddress << std::dec << std::endl;

    ServerSplitter::Timer timer(TIMER_NOINIT);

    std::thread loglabelThread(start_loglabel, std::ref(timer));
    loglabelThread.detach();

    // wait till panorama loads
    while (*reinterpret_cast<int*>(onGameLoadAddress) != GAME_LOADED) { Sleep(100); }
    std::cout << "game loaded main\n";

    InitConsole();
    engine->ConsoleCommand("play playonce\\level\\sad_party_horn_01.wav");

    // todo: add config for binds n settings stuff

    std::thread kbThread(keybindsThread);
    kbThread.detach();
    std::thread commandThread(ccommand::start); // ccommand loop thread (requires InitConsole!)
    commandThread.detach();
    //dx11hook_init(); // init imgui
    DirectX11Hook::fixWindow();

    monitorReset(timer);

    CloseHandle(processHandle);
}

static void InitConsole() {
    engine = new Engine();
    engine->Init();

    for (bool worked = false; !worked;) {
        try
        {
            engine->ConsoleCommand("echo p2ce-autosplitter");

            worked = true;
            engine->ConsoleCommand("demo_autorecord 0");
        }
        catch (const std::exception&)
        {
            // zaglushka
        }
    }
    //MessageBoxW(nullptr, L"p2ce-autosplitter injected.", L"Injected", MB_OK);
}

_declspec(dllexport) auto Load() {
    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&mainThread, nullptr, 0, nullptr);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH) {
        Load();
    }
    return TRUE;
}
