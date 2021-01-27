#include "ContentsViewerDialog.h"
#include "ContentsViewerPlugin.h"
#include <regex>
#include <io.h>

using namespace regex_constants;
using namespace std;

CDocument::CDocument()
{
	m_bDebug = false;
}

CDocument::~CDocument()
{
	ClearmapContent();
}

void CDocument::SetParameters(const size_t ulineBetweenParagraphs, const TCHAR* tszNum, const size_t uCountX, const TCHAR* tszKeyword)
{
	m_ulineBetweenParagraphs = ulineBetweenParagraphs;
	m_uCountX = uCountX;
	_tcscpy(m_tszNum, tszNum);
	_tcscpy(m_tszKeyword, tszKeyword);
}

//wait to test
void CDocument::MergeParagraphs()
{
	LOGFUNBGN;

	//m_tDoc = regex_replace(m_tDoc, wregex(_T("\r\n\r\n"), flags), _T("@"));
	ReplaceAll(_T("\r\n\r\n"), _T("@"), true);
	//m_tDoc = regex_replace(m_tDoc, wregex(_T("\r\n"), flags), _T(""));
	ReplaceAll(_T("\r\n"), _T(""), true);
	//m_tDoc = regex_replace(m_tDoc, wregex(_T("@"), flags), _T("\r\n\r\n"));
	ReplaceAll(_T("@"), _T("\r\n\r\n"), true);
	//m_tDoc = regex_replace(m_tDoc, wregex(_T("^　　"), flags), _T("@"));
	ReplaceAll(_T("^　　"), _T("@"), true);
	//m_tDoc = regex_replace(m_tDoc, wregex(_T("　　"), flags), _T(""));
	ReplaceAll(_T("　　"), _T(""), true);
	//m_tDoc = regex_replace(m_tDoc, wregex(_T("^@"), flags), _T("　　"));
	ReplaceAll(_T("^@"), _T("　　"), true);

	LOGFUNEND;	
}

//wait to test
void CDocument::ScriptReplace(const tString script)
{
	LOGFUNBGN;

	FILE* repFile = NULL;
	repFile = _tfopen(script.c_str(), _T("r"));
	assert(repFile);

	LOGINFO(_T("Start to replace : %s."), script.c_str());

	TCHAR tszLine[256];

	TCHAR* tszFind;
	TCHAR* tszReplace;

	while(_fgetts(tszLine, 256, repFile))
	{
		int i = _tcslen(tszLine);

		tszLine[i - 1] = 0;

		LOGINFO(_T("Line：%s."), tszLine);

		tszFind = _tcstok(tszLine, _T("\t"));
		tszReplace = _tcstok(NULL, _T("\t"));

		if (NULL == tszReplace)	tszReplace = _T("");

		LOGINFO(_T("tFind：%s, tReplace：%s."), tszFind, tszReplace);

		int count = ReplaceAll(tszFind, tszReplace, false);

		LOGINFO(_T("%d was found and replaced."), count);
	}

	fclose(repFile);
	repFile = NULL;

	LOGFUNEND;
}

void CDocument::Big5ToGBK()
{
}

//wait to test
size_t CDocument::ReplaceAll(const tString find, const tString replacer, bool bRegExp)
{
	int count = 0;
	if(false == bRegExp) {
		tString::size_type pos = m_tDoc.find(find.c_str());
		int len = _tcslen(find.c_str());

		while(tString::npos != pos)
		{
			count++;
			m_tDoc.replace(pos, len, replacer.c_str());
			pos = m_tDoc.find(find.c_str());
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
			const std::wregex pattern(find.c_str());
			//syntax_option_type flags = ECMAScript | icase;
			//const std::wregex wregex(find.c_str(), flags);

			// transformation pattern, reverses the position of all capture groups
			//tString replacer = tszTo;

			// apply the tranformation
			//m_tDoc = regex_replace(m_tDoc, wregex(find.c_str(), flags), rep.c_str());
			m_tDoc = std::regex_replace(m_tDoc, pattern, replacer.c_str());
		}
		catch (regex_error& e)
		{
			LOGINFO(_T("%s: %d."), e.what(), e.code());
		}
	}
	return count;
}

