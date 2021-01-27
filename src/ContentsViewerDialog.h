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

using namespace std;

class CContentsListView;
class CContentsTreeView;

typedef struct _content
{
	tString tContent;
	tString tKeyword;
	int nLevel;
} ContentData;

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
	void OnFunMenuClicked();
	void OnListViewItemClicked();
	void OnListViewItemRClicked(NMHDR* lParam, LRESULT* pResult);

	void OnHeadIndexContent();

	void OnExportContent();
	void OnImportContent();
	
	void OnDeleteDuplicateContent();
	void OnMarkEmptyContent();
	void OnNumberContent();
	void OnTrimContent();
		
	void OnExecuteReplace();
	void OnBig5ToGBK();
	void OnMergeParagraph();
	void OnAlignParagraph();

	void ClearMapContent();

	void UpdateContentListview();

public:
	void ParseCurrentFile();
	void ReparseCurrentFile();

	int AddContent(int nItem, const TCHAR* tszContentName, int line, int level, const TCHAR* tszKeyword);
	
	void SavePos(int line);

	void SetNppWnd(HWND hNppWnd) {m_hNppWnd = hNppWnd;}

	void FocuseContentListviewItemByLine(int line);

private:
	bool m_bDebug;
	CContentsListView* m_pContentsLV;
	//CContentsTreeView mContentsTV;
	CToolBar mToolBar;
	HMENU mHContentItemMenu;
	HMENU mHFunMenu;
	CIniFile* m_pIniFile;

	bool m_bHeadIndexContent;
	
	map<int, ContentData*> m_mapContent;	
	map<int, tString> m_mapRepScript;

	int m_nMaxLevel;
	//vector<int> m_vecLine;

protected:
    HWND m_hNppWnd;
	//TCHAR m_tszAppPath[MAX_PATH];
	TCHAR m_tszCfgPath[MAX_PATH];
	TCHAR m_tszScriptPath[MAX_PATH];
private:
	//int FindAndReplace(const TCHAR* tszFind, const TCHAR* tszReplace);
	void ListRepScript();
	void ExecRepScript(UINT id);
};

#endif //_CONTENTSVIEWERDIALOG_H_