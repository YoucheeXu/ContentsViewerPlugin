#include "ContentsViewerDialog.h"
#include "ContentsViewerPlugin.h"
#include "ContentsListView.h"
//#include "ContentsTreeView.h"
#include <regex>
//#include <fstream>
#include <io.h>
#include "..\third-party\Win32xx_860\include\wxx_commondlg.h"

//extern NppData nppData;
extern FuncItem funcItem[2];
//extern CContentsViewerPlugin thePlugin;
//extern CLogFile* g_pLogFile;
//extern bool g_bDebug;
using namespace regex_constants;
using namespace std;
// using namespace Win32xx;

//#define REP_SCRIPT_PATH _T("\\plugins\\Config\\ContentsViewerPlugin\\Rep_Script\\")
//#define INI_PATH _T("\\plugins\\Config\\ContentsViewerPlugin.ini")
#define INI_PATH _T("\\ContentsViewerPlugin.ini")

CContentsViewerDialog::CContentsViewerDialog() : CDialog(IDD_CVDLG)
{
	m_bDebug = false;

	/*TCHAR tszIniPath[MAX_PATH], tszLogPath[MAX_PATH];
	//_tcscpy(m_TszCfgPath, thePlugin.GetPluginsConfigDir());

	GetModuleFileName(NULL, m_tszAppPath, MAX_PATH); //得到当前模块路径
	(_tcsrchr(m_tszAppPath, _T('\\')))[0] = 0;

	_stprintf(tszIniPath, _T("%s%s"), m_tszAppPath, INI_PATH);
	
	m_pIniFile = new CIniFile(tszIniPath);

	g_bDebug = m_pIniFile->GetBool(_T("Debug"), _T("Enable"));
	g_pLogFile = NULL;
	if (g_bDebug)
	{
		m_pIniFile->GetString(_T("Debug"), _T("file"), tszLogPath);
		g_pLogFile = new CLogFile(tszLogPath);
		LOGINFO(_T("LogDir: %s"), tszLogPath);
	}*/

	m_pContentsLV = new CContentsListView();

	m_bHeadIndexContent = false;

	m_nMaxLevel = 0;
}

CContentsViewerDialog::~CContentsViewerDialog()
{
	ClearMapContent();

	delete m_pContentsLV;
	delete m_pIniFile;

	Destroy(); // to be sure GetHwnd() returns NULL

	//if(g_pLogFile != NULL) delete g_pLogFile;
}

BOOL CContentsViewerDialog::OnInitDialog()
{
	//InitOptions();

	TCHAR tszIniPath[MAX_PATH];
	_tcscpy(m_tszCfgPath, thePlugin.GetPluginsConfigDir());

	_tcscat(m_tszCfgPath, _T("\\ContentsViewerPlugin"));

	//GetModuleFileName(NULL, m_tszAppPath, MAX_PATH); //得到当前模块路径
	//(_tcsrchr(m_tszAppPath, _T('\\')))[0] = 0;
	//LOGINFO(_T("AppDir: %s"), m_tszAppPath);

	//_stprintf(tszIniPath, _T("%s%s"), m_tszAppPath, INI_PATH);
	_stprintf(tszIniPath, _T("%s%s"), m_tszCfgPath, INI_PATH);
	
	m_pIniFile = new CIniFile(tszIniPath);

	g_bDebug = m_pIniFile->GetBool(_T("Debug"), _T("Enable"));
	if (g_bDebug)
	{
		TCHAR tszLogPath[MAX_PATH], tszLogFullPath[MAX_PATH];
		m_pIniFile->GetString(_T("Debug"), _T("file"), tszLogPath);
		_stprintf(tszLogFullPath, _T("%s%s"), m_tszCfgPath, tszLogPath);
		LOGINFO(_T("LogDir: %s"), tszLogFullPath);
		g_pLogFile->StartLog(tszLogFullPath);
		LOGINFO(_T("LogDir: %s"), tszLogFullPath);
	}

	HWND hToolBar = mToolBar.CreateEx(
		0, //WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE, 
		TOOLBARCLASSNAME, _T(""), 
		/*WS_TABSTOP | */ WS_VISIBLE | WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 
		//1, 1, 0, 0, m_hNppWnd, NULL);
		1, 1, 0, 0, GetHwnd(), NULL);
	assert(hToolBar);
	mToolBar.AddButton(IDM_PREVPOS);
	mToolBar.AddButton(IDM_NEXTPOS);
	mToolBar.AddButton(0); // separator
	mToolBar.AddButton(IDM_SORTLINE);
	mToolBar.AddButton(IDM_SORTNAME);
	mToolBar.AddButton(0); // separator
	mToolBar.AddButton(IDM_VIEWLIST);
	mToolBar.AddButton(IDM_VIEWTREE);
	mToolBar.AddButton(0); // separator
	mToolBar.AddButton(IDM_UPDATE);
	mToolBar.AddButton(0); // separator
	mToolBar.AddButton(IDM_MENU);
	// mToolBar.SetImages(RGB(192,192,192), IDB_TOOLBAR, 0, 0);
	// mToolBar.SetBitmap(IDB_TOOLBAR);
	mToolBar.AddBitmap(IDB_TOOLBAR);

	//m_pContentsLV->Attach(::GetDlgItem(m_hNppWnd, IDC_LV_CV));
	m_pContentsLV->Attach(GetDlgItem(IDC_LV_CV));
	m_pContentsLV->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_pContentsLV->InsertColumn(0, _T("Contents"), LVCFMT_LEFT, 220);
	m_pContentsLV->InsertColumn(1, _T("Line"), LVCFMT_RIGHT, 50);

	mToolBar.SetButtonState(IDM_PREVPOS, 0);
	mToolBar.SetButtonState(IDM_NEXTPOS, 0);
	mToolBar.SetButtonState(IDM_SORTLINE, 0);
	mToolBar.SetButtonState(IDM_SORTNAME, 0);
	mToolBar.SetButtonState(IDM_VIEWLIST, 0);
	mToolBar.SetButtonState(IDM_VIEWTREE, 0);
	//mToolBar.SetButtonState(IDM_CLOSE, 1);

	mHFunMenu = LoadMenu(thePlugin.GetInstanceHandle(), MAKEINTRESOURCE(IDM_FUNCTION_CONTENTITEM));
	mHFunMenu = GetSubMenu(mHFunMenu, 0);

	mHContentItemMenu = LoadMenu(thePlugin.GetInstanceHandle(), MAKEINTRESOURCE(IDM_RIGHT_CONTENTITEM));
	mHContentItemMenu = GetSubMenu(mHContentItemMenu, 0);

	OnSize(true);

	//int line = thePlugin.GetCurrentLine();
	int line = 1;
	//m_vecLine.push_back(line);
	mToolBar.SetButtonState(IDM_PREVPOS, 1);
	
	// list all RepScript and add it to submenu of "rep script"
	//ListRepScript();

	return TRUE;
}

