#pragma once
#include "includes.h"
#include "misc.h"
#include "offsets.h"
#include "timercontrol.h"
#include "readmem.h"

void editMainMenu(HANDLE processHandle, uintptr_t panoramaBaseAddress, uintptr_t gameoverlayrenderer64BaseAddress);
void start_loglabel(HANDLE processHandle, uintptr_t panoramaBaseAddress, LPCVOID mapAddress, ServerSplitter::Timer &timer);
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
