#pragma once
#include "includes.h"
#include "misc.h"
#include "offsets.h"
#include "timercontrol.h"
#include "readmem.h"

#define GAME_LOADED 0

extern bool livesplit_connected;

void editMainMenu(HANDLE processHandle, uintptr_t panoramaBaseAddress, uintptr_t gameoverlayrenderer64BaseAddress);
void start_loglabel(ServerSplitter::Timer &timer);
void console_log(std::string str);

//class Panorama
//{
//public:
//    Panorama();
//    ~Panorama();
//    static
//
//private:
//
//};
//
//Panorama::Panorama()
//{
//}
//
//Panorama::~Panorama()
//{
//}
