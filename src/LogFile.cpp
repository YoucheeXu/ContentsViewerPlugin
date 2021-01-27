#include "LogFile.h"

void PrintDebugString(const char* szString, ...)
{
	char szBuffer[4096];
	va_list pArgList;

	va_start(pArgList, szString);

	_vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(char),
		szString, pArgList);

	va_end(pArgList);

#ifdef _WIN32
	OutputDebugStringA(szBuffer);
#else

#endif
}

void PrintDebugString(const wchar_t* wszString, ...)
{
	wchar_t wszBuffer[4096];
	va_list pArgList;

	va_start(pArgList, wszString);

	_vsnwprintf(wszBuffer, sizeof(wszBuffer) / sizeof(wchar_t),
		wszString, pArgList);

	va_end(pArgList);

#ifdef _WIN32
	OutputDebugStringW(wszBuffer);
#else

#endif
}