INT_PTR CContentsViewerDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		//case WM_UPDATETAGSVIEW:
		//    UpdateTagsView();
		//    return 0;

		//case WM_CTAGSPATHFAILED:
		//    if (m_pEdWr)
		//    {
		//        tString err = _T("File \'ctags.exe\' was not found. The path is incorrect:\n");
		//        err += m_ctagsPath;
		//        ::MessageBox(m_pEdWr->ewGetMainHwnd(), err.c_str(), _T("TagsView Error"), MB_OK | MB_ICONERROR);
		//    }
		//    return 0;

		case WM_SIZE:
			OnSize();
			break;

		case WM_INITDIALOG:
			return OnInitDialog();

		case WM_NOTIFY:
			NMHDR* pnmh = (NMHDR*) lParam;
			if (pnmh->hwndFrom == m_hNppWnd)
			{
				if (LOWORD(pnmh->code) == DMN_CLOSE)
				{
					::SendMessage(m_hNppWnd, NPPM_SETMENUITEMCHECK,
						funcItem[0]._cmdID, FALSE);
					return 0;
				}
			}
			break;
	}

	return DialogProcDefault(uMsg, wParam, lParam);
}

void CContentsViewerDialog::OnSize(bool bInitial)
{
	if (bInitial)
	{
		CRect r = GetWindowRect();
		//::MoveWindow(GetHwnd(), 0, 0, m_opt.getInt(OPT_VIEW_WIDTH), r.Height(), TRUE);
		return;
	}

	CRect r = GetClientRect();
	const int width = r.Width();
	const int height = r.Height();
	const int left = 2;
	int top = 1;

	// toolbar
	r = mToolBar.GetWindowRect();
	::MoveWindow(mToolBar, left, top, width - 2*left, r.Height(), TRUE);
	top += (r.Height() + 1);

	////edit window
	//r = m_edFilter.GetWindowRect();
	//::MoveWindow(m_edFilter, left, top, width - 2*left, r.Height(), TRUE);
	//top += (r.Height() + 1);

	//list-view window
	r = m_pContentsLV->GetWindowRect();
	::MoveWindow(m_pContentsLV->GetHwnd(), left, top, width - 2 * left, height - top - 2, TRUE);
}

//
BOOL CContentsViewerDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = LOWORD(wParam);
	
	if(uMsg >= ID_NEWMENU){
		ExecRepScript(uMsg);
	}
	
	switch (uMsg)
	{
		case IDM_PREVPOS:
			OnGotoPrevPos();
			break;
			
		case IDM_NEXTPOS:
			OnGotoNextPos();
			break;

		case IDM_UPDATE:
			ReparseCurrentFile();
			break;

		case IDM_MENU:
			OnFunMenuClicked();
			break;

		case IDM_HEAD_INDEX_CONTENT:
			OnHeadIndexContent();
			ReparseCurrentFile();
			break;
			
		case IDM_EXPORT_CONTENT:
			ReparseCurrentFile();
			OnExportContent();
			break;
		
		case IDM_IMPORT_CONTENT:
			OnImportContent();
			ReparseCurrentFile();			
			break;
	
		case IDM_DEL_DUPLICATE_CONTENT:
			//MessageBox(_T("IDM_DEL_DUPLICATE_CONTENT was clicked"), _T("Contents Viewer"), MB_OK);
			OnDeleteDuplicateContent();
			ReparseCurrentFile();
			break;

		case IDM_MARK_EMPTY_CONTENT:
			OnMarkEmptyContent();
			ReparseCurrentFile();
			//MessageBox(_T("IDM_MARK_FALSE_CONTENT was clicked"), _T("Contents Viewer"), MB_OK);
			break;
			
		case IDM_SC_INTO_NUMBER:
			//MessageBox(_T("IDM_SC_INTO_NUMBER was clicked"), _T("Contents Viewer"), MB_OK);
			if (true == m_bHeadIndexContent) OnHeadIndexContent();
			ReparseCurrentFile();
			OnNumberContent();
			ReparseCurrentFile();
			break;

		case IDM_SC_ALIGN_NUMBER:
			MessageBox(_T("IDM_SC_ALIGN_NUMBER was clicked"), _T("Contents Viewer"), MB_OK);
			break;
			
		case IDM_SC_TRIM_CONTENT:
			if(true == m_bHeadIndexContent) OnHeadIndexContent();
			ReparseCurrentFile();
			OnTrimContent();
			ReparseCurrentFile();
			break;

		case IDM_SC_SORT_CONTENT:
			MessageBox(_T("IDM_SC_SORT_PARAGRAPH was clicked"), _T("Contents Viewer"), MB_OK);
			//ReparseCurrentFile();
			break;

		case IDM_REPLACE_WORDS :
			//MessageBox(_T("IDM_REPLACE_PINYINWORD was clicked"), _T("Contents Viewer"), MB_OK);
			OnExecuteReplace();
			ReparseCurrentFile();
			break;

		case IDM_BIG5_GBK:
			//MessageBox(_T("IDM_REMOVE_WORONGWORD was clicked"), _T("Contents Viewer"), MB_OK);
			OnBig5ToGBK();
			ReparseCurrentFile();
			break;

		case IDM_MERGE_PARAGRAPH:
			//MessageBox(_T("IDM_MERGE_PARAGRAPH was clicked"), _T("Contents Viewer"), MB_OK);
			OnMergeParagraph();
			break;
			
		case IDM_ALIGN_PARAGRAPH:
			//MessageBox(_T("IDM_ADJUST_BLANKPARAGRAPH was clicked"), _T("Contents Viewer"), MB_OK);
			OnAlignParagraph();
			ReparseCurrentFile();
			break;

		case IDM_DEL_CONTENT:
			//MessageBox(_T("IDM_DEL_CONTENT was clicked"), _T("Contents Viewer"), MB_OK);
			//m_pContentsLV->OnDelContent();
			m_pContentsLV->OnCutContent();
			ReparseCurrentFile();
			break;

		case IDM_CUT_CONTENT:
			//MessageBox(_T("IDM_CUT_CONTENT was clicked"), _T("Contents Viewer"), MB_OK);
			m_pContentsLV->OnCutContent();
			ReparseCurrentFile();
			break;

		case IDM_INSERT_CONTENT_BEFORE:
			//MessageBox(_T("IDM_INSERT_CONTENT_BEFORE was clicked"), _T("Contents Viewer"), MB_OK);
			m_pContentsLV->OnInsetContentBefore();
			ReparseCurrentFile();
			break;

		case IDM_SELECT_CONTENT:
			//ReparseCurrentFile();
			m_pContentsLV->OnSelectContent();
			break;
	}
	return FALSE;
}

