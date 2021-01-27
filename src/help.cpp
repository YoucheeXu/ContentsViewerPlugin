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

void StringTrimLeft(tString& str)
{
	const tString drop1 = _T(" ");
	const tString drop2 = _T("　");

	str.erase(0, str.find_first_not_of(drop1));
	str.erase(0, str.find_first_not_of(drop2));
}

void StringTrimRight(tString& str)
{
	const tString drop1 = _T(" ");
	const tString drop2 = _T("　");

	str.erase(str.find_last_not_of(drop1) + 1);
	str.erase(str.find_last_not_of(drop2) + 1);
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

/*int StringRegReplaceAll(tString& tOrig, wregex regExpress, const TCHAR* tszReplace)
{
	return 0;
}*/

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

bool CheckPath(TCHAR* path)
{
	return (PathIsDirectory(path) == TRUE);
}