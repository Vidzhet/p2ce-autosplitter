#pragma once
#include "includes.h"

DWORD GetProcessID(const std::wstring& processName);
uintptr_t GetModuleBaseAddress(DWORD processID, const std::wstring& moduleName);
uintptr_t ResolvePointerChain(HANDLE processHandle, uintptr_t baseAddress, const std::vector<uintptr_t>& offsets);
void getDemoPath();
