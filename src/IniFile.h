/*******************************************************************************
 * class for reading Ini File on Unix/Linux/Window
*******************************************************************************/
#pragma once
#ifndef __CINIFILE_H__
#define __CINIFILE_H__


//************************* begin of -- include ******************************
//#ifndef _CRT_SECURE_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS
//#endif
//
//#ifndef _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_DEPRECATE
//#endif
#include "help.h"
#include <cstdio>   // for FILE
#include <cassert>
#include <map>

// typedef std::basic_string<wchar_t> wstring;

//************************* **end of -- include ******************************


//************************* begin of -- macro definitions ********************
//#define 	M_MAX_BUFFER_SIZE  6000
#define 	M_MAX_VALUE_BUFFER_SIZE 512
#define 	M_MAX_INTVAL_BUFFER_SIZE  32
//#define 	M_MAX_SECTION_SIZE 32
//************************* **end of -- macro definitions ********************

using namespace std;

class CIniFile
{
public:
	CIniFile();
	CIniFile(const wchar_t* wszfile);
	~CIniFile();
	
private:
	FILE* m_pFile;	//p_inifile
	//typedef map<wstring, wstring> keymap;
	map<wstring, map<wstring, wstring>> m_mapData;
	//map<std::basic_string<wchar_t>, map<std::basic_string<wchar_t>, std::basic_string<wchar_t>>> m_mapData;
	//map<wstring, wstring>::iterator intertr;
	bool m_pChanged;
	wchar_t* m_wszfile;

public:
	int OpenFile(const wchar_t* wszFile);

	bool GetString(const wchar_t* wszSection, const wchar_t* wszKey, wchar_t* wszValue);
	bool SetString(const wchar_t* wszSection, const wchar_t* wszKey, const wchar_t* wszValue);

	int GetInteger(const wchar_t* wszSection, const wchar_t* wszKey, int default_value = 0);
	bool SetInteger(const wchar_t* wszSection, const wchar_t* wszKey, const int value);

	long GetLong(const wchar_t* wszSection, const wchar_t* wszKey, long default_value = 0);
	bool SetLong(const wchar_t* wszSection, const wchar_t* wszKey, const long value);

	double GetDouble(const wchar_t* wszSection, const wchar_t* wszKey, double default_value = 0);
	bool SetDouble(const wchar_t* wszSection, const wchar_t* wszKey, const double value);

	bool GetBool(const wchar_t* wszSection, const wchar_t* wszKey, bool default_value = false);
	bool SetBool(const wchar_t* wszSection, const wchar_t* wszKey, const bool value);

private:
	// DISALLOW_COPY_AND_ASSIGN(CIniFile);

	int CloseFile(void);
	bool m_bDebug;
};

#endif // __CINIFILE_H__