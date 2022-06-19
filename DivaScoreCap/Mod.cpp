#include "pch.h"
#include <Helpers.h>
#include <stdio.h>
#include <cstdint>
#include <SigScan.h>
#include <detours.h>
#include <string>
#include <map>
#include "Diva.h"

typedef struct _DIVA_PV_RESULT {
    std::string PVTitle;
    DIVA_PV PVInfo;
    DIVA_SCORE Score;
    DIVA_STAT Stats;
} DIVA_PV_RESULT;

const uint64_t DivaScoreBaseAddress = 0x00000001412F641c;
const uint64_t DivaScoreCompletionRateAddress = 0x00000001412F64E4;
const uint64_t DivaCurrentPVAddress = 0x00000001412BD22C;
const uint64_t DivaCurrentPVTitleAddress = 0x00000001412F60D8;


void* DivaScoreTrigger = sigScan(
    "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8B\xEC\x48\x83\xEC\x60\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\xF8\x48\x8B\xF9\x80\xB9\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00",
    "xxxx?xxxx?xxxx?xxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxx?????xx????"
);


std::string GetDifficultyName(DIVA_DIFFICULTY difficulty) {
    switch (difficulty) {
    case DIVA_DIFFICULTY::Easy:
        return "Easy";
    case DIVA_DIFFICULTY::Normal:
        return "Normal";
    case DIVA_DIFFICULTY::Hard:
        return "Hard";
    case DIVA_DIFFICULTY::Extreme:
        return "Extreme";
    case DIVA_DIFFICULTY::ExExtreme:
        return "ExExtreme";
    }
    return "Unknown";
}


HOOK(int, __fastcall, _PrintResult, DivaScoreTrigger, int a1) {
    DIVA_SCORE DivaScore = *(DIVA_SCORE*)DivaScoreBaseAddress;
    DIVA_STAT DivaStat = *(DIVA_STAT*)DivaScoreCompletionRateAddress;
    std::string& aTitleBuffer = *(std::string*)DivaCurrentPVTitleAddress;
    DIVA_PV DivaPV = *(DIVA_PV*)DivaCurrentPVAddress;
    if (GetConsoleWindow()) {
        if (GetConsoleOutputCP() != CP_UTF8) {
            SetConsoleOutputCP(CP_UTF8);
        }
        printf("%s (%d) - %s\n", aTitleBuffer.c_str(), DivaPV.Id, GetDifficultyName(DivaPV.Difficulty).c_str());
        printf("- Total: %d; Combo: %d; Cool: %d; Fine: %d; Safe: %d; Sad: %d; Worst: %d\n", DivaScore.TotalScore, DivaScore.Combo, DivaScore.Cool, DivaScore.Fine, DivaScore.Safe, DivaScore.Sad, DivaScore.Worst);
        printf("- Song completion rate: %f%\n", DivaStat.CompletionRate);
    }
    
	return original_PrintResult(a1);
};

extern "C" __declspec(dllexport) void Init()
{
    
	INSTALL_HOOK(_PrintResult);
}
