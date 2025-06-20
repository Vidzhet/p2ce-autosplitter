#pragma once
#include "includes.h"

extern HMODULE g_hmodule;

extern void* g_pTimer;

extern bool livesplit_connected;
extern bool timer_resume;
extern bool ent_cvar_protect;

extern std::wstring gamePath;
extern std::string gameName;
extern std::wstring gameNameW;

extern std::string speedrun_endMap;
extern std::string speedrun_endInput;
extern std::string speedrun_endInputEntity;
