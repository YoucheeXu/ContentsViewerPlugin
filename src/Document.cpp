// Document.cpp
#include "ContentsViewerDialog.h"
#include "ContentsViewer.h"
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

void CDocument::SetParameters(const size_t uLineBetweenParagraphs, const wchar_t *wszNum, const size_t uCountX, const wchar_t *wszKeyword)
{
	m_ulineBetweenParagraphs = uLineBetweenParagraphs;
	m_uCountX = uCountX;
	wcscpy(m_wszNum, wszNum);
	wcscpy(m_wszKeyword, wszKeyword);
}

void CDocument::MergeParagraphs()
{
	StringReplaceAll(m_wDoc, L"\r\n\r\n", L"@");

	StringReplaceAll(m_wDoc, L"\r\n", L"");

	StringReplaceAll(m_wDoc, L"@", L"\r\n\r\n");

	StringReplaceAll(m_wDoc, L"^　　", L"@");

	StringReplaceAll(m_wDoc, L"　　", L"");

	StringReplaceAll(m_wDoc, L"^@", L"　　");
}

void CDocument::ScriptReplace(const wstring wScript)
{
	FILE *repFile = nullptr;
	repFile = _wfopen(wScript.c_str(), L"r");
	assert(repFile);

	LOGINFO(L"Start to replace : %s.", wScript.c_str());

	wchar_t wszLine[256];

	wchar_t *wszFind;
	wchar_t *wszReplace;
	int nCount = 0;
	int i = 0;
	int nLine = 1;

	while (fgetws(wszLine, 256, repFile))
	{
		i = wcslen(wszLine);

		wszLine[i - 1] = 0;

		LOGINFO(L"Line：%s.", wszLine);

		wszFind = _wcstok(wszLine, L"\t");
		wszReplace = _wcstok(NULL, L"\t");

		if (NULL == wszReplace)
			wszReplace = L"";

		LOGINFO(L"wFind：%s, wReplace：%s.", wszFind, wszReplace);

		try
		{
			if (wszFind != NULL)
			{
				nCount = StringReplaceAll(m_wDoc, wszFind, wszReplace, false);
				LOGINFO(L"%d was found and replaced.", nCount);
			}
			else
			{
				LOGERR(L"Wrong syntax in nLine %d", nLine);
				break;
			}
		}
		catch (exception &e)
		{
			LOGINFO(L"Fail to found and replaced: %s", e.what());
		}

		nLine++;
	}

	fclose(repFile);
	repFile = nullptr;
}

void CDocument::Big5ToGBK()
{
}

size_t CDocument::Parse(stIndexType_t stIndexType, const wchar_t *wszIndex)
{
	ClearmapContent();

	size_t nMaxLevel = 0;
	int nTotalNum = 0;
	if (NormIndexTyp == stIndexType)
	{
		wchar_t wt[10];
		_swprintf(wt, L"%d", m_uCountX * 2);

		wstring wText = L"^[ 　\\t]*?第[";

		wText = wText + m_wszNum;
		wText = wText + L"]{0,";

		wText = wText + wt;
		wText = wText + L"}卷.*";

		LOGINFO(L"wText: %s", wText.c_str());

		size_t nPos = wText.find(L"卷");

		LOGINFO(L"nPos: %d", nPos);

		size_t nTotalLevel = wcslen(m_wszKeyword);

		int nLevel = 1;
		int nNum = 0;
		wstring wkeyWord;
		for (size_t i = 1; i <= nTotalLevel; i++)
		{
			//_tcscpy(tszkeyWord, m_wszKeyword[i - 1]);
			wkeyWord = m_wszKeyword[i - 1];
			LOGINFO(L"level: %d, wKeywrod: %s.", i, wkeyWord.c_str());

			wText = wText.replace(nPos, 1, wkeyWord.c_str());
			LOGINFO(L"wText: %s", wText.c_str());

			if (nNum > 0)
				nLevel++;
			nNum = IndexContents(wText.c_str(), wkeyWord.c_str(), nLevel);

			if (nNum > 0)
				nMaxLevel = nLevel;
			nTotalNum = nTotalNum + nNum;
			LOGINFO("End to index %s.\n", wkeyWord.c_str());
		}
	}
	else if (HeadIndexTyp == stIndexType)
	{
		nTotalNum = IndexContents(L"^[^ 　\\t\\r\\n].+", L"", 1);
		LOGINFO("End to head index content.");
	}
	else if (SpecificIndexTyp == stIndexType)
	{
		nTotalNum = IndexContents(wszIndex, L"", 1);
		LOGINFO("End to specific index content.");
	}

	return nMaxLevel;
}

