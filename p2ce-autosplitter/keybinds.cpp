#include "keybinds.h"
#include "includes.h"
#include "demoui.h"
#include "imgui_menu.h"
#include "console.h"

void keybindsThread()
{
    while (true) {
        if (GetMainWindowHwnd(GetCurrentProcessId()) == GetForegroundWindow()) {
            KBIND(VK_RSHIFT) {
                engine->ClientCmd_Unrestricted("democontrol");
                //dx11hook_init();
            }
            if (!engine->IsPaused()) {
                KBIND(VK_RIGHT) {
                    demo_forward();
                }
                KBIND(VK_LEFT) {
                    demo_back();
                }
                KBIND(VK_SPACE) {
                    if (engine->IsPlayingDemo()) {
                        engine->ClientCmd_Unrestricted("demo_togglepause");
                        demo_paused = !demo_paused;
                    }
                }
            }
        }
        Sleep(100);
    }
}
