#pragma once
#include "includes.h"
#include "timercontrol.h"
#define MAP_WAKEUP_SAVE 1151197411
#define MAP_WAKEUP_TRIGGER -999098823

DWORD GetProcessID(const std::wstring& processName);
uintptr_t GetModuleBaseAddress(DWORD processID, const std::wstring& moduleName);
uintptr_t ResolvePointerChain(HANDLE processHandle, uintptr_t baseAddress, const std::vector<uintptr_t>& offsets);
void monitorStatus(const HANDLE &processHandle, const LPCVOID &targetAddress, const LPCVOID& mapAddress, ServerSplitter::Timer& timer);
void monitorMap(const HANDLE& processHandle, const LPCVOID& targetAddress, ServerSplitter::Timer& timer);
void monitorReset(const HANDLE& processHandle, const LPCVOID& statusAddress, const LPCVOID& mapAddress, ServerSplitter::Timer& timer);