void CDocument::TrimContents()
{
	wstring wContent, wKeywrod;
	int nLine;
	size_t nPos = 0;

	// mapContent_t::iterator intertr;
	ContentData_t *pContent = nullptr;
	for (auto intertr = m_mapContents.begin(); intertr != m_mapContents.end(); intertr++)
	{
		nLine = intertr->first;
		pContent = intertr->second;

		LOGINFO("nLine: %d", nLine);

		if (nullptr == pContent)
		{
			LOGINFO("error in nLine: %d", nLine);
			continue;
		}

		wContent = pContent->wContent;
		LOGINFO(L"raw wContent: '%s'", wContent.c_str());

		wKeywrod = pContent->wKeyword;
		LOGINFO(L"wKeyword: '%s'", wKeywrod.c_str());

		StringReplaceAll(wContent, L"^[ 　\t]+", L"");

		StringReplaceAll(wContent, L"[ 　\t]+$", L"");

		LOGINFO(L"Trimed wContent: '%s'", wContent.c_str());

		nPos = wContent.find(L"　");
		while (nPos != wstring::npos)
		{
			wContent = wContent.replace(nPos, 1, L" ");
			nPos = wContent.find(L"　");
		}
		LOGINFO(L"banked 1: '%s'", wContent.c_str());

		nPos = wContent.find(L"\t");
		while (nPos != wstring::npos)
		{
			wContent = wContent.replace(nPos, 1, L" ");
			nPos = wContent.find(L"\t");
		}
		LOGINFO(L"banked 1: '%s'", wContent.c_str());

		nPos = wContent.find(wKeywrod.c_str());

		if (wstring::npos != nPos)
		{
			wContent.insert(nPos + 1, L" ");

			LOGINFO(L"banked 2: '%s'", wContent.c_str());

			nPos = wContent.find(L"  ");

			while (string::npos != nPos)
			{
				wContent.replace(nPos, 2, L" ");

				nPos = wContent.find(L"  ");
			}
			LOGINFO(L"banked 3: '%s'", wContent.c_str());
		}

		LOGINFO(L"Going to replace %d line by '%s'", nLine, wContent.c_str());
		ReplaceLine(nLine, wContent.c_str());
	}
	LOGFUNEND;
}

