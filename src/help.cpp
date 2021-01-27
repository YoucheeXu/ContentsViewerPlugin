#include "help.h"

bool g_bDebug;
//CLogFile* g_pLogFile;
using namespace std;

TCHAR* _tcstrim(TCHAR* str)
{
	//TCHAR* newstr;
	TCHAR newstr[256];
	int len = _tcslen(str);
	if (len == 1) return str;
	//newstr = new TCHAR[len];
	int j = 0;
	for (int i = 0; str[i] != _T('\0'); i++)
	{
		if (str[i] == _T(' ') || str[i] == _T('\n')) continue;
		newstr[j] = str[i];
		j++;
	}
	newstr[j] = _T('\0');
	_tcscpy(str, newstr);
	//delete newstr;
	return str;
}

// wait to test
TCHAR* _tcstrimAll(TCHAR* str)
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
	return _tcstrim(str);
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

void StringTrimLeft(tString& str)
{
	const tString drop = _T(" 　\r\n\t");

	size_t pos = str.find_first_not_of(drop);
	if (pos != tString::npos)
	{
		str.erase(0, pos);
	}
}

void StringTrimRight(tString& str)
{
	//const tString drop = _T(" 　\r\n\t");
	const tString drop = _T(" 　\t");

	size_t pos = str.find_last_not_of(drop);

	if (pos != tString::npos)
	{
		str.erase(pos+1);
	}
}

void StringReplace(tString& tOrg, const TCHAR* tszFrom, const TCHAR* tszTo)
{
	size_t pos = tOrg.find(tszFrom);
	int len = _tcslen(tszFrom);
	if (tString::npos != pos)
	{
		tOrg.replace(pos, len, tszTo);
		//pos = tOrg.find(tszFrom);
	}
}

int StringReplaceAll(tString& tOrg, const TCHAR* tszFrom, const TCHAR* tszTo, bool bRegrex)
{
	int count = 0;
	if(false == bRegrex) {
		tString::size_type pos = tOrg.find(tszFrom);
		int len = _tcslen(tszFrom);

		while(tString::npos != pos)
		{
			count++;
			tOrg.replace(pos, len, tszTo);
			pos = tOrg.find(tszFrom);
		}
	}
	else {		//wait to test
		/*template <class traits, class charT>
			basic_string<charT> regex_replace(
			const basic_string<charT>& s,
			const basic_regex<charT,traits >& e,
			const basic_string<charT>& fmt,
			match_flag_type flags = match_default);*/
			
		// regular expression
		const std::wregex pattern(tszFrom);

		// transformation pattern, reverses the position of all capture groups
		tString replacer = tszTo;

		// apply the tranformation
		tOrg = std::regex_replace(tOrg, pattern, replacer);
	}
	return count;
}

//wait to test
size_t StringReplaceAll(tString & tStr, const tString srcStr, const tString desStr, bool bRegExp)
{
	size_t count = 0;
	if(false == bRegExp) {
		//tString::size_type pos = tStr.find(finder.c_str();
		tString::size_type pos = 0;
		// tString::size_type srclen = _tcslen(finder.c_str());
		tString::size_type srcLen = srcStr.size();
		tString::size_type dstLen = desStr.size();

		while((pos = tStr.find(srcStr, pos)) != std::string::npos)
		// while(tString::npos != pos)
		{
			count++;
			// tStr.replace(pos, srcLen, desStr.c_str());
			tStr.replace(pos, srcLen, desStr);
			// pos = tStr.find(srcStr.c_str());
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
			const std::wregex pattern(srcStr.c_str());

			// transformation pattern, reverses the position of all capture groups
			//tString desStr = tszTo;

			// apply the tranformation
			//tStr = regex_replace(tStr, wregex(find.c_str(), flags), rep.c_str());
			tStr = std::regex_replace(tStr, pattern, desStr.c_str());
		}
		catch (regex_error& e)
		{
			LOGFUNMSG(_T("%s: %d."), e.what(), e.code());
		}
	}
	return count;
}

void StringErase(tString& tOrg, const TCHAR* tszDrop)
{
	size_t pos = tOrg.find(tszDrop);
	int len = _tcslen(tszDrop);
	if (pos != tString::npos) tOrg.erase(pos, len);
}

int StringEraseAll(tString& tOrg, const TCHAR* tszDrop)
{
	size_t pos = tOrg.find(tszDrop);
	int len = _tcslen(tszDrop);
	
	int count = 0;
	while(pos != tString::npos)
	{
		count++;
		tOrg.erase(pos, len);
		pos = tOrg.find(tszDrop);
	}
	return count;
}

/*bool CheckPath(TCHAR* path)
{
	return (PathIsDirectory(path) == TRUE);
}*/