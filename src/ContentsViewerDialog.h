// ContentsViewerDialog.h
#pragma once
#ifndef _CONTENTSVIEWERDIALOG_H_
#define _CONTENTSVIEWERDIALOG_H_

#pragma comment(lib, "comctl32.lib")

#include "help.h"
#include "..\third_party\Win32xx_860\include\wxx_dialog.h"
#include "..\third_party\Win32xx_860\include\wxx_toolbar.h"
#include "resource.h"
#include "IniFile.h"
#include <map>
#include <vector>
#include "Document.h"
#include "ScriptPlugin.h"

using namespace std;

class CContentsListView;
class CContentsTreeView;

class CContentsViewerDialog: public CDialog
{
public :
	CContentsViewerDialog();
	virtual ~CContentsViewerDialog();

private:
	DISALLOW_COPY_AND_ASSIGN(CContentsViewerDialog);

protected:
	virtual BOOL OnInitDialog();
	BOOL CreateToolbar();

	virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnSize(bool bInitial = false);

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnGotoPrevPos(){};
	void OnGotoNextPos(){};
	void OnScriptReplaceMenu();
	void OnScriptReplace(size_t uMsg);
	void OnPyScriptReplaceMenu();
	void OnPyScriptReplace(size_t uMsg);
	void OnListView(){};
	void OnTreeView(){};
	void OnExtraMenu();

	virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);
	void OnListViewItemClicked();
	void OnListViewItemRClicked(NMHDR* lParam, LRESULT* pResult);

public:
	size_t ParseCurFile();
	size_t ReParseCurFile();

	int AddContent(int nItem, const wchar_t* wszContentName, int line, int nLevel, const wchar_t* wszKeyword);

	void SavePos(int line){};

	void UpdateContentListview();
	void FocuseContentListviewItemByLine(int line);

	void HeadIndex();
	void SpecificIndex();
	void ExportContents();
	void ImportContents();

	void NumberContents();
	void AlignNumber() {};
	void TrimContents();
	void DelDuplicateContents();
	void MarkEmptyContents();

	void Big5ToGBK();
	void MergeParagraphs();
	void AlignParagraphs();

	void GetDocument(const CDocument* pDocument){pDocument = m_pDocument;};
private:
	bool m_bDebug;

	CImageList m_ImageList;
	CToolBar m_ToolBar;

	//HMENU m_hScriptMenu;
	CMenu m_ScriptMenu;
	//HMENU m_hPyScriptMenu;
	CMenu m_PyScriptMenu;

	HMENU m_hExtraMenu;

	CContentsListView* m_pContentsLV;
	HMENU m_hContentContextMenu;

	wchar_t m_wszScriptPath[MAX_PATH];

	CIniFile* m_pIniFile;

	int m_nMaxLevel;
	stIndexType_t m_stIndexType;
	wstring m_wIndex;

	mapContent_t m_mapContent;
	map<int, wstring> m_mapScriptMenu;
	// map<tString, tString> m_mapScriptExe;
	// vector<tString> m_vecScriptExt;
	CScriptPlugin* m_pScriptPlugin;

	CDocument* m_pDocument;

	size_t m_ulineBetweenParagraphs;
	wchar_t m_wszNum[MAX_PATH];
	size_t m_uCountX;
	wchar_t m_wszKeyword[40];

private:
	//int FindAndReplace(const wchar_t* tszFind, const wchar_t* tszReplace);
	void ListScripts(const wchar_t* scriptPath);
	//void ExecScript(size_t id);
	void ClearMapContent();

protected:
	// follow function copy from Editor.h of scintilla v4.23
	// Coercion functions for transforming WndProc parameters into pointers
	static void *PtrFromSPtr(sptr_t lParam) noexcept {
		return reinterpret_cast<void *>(lParam);
	}
	static const char *ConstCharPtrFromSPtr(sptr_t lParam) noexcept {
		return static_cast<const char *>(PtrFromSPtr(lParam));
	}
	static const unsigned char *ConstUCharPtrFromSPtr(sptr_t lParam) noexcept {
		return static_cast<const unsigned char *>(PtrFromSPtr(lParam));
	}
	static char *CharPtrFromSPtr(sptr_t lParam) noexcept {
		return static_cast<char *>(PtrFromSPtr(lParam));
	}
	static wchar_t *WCharPtrFromSPtr(sptr_t lParam) noexcept {
		return static_cast<wchar_t *>(PtrFromSPtr(lParam));
	}
	static unsigned char *UCharPtrFromSPtr(sptr_t lParam) noexcept {
		return static_cast<unsigned char *>(PtrFromSPtr(lParam));
	}
	static void *PtrFromUPtr(uptr_t wParam) noexcept {
		return reinterpret_cast<void *>(wParam);
	}
	static const char *ConstCharPtrFromUPtr(uptr_t wParam) noexcept {
		return static_cast<const char *>(PtrFromUPtr(wParam));
	}
};

#endif //_CONTENTSVIEWERDIALOG_H_