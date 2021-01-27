/*******************************************************************************
class for log File on Unix/Linux/Window

Version :  v1.0.0.0
Description :
change Log

************************************************************************/
#pragma once
#ifndef __LOGFILE__H__
#define __LOGFILE__H__

#ifdef _WIN32
#pragma once
#endif

//#ifndef _CRT_SECURE_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS
//#endif
//
//#ifndef _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_DEPRECATE
//#endif
//
//#define _CRT_NON_CONFORMING_SWPRINTFS
#pragma warning (disable: 4996)
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>
#ifdef _WIN32
#include <tchar.h>
#include <winsock2.h>
#include <windows.h>
//#else
//	#ifdef _UNICODE
//		typedef wchar_t	TCHAR
//	#else
//		typedef char	TCHAR
//	#endif
#endif

using namespace std;

class CLogFile
{
public:
	CLogFile(const TCHAR* file) 
	{
		m_pLog = NULL;
		StartLog(file);
		//_tcscpy(m_tszFileName, file);
	}
	CLogFile()
	{
		m_pLog = NULL;
		//StartLog(_T("D:\\Log_ReadMdict.txt"));
		//_tcscpy(m_tszFileName, _T("D:\\Log.txt"));
	}

	virtual ~CLogFile(void) {
		if(m_pLog != NULL) fclose(m_pLog);
	};

private:
	FILE* m_pLog;

public:
	void StartLog(const TCHAR* pFilePath) 
	{
		// LOGINFO(_T("will log in %s."), pFilePath);
		OutputDebugString(pFilePath);
		//m_pLog = _tfopen(_T("C:\\log.txt"), _T("w"));
		m_pLog = _tfopen(pFilePath, _T("w+"));
		//_tfopen_s(&m_pLog, pFilePath, _T("w+"));
		assert(m_pLog);

		LogTime();
	}

	/*void GetName(TCHAR* name) {
		_tcscpy(name, m_tszFileName);
	}

	TCHAR* GetName() {
		return m_tszFileName;
	}*/

	void LogTime()
	{
		wchar_t datebuf[128];
		wchar_t timebuf[128];

		/* Display operating system-style date and time. */
		_wstrdate(datebuf);
		//fprintf(m_pLog, "OS time:\t%s", tmpbuf); //打印当前日期
		_wstrtime(timebuf);
		//fprintf(m_pLog, "\t%s\n\n", tmpbuf); //打印当前时间
		//fflush(m_pLog);
		//LogOut(_T("Start to log.\n\n"));
		LogOut(_T("OS time:\t%s\t%s\n\n"), datebuf, timebuf);
	}

	void LogOut(const wchar_t* wString, ...)
	{
		// assert(m_pLog);

		wchar_t wBuffer[4096];
		va_list pArgList;

		// The va_start macro (defined in STDARG.H) is usually equivalent to:
		// pArgList = (char *) &szFormat + sizeof (szFormat) ;

		va_start(pArgList, wString);

		// The last argument to wvsprintf points to the arguments

		// int n = sizeof(wBuffer) / sizeof(wchar_t);

		_vsnwprintf(wBuffer, sizeof(wBuffer) / sizeof(wchar_t),
			wString, pArgList);

		// The va_end macro just zeros out pArgList for no good reason

		va_end(pArgList);

#ifdef _WIN32
		OutputDebugStringW(wBuffer);
		//OutputDebugStringW(_T("\n"));
#else

#endif
		if (NULL == m_pLog) return;

#ifdef UNICODE
		_wsetlocale(0, _T("chs")); //必须加上，否则fwprintf对中文不支持
#endif
		fwprintf(m_pLog, wBuffer);
		//fwprintf(m_pLog, _T("\n"));

		fflush(m_pLog);
	}

	void LogOut(const char* szString, ...)
	{
		// assert(m_pLog);

		char szBuffer[1024];
		va_list pArgList;

		// The va_start macro (defined in STDARG.H) is usually equivalent to:
		// pArgList = (char *) &szFormat + sizeof (szFormat) ;

		va_start(pArgList, szString);

		// The last argument to wvsprintf points to the arguments
	
		_vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(char),
			szString, pArgList);

		// The va_end macro just zeroes out pArgList for no good reason

		va_end(pArgList);

#ifdef _WIN32
		OutputDebugStringA(szBuffer);
		//OutputDebugStringA("\n");
#else

#endif
		if (NULL == m_pLog) return;
		//_twsetlocale(0, T"chs"); //必须加上，否则fwprintf对中文不支持

		fprintf(m_pLog, szBuffer);
		//fprintf(m_pLog, "\n");

		fflush(m_pLog);
	}
};

//extern CLogFile theLogFile;
extern bool g_bDebug;
extern CLogFile* g_pLogFile; //= &theLogFile;

//for debuging
#define LOGORNOT	\ g_bDebug && m_bDebug
#define LOGORNOT2	\ g_bDebug && bDebug

#define BGNLOG		\
	bool m_bDebug = true;

#define ENDLOG		\
	bool m_bDebug = false;

//#define LOGMSG		\
//	if(g_bDebug && m_bDebug) theLogFile.LogOut
#define LOGMSG		\
	if(g_bDebug && m_bDebug) g_pLogFile->LogOut
	
//#define LOGOUT theLogFile.LogOut
#define LOGOUT g_pLogFile->LogOut

// #define LOGMSG theLogFile.LogOut
// #define LOGERR theLogFile.LogOut

// #define LOGMSG LOGOUT
// #define LOGMSGA LOGOUTA

#define LOGFUNBGN		\
	m_bDebug = true;		\
	if(g_bDebug && m_bDebug) {	\
		LOGOUT(__FUNCTION__);		\
		LOGOUT("\nLine: %d", __LINE__);		\
		LOGOUT(_T(" is to start.\n"));	\
	}

#define LOGFUNEND		\
	if(g_bDebug && m_bDebug) {	\
		LOGOUT(__FUNCTION__);	\
		LOGOUT(_T(" is finished.\n"));		\
	}

#define LOGFUNMSG		\
	if(g_bDebug) {		\
		LOGOUT(__FUNCTION__);	\
		LOGOUT("\nLine: %d", __LINE__);		\
		LOGOUT("\n");		\
	}
	
#define LOGINFO(tInfo, ...)	\
	if(g_bDebug && m_bDebug) {	\
		LOGOUT("INFO:\t");	\
		LOGOUT(tInfo, ## __VA_ARGS__);		\
		LOGOUT("\n");		\
	}

#define LOGERR(tErr, ...)\
	if(g_bDebug && m_bDebug) {	\
		LOGOUT("ERROR:\t");	\
		LOGOUT(tErr, ## __VA_ARGS__);	\
		LOGOUT("\n");	\
	}

#define LOGOK(tOK, ...)	\
	if(g_bDebug && m_bDebug) {	\
		LOGOUT("OK:\t");		\
		LOGOUT(tOK, ## __VA_ARGS__);		\
		LOGOUT("\n\n");		\
	}

#define VOK(b) 	\
	if(!b) {	\
		LOGERR(#b); 	\
		return false;	\
	}

//#define VLOG(b)	\
//	if(!b) {	\
//		LOGERR(#b);		\
//		return false;	\
//	}	\
//	else LOGOK(#b);

#define VLOG(b) 	\
	if(!b) {LOGERR(#b); return false;}	\
		else LOGOK(#b);

#endif//!__LOGFILE__H__