//wait to test
size_t CDocument::Parse(bool bHeadIndexContent)
{
	LOGFUNBGN;

	ClearmapContent();

	size_t maxLevel = 0;
	int totalNum = 0;
	if(false == bHeadIndexContent)
	{
		TCHAR tt[10];
		_stprintf(tt, _T("%d"), m_uCountX*2);

		tString tText = _T("^[ 　\\t]*?第[");

		tText = tText + m_tszNum;
		tText = tText + _T("]{0,");

		tText = tText + tt;
		tText = tText + _T("}卷.*");

		LOGINFO(_T("tText: %s"), tText.c_str());

		size_t pos = tText.find(_T("卷"));

		LOGINFO(_T("pos: %d"), pos);

		size_t totalLevel = _tcsclen(m_tszKeyword);

		int level = 1;
		int num = 0;
		tString tkeyWord;
		for (size_t i = 1; i <= totalLevel; i++)
		{
			//_tcscpy(tszkeyWord, m_tszKeyword[i - 1]);
			tkeyWord = m_tszKeyword[i - 1];
			LOGINFO(_T("tszLevelItem: %d, keywrod: %s."), i, tkeyWord.c_str());

			tText = tText.replace(pos, 1, tkeyWord.c_str());
			LOGINFO(_T("tText: %s"), tText.c_str());

			if(num > 0) level++;
			num = IndexContents(tText.c_str(), tkeyWord.c_str(), level);

			if (num > 0) maxLevel = level;
			totalNum = totalNum + num;
			LOGINFO(_T("End to index %s.\n"), tkeyWord.c_str());
		}
	}
	else
	{
		totalNum = IndexContents(_T("^[^ 　\\t\\r\\n].+"), _T(""), 1);
		LOGINFO(_T("Head index content."));
	}

	LOGFUNEND;

	return maxLevel;
}

//wait to test
void CDocument::TrimContents()
{
	LOGFUNBGN;

	tString tContent, tKeywrod;
	int line;
	size_t pos = 0;

	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;

	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++)
	{
		line = intertr->first;
		pContent = intertr->second;

		LOGINFO(_T("line: %d"), line);

		if(NULL == pContent)
		{
			LOGINFO(_T("error in line: %d"), line);
			continue;
		}
		
		tContent = pContent->tContent;
		LOGINFO(_T("raw tContent: '%s'"), tContent.c_str());

		tKeywrod = pContent->tKeyword;
		LOGINFO(_T("keyword: %s"), tKeywrod.c_str());

		StringTrimLeft(tContent);
		StringTrimRight(tContent);

		LOGINFO(_T("trimed tContent: '%s'"), tContent.c_str());

		pos = tContent.find(_T("　"));

		while (pos != tString::npos)
		{
			tContent = tContent.replace(pos, 1, _T(" "));
			pos = tContent.find(_T("　"));
		}
		LOGINFO(_T("banked 1: '%s'"), tContent.c_str());

		pos = tContent.find(tKeywrod.c_str());

		if (tString::npos != pos)
		{
			tContent.insert(pos + 1, _T(" "));

			LOGINFO(_T("banked 2: '%s'"), tContent.c_str());

			pos = tContent.find(_T("  "));

			while (string::npos != pos)
			{
				tContent.replace(pos, 2, _T(" "));

				pos = tContent.find(_T("  "));
			}
			LOGINFO(_T("banked 3: '%s'"), tContent.c_str());
		}
		LOGINFO(_T("Going to replace %d line by '%s'"), line, tContent.c_str());

		//thePlugin.ReplaceLine(line, tContent.c_str());
	}
	LOGFUNEND;	
}

