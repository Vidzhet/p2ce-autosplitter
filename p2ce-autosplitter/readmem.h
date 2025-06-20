#pragma once
#include "includes.h"
#include "timercontrol.h"
#include "externs.h"
#include "misc.h"
#include "imgui_menu.h"

extern bool speedrun_restarted;
extern bool speedrun_finished;

void monitorStatus(ServerSplitter::Timer& timer);
void monitorBsp(ServerSplitter::Timer& timer);
void recordDemo();
void monitorReset(ServerSplitter::Timer& timer);