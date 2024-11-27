#pragma once
#include "includes.h"
#include "timercontrol.h"
#include "externs.h"
#include "misc.h"
#define MAP_WAKEUP_SAVE 1151197411
#define MAP_WAKEUP_TRIGGER -999098823
#define MAP_MAIN_MENU 1176128480
#define MAP_FINALE 1166668626
//1151197411 reunion id
//1135606784 shrodinger intro id

extern bool speedrun_restarted;
extern bool speedrun_finished;

void monitorStatus(ServerSplitter::Timer& timer);
#ifdef oldsplitting
void monitorMap(const HANDLE& processHandle, const LPCVOID& targetAddress, ServerSplitter::Timer& timer);
void monitorBsp(const HANDLE& processHandle, const LPCVOID& bspAddress, const LPCVOID& endAddress, ServerSplitter::Timer& timer);
#else
void monitorBsp(ServerSplitter::Timer& timer);
#endif
void recordDemo();
void monitorReset(ServerSplitter::Timer& timer);