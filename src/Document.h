// Document.h
#pragma once
#ifndef _CDOCUMENT_H_
#define _CDOCUMENT_H_

#include "help.h"
#include "..\third_party\Npp\PluginInterface.h"
#include <map>
#include <vector>
#include "ContentsViewerMsgs.h"

using namespace std;

typedef struct _ContentData
{
	wstring wContent;
	wstring wKeyword;
	int nLevel;
} ContentData_t;

typedef map<int, ContentData_t*> mapContent_t;

class CDocument
{
public :
	CDocument();
	virtual ~CDocument();
	
private:
	DISALLOW_COPY_AND_ASSIGN(CDocument);

private:
	size_t LineNumFromPos(size_t pos);
	size_t PosFromLineNum(size_t lineNum);

public:

	void SetParameters(const size_t ulineBetweenParagraphs, const wchar_t* wszNum, const size_t uCountX, const wchar_t* wszKeyword);
	//void SetParameters(const size_t ulineBetweenParagraphs, const wchar_t wszNum[], const size_t uCountX, const wchar_t wszKeyword[]);

	void FetchDocument();
	void UpdateDocument();

	void ClearmapContent();

	size_t IndexContents(const wchar_t* wszText, const wchar_t* wszKeyword, int nLevel);

	void DelLine(int nLineNum);
	void CutLines(int nLineStart, int nLineEnd);
	void PasteBeforeLine(int nLineNum);
	void ReplaceLine(int nLineNum, const wchar_t* wszTxt);
	size_t GetLine(int nLineNum, wstring& wLine);
	size_t GetLineCount();

	void GetDocument(wstring& wDoc){wDoc = m_wDoc;};
	void MergeParagraphs();
	void AlignParagraphs();
	void ScriptReplace(const wstring wScript);
	void Big5ToGBK();
	// size_t ReplaceString(wstring & wStr, const wstring srcStr, const wstring desSrc, bool bRegExp);
	void SetDocument(const wchar_t* wszDoc){m_wDoc = wszDoc;};

	size_t Parse(stIndexType_t stIndexType, const wchar_t* wszIndex);
	void TrimContents();
	void NumberContents();
	void DelDuplicateContents();
	void MarkEmptyContents();
	void UpdateContents();

	void GetContent(mapContent_t& content){content = m_mapContents;}
	void SetContent(const mapContent_t content){m_mapContents = content;}

	int AddContent(int nItem, const wchar_t* wszContentName, int nLine, int nLevel, const wchar_t* wszKeyword);

	void ExportContents(const wstring wFile);
	void ImportContents(const wstring wFile);

private:
	bool m_bDebug;

	mapContent_t m_mapContents;
	wstring m_wDoc;

	size_t m_ulineBetweenParagraphs;
	wchar_t m_wszNum[MAX_PATH];
	size_t m_uCountX;
	wchar_t m_wszKeyword[40];
};

#endif //_CDOCUMENT_H_