
#include "pch.h"
#include <Helpers.h>
#include <stdlib.h>
#include <stdio.h>
#include <Psapi.h>
#include <stdint.h>
#include <SigScan.h>
#include <string>
#include <format>
#include <filesystem>
#include <Shlobj.h>
#include <detours.h>

void* sig_fwrite = sigScan(
    "\x48\x8B\xC4\x4C\x89\x48\x20\x4C\x89\x40\x18\x48\x89\x50\x10\x48\x89\x48\x08\x55\x48\x8B\xEC\x48\x83\xEC\x60\x48\x85\xD2\x74\x1A\x4D\x85\xC0\x74\x15\x4D\x85\xC9\x75\x18\xE8\x00\x00\x00\x00\xC7\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x33\xC0\x48\x83\xC4\x60\x5D\xC3",
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xx????x????xxxxxxxx"
);
const char* aBackupDirectory = ".divabackup";

bool TryPrintToConsole(const char* _Format, ...) {
    bool bIsConsoleOpen = GetConsoleWindow();
    if (bIsConsoleOpen) {
        va_list args;
        va_start(args, _Format);
        vprintf(_Format, args);
        va_end(args);
    }
    return bIsConsoleOpen;
}

HOOK(size_t, __fastcall, _fwriteTo, sig_fwrite, const void* buf, size_t size, size_t n, FILE* fp) {
    std::string targetFilename;
    std::filesystem::path fsTargetAbsolutePath;
    PWSTR pwProfilePath;
    FILE* backupFp;
    
    if (size == 0x30) {
        targetFilename = "localsys";
    }
    else if (size == 0x120) {
        targetFilename = "pl";
    }
    else if (size >= 0x3a50) {
        targetFilename = "sys";
    }
    else {
        targetFilename = "unknown";
    }
    targetFilename = std::format("{}-{}.dat", targetFilename, GetTickCount64());

    if (SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &pwProfilePath) == S_OK) {
        fsTargetAbsolutePath = std::filesystem::path(pwProfilePath) / aBackupDirectory;
        bool bIsDirectoryReady = std::filesystem::exists(fsTargetAbsolutePath);
        if (!bIsDirectoryReady) {
            bIsDirectoryReady = std::filesystem::create_directory(fsTargetAbsolutePath);
        }
        if (bIsDirectoryReady) {
            auto sTargetAbsolutePath = (fsTargetAbsolutePath / targetFilename).string();
            if (fopen_s(&backupFp, sTargetAbsolutePath.c_str(), "wb") == 0) {
                fwrite(buf, size, n, backupFp);
                if (ferror(backupFp)) {
                    TryPrintToConsole("[DivaBackup] Failed to write to %s.\n", sTargetAbsolutePath.c_str());
                }
                else {
                    TryPrintToConsole("[DivaBackup] Backed up %s.\n", sTargetAbsolutePath.c_str());
                }
                fclose(backupFp);
            }
            else {
                TryPrintToConsole("[DivaBackup] Failed to open %s.\n", sTargetAbsolutePath.c_str());
            }
        }
        else {
            TryPrintToConsole("[DivaBackup] Failed to create %s.\n", fsTargetAbsolutePath.string().c_str());
        }
    }
    CoTaskMemFree(pwProfilePath);
    return original_fwriteTo(buf, size, n, fp);
};

extern "C" __declspec(dllexport) void Init()
{
    INSTALL_HOOK(_fwriteTo);
}
