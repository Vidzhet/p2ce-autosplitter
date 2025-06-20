#pragma once
void keybindsThread();

#define KBIND(vKey) \
    static bool vKey##_kbpressed; \
    if (!(GetAsyncKeyState(vKey) & 0x8000) && vKey##_kbpressed) { vKey##_kbpressed = false; } \
    else if ((GetAsyncKeyState(vKey) & 0x8000) && !(vKey##_kbpressed) && (vKey##_kbpressed = true))
