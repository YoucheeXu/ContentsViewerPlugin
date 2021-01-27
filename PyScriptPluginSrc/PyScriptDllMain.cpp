// PyScriptDllMain.cpp
#include "PyScriptPlugin.h"

//BOOL APIENTRY DllMain( HANDLE hModule, 
//					  DWORD  reasonForCall, 
//					  LPVOID lpReserved )
extern "C" BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	//switch (reasonForCall)
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		pluginInitialize(hInstance);
		break;

	case DLL_PROCESS_DETACH:
		pluginCleanUp();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}

/*int main(int argc, char* argv[])
{
	pluginInitialize(NULL);

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("print('python start')");
	PyRun_SimpleString("print('python end')");
}*/