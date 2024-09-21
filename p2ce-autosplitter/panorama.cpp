#include "panorama.h"

void editMainMenu(HANDLE processHandle, uintptr_t panoramaBaseAddress, uintptr_t gameoverlayrenderer64BaseAddress)
{
    //uintptr_t scriptAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4E58, scriptOffsets);
    //WriteProcessMemory(processHandle, (LPVOID)scriptAddress, "map wakeup_trigger", 32, NULL);


    //uintptr_t labelAddress = ResolvePointerChain(processHandle, panoramaBaseAddress + 0x004F4B00, labelOffsets);
    //std::cout << std::hex << labelAddress << std::dec << std::endl;
    //WriteProcessMemory(processHandle, (LPVOID)labelAddress, "START SPEEDRUN", 32, NULL); // 32 bytes is limit for this type of strings for engine, higher value can cause many errors
}
