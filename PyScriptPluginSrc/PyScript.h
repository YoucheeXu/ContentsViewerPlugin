//PyScriptInterface.h
#pragma once
#ifndef _PYSCRIPT_H_
#define _PYSCRIPT_H_

#include <Python.h>
// #include "PyScriptPlugin.h"

extern void log2File(int level, const char* szMsg, ...);

extern void log2File(int level, const wchar_t* wszMsg, ...);

PyMODINIT_FUNC PyInit_thePlugin_module(void);

PyMODINIT_FUNC PyInit_theDocument_module(void);

PyMODINIT_FUNC PyInit_theCViewerDlg_module(void);

#endif //_PYSCRIPT_H_