//To-Do: align number
//Bug: sometimes lose "1" in "12" and so on
void CDocument::NumberContents()
{
	wstring wContent, wNumber;
	int nLine, nLastpos;
	size_t nPos;
	// mapContent_t::iterator intertr;
	ContentData_t *pContent = nullptr;
	for (auto intertr = m_mapContents.begin(); intertr != m_mapContents.end(); intertr++)
	{
		nLine = intertr->first;
		pContent = intertr->second;
		wContent = pContent->wContent;
		StringTrimLeft(wContent);

		nLastpos = wContent.find(pContent->wKeyword.c_str());

		if (nLastpos == wstring::npos)
			return;

		wNumber = wContent.substr(1, nLastpos - 1);

		LOGINFO(L"Content to wNumber : %s, wKeyword: %s, wContent: %s.", wNumber.c_str(), pContent->wKeyword.c_str(), wContent.c_str());

		if (0 == wcscmp(wNumber.c_str(), L"十"))
		{
			wNumber = L"10";

			wContent = wContent.replace(1, nLastpos - 1, wNumber);

			LOGINFO(L"Numbered content: %s.", wContent.c_str());

			pContent->wContent = wContent.c_str();
			ReplaceLine(nLine, pContent->wContent.c_str());
			continue;
		}

		LOGINFO(L"Step one: %s.", wNumber.c_str());

		if (0 == wcscmp(wNumber.substr(0, 1).c_str(), L"十"))
		{
			wNumber = wNumber.replace(0, 1, L"1");
		}

		nPos = wNumber.find(L"0");
		while (nPos != wstring::npos)
		{
			wNumber = wNumber.replace(nPos, 1, L"零");
			nPos = wNumber.find(L"0");
		}

		nPos = wNumber.find(L"o");
		while (nPos != wstring::npos)
		{
			wNumber = wNumber.replace(nPos, 1, L"零");
			nPos = wNumber.find(L"o");
		}

		LOGINFO(L"Step two: %s.", wNumber.c_str());

		wchar_t wszLast[2];
		wcscpy(wszLast, wNumber.substr(nLastpos - 2, 1).c_str());

		if (0 == wcscmp(wszLast, L"十"))
			wNumber = wNumber.replace(nLastpos - 2, 1, L"0");
		else if (0 == wcscmp(wszLast, L"百"))
			wNumber = wNumber.replace(nLastpos - 2, 1, L"00");
		else if (0 == wcscmp(wszLast, L"千"))
			wNumber = wNumber.replace(nLastpos - 2, 1, L"000");

		LOGINFO(L"Step three: %s.", wNumber.c_str());

		nPos = wNumber.find(L"十零");
		if (nPos != wstring::npos)
			wNumber.erase(nPos, 2);

		nPos = wNumber.find(L"十");
		if (nPos != wstring::npos)
			wNumber.erase(nPos, 1);

		nPos = wNumber.find(L"百零");
		if (nPos != wstring::npos)
			wNumber = wNumber.replace(nPos, 2, L"0");

		nPos = wNumber.find(L"千零");
		if (nPos != wstring::npos)
		{
			if (nLastpos - nPos <= 4)
				wNumber = wNumber.replace(nPos, 2, L"00");
			else
				wNumber = wNumber.replace(nPos, 2, L"0");
		}

		LOGINFO(L"Step four: %s.", wNumber.c_str());

		wstring wLefts = L"一二三四五六七八九零";
		wstring wRights = L"1234567890";

		wstring wLeft, wRight;
		LOGINFO(L"wLefts: %s, Rights: %s.", wLefts.c_str(), wRights.c_str());

		for (int i = 0; i < 10; i++)
		{
			wLeft = wLefts.substr(i, 1);
			wRight = wRights.substr(i, 1);

			nPos = wNumber.find(wLeft.c_str());

			while (nPos != wstring::npos)
			{
				wNumber = wNumber.replace(nPos, 1, wRight);

				nPos = wNumber.find(wLeft.c_str());

				LOGINFO(L"wLeft: %s, wRight: %s.", wLeft.c_str(), wRight.c_str());
			}
		}

		LOGINFO(L"Step five: %s.", wNumber.c_str());

		nPos = wNumber.find(L"百");
		if (nPos != wstring::npos)
		{
			if (wNumber.length() - nPos >= 3)
			{
				wNumber = wNumber.erase(nPos, 1);
			}
			else
			{
				wNumber = wNumber.replace(nPos, 1, L"0");
			}
		}

		nPos = wNumber.find(L"千");
		if (nPos != wstring::npos)
		{
			if (wNumber.length() - nPos >= 4)
			{
				wNumber = wNumber.erase(nPos, 1);
			}
			else
			{
				wNumber = wNumber.replace(nPos, 1, L"0");
			}
		}

		LOGINFO(L"Numbered content: %s.", wContent.c_str());

		//remove left banks
		wContent = wContent.replace(1, nLastpos - 1, wNumber);

		LOGINFO(L"Numbered content: %s.", wContent.c_str());

		pContent->wContent = wContent.c_str();
		ReplaceLine(nLine, pContent->wContent.c_str());
	}
}

void CDocument::UpdateContents()
{
	// mapContent_t::iterator intertr;
	ContentData_t *pContent = nullptr;

	wstring wContentName;
	int nLine, nLevel;
	bool bIsFistLineIndexed = false;
	for (auto intertr = m_mapContents.begin(); intertr != m_mapContents.end(); intertr++)
	{
		nLine = intertr->first;
		pContent = intertr->second;
		wContentName = pContent->wContent;
		nLevel = pContent->nLevel;

		if (1 == nLine)
			bIsFistLineIndexed = true;

		ReplaceLine(nLine, wContentName.c_str());
	}
}

