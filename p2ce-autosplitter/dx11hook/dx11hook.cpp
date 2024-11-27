#include "dx11hook.h"
#include "../externs.h"

DirectX11Hook t;

void dx11hook_init() {
    CreateThread(NULL, 0, hkThread, 0, 0, 0);
}

DWORD WINAPI hkThread(LPVOID)
{
    strcpy_s(reinterpret_cast<char*>(errorsoundAddress), 10, "nullsound"); // disable fiddlesticks
	t.Initialize(false); // leave false here if you used fixwindow. if not, leave this parameter empty!
	return 0;
}