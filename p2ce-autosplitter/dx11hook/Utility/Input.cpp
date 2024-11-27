#pragma once
#include "..\\DirectX11Hook.h"
#include <thread>
#include <set>

Input* Input::m_pInstance;

Input::Input()
{

}

Input::~Input()
{

}

void Input::StartThread()
{
	m_hThread = CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(MenuKeyMonitor), NULL, NULL, NULL);
	mw_hThread = CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(MWheelMonitor), NULL, NULL, NULL);
}

void Input::StopThread()
{
	TerminateThread(m_hThread, 0);
	TerminateThread(mw_hThread, 0);
}

HHOOK hMouseHook;
ImGuiIO* inputio;

// mwheel hk callback
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
        if (wParam == WM_MOUSEWHEEL) {
            short delta = GET_WHEEL_DELTA_WPARAM(pMouseStruct->mouseData);
            if (delta > 0) {
                inputio->MouseWheel = 1;
                ///std::cout << "up" << std::endl;
            }
            else if (delta < 0) {
                //std::cout << "down" << std::endl;
                inputio->MouseWheel = -1;
            }
        }
    }
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

void Input::MWheelMonitor() {
    ImGuiIO& io = ImGui::GetIO();
    inputio = &io;
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hMouseHook);
}

void Input::MenuKeyMonitor()
{
	HWND gameWindow = GetMainWindowHwnd(GetCurrentProcessId());

	while (true)
	{
        POINT mousePosition;
        GetCursorPos(&mousePosition);
        ScreenToClient(gameWindow, &mousePosition);

        ImGuiIO& io = ImGui::GetIO();
        io.MousePos.x = (float)mousePosition.x;
        io.MousePos.y = (float)mousePosition.y;

        if (GetAsyncKeyState(VK_LBUTTON)) {
            io.MouseDown[0] = true;
        }
        else {
            io.MouseDown[0] = false;
        }

        static double lastInputTime = 0;
        static const double inputDelay = 0.2f; // seconds
        static std::set<int> cooldown_keys;
        static bool cooldown = false;

        double currentTime = ImGui::GetTime();

        // text input
        for (int key = 0x08; key <= 0xFF; key++) {  // check keys from backspace
            if (GetAsyncKeyState(key) & 0x8000) {
                io.KeysDown[key] = true;

                if (currentTime - lastInputTime >= inputDelay) {
                    cooldown = false;
                    cooldown_keys.clear();
                }
                else { cooldown = true; }

                if (!cooldown || cooldown_keys.find(key) == cooldown_keys.end()) {
                    if ((key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9') || key == VK_SPACE) {
                        io.AddInputCharacter((unsigned int)key);
                        cooldown_keys.insert(key);
                        lastInputTime = currentTime; // update time
                    }
                    else if (key == VK_OEM_PERIOD) {
                        io.AddInputCharacter('.');
                        cooldown_keys.insert(key);
                        lastInputTime = currentTime;
                    }
                }
            }
            else {
                io.KeysDown[key] = false;
            }
        }
	}
}

Input* Input::GetInstance()
{
	if (!m_pInstance)
		m_pInstance = new Input();

	return m_pInstance;
}