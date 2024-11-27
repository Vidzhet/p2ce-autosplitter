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
    } REGISTER();

    CCOMMAND("reset") {
        engine->ConsoleCommand("load wakeup_save");
        speedrun_restarted = true;
    } REGISTER();

    CCOMMAND("alloc_console") {
        AllocConsole();
        FILE* NewStdOut = nullptr;
        freopen_s(&NewStdOut, "CONOUT$", "w", stdout);
    }REGISTER();

    CCOMMAND("stop_speedrun") {
        speedrun_finished = true;
        engine->ConsoleCommand("echo Speedrun Terminated.");
    }REGISTER();

    /*CCOMMAND("cheat_autobunnyhop") { // i wont do that, i am lazy tbh
        cheat_autobunnyhop = !cheat_autobunnyhop;
    }REGISTER();*/

    ALIAS("dis", "disconnect"); // example of usage (not an feature but still useful sometimes)
    ALIAS("legacy_demoui", "demoui"); // alias to avoid demoui command hook

    CCOMMAND_HOOK("exit") {
        engine->ConsoleCommand("echo bye-bye!"); // just for fun
    } REGISTER();
    CCOMMAND_HOOK("quit") {
        engine->ConsoleCommand("echo bye-bye!"); // maybe ill add force kill process bc game throws segfault when shutting down
    } REGISTER();
    CCOMMAND_HOOK_ONLY("mat_setvideomode ") { // added extra spacing to be sure command like mat_setvideomode_something will not work.
        if (demoui_loaded) {
            engine->ConsoleCommand("quit"); // yes, this is very bad example of how to fix this issue. Unfortunately i cant fix this such a complicated issue in a different way.
            system(("start steam://rungameid/" + appid).c_str()); // todo: add definition on game name etc (done)
        }
    } REGISTER();
    CCOMMAND_HOOK("demoui") {
        engine->ConsoleCommand("legacy_demoui");
        if (!demoui_loaded) {
            dx11hook_init();
            demoui_loaded = true;
        }
        imgui_menu = !imgui_menu;
    } REGISTER();
    CCOMMAND("democontrol") {
        if (!demoui_loaded) {
            dx11hook_init();
            demoui_loaded = true;
        }
        imgui_menu = !imgui_menu;
    } REGISTER();
}