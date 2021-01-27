#pragma once
#ifndef _HELP_H_
#define _HELP_H_

//#ifndef _CRT_SECURE_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS
//#endif
//
//#ifndef _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_DEPRECATE
//#endif

#include <stdio.h>
#include <tchar.h>
#include <regex>
#include "LogFile.h"
#include "Win32xx_860/include/wxx_wincore.h"

// 禁止使用拷贝构造函数和 operator= 赋值操作的宏
// 应该类的 private: 中使用
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&); \
	void operator=(const TypeName&)

TCHAR* _tcstrim(TCHAR* str);
void StringTrimLeft(tString& str);
void StringTrimRight(tString& str);
void StringReplace(tString& tOrg, const TCHAR* tszFrom, const TCHAR* tszTo);
int StringReplaceAll(tString& tOrg, const TCHAR* tszFrom, const TCHAR* tszTo, bool bRegrex = false);
//int StringRegReplaceAll(tString& tOrig, wregex regExpress, const TCHAR* tszReplace);
void StringErase(tString& tOrg, const TCHAR* tszDrop);
int StringEraseAll(tString& tOrg, const TCHAR* tszDrop);
bool CheckPath(TCHAR* path);

#endif //_HELP_H_