LRESULT CContentsViewerDialog::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (((LPNMHDR)lParam)->code)
	{
		case TTN_GETDISPINFO:
			{
				// code from Win32++
				int iIndex = mToolBar.HitTest();
				if (iIndex >= 0)
				{
					int nID = mToolBar.GetCommandID(iIndex);
					if (nID > 0)
					{
						LPNMTTDISPINFO lpDispInfo = (LPNMTTDISPINFO) lParam;
						lpDispInfo->lpszText = (LPWSTR)(LPCWSTR)LoadString(nID);
					}
				}
			}
			break;

		case LVN_COLUMNCLICK:
			//SetSortMode((eTagsSortMode) (TWM_NAME + ((LPNMLISTVIEW) lParam)->iSubItem));
			break;

		case LVN_ITEMACTIVATE:
			break;

		case TVN_SELCHANGED:
			break;

		case NM_DBLCLK :
			break;

		case NM_CLICK:
			if(IDC_LV_CV == LOWORD(wParam))
				OnListViewItemClicked();
			break;

		case NM_RCLICK:
			if(IDC_LV_CV == LOWORD(wParam))
				//NMHDR* pNMHDR, LRESULT* pResult
				OnListViewItemRClicked((NMHDR*)lParam, 0);
			break;
		}

		return 0;
}

void CContentsViewerDialog::OnFunMenuClicked()
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	TrackPopupMenu(mHFunMenu, TPM_LEFTBUTTON, point.x, point.y, 0, this->GetHwnd(), NULL);
}

void CContentsViewerDialog::OnListViewItemClicked()
{
	int iItem = m_pContentsLV->GetSelectionMark();
	if (iItem >= 0)
	{
		int state = m_pContentsLV->GetItemState(iItem, LVIS_FOCUSED | LVIS_SELECTED);
		if (state & (LVIS_FOCUSED | LVIS_SELECTED))
		{
			m_pContentsLV->SetItemState(iItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			m_pContentsLV->SetSelectionMark(iItem);
			//m_pContentsLV->SetLastItem(iItem);

			int line = m_pContentsLV->GetLineByItem(iItem);
			if (line >= 1)
			{
				//thePlugin.GotoLine(line + 5);
				thePlugin.GotoLine(line);
			}
		}
	}
}

void CContentsViewerDialog::OnListViewItemRClicked(NMHDR* lParam, LRESULT* pResult)
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	TrackPopupMenu(mHContentItemMenu, TPM_LEFTBUTTON, point.x, point.y, 0, this->GetHwnd(), NULL);

	*pResult = 0;
}

int CContentsViewerDialog::AddContent(int nItem, const TCHAR* tszContentName, int line, int level, const TCHAR* tszKeyword)
{
	LOGFUNBGN;

	LOGINFO(_T("AddContent: %s, Level : %d, Line: %d"), tszContentName, level, line);
	
	//int maxlen = 16;
	
	//TCHAR tszContent[256];

	if (m_mapContent.end() == m_mapContent.find(line))
	{
		ContentData* pContent = new ContentData;
		//if(_Tcslen(tszContentName) > maxlen)
		//{
		//	_Tcsncat(tszContent, tszContentName, maxlen);
		//}
		//pContent->tContent = tszContent;
		pContent->tContent = tszContentName;
		pContent->tKeyword = tszKeyword;
		pContent->nLevel = level;

		m_mapContent[line] = pContent;

		return 1;
	}

	return -1;
}

void CContentsViewerDialog::ParseCurrentFile()
{
}

