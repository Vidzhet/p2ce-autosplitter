#pragma once
#include "dx11hook.h"

typedef HRESULT(__stdcall *tD3D11Present)(IDXGISwapChain* pSwapChain, UINT SysInterval, UINT Flags);
typedef HRESULT(__stdcall* tResizeBuffers)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
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
	extern tResizeBuffers oResizeBuffers;
    extern WNDPROC oWndProc;
    HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	HRESULT __stdcall hkD3D11Present(IDXGISwapChain* pSwapChain, UINT SysInterval, UINT Flags);
    LRESULT CALLBACK hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}

namespace DirectX11Hook
{
    // works only if mainwindow in fullscreen mode
    void minimizeWindow();
	void Initialize();
	void Release();
}

