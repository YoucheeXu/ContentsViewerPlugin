//CScriptPlugin.cpp
#include "ScriptPlugin.h"
#include "ContentsViewer.h"

CScriptPlugin::CScriptPlugin()
{
	m_bDebug = true;
	m_hModule = NULL;
}

CScriptPlugin::~CScriptPlugin()
{
	if(NULL != m_hModule)
		FreeLibrary(m_hModule);
}

int CScriptPlugin::Load(const wchar_t* wszPluginDLL)
{
	m_hModule = LoadLibrary(wszPluginDLL);
	if(NULL == m_hModule)
	{
		LOGERR(L"Fail to load %s", wszPluginDLL);
		return 0;
	}
	else
	{
		LOGOK(L"Success to load %s", wszPluginDLL);
	}

	SetHost func = (SetHost)GetProcAddress(m_hModule, "SetHost");
	return func(thePlugin.GetCVDlgHWnd());
}

int CScriptPlugin::Execute(const char* szPyFile)
{
	Exe exe = (Exe)GetProcAddress(m_hModule, "ExecuteA");
	return exe(szPyFile);
}

int CScriptPlugin::Execute(const wchar_t* wszPyFile)
{
	WExe wexe = (WExe)GetProcAddress(m_hModule, "ExecuteW");
	return wexe(wszPyFile);
}