#include "help.h"
#include <regex>

bool g_bDebug;
//CLogFile* g_pLogFile;
using namespace std;

wchar_t* _wcstrim(wchar_t* wsz)
{
	wchar_t newWsz[256];
	int len = wcslen(wsz);
	if (len == 1) return wsz;

	int j = 0;
	for (int i = 0; wsz[i] != L'\0'; i++)
	{
		if (wsz[i] == L' ' || wsz[i] == L'\n') continue;
		newWsz[j] = wsz[i];
		j++;
	}
	newWsz[j] = L'\0';
	wcscpy(wsz, newWsz);

	return wsz;
}

// wait to test
wchar_t* _wcstrimAll(wchar_t* wsz)
{
	/*
	//TCHAR* newstr;
	TCHAR newstr[256];
	int len = _tcslen(str);
	if (len == 1)
	{
		_tcscpy(newstr, str);
		return newstr;
	}
	//newstr = new TCHAR[len];
	int j = 0;
	for (int i = 0; str[i] != _T('\0'); i++)
	{
		if (str[i] == _T(' ') || str[i] == _T('　') || str[i] == _T('\t') || str[i] == _T('\r') || str[i] == _T('\n')) continue;
		newstr[j] = str[i];
		j++;
	}
	newstr[j] = _T('\0');
	// _tcscpy(str, newstr);
	//delete newstr;
	return newstr;*/
	return _wcstrim(wsz);
}

/* Error
TCHAR* _tcstrimRight(const TCHAR* str)
{
	//TCHAR* newstr;
	TCHAR newRevstr[256];
	int len = _tcslen(str);
	if (len == 1)
	{
		_tcscpy(newRevstr, str);
		return newRevstr;
	}
	//newstr = new TCHAR[len];

	int j = 0;
	for (int i = len; str[i] <= 0; i--)
	{
		if (str[i] == _T(' ') || str[i] == _T('　') || str[i] == _T('\t') || str[i] == _T('\0')) continue;
		newRevstr[j] = str[i];
		j++;
	}

	TCHAR newStr[256];
	for (int i = 0; i < j; i++)
	{
		newStr[i] = newRevstr[j - i - 1];
	}

	newStr[j] = _T('\0');
	// _tcscpy(str, newstr);
	//delete newstr;
	return newStr;
}*/

/*TCHAR* _tcstrim(const TCHAR* str)
{
	TCHAR newStr[256];
	_tcscpy(newStr, _tcstrimLeft(str));
	_tcscpy(newStr, _tcstrimRight(newStr));
	return newStr;
}*/

/*static void trim(char *source)
{
	size_t realStart = 0;
	size_t len = strlen(source);
	for (; realStart < len; ++realStart)
	{
		if (source[realStart] != ' ' &&
			source[realStart] != '\t' &&
			source[realStart] != '\r' &&
			source[realStart] != '\n')
			break;
	}

	size_t copyLen = len + 1;
#if defined (WIN32)
	strcpy_s(source, copyLen, source + realStart);
#else
	strncpy(source, source + realStart, copyLen);
#endif
}*/

void StringTrimLeft(wstring& wStr)
{
	const wstring wDrop = L" 　\r\n\t";

	size_t pos = wStr.find_first_not_of(wDrop);
	if (pos != wstring::npos)
	{
		wStr.erase(0, pos);
	}
}

void StringTrimRight(wstring& wStr)
{
	//const tString drop = _T(" 　\r\n\t");
	const wstring wDrop = L" 　\t";

	size_t pos = wStr.find_last_not_of(wDrop);

	if (pos != wstring::npos)
	{
		wStr.erase(pos+1);
	}
}

void StringReplace(wstring& wOrg, const wchar_t* wszFrom, const wchar_t* wszTo)
{
	size_t pos = wOrg.find(wszFrom);
	int len = wcslen(wszFrom);
	if (wstring::npos != pos)
	{
		wOrg.replace(pos, len, wszTo);
		//pos = tOrg.find(tszFrom);
	}
}

int StringReplaceAll(wstring& wOrg, const wchar_t* wszFrom, const wchar_t* wszTo, bool bRegrex)
{
	int count = 0;
	if(false == bRegrex)
	{
		wstring::size_type pos = wOrg.find(wszFrom);
		int len = wcslen(wszFrom);

		while(wstring::npos != pos)
		{
			count++;
			wOrg.replace(pos, len, wszTo);
			pos = wOrg.find(wszFrom);
		}
	}
	else
	{		//wait to test
		/*template <class traits, class charT>
			basic_string<charT> regex_replace(
			const basic_string<charT>& s,
			const basic_regex<charT,traits >& e,
			const basic_string<charT>& fmt,
			match_flag_type flags = match_default);*/
			
		// regular expression
		const std::wregex wPattern(wszFrom);

		// transformation pattern, reverses the position of all capture groups
		wstring wReplacer = wszTo;

		// apply the tranformation
		wOrg = std::regex_replace(wOrg, wPattern, wReplacer);
	}
	return count;
}

//wait to test
size_t StringReplaceAll(wstring& wStr, const wstring wSrcStr, const wstring wDesStr, bool bRegExp)
{
	size_t count = 0;
	if(false == bRegExp)
	{
		//tString::size_type pos = tStr.find(finder.c_str();
		wstring::size_type pos = 0;
		// tString::size_type srclen = _tcslen(finder.c_str());
		wstring::size_type srcLen = wSrcStr.size();
		wstring::size_type dstLen = wDesStr.size();

		while((pos = wStr.find(wSrcStr, pos)) != std::string::npos)
		// while(tString::npos != pos)
		{
			count++;
			// tStr.replace(pos, srcLen, desStr.c_str());
			wStr.replace(pos, srcLen, wDesStr);
			// pos = tStr.find(wSrcStr.c_str());
			pos += dstLen;
		}
	}
	else
	{		
		try
		{
			/*template <class traits, class charT>
				basic_string<charT> regex_replace(
				const basic_string<charT>& s,
				const basic_regex<charT,traits >& e,
				const basic_string<charT>& fmt,
				match_flag_type flags = match_default);*/
				
			// regular expression
			//syntax_option_type flags = ECMAScript | icase;
			//const std::wregex wregex(srcStr.c_str(), flags);
			const std::wregex wPattern(wSrcStr.c_str());

			// transformation pattern, reverses the position of all capture groups
			//tString desStr = tszTo;

			// apply the tranformation
			//tStr = regex_replace(tStr, wregex(find.c_str(), flags), rep.c_str());
			wStr = std::regex_replace(wStr, wPattern, wDesStr.c_str());
		}
		catch (regex_error& e)
		{
			//LOGFUNMSG(_T("%s: %d."), e.what(), e.code());
		}
	}
	return count;
}

void StringErase(wstring& wOrg, const wchar_t* wszDrop)
{
	size_t pos = wOrg.find(wszDrop);
	int len = wcslen(wszDrop);
	if (pos != wstring::npos) wOrg.erase(pos, len);
}

int StringEraseAll(wstring& wOrg, const wchar_t* wszDrop)
{
	size_t pos = wOrg.find(wszDrop);
	int len = wcslen(wszDrop);
	
	int count = 0;
	while(pos != wstring::npos)
	{
		count++;
		wOrg.erase(pos, len);
		pos = wOrg.find(wszDrop);
	}
	return count;
}

/*bool CheckPath(TCHAR* path)
{
	return (PathIsDirectory(path) == TRUE);
}*/