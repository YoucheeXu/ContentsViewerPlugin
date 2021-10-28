// PyScriptPlugin.h
#pragma once
#ifndef _PYSCRIPTPLUGIN_H_
#define _PYSCRIPTPLUGIN_H_

#include <Windows.h>
#include "..\src\ContentsViewerMsgs.h"

#define DLL_EXPORT	extern "C" __declspec(dllexport)

extern LRESULT SendCVMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);

extern void pluginInitialize(HANDLE hModule);

extern void pluginCleanUp();

DLL_EXPORT int ExecuteA(const char *pyFile);

extern int ExecutePyScript(const char *szPyFile);

extern int ExecutePyCmd(const char* cmd);

#endif //_PYSCRIPTPLUGIN_H_