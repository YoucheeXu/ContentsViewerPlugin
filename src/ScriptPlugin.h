//CScriptPlugin.h
#pragma once
#ifndef _CSCRIPTPLUGIN_H_
#define _CSCRIPTPLUGIN_H_

#include "help.h"

typedef int(* SetHost)(HWND hWnd);
typedef int(* Exe)(const char* pyFile);
typedef int(* WExe)(const wchar_t* pyFile);

class CScriptPlugin
{
public :
	CScriptPlugin();
	virtual ~CScriptPlugin();
	
private:
	DISALLOW_COPY_AND_ASSIGN(CScriptPlugin);

private:
	bool m_bDebug;
	HMODULE m_hModule;

public:
	int Load(const wchar_t* wszPluginDLL);
	int Execute(const char* szPyFile);
	int Execute(const wchar_t* wszPyFile);

private:

};

#endif //_CSCRIPTPLUGIN_H_