void CContentsViewerDialog::ReparseCurrentFile()
{
	LOGFUNBGN;

	thePlugin.UpdateCurrentFileCodePage();

	m_pContentsLV->SetRedraw(FALSE);
	m_pContentsLV->DeleteAllItems();
	//m_pContentsLV->SetRedraw(TRUE);

	ClearMapContent();

	tString tDocument;

	//thePlugin.GetDocument(tDocument);
	
	int totalnum = 0;
	if(false == m_bHeadIndexContent)
	{
		/*LOGINFO(_T("Start to index 卷.\n"));
		int level = 1;
		tString tText = _T("^[ 　]*第[^ 　]{0, 14}卷.*");
		LOGINFO(tText.c_str());
		int i = thePlugin.IndexContents(tText.c_str(), level);
		if (i > 0) level++;
		LOGINFO(_T("End to index 卷.\n"));

		LOGINFO(_T("Start to index 集.\n"));
		int pos = tText.find(_T("卷"));
		tText = tText.replace(pos, 1, _T("集"));
		LOGINFO(tText.c_str());
		i = thePlugin.IndexContents(tText.c_str(), level);
		if (i > 0) level++;
		LOGINFO(_T("End to index 集.\n"));
		
		LOGINFO(_T("Start to index 章.\n"));
		tText = tText.replace(pos, 1, _T("章"));
		LOGINFO(tText.c_str());
		i = thePlugin.IndexContents(tText.c_str(), level);
		if (i > 0) level++;
		LOGINFO(_T("End to index 章.\n"));
		
		LOGINFO(_T("Start to index 回.\n"));
		tText = tText.replace(pos, 1, _T("回"));
		LOGINFO(tText.c_str());
		i = thePlugin.IndexContents(tText.c_str(), level);
		LOGINFO(_T("End to index 回.\n"));
		
		LOGINFO(_T("Start to sort.\n"));*/
		
		//tString tlevelItem;
		TCHAR tszKeyword[40], X[MAX_PATH], tszLevelItem[20];
		int totalLevel = m_pIniFile->GetInteger(_T("Content"), _T("totallevel"));
		int CountX = m_pIniFile->GetInteger(_T("Content"), _T("CountX"));
		m_pIniFile->GetString(_T("Content"), _T("X"), X);
		LOGINFO(_T("totalLevel: %d, CountX: %d, X: %s"), totalLevel, CountX, X);
		TCHAR tt[10];
		_stprintf(tt, _T("%d"), CountX*2);

		int level = 1;
		int num = 0;
		//_stprintf(tszText, _T("^[ 　]*第[%s]{0, %d}卷.*"), X, CountX*2);
		//tString tText = _T("^[ 　\t]*第[");
		tString tText = _T("^[ 　\\t]*?第[");
		tText = tText + X;
		tText = tText + _T("]{0,");
		tText = tText + tt;
		tText = tText + _T("}卷.*");
		LOGINFO(_T("tText: %s"), tText.c_str());
		//MessageBoxW(tText.c_str(), tText.c_str(), MB_OK);
		int pos = tText.find(_T("卷"));	
		LOGINFO(_T("pos: %d"), pos);
		LOGINFO(_T("totalLevel: %d"), totalLevel);

		for (int i = 1; i <= totalLevel; i ++)
		{
			_stprintf(tszLevelItem, _T("%s%d"), _T("level"), i);
			m_pIniFile->GetString(_T("Content"), tszLevelItem, tszKeyword);
			LOGINFO(_T("tszLevelItem: %s, keywrod: %s."), tszLevelItem, tszKeyword);
			tText = tText.replace(pos, 1, tszKeyword);
			LOGINFO(_T("tText: %s"), tText.c_str());
			if(num > 0) level++;
			num = thePlugin.IndexContents(tText.c_str(), tszKeyword, level);
			//num = thePlugin.IndexContents(tDocument, tText.c_str(), tszKeyword, level);
			if (num > 0) m_nMaxLevel = level;
			totalnum = totalnum + num;
			LOGINFO(_T("End to index %s.\n"), tszKeyword);
		}
	}
	else
	{
		totalnum = thePlugin.IndexContents(_T("^[^ 　\\t\\r\\n].+"), _T(""), 1);
		//totalnum = thePlugin.IndexContents(tDocument, _T("^[^ 　\\t\\r\\n].+"), _T("回"), 1);
		LOGINFO(_T("Advanced index content."));

	}
	
	if (totalnum >= 1)
	{
		m_pContentsLV->SortItemsByLine();

		UpdateContentListview();

		int line = thePlugin.GetCurrentLine();
		FocuseContentListviewItemByLine(line);

		//int line = thePlugin.GetLastLine();
		//if(line > 30)
		//	thePlugin.GotoLine(line);
		//m_pContentsLV->FocusLastItem();
		//ActiveContentListviewItem();
	}
	
	m_pContentsLV->SetRedraw(TRUE);
	
	LOGFUNEND;
}

//wait to finish
void CContentsViewerDialog::SavePos(int line)
{
	line = thePlugin.GetCurrentLine();
	//m_vecLine.push_back(line);
	mToolBar.SetButtonState(IDM_PREVPOS, 1);
}

//
void CContentsViewerDialog::OnHeadIndexContent()
{
	m_bHeadIndexContent = !m_bHeadIndexContent;
	if(true == m_bHeadIndexContent)
	{
		CheckMenuItem(mHFunMenu, IDM_HEAD_INDEX_CONTENT, MF_BYCOMMAND | MF_CHECKED);
	}
	else
	{
		CheckMenuItem(mHFunMenu, IDM_HEAD_INDEX_CONTENT, MF_BYCOMMAND | MF_UNCHECKED);
	}
}