//wait to test
//To-Do: align number
//Bug: sometimes lose "1" in "12" and so on
void CDocument::NumberContents()
{
	LOGFUNBGN;

	tString tContent, tNumber;
	int line, lastpos;
	size_t pos;
	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;

	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++)
	{
		line = intertr->first;
		pContent = intertr->second;
		tContent = pContent->tContent;
		StringTrimLeft(tContent);

		pos = tContent.find(pContent->tKeyword.c_str());

		if (pos == tString::npos) return;
		lastpos = pos;
		tNumber = tContent.substr(1, lastpos - 1);

		LOGINFO(_T("Content to number : %s, keyword: %s, content: %s."), tNumber.c_str(), pContent->tKeyword.c_str(), tContent.c_str());

		pos = 0;

		if (0 == _tcscmp(tNumber.c_str(), _T("十")))
		{ 
			tNumber = _T("10");

			tContent = tContent.replace(1, lastpos - 1, tNumber);

			LOGINFO(_T("Numbered content: %s."), tContent.c_str());

			//thePlugin.ReplaceLine(line, tContent.c_str());

			continue;
		}

		if (0 == _tcscmp(tNumber.substr(0, 1).c_str(), _T("十")))
		{
			tNumber = tNumber.replace(0, 1, _T("1"));

		}

		pos = tNumber.find(_T("0"));

		while (pos != tString::npos)
		{
			tNumber = tNumber.replace(pos, 1, _T("零"));

			pos = tNumber.find(_T("0"));
		}
		LOGINFO(_T("Numbered one: %s."), tNumber.c_str());
		
		pos = tNumber.find(_T("o"));

		while (pos != tString::npos)
		{
			tNumber = tNumber.replace(pos, 1, _T("零"));

			pos = tNumber.find(_T("o"));
		}
		LOGINFO(_T("Numbered two: %s."), tNumber.c_str());

		TCHAR last[2];
		_tcscpy(last, tNumber.substr(lastpos - 2 ,1).c_str());

		if(0 == _tcscmp(last, _T("十")))
			tNumber = tNumber.replace(lastpos - 2, 1, _T("0"));
		else if(0 == _tcscmp(last, _T("百")))
			tNumber = tNumber.replace(lastpos - 2, 1, _T("00"));
		else if(0 == _tcscmp(last, _T("千")))
			tNumber = tNumber.replace(lastpos - 2, 1, _T("000"));

		LOGINFO(_T("Numbered three: %s."), tNumber.c_str());

		pos = tNumber.find(_T("十零"));

		if(pos != tString::npos) tNumber.erase(pos, 2);

		pos = tNumber.find(_T("十"));

		if (pos != tString::npos) tNumber.erase(pos, 1);

		pos = tNumber.find(_T("百零"));

		if(pos != tString::npos) tNumber = tNumber.replace(pos, 2, _T("0"));

		pos = tNumber.find(_T("百"));

		if (pos != tString::npos) tNumber = tNumber.erase(pos, 1);

		pos = tNumber.find(_T("千零"));

		if (pos != tString::npos)
		{
			if (lastpos - pos <= 4)
				tNumber = tNumber.replace(pos, 2, _T("00"));
			else
				tNumber = tNumber.replace(pos, 2, _T("0"));
		}

		pos = tNumber.find(_T("千"));

		if (pos != tString::npos) tNumber = tNumber.erase(pos, 1);
		
		LOGINFO(_T("Numbered four: %s."), tNumber.c_str());

		tString lefts = _T("一二三四五六七八九零");
		tString rights = _T("1234567890");

		tString left, right;
		LOGINFO(_T("lefts: %s, Rights: %s."), lefts.c_str(), rights.c_str());

		for(int i = 0; i < 10; i++)
		{
			left = lefts.substr(i, 1);
			right = rights.substr(i, 1);

			pos = tNumber.find(left.c_str());
			
			while (pos != tString::npos)
			{
				tNumber = tNumber.replace(pos, 1, right);

				pos = tNumber.find(left.c_str());

				LOGINFO(_T("left: %s, Right: %s."), left.c_str(), right.c_str());
			}
		}
		LOGINFO(_T("Numbered five: %s."), tNumber.c_str());

		//remove left banks
		tContent = tContent.replace(1, lastpos - 1, tNumber);

		LOGINFO(_T("Numbered content: %s."), tContent.c_str());

		//thePlugin.ReplaceLine(line, tContent.c_str());
	}

	LOGFUNEND;
}

//wait to test
void CDocument::UpdateContents()
{
	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;

	tString tContentName;
	int line, level;
	bool IsFistLineIndexed = false;
	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++)
	{
		line = intertr->first;
		pContent = intertr->second;
		tContentName = pContent->tContent;
		level = pContent->nLevel;

		if (1 == line) IsFistLineIndexed = true;

		ReplaceLine(line, tContentName.c_str());
	}
}

