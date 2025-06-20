#include "includes.h"
#include "readmem.h"
#include "timercontrol.h"
#include <intrin.h>
#include "misc.h"
#include "panorama.h"
#include "console.h"
#include "dx11hook/dx11hook.h"
#include "keybinds.h"
#include "demoui.h"

std::wstring gamePath;
std::string gameName;
std::wstring gameNameW;

std::string speedrun_endMap;
std::string speedrun_endInput;
std::string speedrun_endInputEntity;

HMODULE g_hmodule = nullptr;
void* g_pTimer = nullptr;

IVEngineClient* engine;
ICvar* g_pCVar;

//#define DEBUG

void mainThread() {
#ifdef DEBUG
    AllocConsole();
    FILE* NewStdOut = nullptr;
    freopen_s(&NewStdOut, "CONOUT$", "w", stdout);
    std::cout << "Debug mode. Waiting to attach debugger\n";
    system("pause");
#endif
    while (!GetModuleHandleA("client.dll")) { Sleep(100); }
    while (GetMainWindowHwnd(GetCurrentProcessId()) != GetForegroundWindow()) { Sleep(100); } // wait till window get active
    _CreateInterface e_CreateInterface = (_CreateInterface)GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface");
    _CreateInterface vstd_CreateInterface = (_CreateInterface)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "CreateInterface");
    int returncode;
    engine = (IVEngineClient*)e_CreateInterface(VENGINE_CLIENT_INTERFACE_VERSION, &returncode);
    g_pCVar = (ICvar*)vstd_CreateInterface("VEngineCvar007", &returncode);
    while (!engine->IsInGame()) { Sleep(100); }
    while (engine->IsDrawingLoadingImage()) { Sleep(100); }

    std::setlocale(LC_NUMERIC, "C");

    // todo: rewrite all that shit down below using IVEngineClient (done)
    gamePath = getGamePath();
    gameName = engine->GetModDirectory();
    gameNameW = std::wstring(gameName.begin(), gameName.end());

    ServerSplitter::Timer timer(TIMER_NOINIT);
    g_pTimer = &timer;

    //engine->ClientCmd_Unrestricted("play playonce\\level\\sad_party_horn_01.wav");

    LoadSpeedrunConfig(L"speedrun_config.mge");
    std::cout << speedrun_endMap << std::endl << speedrun_endInput << std::endl << speedrun_endInputEntity << std::endl;

    engine->ClientCmd_Unrestricted("demo_autorecord 0");
    std::thread(timer_update_connection, std::ref(timer)).detach();
    std::thread(keybindsThread).detach();
    ccommand_setup();
    dx11hook_init(); // init imgui

    monitorReset(timer); // main thread
}

_declspec(dllexport) void Load() {
    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&mainThread, nullptr, 0, nullptr);
}

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hmodule);
        g_hmodule = hmodule;
        Load();
    }
    return TRUE;
}