//TO-DO: test
void CContentsViewerDialog::OnExportContent(){
	tString tItemText = _T("");
	int line = 0;
	
	TCHAR lineBuffer[4];

	Win32xx::CFileDialog myDlg(FALSE, NULL, NULL, 0, _T("Text|*.txt||"));
	
	if(myDlg.DoModal()!=IDOK) return;
		
	tString tContentPath = myDlg.GetPathName();

	FILE* pContentFile = NULL;
	
	pContentFile = _tfopen(tContentPath.c_str(), _T("w+"));

	if(NULL == pContentFile) {
		LOGERR(_T("Can't open %s to Export!"), tContentPath.c_str());
		return;
	}

	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;

	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++) {
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
void CContentsViewerDialog::OnImportContent(){

	CFileDialog myDlg(TRUE, NULL, NULL, 0, _T("Text|*.txt||"));
	
	if(myDlg.DoModal()!=IDOK) return;
		
	tString tContentPath = myDlg.GetPathName();

	FILE* contentFile = NULL;

	contentFile = _tfopen(tContentPath.c_str(), _T("r"));
	
	if(NULL == contentFile) {
		LOGERR(_T("Can't open %s to Import!"), tContentPath.c_str());
		return;
	}

	assert(contentFile);

	TCHAR tszLine[256];

	TCHAR* tszContent;
	TCHAR* tszLineNo;
	
	int lineNo = 0;

	while(_fgetts(tszLine, 256, contentFile))
	{
		int i = _tcslen(tszLine);

		tszLine[i - 1] = 0;

		LOGINFO(_T("Line：%s."), tszLine);

		tszContent = _tcstok(tszLine, _T("\t"));
		tszLineNo = _tcstok(NULL, _T("\t"));

		LOGINFO(_T("Content: %s, tszLineNo：%s."), tszContent, tszLineNo);
		
		lineNo = (int)(_ttoi(tszLineNo));
		
		thePlugin.ReplaceLine(lineNo, tszContent);
	}

	fclose(contentFile);
	contentFile = NULL;
}

//wait to add
void CContentsViewerDialog::OnDeleteDuplicateContent()
{
	LOGFUNBGN;
	tString tItemText1, tItemText2;
	int line;

	for(int iItem = m_pContentsLV->GetItemCount() - 1; iItem >= 0; iItem--)
	{
		tItemText1 = m_pContentsLV->GetContentByItem(iItem);
		tItemText2 = m_pContentsLV->GetContentByItem(iItem - 1);
		line = m_pContentsLV->GetLineByItem(iItem);

		LOGINFO(_T("tItemText1: %s, tItemText2: %s"), tItemText1.c_str(), tItemText2.c_str());

		if (0 == _tcscmp(tItemText1.c_str(), tItemText2.c_str()))
		{
			thePlugin.DeleteLine(line);
			LOGINFO(_T("tItemText1: %s = tItemText2: %s"), tItemText1.c_str(), tItemText2.c_str());
		}			
	} 
	LOGFUNEND;
}

//wait to add function: 
//1. excusive level 2 up content,  ok
//2. Delete not empty content's empty mark
//3. print a to-do list
void CContentsViewerDialog::OnMarkEmptyContent()
{
	LOGFUNBGN;

	if (true == m_bHeadIndexContent) return;

	ReparseCurrentFile();

	tString tItemText1;

	int line1, line2, level1, level2;

	//for(int iItem = 0; iItem<= m_pContentsLV->GetItemCount() - 2; iItem++)
	//{ 
	//	tItemText1 = m_pContentsLV->GetContentByItem(iItem);
	//	line1 = m_pContentsLV->GetLineByItem(iItem);

	//	line2 = m_pContentsLV->GetLineByItem(iItem + 1);
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

		//tItemText1 = _Tcscpy(_Tcstrim(tItemText1));
		//tItemText2 = _Tcscpy(_Tcstrim(tszItemText2));

		int pos = tItemText1.rfind(_T("+"));

		if (pos != string::npos) LOGINFO(_T("pos of '+' : %d; length: %d."), pos, tItemText1.length());
		if (tItemText1.rfind(_T("+")) == tItemText1.length() - 3) return;

		if (line2 - line1 <= 5 && level1 >= level2)
		{
			//wcscat(szItemText1, _T("+"));
			//szItemText1.append(_T("+"));
			//LOGINFO(_T("Length of %s : %u\n"), szItemText1, _Tcslen(szItemText1.c_str()));
			LOGINFO(_T("Empty content : %s"), tItemText1.c_str());
			//level1 = m_pContentsLV-

			tItemText1.insert(tItemText1.length() - 2, _T("+"));
			//LOGINFO(_T("Length of %s : %u\n"), tItemText1, _Tcslen(tItemText1.c_str()));

			LOGINFO(_T("Line1: %d"), line1);
			LOGINFO(_T("Line2: %d"), line2);
			LOGINFO(_T("Marked content : %s"), tItemText1.c_str());
			LOGINFO(_T("\n"));
			thePlugin.ReplaceLine(line1, tItemText1.c_str());
		}
	} 
	LOGFUNEND;
}

//wait to test
//2. align number
//Bug: sometimes lose "1" in "12" and so on
void CContentsViewerDialog::OnNumberContent()
{
	LOGFUNBGN;
	//if(true == m_bAdvancedIndexContent) return;
	tString tContent, tNumber;
	int line, pos, lastpos;
	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;
	//for (int iItem = 0; iItem <= m_pContentsLV->GetItemCount() - 1; iItem++)
	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++)
	{
		line = intertr->first;
		pContent = intertr->second;
		tContent = pContent->tContent;
		StringTrimLeft(tContent);
		//tNumber = tContent;

		/*pos = tContent.find(_T("第十"));
		if(pos != tString::npos) tContent = tContent.replace(pos, 2, _T("第1"));*/
		
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
			thePlugin.ReplaceLine(line, tContent.c_str());
			continue;
		}

		if (0 == _tcscmp(tNumber.substr(0, 1).c_str(), _T("十")))
		{
			tNumber = tNumber.replace(0, 1, _T("1"));
		}

		//tContent = m_pContentsLV->GetContentByItem(iItem);
		//line = m_pContentsLV->GetLineByItem(iItem);

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
		//tContent.replace(_T("千0"), _T("00"));
		
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
				//LOGINFO(_T("Pos: %d, nPos: %d"), pos, tString::npos);
			}
		}
		LOGINFO(_T("Numbered five: %s."), tNumber.c_str());

		//remove left banks
		tContent = tContent.replace(1, lastpos - 1, tNumber);
		LOGINFO(_T("Numbered content: %s."), tContent.c_str());
		thePlugin.ReplaceLine(line, tContent.c_str());
	}

	LOGFUNEND;
}