//TO-DO: test
void CDocument::ExportContents(const tString file)
{
	tString tItemText = _T("");

	int line = 0;

	TCHAR lineBuffer[4];

	FILE* pContentFile = NULL;

	pContentFile = _tfopen(file.c_str(), _T("w+"));

	if(NULL == pContentFile) {
		LOGERR(_T("Can't open %s to Export!"), file.c_str());
		return;
	}

	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;

	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++)
	{
		line = intertr->first;
		pContent = intertr->second;
		tItemText = pContent->tContent;

		_stprintf(lineBuffer, _T("%d"), line);
		
		_fputts(tItemText.c_str(), pContentFile);

		_fputts(_T("\t"), pContentFile);

		_fputts(lineBuffer, pContentFile);

		_fputts(_T("\n"), pContentFile);
	}

	fflush(pContentFile);

	fclose(pContentFile);

	pContentFile = NULL;	
}

//TO-DO: test
void CDocument::ImportContents(const tString file)
{
	ClearmapContent();

	FILE* pContentFile = NULL;

	pContentFile = _tfopen(file.c_str(), _T("r"));
	
	if(NULL == pContentFile) {
		LOGERR(_T("Can't open %s to Import!"), file.c_str());
		return;
	}

	assert(pContentFile);

	TCHAR tszLine[256];

	TCHAR* tszContent;
	TCHAR* tszLineNo;
	
	int lineNo = 0;

	while(_fgetts(tszLine, 256, pContentFile))
	{
		int i = _tcslen(tszLine);

		tszLine[i - 1] = 0;

		LOGINFO(_T("Line：%s."), tszLine);

		tszContent = _tcstok(tszLine, _T("\t"));

		tszLineNo = _tcstok(NULL, _T("\t"));

		LOGINFO(_T("Content: %s, tszLineNo：%s."), tszContent, tszLineNo);
		
		lineNo = (int)(_ttoi(tszLineNo));
		
		//thePlugin.ReplaceLine(lineNo, tszContent);
	}

	fclose(pContentFile);

	pContentFile = NULL;
}

//wait to test
void CDocument::DelDuplicateContents()
{
	LOGFUNBGN;

	tString tItemText1, tItemText2;

	map<int, ContentData*>::reverse_iterator intertr1, intertr2;
	ContentData* pContent1 = NULL;
	ContentData* pContent2 = NULL;
	size_t line1, line2, level1, level2;

	for (intertr1 = m_mapContent.rbegin(); intertr1 != m_mapContent.rend(); intertr1++)
	{
		line1 = intertr1->first;
		pContent1 = intertr1->second;
		tItemText1 = pContent1->tContent;
		level1 = pContent1->nLevel;

		intertr2 = intertr1; 
		intertr2++;

		if (intertr2 == m_mapContent.rend()) return;
		else
		{
			line2 = intertr2->first;
			pContent2 = intertr2->second;
			tItemText2 = pContent2->tContent;
			level2 = pContent2->nLevel;
		}

		if (0 == _tcscmp(tItemText1.c_str(), tItemText2.c_str()))
		{
			DelLine(line1);

			LOGINFO(_T("tItemText1: %s = tItemText2: %s"), tItemText1.c_str(), tItemText2.c_str());
		}
	}

	LOGFUNEND;
}

//wait to add function: 
//1. excusive level 2 up content,  ok
//2. Delete not empty content's empty mark
//3. print a to-do list
void CDocument::MarkEmptyContents()
{
	LOGFUNBGN;

	//if (true == m_bHeadIndexContent) return;
	//ParseCurrentFile();

	tString tItemText1;

	int line1, line2, level1, level2;

	map<int, ContentData*>::iterator intertr1, intertr2;
	ContentData* pContent1 = NULL;
	ContentData* pContent2 = NULL;

	for (intertr1 = m_mapContent.begin(); intertr1 != m_mapContent.end(); intertr1++)
	{
		line1 = intertr1->first;
		pContent1 = intertr1->second;
		tItemText1 = pContent1->tContent;
		level1 = pContent1->nLevel;

		intertr2 = intertr1; 
		intertr2++;

		if (intertr2 == m_mapContent.end()) return;
		else
		{
			line2 = intertr2->first;
			pContent2 = intertr2->second;
			level2 = pContent2->nLevel;
		}

		size_t pos = tItemText1.rfind(_T("+"));

		if (pos != string::npos) LOGINFO(_T("pos of '+' : %d; length: %d."), pos, tItemText1.length());

		if (tItemText1.rfind(_T("+")) == tItemText1.length() - 3) return;

		if (line2 - line1 <= 5 && level1 >= level2)
		{
			LOGINFO(_T("Empty content : %s"), tItemText1.c_str());

			tItemText1.insert(tItemText1.length() - 2, _T("+"));

			LOGINFO(_T("Line1: %d"), line1);
			LOGINFO(_T("Line2: %d"), line2);
			LOGINFO(_T("Marked content : %s"), tItemText1.c_str());
			LOGINFO(_T("\n"));

			//thePlugin.ReplaceLine(line1, tItemText1.c_str());
		}
	} 
	LOGFUNEND;
}

