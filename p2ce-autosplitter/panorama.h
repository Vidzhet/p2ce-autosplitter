#pragma once
#include "includes.h"
#include "misc.h"
#include "offsets.h"
#include "timercontrol.h"

void editMainMenu(HANDLE processHandle, uintptr_t panoramaBaseAddress, uintptr_t gameoverlayrenderer64BaseAddress);
void start_loglabel(HANDLE processHandle, uintptr_t panoramaBaseAddress, ServerSplitter::Timer &timer);