void CContentsViewerDialog::OnExecuteReplace()
{
	LOGFUNBGN;

	//bool bDebug = true;

	tString tFile;

	//thePlugin.GetSelection(tFile);
	thePlugin.GetDocument(tFile);

	//LOGINFO(_T("Selection:\n%s"), tFile.c_str());

	TCHAR tszRepFile[MAX_PATH], tszRepPath[MAX_PATH];

	m_pIniFile->GetString(_T("Replace"), _T("file"), tszRepFile);

	//_stprintf(tszRepPath, _T("%s%s%s"), m_tszAppPath, _T("\\plugins\\Config\\"), tszRepFile);
	_stprintf(tszRepPath, _T("%s%s"), m_tszScriptPath, tszRepFile);

	bool isRegularMode = m_pIniFile->GetBool(_T("Replace"), _T("RegularMode"));
	
	FILE* repFile = NULL;
	//repFile = _tfopen(tfile, _T("r"));
	repFile = _tfopen(tszRepPath, _T("r"));

	//LOGINFO(_T("Start to replace : %s"), tfile);
	LOGINFO(_T("Start to replace : %s."), tszRepPath);

	assert(repFile);

	TCHAR tszLine[256];

	TCHAR* tszFind;
	TCHAR* tszReplace;

	//while(!fin.eof())
	while(_fgetts(tszLine, 256, repFile))
	//while (getline(&tszLine, 256, repFile)) != -1)
	{
		//strtok();
		//fin.getline(szLine, 256);

		int i = _tcslen(tszLine);
		//if (tszLine[i - 1] == _T('\n')) //因为最后一行没有回车符
		//{
		//	tszLine[i - 1] = 0;
		//}

		tszLine[i - 1] = 0;

		LOGINFO(_T("Line：%s."), tszLine);

		tszFind = _tcstok(tszLine, _T("\t"));
		tszReplace = _tcstok(NULL, _T("\t"));

		if (NULL == tszReplace)	tszReplace = _T("");

		LOGINFO(_T("tFind：%s, tReplace：%s."), tszFind, tszReplace);

		//thePlugin.FindAndReplace(tszFind, tszReplace, isRegularMode);
		//FindAndReplace(tszFind, tszReplace);
		int count = StringReplaceAll(tFile, tszFind, tszReplace);

		LOGINFO(_T("%d was found and replaced."), count);

	}

	//thePlugin.ReplaceSelection(tFile);
	thePlugin.SetDocument(tFile);

	fclose(repFile);
	repFile = NULL;

	//ifstream infile;
	//infile.open(tszRepPath);

	//std::wifstream repFile(tszRepPath);

	//assert(repFile);

	//while (getline(repFile, tszLine))
	//{
	//	
	//}	

	LOGFUNEND;
}

//wait to test
void CContentsViewerDialog::OnBig5ToGBK()
{
	LOGFUNBGN;
	tString tBig5Line, tGBKLine;
	int cchDest;
	WORD wLanguageID = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
	LCID Locale = MAKELCID(wLanguageID, SORT_CHINESE_PRC);
	for(int line = 1; line <= thePlugin.GetMaxLine(); line ++)
	{
		cchDest = thePlugin.GetLine(line, tBig5Line);
		tGBKLine.resize(tBig5Line.length());
		LCMapString(Locale, LCMAP_SIMPLIFIED_CHINESE, tBig5Line.c_str(), -1, const_cast<TCHAR*>(tGBKLine.c_str()), tBig5Line.length());
		thePlugin.ReplaceLine(line, tGBKLine.c_str());
	}
	LOGFUNEND;
}

//bug fix: can't remove the bank in the paragraph.
void CContentsViewerDialog::OnMergeParagraph()
{
	//bool bDebug = true;
	LOGFUNBGN;
	/*
	//thePlugin.FindAndReplace(_T("\r\n\r\n"), _T("@"), true);
	//thePlugin.FindAndReplace("\r\n\r\n", "@", true);
	tString tSel;
	thePlugin.GetSelection(tSel);

	// tr1::cmatch res1;
	// tr1::regex
	// tr1::regex_search
	//thePlugin.FindAndReplace(_T("　　"), _T("@"), false);

	//thePlugin.FindAndReplace(_T("\r\n"), _T(""), true);
	//thePlugin.FindAndReplace("\r\n", "", true);

	//thePlugin.FindAndReplace(_T("@"), _T("\r\n\r\n"), true);
	//thePlugin.FindAndReplace("@", "\r\n\r\n", true);
	
	thePlugin.ReplaceSelection(tSel);
	*/

	tString tFile;

	thePlugin.GetSelection(tFile);
	

	// 表达式选项 
	syntax_option_type flags = ECMAScript | icase;

	try
	{
		tFile = regex_replace(tFile, wregex(_T("\r\n\r\n"), flags), _T("@"));
		//int count = StringRegReplaceAll(tFile, wregex(_T("\r\n\r\n"), flags), _T("@"));
		tFile = regex_replace(tFile, wregex(_T("\r\n"), flags), _T(""));
		//count = StringRegReplaceAll(tFile, wregex(_T("\r\n"), flags), _T(""));
		tFile = regex_replace(tFile, wregex(_T("@"), flags), _T("\r\n\r\n"));
		//count = StringRegReplaceAll(tFile, wregex(_T("@"), flags), _T("\r\n\r\n"));

		//tFile = regex_replace(tFile, wregex(_T("　　")), _T(""), match_default | match_not_bol);
		//tFile = regex_replace(tFile, wregex(_T("\\B　　"), flags), _T(""));
		tFile = regex_replace(tFile, wregex(_T("^　　"), flags), _T("@"));
		tFile = regex_replace(tFile, wregex(_T("　　"), flags), _T(""));
		tFile = regex_replace(tFile, wregex(_T("^@"), flags), _T("　　"));
	}
	catch (regex_error& e)
	{
		LOGINFO(_T("%s: %d."), e.what(), e.code());
	}

	//thePlugin.ReplaceSelection(tFile);

	thePlugin.SetDocument(tFile);

	LOGFUNEND;
}

