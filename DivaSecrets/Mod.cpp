#include "pch.h"
#include <detours.h>
#include <Helpers.h>
#include <stdio.h>
#include <SigScan.h>

void* EncryptSaveData = sigScan(
    "\x40\x53\x55\x56\x57\x41\x54\x41\x57\x48\x83\xEC\x78\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x44\x24\x00\x4C\x8B\xA4\x24\x00\x00\x00\x00\x48\x8B\xF1\x49\x63\xD8\x4D\x8B\xF9\x48\x8B\xCB\x48\x89\x5C\x24\x00\x48\x8B\xFA\xE8\x00\x00\x00\x00\x4C\x8B\xCB\x48\x8D\x54\x24\x00\x4C\x8B\xC7\x48\x8B\xC8\x48\x8B\xE8",
    "xxxxxxxxxxxxxxxx????xxxxxxx?xxxx????xxxxxxxxxxxxxxxx?xxxx????xxxxxxx?xxxxxxxxx"
);

HOOK(int, __fastcall, _GetKey, EncryptSaveData, DWORD* a1, __int64 a2, int a3, unsigned __int8** a4, char* a5) {
    HANDLE hConsoleWindow = GetConsoleWindow();
    if (hConsoleWindow) {
        printf("[DivaSecrets] Obtained key \"%s\"\n", *a1);
    }
    return original_GetKey(a1, a2, a3, a4, a5);
};

extern "C" __declspec(dllexport) void Init()
{
    INSTALL_HOOK(_GetKey);
}