//wait to test
void CDocument::AlignParagraphs()
{	
	LOGFUNBGN;

	//tFile = regex_replace(tFile, wregex(_T("^[ 　\t]+"), flags), _T(""));
	ReplaceAll(_T("^[ 　\t]+"), _T(""), true);
	//tFile = regex_replace(tFile, wregex(_T("[ 　	\t]+$"), flags), _T(""));
	ReplaceAll(_T("[ 　	\t]+$"), _T(""), true);

	if (0 == m_ulineBetweenParagraphs)
	{
		//tFile = regex_replace(tFile, wregex(_T("\r\n\r\n"), flags), _T("\r\n"));
		ReplaceAll(_T("\r\n\r\n"), _T("\r\n"), true);
	}
	else if (1 == m_ulineBetweenParagraphs)
	{
		//tFile = regex_replace(tFile, wregex(_T("\r\n"), flags), _T("\r\n\r\n"))
		ReplaceAll(_T("\r\n"), _T("\r\n\r\n"), true);
		//tFile = regex_replace(tFile, wregex(_T("\r\n\r\n\r\n"), flags), _T("\r\n\r\n"));
		ReplaceAll(_T("\r\n\r\n\r\n"), _T("\r\n\r\n"), true);
	}

	//tFile = regex_replace(tFile, wregex(_T("^"), flags), _T("　　"));
	ReplaceAll(_T("^"), _T("　　"), true);

	Parse(false);

	TrimContents();

	tString tFirstLine;

	GetLine(1, tFirstLine);

	StringTrimLeft(tFirstLine);

	StringTrimRight(tFirstLine);

	ReplaceLine(1, tFirstLine.c_str());
	
	LOGFUNEND;
}

int CDocument::AddContent(int nItem, const TCHAR* tszContentName, int line, int level, const TCHAR* tszKeyword)
{
	LOGFUNBGN;

	LOGINFO(_T("AddContent: %s, Level : %d, Line: %d"), tszContentName, level, line);

	if (m_mapContent.end() == m_mapContent.find(line))
	{
		ContentData* pContent = new ContentData;

		pContent->tContent = tszContentName;
		pContent->tKeyword = tszKeyword;
		pContent->nLevel = level;

		m_mapContent[line] = pContent;

		return 1;
	}
	LOGFUNEND;

	return -1;
}

void CDocument::ClearmapContent()
{
	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;
	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++) {
		pContent = intertr->second;
		if(pContent != NULL) delete pContent;
	}
	m_mapContent.clear();
}

size_t CDocument::LineFromPos(size_t pos)
{
	return pos;
}

//TO-DO: 
size_t CDocument::IndexContents(const TCHAR* tText, const TCHAR* tszKeyword, int level)
{
	return thePlugin.IndexContents(tText, tszKeyword, level);
}

void CDocument::DelLine(int line)
{
	thePlugin.DelLine(line);
}

void CDocument::CutLines(int lineStart, int lineEnd)
{
	thePlugin.CutLines(lineStart, lineEnd);
}

void CDocument::PasteBeforeLine(int line)
{
	thePlugin.PasteBeforeLine(line);
}

void CDocument::ReplaceLine(int line, const TCHAR* tszTxt)
{
	thePlugin.ReplaceLine(line, tszTxt);
}

size_t CDocument::GetLine(int line, tString& tLine)
{
	return thePlugin.GetLine(line, tLine);
}

size_t CDocument::GetLineCount()
{
	return thePlugin.GetLineCount();
}

size_t CDocument::GetCurLineNo()
{
	return thePlugin.GetCurLineNo();
}