#include "pch.h"
#include <Helpers.h>
#include <stdio.h>
#include <cstdint>
#include <SigScan.h>
#include <detours.h>

typedef struct _DIVA_STAT {
    float CompletionRate;
} DIVA_STAT;
typedef struct _DIVA_SCORE {
    unsigned int TotalScore;
    unsigned int Unknown1;
    unsigned int Unknown2;
    unsigned int Unknown3;
    unsigned int Unknown4;
    unsigned int Unknown5;
    unsigned int Unknown6;
    unsigned int Unknown7;
    unsigned int Combo;
    unsigned int preAdjustCool;
    unsigned int preAdjustFine;
    unsigned int preAdjustSafe;
    unsigned int preAdjustSad;
    unsigned int preAdjustWorst;
    unsigned int Cool;
    unsigned int Fine;
    unsigned int Safe;
    unsigned int Sad;
    unsigned int Worst;
} DIVA_SCORE;

const uint64_t DivaScoreBaseAddress = 0x00000001412F641c;
const uint64_t DivaScoreCompletionRateAddress = 0x00000001412F64E4;

void* DivaScoreTrigger = sigScan(
    "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8B\xEC\x48\x83\xEC\x60\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\xF8\x48\x8B\xF9\x80\xB9\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00",
    "xxxx?xxxx?xxxx?xxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxx?????xx????"
);

HOOK(int, __fastcall, _PrintResult, DivaScoreTrigger, int a1) {
    HANDLE hCurrentProcess = GetCurrentProcess();
    HANDLE hConsoleWindow = GetConsoleWindow();
    DIVA_SCORE DivaScore{};
    DIVA_STAT DivaStat{};
    if (hCurrentProcess) {
        size_t nBufferRead;
        if (ReadProcessMemory(hCurrentProcess, (LPCVOID)DivaScoreBaseAddress, &DivaScore, sizeof(DIVA_SCORE), &nBufferRead) && nBufferRead > 0) {
            if (hConsoleWindow)
                printf("Total: %d; Combo: %d; Cool: %d; Fine: %d; Safe: %d; Sad: %d; Worst: %d\n", DivaScore.TotalScore, DivaScore.Combo, DivaScore.Cool, DivaScore.Fine, DivaScore.Safe, DivaScore.Sad, DivaScore.Worst);
        }
        if (ReadProcessMemory(hCurrentProcess, (LPCVOID)DivaScoreCompletionRateAddress, &DivaStat, sizeof(DIVA_SCORE), &nBufferRead) && nBufferRead > 0) {
            if (hConsoleWindow)
                printf("Song completion rate: %f\n", DivaStat.CompletionRate);
        }
    }
	return original_PrintResult(a1);
};
extern "C" __declspec(dllexport) void Init()
{
	INSTALL_HOOK(_PrintResult);
}
