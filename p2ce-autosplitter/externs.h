#pragma once
#include "includes.h"

// some useful global pointers
extern HANDLE processHandle;
extern uintptr_t engineBaseAddress;
extern uintptr_t materialsystemBaseAddress;
extern uintptr_t clientBaseAddress;
extern uintptr_t panoramaBaseAddress;
extern uintptr_t shaderapidx11BaseAddress;
extern uintptr_t soundemittersystemBaseAddress;
extern uintptr_t consolelogAddress;
extern uintptr_t onGameLoadAddress;
extern uintptr_t onClientLoadAddress;
extern uintptr_t statusAddress;
extern uintptr_t statusExtraAddress;
extern uintptr_t statusAddressOld;
extern uintptr_t mapAddress;
extern uintptr_t bspAddress;
extern uintptr_t endAddress;
extern uintptr_t preloadAddress;
extern uintptr_t extra_demoMaxTickAddress;
extern uintptr_t clean_demoMaxTickAddress;
extern uintptr_t demoTickAddress;
extern uintptr_t demo_modeAddress;
extern uintptr_t errorsoundAddress;

extern std::wstring gamePath;
extern std::wstring rootName;
extern std::wstring gameName;
extern std::string appid;;