#include "DirectX11Hook.h"
#include "..\imgui_menu.h"
#include "..\demoui.h"
#include "..\vtablehook.h"

ID3D11Device* pDevice;
ID3D11DeviceContext* pContext;
IDXGISwapChain* pSwapChain;
ID3D11RenderTargetView* pRenderTargetView;

BOOL CALLBACK EnumWindowCallback(HWND hWnd, LPARAM lParam)
{
	ProcessWindowData& wndData = *(ProcessWindowData*)lParam;
	unsigned long lProcessId = 0;
	GetWindowThreadProcessId(hWnd, &lProcessId);
    if ((wndData.lProcessId != lProcessId) || (GetWindow(hWnd, GW_OWNER) != (HWND)0) || !IsWindowVisible(hWnd)) {
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
	EnumWindows(EnumWindowCallback, (LPARAM)&wndData);
	return wndData.hWnd;
}

void DirectX11Hook::Initialize()
{
    HWND hWnd = GetMainWindowHwnd(GetCurrentProcessId());
    RECT rect;
    GetClientRect(hWnd, &rect);

    // check if window is fullscreen or windowed
    DWORD dwStyle = GetWindowLongA(hWnd, GWL_STYLE);
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
    scd.BufferDesc.RefreshRate.Numerator = 0;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.Windowed = isFullscreen ? FALSE : TRUE; // set windowed or fullscreen mode

    scd.SampleDesc.Quality = 0; // quality level
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // discard swap effect

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &scd, &pSwapChain, &pDevice, NULL, &pContext);

    ImGui::CreateContext();
    IMGUI_SETSTYLE();

    //Input::GetInstance()->StartThread();
    Hooks::oWndProc = (WNDPROC)SetWindowLongPtrA(hWnd, GWLP_WNDPROC, (LONG_PTR)Hooks::hkWndProc);
    Hooks::oPresent = (tD3D11Present)vtablehook_hook(pSwapChain, (void*)Hooks::hkD3D11Present, 8);
    Hooks::oResizeBuffers = (tResizeBuffers)vtablehook_hook(pSwapChain, (void*)Hooks::hkResizeBuffers, 13);
}

void DirectX11Hook::Release()
{
	//Input::GetInstance()->StopThread();
    vtablehook_hook(pSwapChain, Hooks::oPresent, 8);
    vtablehook_hook(pSwapChain, Hooks::oResizeBuffers, 13);
}

BOOL isfullscreen;
bool do_minimize = false;
void DirectX11Hook::minimizeWindow()
{
    do_minimize = true;
}

extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Hooks
{
	tD3D11Present oPresent = NULL;
	tResizeBuffers oResizeBuffers = NULL;
    WNDPROC oWndProc = NULL;

    LRESULT CALLBACK hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
            return true;
        }
        if (GetMainWindowHwnd(GetCurrentProcessId()) == GetForegroundWindow()) {
            if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive())
            {
                if (msg == WM_KEYDOWN || msg == WM_KEYUP || msg == WM_CHAR || msg == WM_MOUSEMOVE || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP || msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP || msg == WM_MOUSEWHEEL)
                {
                    return true;

                }
            }
        }

        return CallWindowProcA(oWndProc, hWnd, msg, wParam, lParam);
    }

	bool bOnce = false;
    HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        if (pRenderTargetView) {
            pRenderTargetView->Release();
            pRenderTargetView = nullptr;
        }
        HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
        if (SUCCEEDED(hr)) {
            ID3D11Texture2D* pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
            pBackBuffer->Release();
        }
        return hr;
    }

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
        /*ImGui::Begin("ImGui Window");
        ImGui::End();*/

		ImGui::Render();
		pContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return oPresent(pSwapChain, SysInterval, Flags);
	}
}