//TO-DO: west
void CDocument::ExportContents(const wstring wFile)
{
	wstring wItemText = L"";

	int nLine = 0;

	wchar_t wszLineBuffer[4];

	FILE *pContentFile = nullptr;

	pContentFile = _wfopen(wFile.c_str(), L"w+");

	if (nullptr == pContentFile)
	{
		LOGERR(L"Can't open %s to Export!", wFile.c_str());
		return;
	}

	//map<int, ContentData_t*>::iterator intertr;
	ContentData_t *pContent = nullptr;

	// for (auto &intertr: m_mapContent)
	for (auto intertr = m_mapContents.begin(); intertr != m_mapContents.end(); intertr++)
	{
		nLine = intertr->first;
		pContent = intertr->second;
		wItemText = pContent->wContent;

		_swprintf(wszLineBuffer, L"%d", nLine);

		fputws(wItemText.c_str(), pContentFile);

		fputws(L"\t", pContentFile);

		fputws(wszLineBuffer, pContentFile);

		fputws(L"\n", pContentFile);
	}

	fflush(pContentFile);

	fclose(pContentFile);

	pContentFile = nullptr;
}

//TO-DO: west
void CDocument::ImportContents(const wstring wFile)
{
	ClearmapContent();

	FILE *pContentFile = nullptr;

	pContentFile = _wfopen(wFile.c_str(), L"r");

	if (nullptr == pContentFile)
	{
		LOGERR(L"Can't open %s to Import!", wFile.c_str());
		return;
	}

	assert(pContentFile);

	wchar_t wszLine[256];

	wchar_t *wszContent;
	wchar_t *wszLineNum;

	int nLineNum = 0;

	while (fgetws(wszLine, 256, pContentFile))
	{
		int i = wcslen(wszLine);

		wszLine[i - 1] = 0;

		LOGINFO(L"Line：%s.", wszLine);

		wszContent = _wcstok(wszLine, L"\t");

		wszLineNum = _wcstok(NULL, L"\t");

		LOGINFO(L"wszContent: %s, wszLineNum：%s.", wszContent, wszLineNum);

		nLineNum = _wtoi(wszLineNum);

		ReplaceLine(nLineNum, wszContent);
	}

	fclose(pContentFile);

	pContentFile = nullptr;
}

void CDocument::DelDuplicateContents()
{
	wstring wItemText1, wItemText2;

	mapContent_t::reverse_iterator intertr1, intertr2;
	ContentData_t *pContent1 = nullptr;
	ContentData_t *pContent2 = nullptr;
	size_t nLine1, nLine2, nLevel1, nLevel2;

	for (intertr1 = m_mapContents.rbegin(); intertr1 != m_mapContents.rend(); intertr1++)
	{
		nLine1 = intertr1->first;
		pContent1 = intertr1->second;
		wItemText1 = pContent1->wContent;
		nLevel1 = pContent1->nLevel;

		intertr2 = intertr1;
		intertr2++;

		if (intertr2 == m_mapContents.rend())
			return;
		else
		{
			nLine2 = intertr2->first;
			pContent2 = intertr2->second;
			wItemText2 = pContent2->wContent;
			nLevel2 = pContent2->nLevel;
		}

		if (0 == wcscmp(wItemText1.c_str(), wItemText2.c_str()))
		{
			DelLine(nLine1);

			LOGINFO(L"wItemText1: %s = wItemText2: %s", wItemText1.c_str(), wItemText2.c_str());
		}
	}
}

//1. excusive level 2 up content,				OK
//2. Delete not empty content's empty mark		OK
//3. print a to-do list
void CDocument::MarkEmptyContents()
{
	//if (true == m_bHeadIndexContent) return;
	//ParseCurrentFile();

	wstring wItemText1;

	size_t nLine1, nLine2, nLevel1, nLevel2;

	mapContent_t::iterator intertr1, intertr2;
	ContentData_t *pContent1 = nullptr;
	ContentData_t *pContent2 = nullptr;

	for (intertr1 = m_mapContents.begin(); intertr1 != m_mapContents.end(); intertr1++)
	{
		nLine1 = intertr1->first;
		pContent1 = intertr1->second;
		wItemText1 = pContent1->wContent;
		nLevel1 = pContent1->nLevel;

		intertr2 = intertr1;
		intertr2++;

		if (intertr2 == m_mapContents.end())
			return;
		else
		{
			nLine2 = intertr2->first;
			pContent2 = intertr2->second;
			nLevel2 = pContent2->nLevel;
		}

		size_t nPos = wItemText1.rfind(L"+");

		if (nPos != string::npos)
			LOGINFO(L"pos of '+' : %d; length: %d.", nPos, wItemText1.length());

		if (wItemText1.rfind(L"+") == wItemText1.length() - 3)
			continue;

		if (nLine2 - nLine1 <= 5 && nLevel1 >= nLevel2)
		{
			LOGINFO(L"Empty content : %s", wItemText1.c_str());

			wItemText1.insert(wItemText1.length() - 2, L"+");

			LOGINFO(L"nLine1: %d", nLine1);
			LOGINFO(L"Line2: %d", nLine2);
			LOGINFO(L"Marked content: %s", wItemText1.c_str());
			LOGINFO(L"\n");

			//thePlugin.ReplaceLine(nLine1, wItemText1.c_str());
			ReplaceLine(nLine1, wItemText1.c_str());
		}
	}
}

