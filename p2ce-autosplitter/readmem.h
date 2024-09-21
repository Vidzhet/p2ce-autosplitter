#pragma once
#include "includes.h"
#include "timercontrol.h"
#define MAP_WAKEUP_SAVE 1151197411
#define MAP_WAKEUP_TRIGGER -999098823
#define MAP_MAIN_MENU 1176128480
//1151197411 reunion id
//1135606784 shrodinger intro id
#define newsplitting

void monitorStatus(const HANDLE &processHandle, const LPCVOID &targetAddress, const LPCVOID& mapAddress, ServerSplitter::Timer& timer);
void monitorMap(const HANDLE& processHandle, const LPCVOID& targetAddress, ServerSplitter::Timer& timer);
#ifdef oldsplitting
void monitorBsp(const HANDLE& processHandle, const LPCVOID& bspAddress, const LPCVOID& endAddress, ServerSplitter::Timer& timer);
#else
void monitorBsp(const HANDLE& processHandle, const LPCVOID& bspAddress, const LPCVOID& endAddress, const LPCVOID& statusAddress, ServerSplitter::Timer& timer);
#endif
void monitorReset(const HANDLE& processHandle, const LPCVOID& statusAddress, const LPCVOID& mapAddress, const LPCVOID& bspAddress, const LPCVOID& endAddress, ServerSplitter::Timer& timer);