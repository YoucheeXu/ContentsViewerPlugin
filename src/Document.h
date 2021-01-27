#pragma once
#ifndef _CDOCUMENT_H_
#define _CDOCUMENT_H_

#include "help.h"
#include "PluginInterface.h"
#include "resource.h"
#include "IniFile.h"
#include <map>
#include <vector>

using namespace std;

typedef struct _content
{
	tString tContent;
	tString tKeyword;
	int nLevel;
} ContentData;

typedef map<int, ContentData*> mapContent_t;

class CDocument
{
public :
	CDocument();
	virtual ~CDocument();
	
private:
	DISALLOW_COPY_AND_ASSIGN(CDocument);

private:
	size_t LineFromPos(size_t pos);

public:
	void ClearmapContent();

	size_t IndexContents(const TCHAR* tszText, const TCHAR* tszKeyword, int level);

	void DelLine(int line);
	void CutLines(int lineStart, int lineEnd);
	void PasteBeforeLine(int line);
	void ReplaceLine(int line, const TCHAR* tszTxt);
	size_t GetLine(int line, tString& tLine);
	size_t GetLineCount();
	size_t GetCurLineNo();

	void SetParameters(const size_t ulineBetweenParagraphs, const TCHAR* tszNum, const size_t uCountX, const TCHAR* tszKeyword);
	//void SetParameters(const size_t ulineBetweenParagraphs, const TCHAR tszNum[], const size_t uCountX, const TCHAR tszKeyword[]);

	void GetDocument(tString &tDoc){tDoc = m_tDoc;};
	void MergeParagraphs();
	void AlignParagraphs();
	void ScriptReplace(const tString script);
	void PyScriptReplace(const tString script);
	void Big5ToGBK();
	size_t ReplaceString(tString & tStr, const tString srcStr, const tString desSrc, bool bRegExp);
	void SetDocument(const tString tDoc){m_tDoc = tDoc;};

	size_t Parse(bool bHeadIndexContent);
	void TrimContents();
	void NumberContents();
	void DelDuplicateContents();
	void MarkEmptyContents();
	void UpdateContents();
	void GetContent(mapContent_t& content){content = m_mapContent;};

	int AddContent(int nItem, const TCHAR* tszContentName, int line, int level, const TCHAR* tszKeyword);

	void ExportContents(const tString file);
	void ImportContents(const tString file);

private:
	bool m_bDebug;

	mapContent_t m_mapContent;
	tString m_tDoc;

	size_t m_ulineBetweenParagraphs;
	TCHAR m_tszNum[MAX_PATH];
	size_t m_uCountX;
	TCHAR m_tszKeyword[40];
};

#endif //_CDOCUMENT_H_