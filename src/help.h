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

// ��ֹʹ�ÿ������캯���� operator= ��ֵ�����ĺ�
// Ӧ����� private: ��ʹ��
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