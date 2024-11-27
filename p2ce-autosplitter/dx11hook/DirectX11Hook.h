#pragma once
#include "dx11hook.h"

typedef HRESULT(__stdcall *tD3D11Present)(IDXGISwapChain* pSwapChain, UINT SysInterval, UINT Flags);
BOOL CALLBACK EnumWindowCallback(HWND hWnd, LPARAM lParam);
HWND GetMainWindowHwnd(unsigned long lProcessId);

struct ProcessWindowData
{
	HWND hWnd;
	unsigned long lProcessId;
};

namespace Hooks
{
	extern tD3D11Present oPresent;
	HRESULT __stdcall hkD3D11Present(IDXGISwapChain* pSwapChain, UINT SysInterval, UINT Flags);
}

class DirectX11Hook
{
public:
	DirectX11Hook();
	~DirectX11Hook();

    static void fixWindow();
    // works only if mainwindow in fullscreen mode
    static void minimizeWindow();
	void Initialize(bool setupwinow = true);
	void Release();

	void HookFunction(PVOID *oFunction, PVOID pDetour);
	void UnHookFunction(PVOID *oFunction, PVOID pDetour);
};

