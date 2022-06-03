#include "pch.h"
#include "Helpers.h"
#include "isteamscreenshots.h"
#include "steam_api.h"
DWORD TakeScreenShotAfterDelay(LPVOID lpParam);
HOOK(int, __fastcall, _PrintResult, 0x0000000140652EC0, int a1) {
	bool bIsSteamApiAvailable = SteamScreenshots()->IsScreenshotsHooked();
	if (bIsSteamApiAvailable) {
		CreateThread(NULL, 0, TakeScreenShotAfterDelay, NULL, 0, 0);
	}
	return original_PrintResult(a1);
};
DWORD TakeScreenShotAfterDelay(LPVOID lpParam) {
	Sleep(3000);
	SteamScreenshots()->TriggerScreenshot();
	return 0;
}

extern "C" __declspec(dllexport) void Init()
{
	INSTALL_HOOK(_PrintResult);
}