//wait to test
void CContentsViewerDialog::OnAlignParagraph()
{	
	LOGFUNBGN;
	
	tString tFile;

	//thePlugin.GetSelection(tFile);
	thePlugin.GetDocument(tFile);

	// 表达式选项 
	syntax_option_type flags = ECMAScript;

	try
	{
		int count = 0;
	
		int line = m_pIniFile->GetInteger(_T("paragraph"), _T("linebetweenparagraph"));

		//remove all bank at the head of paragraph
		//theplugin.findandreplace(_T("^[ 　	]+"), _T(""), true);
		tFile = regex_replace(tFile, wregex(_T("^[ 　\t]+"), flags), _T(""));

		//remove all bank at the tail of paragraph
		//theplugin.findandreplace(_T("[ 　	]+$"), _T(""), true);
		tFile = regex_replace(tFile, wregex(_T("[ 　	\t]+$"), flags), _T(""));

		if (0 == line)
		{
			// do
			// {
				// count = theplugin.findandreplace(_T("\r\n\r\n"), _T("\r\n"), true);
			// } while (count >= 1);
			tFile = regex_replace(tFile, wregex(_T("\r\n\r\n"), flags), _T("\r\n"));
		}
		else if (1 == line)
		{
			//theplugin.findandreplace(_T("\r\n"), _T("\r\n\r\n"), true);
			tFile = regex_replace(tFile, wregex(_T("\r\n"), flags), _T("\r\n\r\n"));
			// do
			// {
				// count = theplugin.findandreplace(_T("\r\n\r\n\r\n"), _T("\r\n\r\n"), true);
			// } while (count >= 1);
			tFile = regex_replace(tFile, wregex(_T("\r\n\r\n\r\n"), flags), _T("\r\n\r\n"));
		}
		
		//first line indent
		//theplugin.findandreplace(_T("^"), _T("　　"), true);
		//count = theplugin.findandreplace("^", "　　", true);
		tFile = regex_replace(tFile, wregex(_T("^"), flags), _T("　　"));
		//LOGINFO(_T("%d line was indented."), count);

	}
	catch (regex_error& e)
	{
		LOGINFO(_T("%s: %d."), e.what(), e.code());
	}

	//thePlugin.ReplaceSelection(tFile);
	thePlugin.SetDocument(tFile);

	ReparseCurrentFile();

	OnTrimContent();

	tString tFirstLine;

	thePlugin.GetLine(1, tFirstLine);

	StringTrimLeft(tFirstLine);
	StringTrimRight(tFirstLine);

	thePlugin.ReplaceLine(1, tFirstLine.c_str());
	
	LOGFUNEND;
}

//not for advanced index
void CContentsViewerDialog::OnTrimContent()
{
	LOGFUNBGN;
	if(true == m_bHeadIndexContent) return;
	tString tContent, tKeywrod;
	int line, pos = 0;

	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;
	//for (int iItem = 0; iItem <= m_pContentsLV->GetItemCount() - 1; iItem++)
	//intertr = m_mapContent.end();
	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++)
	//for (intertr++; intertr == m_mapContent.begin(); intertr-- )
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

		//tContent = m_pContentsLV->GetContentByItem(iItem);
		//line = m_pContentsLV->GetLineByItem(iItem);

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
			//tBank = tContent.substr(pos + 1, 1);
			//if (_Tcscmp(tBank.c_str(), _T(" ")) != 0)
			//	tContent.insert(pos + 1, 1, _T(' '));
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
		thePlugin.ReplaceLine(line, tContent.c_str());
	}
	LOGFUNEND;
}

//TO-DO:
void CContentsViewerDialog::OnGotoPrevPos()
{
	//int currentline = thePlugin.GetCurrentLine();
	////int line = m_vecLine.pop_up();

	////int iItem = m_pContentsLV->GetItemByLine(line);
	//m_pContentsLV->FocusItem(iItem);
	//
	//thePlugin.GotoLine(line);
	//
	//m_vecLine.push_back(currentline);

	//mToolBar.SetButtonState(IDM_NEXTPOS, 1);
}

//TO-DO:
void CContentsViewerDialog::OnGotoNextPos()
{
	//mToolBar.SetButtonState(IDM_PREVPOS, 0);
}

void CContentsViewerDialog::UpdateContentListview()
{
	//m_pContentsLV->SetRedraw(FALSE);
	m_pContentsLV->DeleteAllItems();
	
	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;
	int nItem = 0;
	tString tContentName;
	int line, level;
	bool IsFistLineIndexed = false;
	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++)
	{
		line = intertr->first;
		pContent = intertr->second;
		tContentName = pContent->tContent;
		level = pContent->nLevel;

		if (1 == line) IsFistLineIndexed = true; nItem++;
	
		m_pContentsLV->AddListViewItem(nItem, tContentName.c_str(), line, level);
		nItem++;
	}

	if (false == IsFistLineIndexed)
	{
		tString tFisrtLine;
		thePlugin.GetLine(1, tFisrtLine);
		m_pContentsLV->AddListViewItem(0, tFisrtLine.c_str(), 1, 1);
		//m_pContentsLV->SortItemsByLine();
		nItem++;
	}

	m_pContentsLV->AddListViewItem(nItem, _T("Last line!"), thePlugin.GetMaxLine(), 1);

	//m_pContentsLV->SetRedraw(TRUE);
}

void CContentsViewerDialog::FocuseContentListviewItemByLine(int line)
{
	int iItem = m_pContentsLV->GetItemByLine(line);
	m_pContentsLV->FocusItem(iItem);
}

void CContentsViewerDialog::ClearMapContent() {
	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;
	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++) {
		pContent = intertr->second;
		if(pContent != NULL) delete pContent;
	}
	m_mapContent.clear();
}

