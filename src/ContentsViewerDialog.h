#pragma once
#ifndef _CONTENTSVIEWERDIALOG_H_
#define _CONTENTSVIEWERDIALOG_H_

#pragma comment(lib,"comctl32.lib")

#include "help.h"
#include "PluginInterface.h"
#include "..\third-party\Win32xx_860\include\wxx_dialog.h"
#include "..\third-party\Win32xx_860\include\wxx_toolbar.h"
#include "resource.h"
#include "IniFile.h"
#include <map>
#include <vector>
#include "Document.h"

using namespace std;

class CContentsListView;
class CContentsTreeView;

/*typedef struct _content
{
	tString tContent;
	tString tKeyword;
	int nLevel;
} ContentData;*/

class CContentsViewerDialog : public CDialog
{
public :
	CContentsViewerDialog();
	virtual ~CContentsViewerDialog();
	
private:
	DISALLOW_COPY_AND_ASSIGN(CContentsViewerDialog);
	
protected:
	virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//virtual void EndDialog(INT_PTR nResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//virtual void OnOK();

	virtual LRESULT OnNotify(WPARAM wParam, LPARAM lParam);

	void OnSize(bool bInitial = false);

	//void OnPrevPosClicked();
	//void OnNextPosClicked();
	//void OnUpdateBtnClicked();
	void OnGotoPrevPos();
	void OnGotoNextPos();
	void OnScriptReplaceMenu();
	void OnScriptReplace(size_t uMsg);
	void OnPyScriptReplaceMenu();
	void OnPyScriptReplace(size_t uMsg);
	void OnListView(){};
	void OnListViewItemClicked();
	void OnListViewItemRClicked(NMHDR* lParam, LRESULT* pResult);
	void OnTreeView(){};
	void OnParse();
	void OnExtraMenu();

	void OnHeadIndexContents();

	void OnExportContents();
	void OnImportContents();

	void OnNumberContents();
	void OnAlignNumber(){};
	void OnTrimContents();
	void OnDelDuplicateContents();
	void OnMarkEmptyContents();

	void OnBig5ToGBK();
	void OnMergeParagraphs();
	void OnAlignParagraphs();

	void ClearMapContent();

	void UpdateContentListview();

public:
	size_t ParseCurFile();
	size_t ReparseCurFile();

	int AddContent(int nItem, const TCHAR* tszContentName, int line, int level, const TCHAR* tszKeyword);
	
	void SavePos(int line);

	void SetNPPWnd(HWND hNPPWnd) {m_hNPPWnd = hNPPWnd;}

	void FocuseContentListviewItemByLine(int line);

	void GetDocument(const CDocument* pDocument){pDocument = m_pDocument;};

private:
	bool m_bDebug;

	CToolBar mToolBar;

	HMENU m_hScriptMenu;
	HMENU m_hPyScriptMenu;
	CContentsListView* m_pContentsLV;
	HMENU m_hContentContextMenu;
	//CContentsTreeView mContentsTV;

	HMENU m_hExtraMenu;

	bool m_bHeadIndexContent;
	
	map<int, ContentData*> m_mapContent;
	map<int, tString> m_mapScript;

	CDocument* m_pDocument;
	int m_nMaxLevel;
	//vector<int> m_vecLine;

    HWND m_hNPPWnd;
	// TCHAR m_tszAppPath[MAX_PATH];
	TCHAR m_tszCfgPath[MAX_PATH];
	TCHAR m_tszScriptPath[MAX_PATH];

	size_t m_ulineBetweenParagraphs;
	TCHAR m_tszNum[MAX_PATH];
	size_t m_uCountX;
	TCHAR m_tszKeyword[40];

private:
	void readCfg(const TCHAR* cfgFile);
	//int FindAndReplace(const TCHAR* tszFind, const TCHAR* tszReplace);
	void ListScripts();
	//void ExecScript(size_t id);
};

#endif //_CONTENTSVIEWERDIALOG_H_