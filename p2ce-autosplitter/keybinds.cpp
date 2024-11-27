#include "keybinds.h"
#include "includes.h"
#include "demoui.h"
#include "imgui_menu.h"
#include "console.h"

bool rshift_pressed = false;
bool right_pressed = false;
bool left_pressed = false;
bool space_pressed = false;

void keybindsThread()
{
    while (true) {
        if ((GetAsyncKeyState(VK_RSHIFT) & 0x8000) && !rshift_pressed) {
            engine->ConsoleCommand("democontrol");
            rshift_pressed = true;
        }
        else if (!(GetAsyncKeyState(VK_RSHIFT) & 0x8000) && rshift_pressed) {
            rshift_pressed = false;
        }

        if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && !right_pressed) {
            demo_forward();
            right_pressed = true;
        }
        else if (!(GetAsyncKeyState(VK_RIGHT) & 0x8000) && right_pressed) {
            right_pressed = false;
        }

        if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && !left_pressed) {
            demo_back();
            left_pressed = true;
        }
        else if (!(GetAsyncKeyState(VK_LEFT) & 0x8000) && left_pressed) {
            left_pressed = false;
        }

        if (demoui_loaded) {
            if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && !space_pressed) {
                if (demo_mode) {
                    engine->ConsoleCommand("demo_togglepause");
                    demo_paused = !demo_paused;
                }
                space_pressed = true;
            }
            else if (!(GetAsyncKeyState(VK_SPACE) & 0x8000) && space_pressed) {
                space_pressed = false;
            }
        }

        Sleep(10);
    }
}
