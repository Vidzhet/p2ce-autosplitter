#pragma once
#include <cstdint>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#define MAX_PATH 4096
#endif

#include <memory>
#include <string>
#include <vector>

namespace Memory {

struct ModuleInfo {
    char name[MAX_PATH];
    uintptr_t base;
    uintptr_t size;
    char path[MAX_PATH];
};

extern std::vector<ModuleInfo> moduleList;

bool TryGetModule(const char* moduleName, ModuleInfo* info);
const char* GetModulePath(const char* moduleName);
void* GetModuleHandleByName(const char* moduleName);
void CloseModuleHandle(void* moduleHandle);
std::string GetProcessName();

uintptr_t FindAddress(const uintptr_t start, const uintptr_t end, const char* target);
uintptr_t Scan(const char* moduleName, const char* pattern, int offset = 0);
std::vector<uintptr_t> MultiScan(const char* moduleName, const char* pattern, int offset = 0);

#ifdef _WIN32
class Patch {
private:
    uintptr_t location;
    unsigned char* original;
    size_t size;

public:
    ~Patch();
    bool Execute(uintptr_t location, unsigned char *bytes, size_t size);
	template <size_t size>
	bool Execute(uintptr_t location, unsigned char (&bytes)[size]) {
		return Execute(location, bytes, size);
	}
    bool Restore();
};
#endif

struct Pattern {
    const char* signature;
    std::vector<int> offsets;
};

typedef std::vector<int> Offset;
typedef std::vector<const Pattern*> Patterns;

#define PATTERN(name, sig, ...) Memory::Pattern name { sig, Memory::Offset({ __VA_ARGS__ }) }
#define PATTERNS(name, ...) Memory::Patterns name({ __VA_ARGS__ })

std::vector<uintptr_t> Scan(const char* moduleName, const Pattern* pattern);
std::vector<std::vector<uintptr_t>> MultiScan(const char* moduleName, const Patterns* patterns);

template <typename T = uintptr_t>
T Absolute(const char* moduleName, int relative)
{
    auto info = Memory::ModuleInfo();
    return (Memory::TryGetModule(moduleName, &info)) ? (T)(info.base + relative) : (T)0;
}
template <typename T = void*>
T GetSymbolAddress(void* moduleHandle, const char* symbolName)
{
#ifdef _WIN32
    return (T)GetProcAddress((HMODULE)moduleHandle, symbolName);
#else
    return (T)dlsym(moduleHandle, symbolName);
#endif
}
template <typename T = void*>
inline T VMT(void* ptr, int index)
{
    return reinterpret_cast<T>((*((void***)ptr))[index]);
}
template <typename T = uintptr_t>
inline T Read(uintptr_t source)
{
    auto rel = *reinterpret_cast<int32_t*>(source);
    return (T)(source + rel + sizeof(int32_t));
}
template <typename T = uintptr_t>
void Read(uintptr_t source, T* destination)
{
    auto rel = *reinterpret_cast<int32_t*>(source);
    *destination = (T)(source + rel + sizeof(int32_t));
}
template <typename T = void*>
inline T Deref(uintptr_t source)
{
    return *reinterpret_cast<T*>(source);
}
template <typename T = void*>
void Deref(uintptr_t source, T* destination)
{
#ifdef __x86_64
    *destination = Read<T>(source);
#else
    *destination = *reinterpret_cast<T*>(source);
#endif
}
template <typename T = uintptr_t>
T Scan(const char* moduleName, const char* pattern, int offset = 0)
{
    uintptr_t result = 0;

    auto info = Memory::ModuleInfo();
    if (Memory::TryGetModule(moduleName, &info)) {
        auto start = uintptr_t(info.base);
        auto end = start + info.size;
        result = Memory::FindAddress(start, end, pattern);
        if (result) {
            result += offset;
        }
    }
    return reinterpret_cast<T>(result);
}
}
