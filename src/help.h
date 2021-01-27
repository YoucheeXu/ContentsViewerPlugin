#pragma once
#ifndef _HELP_H_
#define _HELP_H_

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

//#ifndef _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_DEPRECATE
//#endif

#include "LogFile.h"
#include "..\third_party\Win32xx_860\include\wxx_wincore.h"

// ��ֹʹ�ÿ������캯���� operator= ��ֵ�����ĺ�
// Ӧ����� private: ��ʹ��
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&); \
	void operator=(const TypeName&)

wchar_t* _wcstrim(wchar_t* wsz);
wchar_t* _wcstrimAll(wchar_t* wsz);
//TCHAR* _tcstrimLeft(const TCHAR* str);
//TCHAR* _tcstrimRight(const TCHAR* str);
//TCHAR* _tcstrim(const TCHAR* str);
void StringTrimLeft(wstring& wStr);
void StringTrimRight(wstring& wStr);
void StringReplace(wstring& wOrg, const wchar_t* wszFrom, const wchar_t* wszTo);
size_t StringReplaceAll(wstring& wStr, const wstring wSrcStr, const wstring wDesStr, bool bRegExp = true);
void StringErase(wstring& wOrg, const wchar_t* wszDrop);
int StringEraseAll(wstring& wOrg, const wchar_t* wszDrop);
//bool CheckPath(TCHAR* path);

#endif //_HELP_H_