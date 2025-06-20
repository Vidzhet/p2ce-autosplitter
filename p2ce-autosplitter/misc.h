#pragma once
#include "includes.h"

DWORD GetProcessID(const std::string& processName);
uintptr_t GetModuleBaseAddress(DWORD processID, const std::string& moduleName);
uintptr_t ResolvePointerChain(HANDLE processHandle, uintptr_t baseAddress, const std::vector<uintptr_t>& offsets);
std::wstring getGamePath();
std::wstring getCurrentTimeW();
// converts float to %.1 format str
std::string float_tostr(float value);
bool compareFileNames(const std::string& a, const std::string& b);
void LoadSpeedrunConfig(const wchar_t* file);

namespace memory {
    uintptr_t ptrOffset(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets);
    char* find_str(const char* moduleName, const std::string& searchString);
    char* find_const_str(const char* moduleName, const std::string& searchString);
}