//wait to west
void CDocument::AlignParagraphs()
{
	//earse wailing blankspace of paragraphs
	StringReplaceAll(m_wDoc, L"^[ 　\t]+$", L"");
	//replace heading blanksapce of paragraphs to "　　"
	StringReplaceAll(m_wDoc, L"^[ 　\t]*", L"　　");
	//earse wailing blankspace of paragraphs
	StringReplaceAll(m_wDoc, L"^[ 　\t]+$", L"");

	if (0 == m_ulineBetweenParagraphs)
	{
		//tFile = regex_replace(tFile, wregex(L"\r\n\r\n"), flags), L"\r\n"));
		StringReplaceAll(m_wDoc, L"[\r\n]+?", L"\r\n");
	}
	else if (1 == m_ulineBetweenParagraphs)
	{
		//tFile = regex_replace(tFile, wregex(L"\r\n"), flags), L"\r\n\r\n"))
		StringReplaceAll(m_wDoc, L"\r\n", L"\r\n\r\n");
		//tFile = regex_replace(tFile, wregex(L"\r\n\r\n\r\n"), flags), L"\r\n\r\n"));
		StringReplaceAll(m_wDoc, L"\r\n\r\n\r\n", L"\r\n\r\n");
	}

	Parse(NormIndexTyp, nullptr);

	TrimContents();

	wstring wFirstLine;

	GetLine(1, wFirstLine);

	StringTrimLeft(wFirstLine);

	StringTrimRight(wFirstLine);

	ReplaceLine(1, wFirstLine.c_str());
}

int CDocument::AddContent(int nItem, const wchar_t *wszContentName, int nLine, int nLevel, const wchar_t *wszKeyword)
{
	LOGINFO(L"Doc AddContent: %s, Level : %d, nLine: %d", wszContentName, nLevel, nLine);

	if (m_mapContents.end() == m_mapContents.find(nLine))
	{
		ContentData_t *pContent = new ContentData_t;

		pContent->wContent = wszContentName;
		pContent->wKeyword = wszKeyword;
		pContent->nLevel = nLevel;

		m_mapContents[nLine] = pContent;

		// m_mapContent.emplace(nLine, pContent);

		return 1;
	}

	return -1;
}

void CDocument::ClearmapContent()
{
	// mapContent_t::iterator intertr;
	ContentData_t *pContent = nullptr;
	for (auto intertr = m_mapContents.begin(); intertr != m_mapContents.end(); intertr++)
	{
		pContent = intertr->second;
		if (pContent != nullptr)
			delete pContent;
	}
	m_mapContents.clear();
}

// TODO:
size_t CDocument::LineNumFromPos(size_t nPos)
{
	return nPos;
}

size_t CDocument::PosFromLineNum(size_t nLineNum)
{
	return nLineNum;
}

size_t CDocument::IndexContents(const wchar_t *wszText, const wchar_t *wszKeyword, int nLevel)
{
	return thePlugin.IndexContents(wszText, wszKeyword, nLevel);
}

void CDocument::DelLine(int nLineNum)
{
	thePlugin.DelLine(nLineNum);
}

void CDocument::CutLines(int nLineStart, int nLineEnd)
{
	thePlugin.CutLines(nLineStart, nLineEnd);
}

void CDocument::PasteBeforeLine(int nLineNum)
{
	thePlugin.PasteBeforeLine(nLineNum);
}

void CDocument::ReplaceLine(int nLineNum, const wchar_t *wszTxt)
{
	thePlugin.ReplaceLine(nLineNum, wszTxt);
}

size_t CDocument::GetLine(int nLineNum, wstring &wLine)
{
	return thePlugin.GetLine(nLineNum, wLine);
}

size_t CDocument::GetLineCount()
{
	return thePlugin.GetLineCount();
}