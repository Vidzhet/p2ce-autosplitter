#include "Engine.h"
#include "Interface.h"

void Engine::ConsoleCommand(const char* text, int delay)
{
    //auto slot = this->GetActiveSplitScreenPlayerSlot();
    int slot = 0;
    this->Cbuf_AddText(slot, text, 0, delay);
}

void Engine::Init()
{
    for (bool worked = false; !worked;) {
        try {
            // NEEDED !!!
            this->engineClient = Interface::Create(MODULE("engine"), "VEngineClient0", false);
            this->ClientCmd = this->engineClient->Original<_ClientCmdTemp>(44);
            Memory::Read<_Cbuf_AddText>((uintptr_t)this->ClientCmd + 54, &this->Cbuf_AddText);
            // auto g_SplitScreenMgr1 = Memory::Read<void*>((uintptr_t)this->GetLocalClient1 + 9);
            this->GetActiveSplitScreenPlayerSlot = this->engineClient->Original<_GetActiveSplitScreenPlayerSlot>(5);
            // this->GetActiveSplitScreenPlayerSlot1 = Memory::VMT<_GetActiveSplitScreenPlayerSlot1>(g_SplitScreenMgr1, 5);

            engine->ConsoleCommand("echo init Cbuf_AddText done.", 0);
            worked = true;
        }
        catch (std::exception&) {
            // zaglushka
            //MessageBoxA(nullptr, ex.what(), "exeption", MB_OK);
        }
    }
}

Engine* engine;