//TO-DO: wait to test
void CContentsViewerDialog::ListRepScript(){
	m_mapRepScript.clear();

	//TCHAR tszName[MAX_PATH]

	UINT curNewMenu = ID_NEWMENU + 1;
	//tString tNewMenu = _T("");
	
	//获取菜单
    //CMenu* pMenu = GetMenu()
	CMenu menu = GetMenu();
    //获取索引为2的菜单项，即第3列菜单
    //CMenu* pSubMenu = pMenu->GetSubMenu(2);
	CMenu subMenu = menu.GetSubMenu(2);

	//CIniFile* pScriptFile = new CIniFile();

	//find the first file
	_tfinddata64_t srcFileData;

	TCHAR searchPath[MAX_PATH];

	//_stprintf(searchPath, _T("%s%s"), m_tszAppPath, REP_SCRIPT_PATH);
	_tcscpy(searchPath, m_tszScriptPath);
	if (false == CheckPath(searchPath)) return;
	_tcscat(searchPath, _T("*.ini"));
	
	LOGINFO(_T("scriptPath: %s;\n"), searchPath);

	intptr_t hFile = _tfindfirst64(searchPath, &srcFileData);
	if (-1 != hFile)
	{
		//search all files
		do
		{
			if (_tcslen(srcFileData.name) == 1 && srcFileData.name[0] == _T('.') ||
				_tcslen(srcFileData.name) == 2 && srcFileData.name[0] == _T('.') && srcFileData.name[1] == _T('.')) {
				continue;
			}
				
			TCHAR newPath[MAX_PATH];
			_stprintf(newPath, _T("%s%s%s%s"), m_tszScriptPath, _T("\\"), srcFileData.name, _T("\\"));
			//if ((srcFileData.attrib & _A_SUBDIR) && (true == m_sFileMode.bIncludeSubDir))
			if (srcFileData.attrib & _A_SUBDIR)
			{
				// SyncFiles(srcPath1, desPath1, newPath2, ext);
				LOGINFO(_T("Found a folder: %s.\n"), newPath);
			}
			else
			{
				//tString tFileName = srcFileData.name;
				//LOGINFO(_T("Found a file: %s."), tFileName.c_str());
				//tFileName.erase(0, tNameExt.find_last_of(_T("\\")));
				//LOGINFO(_T("file's name: %s."), tFileName.c_str());

				TCHAR fileName[MAX_PATH];
				_tcscpy(fileName, srcFileData.name);
				LOGINFO(_T("Found a file: %s."), fileName);

				(_tcsrchr(fileName, _T('.')))[0] = 0;

				/*pScriptFile = Load(srcFileData.name);				
				pScriptFile->GetString(_T("Common"), _T("name"), tszName);*/

				//insert to map
				//m_mapRepScript[curNewMenu] = tFileName.c_str();
				m_mapRepScript[curNewMenu] = fileName;
				
				//pSubMenu->AppendMenu(MF_SEPARATOR); //分隔线
				//pSubMenu->AppendMenu(MF_STRING | MF_ENABLED, curNewMenu, tFileName.c_str());
				//pSubMenu->AppendMenu(MF_STRING | MF_ENABLED, curNewMenu, fileName);
				subMenu.AppendMenu(MF_STRING | MF_ENABLED, curNewMenu, fileName);
			}
		} while (_tfindnext64(hFile, &srcFileData) == 0);
	}

	//close search handle
	_findclose(hFile);
	
	//delete pScriptFile;
}

//Fix it: can't exec in regrex mode
void CContentsViewerDialog::ExecRepScript(UINT id)
{
	tString tFile = m_mapRepScript[id];
	
	TCHAR scriptFile[MAX_PATH];
	//_stprintf(scriptfile, _T("%s%s%s%s"), m_tszAppPath, REP_SCRIPT_PATH, tfile, _T(".ini"));
	_stprintf(scriptFile, _T("%s%s%s%s"), m_tszScriptPath, _T("\\"), tFile.c_str(), _T(".ini"));
	LOGINFO(_T("Start to replace : %s."), tFile.c_str());

	CIniFile* pScriptFile = new CIniFile(scriptFile);

	//pScriptFile->GetString(_T("Common"), _T("name"), tszName);
	bool isRegularMode = pScriptFile->GetBool(_T("Common"), _T("regrex"));
	
	int amount = pScriptFile->GetInteger(_T("Common"), _T("amount"));

	TCHAR tszFindItem[MAX_PATH], tszReplaceItem[MAX_PATH];
	TCHAR tszFindString[MAX_PATH], tszReplaceString[MAX_PATH];
	
	//thePlugin.GetSelection(tFile);
	thePlugin.GetDocument(tFile);

	for(int i = 1; i <= amount; i++)
	{
		_stprintf(tszFindItem, _T("%s%d"), _T("f"), i);
		_stprintf(tszReplaceItem, _T("%s%d"), _T("r"), i);
		
		pScriptFile->GetString(_T("Code"), tszFindItem, tszFindString);
		pScriptFile->GetString(_T("Code"), tszReplaceItem, tszReplaceString);

		//if (NULL == tszReplace) tszReplace = _T("");

		LOGINFO(_T("tFind：%s, tReplace：%s."), tszFindString, tszReplaceString);

		//FindAndReplace(tszFind, tszReplace);
		int count = StringReplaceAll(tFile, tszFindItem, tszReplaceItem, isRegularMode);

		LOGINFO(_T("%d was found and replaced."), count);
	}

	thePlugin.SetDocument(tFile);

	delete pScriptFile;
	pScriptFile = NULL;
}

//int CContentsViewerDialog::FindAndReplace(const TCHAR* tszFind, const TCHAR* tszReplace)
//{
//	bool bDebug = true;
//
//	tString tFile;
//
//	thePlugin.GetSelection(tFile);
//
//	//LOGINFO(_T("Selection:\n%s"), tFile.c_str());
//
//	int count = StringReplaceAll(tFile, tszFind, tszReplace);
//
//	LOGINFO(_T("%d was found and replaced."), count);
//
//	thePlugin.ReplaceSelection(tFile);
//	return count;
//}