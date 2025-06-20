#pragma once
#include "console.h"
#include "readmem.h"
#include "dx11hook\dx11hook.h"
#include "imgui_menu.h"

extern bool demoui_loaded = false;
bool cheat_autobunnyhop = false;

CCOMMAND_LIST()
{
    CCOMMAND("suka_test") {
    MessageBoxA(nullptr, "suka", "test", MB_OK);
    });

    CCOMMAND("reset") {
        engine->ClientCmd_Unrestricted("load wakeup_save");
        speedrun_restarted = true;
    });

    CCOMMAND("alloc_console") {
        AllocConsole();
        FILE* NewStdOut = nullptr;
        freopen_s(&NewStdOut, "CONOUT$", "w", stdout);
        std::cout.clear(); // fix
    });

    CCOMMAND("speedrun_signal_end") {
        if (g_pTimer && livesplit_connected) {
            reinterpret_cast<ServerSplitter::Timer*>(g_pTimer)->split(); // final stop timer
        }
        speedrun_finished = true;
    });

    CCOMMAND("cout_test") {
        std::cout << "cout_test\n";
    });

    CCOMMAND("stop_speedrun") {
        speedrun_finished = true;
        engine->ClientCmd_Unrestricted("echo Speedrun Terminated.");
    });

    ALIAS("dis", "disconnect"); // example of usage (not an feature but still useful sometimes)
    ALIAS("legacy_demoui", "demoui"); // alias to avoid demoui command hook
    ALIAS("cheats_toggle", "incrementvar sv_cheats 0 1 1");

    CCOMMAND_HOOK("exit") {
        FreeLibrary(g_hmodule);
        engine->ClientCmd_Unrestricted("echo bye-bye!"); // just for fun
    });
    CCOMMAND_HOOK("quit") {
        FreeLibrary(g_hmodule);
        engine->ClientCmd_Unrestricted("echo bye-bye!"); // maybe ill add force kill process bc game throws segfault when shutting down
    });
    CCOMMAND_HOOK("demoui") {
        engine->ClientCmd_Unrestricted("legacy_demoui"); // hiding it back
        imgui_menu = !imgui_menu;
    });
    CCOMMAND("democontrol") {
        static bool do_once = true;
        if (do_once) {
            apply_def_pos = true;
            do_once = false;
        }
        imgui_menu = !imgui_menu;
    });
}