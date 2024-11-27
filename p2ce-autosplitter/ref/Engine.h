#pragma once
#include <cstdint>
#include "Interface.h"

#define MODULE(name) name ".dll"

class Engine {
public:
    Interface* engineClient = nullptr;
    using _ClientCmd = int(__fastcall*)(void* thisptr, const char* szCmdString);
    using _GetLocalClient = uintptr_t(__fastcall*)(int index);
    using _Cbuf_AddText = void(__fastcall*)(int eTarget, const char* pText, int cmdSource, int nTickDelay);
    using _GetActiveSplitScreenPlayerSlot = int (*)();
    using _ClientCmdTemp = int(__fastcall*)(void* thisptr, const char* szCmdString);
    _ClientCmdTemp ClientCmdTemp = nullptr;
    _ClientCmd ClientCmd = nullptr;
    _GetActiveSplitScreenPlayerSlot GetActiveSplitScreenPlayerSlot = nullptr;
    _Cbuf_AddText Cbuf_AddText = nullptr;
    _GetLocalClient GetLocalClient = nullptr;
    
    void ConsoleCommand(const char* text, int delay = 0);
    void Init();
};

extern Engine* engine;
