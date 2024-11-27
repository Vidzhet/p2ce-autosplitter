#pragma once
#include "externs.h"
#include "includes.h"

const std::vector<std::string> ccommand_ignore = { "demo", "toggleconsole", "gameui_", "disconnect", "play", "echo", "setpause", "connect", "record", "stop", "map", "sound"};

int demo_tick();
int demo_maxtick();
std::string demo_time();
std::string demo_maxtime();
void demo_nexttick();
void demo_prevtick();
void demo_forward();
void demo_back();
bool ccommand_valid();
std::string openfile_dem(const std::wstring& initialDir, const std::wstring& cut);
std::wstring openfolder(const std::wstring& initialDir);
std::string demo_getname(const std::string& demopath);
std::vector<std::string> demo_getlist(const std::wstring& folderpath, const std::wstring& cut);