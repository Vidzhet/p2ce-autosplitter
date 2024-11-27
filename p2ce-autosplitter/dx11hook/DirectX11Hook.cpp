#include "DirectX11Hook.h"
#include "..\imgui_menu.h"
#include "..\demoui.h"

ID3D11Device* pDevice;
ID3D11DeviceContext* pContext;
IDXGISwapChain* pSwapChain;
ID3D11RenderTargetView* pRenderTargetView;

DirectX11Hook::DirectX11Hook()
{

}


DirectX11Hook::~DirectX11Hook()
{

}

BOOL CALLBACK EnumWindowCallback(HWND hWnd, LPARAM lParam)
{
	ProcessWindowData& wndData = *(ProcessWindowData*)lParam;
	unsigned long lProcessId = 0;
	::GetWindowThreadProcessId(hWnd, &lProcessId);
    if ((wndData.lProcessId != lProcessId) || (::GetWindow(hWnd, GW_OWNER) != (HWND)0) || !::IsWindowVisible(hWnd)) {
        return 1;
    }
	wndData.hWnd = hWnd;
	return 0;
}

HWND GetMainWindowHwnd(unsigned long lProcessId)
{
	ProcessWindowData wndData;
	wndData.hWnd = 0;
	wndData.lProcessId = lProcessId;
	::EnumWindows(EnumWindowCallback, (LPARAM)&wndData);
	return wndData.hWnd;
}

void DirectX11Hook::fixWindow()
{
    HWND hWnd = GetMainWindowHwnd(GetCurrentProcessId());
    RECT rect;
    GetClientRect(hWnd, &rect);
    // check if window is fullscreen or windowed
    DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    bool isFullscreen = (dwStyle & WS_OVERLAPPEDWINDOW) == 0 && GetWindowRect(hWnd, &rect);

    // setup DXGI_SWAP_CHAIN_DESC with proper windowed/fullscreen mode handling
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
    scd.BufferCount = 1; // single buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // rgba format
    scd.BufferDesc.Width = rect.right - rect.left;
    scd.BufferDesc.Height = rect.bottom - rect.top;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // no scaling
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // no scanline ordering
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // render target usage
    scd.OutputWindow = hWnd; // target window
    scd.SampleDesc.Count = 1; // single sample
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switch
    scd.BufferDesc.RefreshRate.Numerator = 144;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.Windowed = isFullscreen ? FALSE : TRUE; // set windowed or fullscreen mode

    scd.SampleDesc.Quality = 0; // quality level
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // discard swap effect

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &scd, &pSwapChain, &pDevice, NULL, &pContext); // bug here (FIXED NAHUI!!)
}

void DirectX11Hook::Initialize(bool setupwinow)
{
    if (setupwinow) {
        HWND hWnd = GetMainWindowHwnd(GetCurrentProcessId());
        RECT rect;
        GetClientRect(hWnd, &rect);

        // check if window is fullscreen or windowed
        DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        bool isFullscreen = (dwStyle & WS_OVERLAPPEDWINDOW) == 0 && GetWindowRect(hWnd, &rect);

        // setup DXGI_SWAP_CHAIN_DESC with proper windowed/fullscreen mode handling
        DXGI_SWAP_CHAIN_DESC scd;
        ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
        scd.BufferCount = 1; // single buffer
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // rgba format
        scd.BufferDesc.Width = rect.right - rect.left;
        scd.BufferDesc.Height = rect.bottom - rect.top;
        scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // no scaling
        scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // no scanline ordering
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // render target usage
        scd.OutputWindow = hWnd; // target window
        scd.SampleDesc.Count = 1; // single sample
        scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switch
        scd.BufferDesc.RefreshRate.Numerator = 144;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        scd.Windowed = isFullscreen ? FALSE : TRUE; // set windowed or fullscreen mode

        scd.SampleDesc.Quality = 0; // quality level
        scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // discard swap effect

        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
        D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &scd, &pSwapChain, &pDevice, NULL, &pContext); // bug here (FIXED NAHUI!!)
    }

    DWORD_PTR* pSwapChainVT = NULL;
    pSwapChainVT = (DWORD_PTR*)pSwapChain;
    pSwapChainVT = (DWORD_PTR*)pSwapChainVT[0];
    Hooks::oPresent = (tD3D11Present)pSwapChainVT[8];

    Renderer::GetInstance()->Initialize();
    Renderer::GetInstance()->SetStyle();
    Input::GetInstance()->StartThread();

    HookFunction((PVOID*)&Hooks::oPresent, (PVOID)&Hooks::hkD3D11Present);
}

void DirectX11Hook::Release()
{
	Input::GetInstance()->StopThread();
	UnHookFunction((PVOID *)&Hooks::oPresent, (PVOID)&Hooks::hkD3D11Present);
}

void DirectX11Hook::HookFunction(PVOID * oFunction, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(oFunction, pDetour);
	DetourTransactionCommit();
}

void DirectX11Hook::UnHookFunction(PVOID * oFunction, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(oFunction, pDetour);
	DetourTransactionCommit();
}

BOOL isfullscreen;
bool do_minimize = false;
void DirectX11Hook::minimizeWindow()
{
    do_minimize = true;
}

namespace Hooks
{
	tD3D11Present oPresent = NULL;
	bool bOnce = false;

	HRESULT __stdcall hkD3D11Present(IDXGISwapChain* pSwapChain, UINT SysInterval, UINT Flags)
	{
		if (!bOnce)
		{
			HWND hWindow = GetMainWindowHwnd(GetCurrentProcessId());
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)(&pDevice))))
			{
				pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)(&pDevice));
				pDevice->GetImmediateContext(&pContext);
			}

			ID3D11Texture2D* renderTargetTexture = nullptr;
			if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&renderTargetTexture)))
			{
				pDevice->CreateRenderTargetView(renderTargetTexture, NULL, &pRenderTargetView);
				renderTargetTexture->Release();
			}

			ImGui_ImplDX11_Init(hWindow, pDevice, pContext);
			ImGui_ImplDX11_CreateDeviceObjects();

			//ImGui::StyleColorsDark();

			bOnce = true;
		}

        if (do_minimize) {
            pSwapChain->GetFullscreenState(&isfullscreen, NULL);
            if (isfullscreen == TRUE) {
                ShowWindow(GetMainWindowHwnd(GetCurrentProcessId()), SW_MINIMIZE);
            }
            do_minimize = false;
        }

		ImGui_ImplDX11_NewFrame();

        IMGUI_RENDER();

		ImGui::Render();
		pContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return oPresent(pSwapChain, SysInterval, Flags);
	}
}