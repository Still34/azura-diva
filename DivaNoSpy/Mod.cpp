#pragma once

#include "pch.h"
#include "Helpers.h"
#include <stdlib.h>
#include <stdio.h>
#include <Psapi.h>
#include <stdint.h>
bool sigValid = true;

MODULEINFO moduleInfo;

inline const MODULEINFO& getModuleInfo()
{
    if (moduleInfo.SizeOfImage)
        return moduleInfo;

    ZeroMemory(&moduleInfo, sizeof(MODULEINFO));
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(MODULEINFO));

    return moduleInfo;
}

inline void* sigScan(const char* signature, const char* mask)
{
    const MODULEINFO& info = getModuleInfo();
    const size_t length = strlen(mask);

    for (size_t i = 0; i < info.SizeOfImage; i++)
    {
        char* memory = (char*)info.lpBaseOfDll + i;

        size_t j;
        for (j = 0; j < length; j++)
        {
            if (mask[j] != '?' && signature[j] != memory[j])
                break;
        }

        if (j == length)
            return memory;
    }

    return nullptr;
}

#define SIG_SCAN(x, ...) \
    void* x(); \
    void* x##Addr = x(); \
    void* x() \
    { \
        static const char* x##Data[] = { __VA_ARGS__ }; \
        if (!x##Addr) \
        { \
            for (int i = 0; i < _countof(x##Data); i += 2) \
            { \
                x##Addr = sigScan(x##Data[i], x##Data[i + 1]); \
                if (GetConsoleWindow() != NULL) \
                    printf("[Signature] %s received: 0x%08x\n", #x, x##Addr); \
                if (x##Addr) \
                    return x##Addr; \
            } \
            sigValid = false; \
        } \
        return x##Addr; \
    }

SIG_SCAN(
	sigSendHardwareInfo,
	"\x48\x8B\xC4\x48\x89\x58\x18\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\x70\xB8\x0F\x29\x78\xA8\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x48\x8B\xFA\x48\x8B\xD9\xC6\x85\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x4C\x8B\xC0\x48\x8D\x8D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x41\x86\x00\x48\x8D\x0D\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x48\x89\x1D\x00\x00\x00\x00\x48\x89\x3D\x00\x00\x00\x00\x48\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00",
	"xxxxxxxxxxxxxxxxxxxxx????xxx????xxxxxxxxxxx????xxxxxx????xxxxxxxx?????x????xxxxxx????x????xxxxxx????xx????xxx????xxx????xxx????????"
);
SIG_SCAN(
	sigSendHeartbeat,
	"\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x55\x48\x8D\x6C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\x50\x33\xF6\x48\x89\x74\x24\x00\x66\x0F\x6F\x05\x00\x00\x00\x00\x66\x0F\x7F\x44\x24\x00\x40\x88\x74\x24\x00\x48\x89\x75\x80\x66\x0F\x7F\x45\x00",
	"xxxx?xxxx?xxxx?xxxxx?xxx????xxx????xxxxxxxxxxxxx?xxxx????xxxxx?xxxx?xxxxxxxx?"
);
SIG_SCAN(
	sigSendCloseSignal,
	"\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x55\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x0F\xB6\x08\x90\x84\xC9\x0F\x84\x00\x00\x00\x00\xC6\x45\xD0\x00\xE8\x00\x00\x00\x00",
	"xxxx?xxxx?xxxx?xxxxx????xxx????xxx????xxxxxx????x????xxxxxxxx????xxxxx????"
);

extern "C" __declspec(dllexport) void Init()
{
    WRITE_MEMORY(sigSendCloseSignal(), uint8_t, 0xc3, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(sigSendHeartbeat(), uint8_t, 0xc3, 0x90, 0x90, 0x90, 0x90);
    WRITE_MEMORY(sigSendHardwareInfo(), uint8_t, 0xc3, 0x90, 0x90);
}
