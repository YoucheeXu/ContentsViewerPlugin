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
#include <cstring>
#include <tchar.h>

// typedef std::basic_string<TCHAR> tString;

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
	CIniFile(const TCHAR* tszfile);
	~CIniFile();
	
private:
	FILE* m_pFile;	//p_inifile
	//typedef map<tString, tString> keymap;
	map<tString, map<tString, tString>> m_mapData;
	//map<std::basic_string<TCHAR>, map<std::basic_string<TCHAR>, std::basic_string<TCHAR>>> m_mapData;
	//map<tString, tString>::iterator intertr;
	bool m_pChanged;
	TCHAR* m_tszfile;

public:
	int OpenFile(const TCHAR* psz_file);

	bool GetString(const TCHAR* section, const TCHAR* key, TCHAR* value);
	bool SetString(const TCHAR* section, const TCHAR* key, const TCHAR* value);

	int GetInteger(const TCHAR* section, const TCHAR* key, int default_value = 0);
	bool SetInteger(const TCHAR* section, const TCHAR* key, const int value);

	long GetLong(const TCHAR* section, const TCHAR* key, long default_value = 0);
	bool SetLong(const TCHAR* section, const TCHAR* key, const long value);

	double GetDouble(const TCHAR* section, const TCHAR* key, double default_value = 0);
	bool SetDouble(const TCHAR* section, const TCHAR* key, const double value);

	bool GetBool(const TCHAR* section, const TCHAR* key, bool default_value = false);
	bool SetBool(const TCHAR* section, const TCHAR* key, const bool default_value);

private:
	// DISALLOW_COPY_AND_ASSIGN(CIniFile);
	//bool Init(void);
	int CloseFile(void);
	bool m_bDebug;
};

#endif // __CINIFILE_H__