// DllMain.cpp

//#include <Windows.h>
#include "ContentsViewer.h"
#include "Plugin.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
// extern "C" BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	//switch (reasonForCall)
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		//pluginInit(hModule);
		//thePlugin.Initialize(hInstance);
		thePlugin.Initialize(static_cast<HINSTANCE>(hModule));
		break;

	case DLL_PROCESS_DETACH:
		commandMenuCleanUp();
		//pluginCleanUp();
		thePlugin.Uninitialize();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}