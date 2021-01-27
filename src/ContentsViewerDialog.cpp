#include "ContentsViewerDialog.h"
#include "ContentsViewerPlugin.h"
#include "ContentsListView.h"
//#include "ContentsTreeView.h"
#include <regex>
#include <io.h>
#include <cstdlib>
//#include <fstream>
#include "Win32xx_860\include\wxx_commondlg.h"

//extern NPPData nppData;
extern FuncItem funcItem[2];
//extern CContentsViewerPlugin thePlugin;
//extern CLogFile* g_pLogFile;
//extern bool g_bDebug;
using namespace regex_constants;
using namespace std;
//using namespace Win32xx;

//#define REP_SCRIPT_PATH _T("\\plugins\\Config\\ContentsViewerPlugin\\Rep_Script\\")
//#define REP_SCRIPT_PATH _T("\\Rep_Script\\")
//#define INI_PATH _T("\\plugins\\Config\\ContentsViewerPlugin.ini")
#define INI_PATH _T("\\ContentsViewerPlugin.ini")

CContentsViewerDialog::CContentsViewerDialog() : CDialog(IDD_CVDLG)
{
	m_bDebug = true;

	m_pContentsLV = new CContentsListView();

	m_bHeadIndexContent = false;

	m_nMaxLevel = 0;
}

CContentsViewerDialog::~CContentsViewerDialog()
{
	//ClearmapContent();

	delete m_pContentsLV;
	delete m_pDocument;

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

	ReadCfg(tszIniPath);

	CreateToolbar();

	//m_pContentsLV->Attach(::GetDlgItem(m_hNppWnd, IDC_LV_CV));
	m_pContentsLV->Attach(GetDlgItem(IDC_LISTVIEW));
	// LVS_EX_INFOTIP enables tooltips
	// LVS_EX_LABELTIP ensures the full tooltip is shown
	//    and is not partially hidden
	m_pContentsLV->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_LABELTIP);
	m_pContentsLV->InsertColumn(0, _T("Contents"), LVCFMT_LEFT, 170);
	m_pContentsLV->InsertColumn(1, _T("Line"), LVCFMT_RIGHT, 50);

	m_ToolBar.SetButtonState(IDB_PREVPOS, 0);
	m_ToolBar.SetButtonState(IDB_NEXTPOS, 0);
	//m_ToolBar.SetButtonState(IDB_SORTLINE, 0);
	//m_ToolBar.SetButtonState(IDB_SORTNAME, 0);
	m_ToolBar.SetButtonState(IDB_VIEWLIST, 0);
	m_ToolBar.SetButtonState(IDB_VIEWTREE, 0);
	//m_ToolBar.SetButtonState(IDM_CLOSE, 1);

	//m_hScriptMenu = LoadMenu(thePlugin.GetInstanceHandle(), MAKEINTRESOURCE(IDM_SCRIPT));
	//m_hScriptMenu = GetSubMenu(m_hScriptMenu, 0);
	//assert(m_hScriptMenu);
	m_ScriptMenu.CreatePopupMenu();

	//m_hPyScriptMenu = LoadMenu(thePlugin.GetInstanceHandle(), MAKEINTRESOURCE(IDM_PYSCRIPT));
	//m_hPyScriptMenu = GetSubMenu(m_hPyScriptMenu, 0);
	//assert(m_hPyScriptMenu);
	m_PyScriptMenu.CreatePopupMenu();

	m_hExtraMenu = LoadMenu(thePlugin.GetInstanceHandle(), MAKEINTRESOURCE(IDM_EXTRA));
	m_hExtraMenu = GetSubMenu(m_hExtraMenu, 0);
	assert(m_hExtraMenu);

	m_hContentContextMenu = LoadMenu(thePlugin.GetInstanceHandle(), MAKEINTRESOURCE(IDM_CONTEXT_CONTENT));
	m_hContentContextMenu = GetSubMenu(m_hContentContextMenu, 0);
	assert(m_hContentContextMenu);

	OnSize(true);

	//int line = thePlugin.GetCurrentLine();
	int line = 1;
	//m_vecLine.push_back(line);
	m_ToolBar.SetButtonState(IDB_PREVPOS, 1);

	// list all RepScript and add it to submenu of "rep script"
	ListScripts();

	return TRUE;
}

int CContentsViewerDialog::CreateToolbar() 
{
	HWND hToolBar = m_ToolBar.CreateEx(
		0, //WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE, 
		TOOLBARCLASSNAME, _T(""), //WS_TABSTOP |
		//WS_VISIBLE | WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_LIST | TBSTYLE_EX_MIXEDBUTTONS,
		WS_VISIBLE | WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
		1, 1, 0, 0, GetHwnd(), NULL);
	assert(hToolBar);

	m_ToolBar.AddButton(IDB_PREVPOS);
	m_ToolBar.AddButton(IDB_NEXTPOS);
	m_ToolBar.AddButton(0); // separator
	m_ToolBar.AddButton(IDB_SCRIPT_MENU);
	m_ToolBar.AddButton(IDB_PYSCRIPT_MENU);
	m_ToolBar.AddButton(0); // separator
	m_ToolBar.AddButton(IDB_VIEWLIST);
	m_ToolBar.AddButton(IDB_VIEWTREE);
	m_ToolBar.AddButton(0); // separator
	m_ToolBar.AddButton(IDB_PARSE);
	m_ToolBar.AddButton(0); // separator
	m_ToolBar.AddButton(IDB_EXTRA_MENU);

	// m_ToolBar.SetImages(RGB(192,192,192), IDB_TOOLBAR, 0, 0);
	//m_ToolBar.AddBitmap(IDB_TOOLBAR);

	HINSTANCE hInstance = GetApp().GetInstanceHandle();
	COLORREF crMask = RGB(192, 192, 192);

	m_ImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 8, 0);
	HBITMAP hBitmap = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TOOLBAR));
	assert(hBitmap);
	m_ImageList.Add(hBitmap, crMask);
	m_ToolBar.SetImageList(m_ImageList);
	::DeleteObject(hBitmap);

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
		NMHDR* pnmh = (NMHDR*)lParam;
		if (pnmh->hwndFrom == m_hNPPWnd)
		{
			if (LOWORD(pnmh->code) == DMN_CLOSE)
			{
				::SendMessage(m_hNPPWnd, NPPM_SETMENUITEMCHECK, funcItem[0]._cmdID, FALSE);
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
	r = m_ToolBar.GetWindowRect();
	::MoveWindow(m_ToolBar, left, top, width - 2*left, r.Height(), TRUE);
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

	int line = thePlugin.GetCurLineNo();

	if((uMsg >= IDM_SCRIPT_FIRST)&& (uMsg <= IDM_SCRIPT_LAST))
	{
		OnScriptReplace(uMsg);
		thePlugin.GotoLine(line);
	}
	else if((uMsg >= IDM_PYSCRIPT_FIRST) && (uMsg <= IDM_PYSCRIPT_LAST))
	{
		OnPyScriptReplace(uMsg);
		thePlugin.GotoLine(line);
	}

	switch (uMsg)
	{
		case IDB_PREVPOS:
			OnGotoPrevPos();
			break;
			
		case IDB_NEXTPOS:
			OnGotoNextPos();
			break;

		case IDB_SCRIPT_MENU:
			OnScriptReplaceMenu();
			break;

		case IDB_PYSCRIPT_MENU:
			OnPyScriptReplaceMenu();
			break;

		case IDB_VIEWLIST:
			MessageBox(_T("IDB_VIEWLIST was clicked"), _T("Contents Viewer"), MB_OK);
			break;

		case IDB_VIEWTREE:
			MessageBox(_T("IDB_VIEWTREE was clicked"), _T("Contents Viewer"), MB_OK);
			break;

		case IDB_PARSE:
			ParseCurFile();
			UpdateContentListview();
			LOGOUT("CurLine: %d", line);
			FocuseContentListviewItemByLine(line);
			break;

		case IDB_EXTRA_MENU:
			OnExtraMenu();
			break;

		case IDM_HEAD_INDEX_CONTENTS:
			HeadIndexContents();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;

		case IDM_EXPORT_CONTENTS:
			ParseCurFile();
			UpdateContentListview();
			ExportContents();
			break;
		
		case IDM_IMPORT_CONTENTS:
			ImportContents();
			ParseCurFile();
			UpdateContentListview();
			break;
			
		case IDM_SC_INTO_NUMBER:
			//MessageBox(_T("IDM_SC_INTO_NUMBER was clicked"), _T("Contents Viewer"), MB_OK);
			if (true == m_bHeadIndexContent) HeadIndexContents();
			//ReParseCurFile();
			ParseCurFile();
			NumberContents();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;

		case IDM_SC_ALIGN_NUMBER:
			MessageBox(_T("IDM_SC_ALIGN_NUMBER was clicked"), _T("Contents Viewer"), MB_OK);
			break;
			
		case IDM_SC_TRIM_CONTENTS:
			if(true == m_bHeadIndexContent) HeadIndexContents();
			//ReParseCurFile();
			ParseCurFile();
			TrimContents();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;

		case IDM_SC_SORT_CONTENTS:
			MessageBox(_T("IDM_SC_SORT_PARAGRAPH was clicked"), _T("Contents Viewer"), MB_OK);
			break;

		case IDM_SC_DEL_DUPLICATECONTENTS:
			//ReParseCurFile();
			ParseCurFile();
			DelDuplicateContents();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;

		case IDM_SC_MARK_EMPTYCONTENTS:
			//ReParseCurFile();
			ParseCurFile();
			MarkEmptyContents();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;

		/*case IDM_REPLACE_WORDS :
			//MessageBox(_T("IDM_REPLACE_PINYINWORD was clicked"), _T("Contents Viewer"), MB_OK);
			OnExecuteReplace();
			ReParseCurFile();
			break;*/

		case IDM_BIG5_GBK:
			Big5ToGBK();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;

		case IDM_MERGE_PARAGRAPHS:
			MergeParagraphs();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;
			
		case IDM_ALIGN_PARAGRAPHS:
			AlignParagraphs();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;

		case IDM_DEL_CONTENTS:
			//m_pContentsLV->OnDelContent();
			m_pContentsLV->OnCutContents();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;

		case IDM_CUT_CONTENTS:
			m_pContentsLV->OnCutContents();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;

		case IDM_INSERT_CONTENTS_BEFORE:
			m_pContentsLV->OnInsetContentsBefore();
			//ReParseCurFile();
			ParseCurFile();
			UpdateContentListview();
			break;

		case IDM_SELECT_CONTENTS:
			//ReParseCurFile();
			//ParseCurFile();
			m_pContentsLV->OnSelectContents();
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
		//HINSTANCE hInstance = GetApp().GetInstanceHandle();
		LPNMTTDISPINFO lpDispInfo = (LPNMTTDISPINFO)lParam;
		//lpDispInfo->hinst = hInstance;
		UINT nID = lpDispInfo->hdr.idFrom;

		//lpDispInfo->lpszText = (LPTSTR)Win32xx::LoadString(nID).c_str();
		//lpDispInfo->lpszText = MAKEINTRESOURCE(nID);
		//TCHAR buffer[80];
		//::LoadString(hInstance, nID, buffer, 80);
		//lpDispInfo->lpszText = buffer;
		_tcscpy(lpDispInfo->szText, Win32xx::LoadString(nID).c_str());
	}
	break;

	case LVN_GETINFOTIP:
	{
		LPNMLVGETINFOTIP lpNmlvgetinfotip = (LPNMLVGETINFOTIP)lParam;
		int iItem = lpNmlvgetinfotip->iItem;
		int line = m_pContentsLV->GetLineByItem(iItem);
		_stprintf(lpNmlvgetinfotip->pszText, _T("%d"), line);
	}
	break;

	case LVN_COLUMNCLICK:
		//SetSortMode((eTagsSortMode) (TWM_NAME + ((LPNMLISTVIEW) lParam)->iSubItem));
		break;

	case LVN_ITEMACTIVATE:
		break;

	case TVN_SELCHANGED:
		break;

	case NM_DBLCLK:
		break;

	case NM_CLICK:
		if (IDC_LISTVIEW == LOWORD(wParam))
			OnListViewItemClicked();
		break;

	case NM_RCLICK:
		if (IDC_LISTVIEW == LOWORD(wParam))
			//NMHDR* pNMHDR, LRESULT* pResult
			OnListViewItemRClicked((NMHDR*)lParam, 0);
		break;
	}

	return 0;
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

	//m_ToolBar.SetButtonState(IDM_NEXTPOS, 1);
}

//TO-DO:
void CContentsViewerDialog::OnGotoNextPos()
{
	//m_ToolBar.SetButtonState(IDM_PREVPOS, 0);
}

void CContentsViewerDialog::OnScriptReplaceMenu()
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	//::TrackPopupMenu(m_hScriptMenu, TPM_LEFTBUTTON, point.x, point.y, 0, this->GetHwnd(), NULL);
	m_ScriptMenu.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, this->GetHwnd(), NULL);
}

void CContentsViewerDialog::OnPyScriptReplaceMenu()
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	//::TrackPopupMenu(m_hPyScriptMenu, TPM_LEFTBUTTON, point.x, point.y, 0, this->GetHwnd(), NULL);
	m_PyScriptMenu.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, this->GetHwnd(), NULL);
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

	TrackPopupMenu(m_hContentContextMenu, TPM_LEFTBUTTON, point.x, point.y, 0, this->GetHwnd(), NULL);

	*pResult = 0;
}

void CContentsViewerDialog::OnExtraMenu()
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	TrackPopupMenu(m_hExtraMenu, TPM_LEFTBUTTON, point.x, point.y, 0, this->GetHwnd(), NULL);
}

//
void CContentsViewerDialog::HeadIndexContents()
{
	m_bHeadIndexContent = !m_bHeadIndexContent;
	if(true == m_bHeadIndexContent)
	{
		CheckMenuItem(m_hExtraMenu, IDM_HEAD_INDEX_CONTENTS, MF_BYCOMMAND | MF_CHECKED);
	}
	else
	{
		CheckMenuItem(m_hExtraMenu, IDM_HEAD_INDEX_CONTENTS, MF_BYCOMMAND | MF_UNCHECKED);
	}
}

//TO-DO: test
void CContentsViewerDialog::ExportContents()
{
	Win32xx::CFileDialog myDlg(FALSE, NULL, NULL, 0, _T("Text|*.txt||"));

	if(myDlg.DoModal()!=IDOK) return;
	
	tString tContentPath = myDlg.GetPathName();

	/*FILE* pContentFile = NULL;

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
	pContentFile = NULL;*/

	m_pDocument->ExportContents(tContentPath);
}

//TO-DO: test
void CContentsViewerDialog::ImportContents()
{
	CFileDialog myDlg(TRUE, NULL, NULL, 0, _T("Text|*.txt||"));
	
	if(myDlg.DoModal()!=IDOK) return;
		
	tString tContentPath = myDlg.GetPathName();

	/*FILE* contentFile = NULL;

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
	contentFile = NULL;*/

	m_pDocument->ImportContents(tContentPath);
}

//wait to test
//2. align number
//Bug: sometimes lose "1" in "12" and so on
void CContentsViewerDialog::NumberContents()
{
	/*LOGFUNBGN;
	//if(true == m_bAdvancedIndexContent) return;
	tString tContent, tNumber;
	int line, lastpos;
	size_t pos;
	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;
	//for (int iItem = 0; iItem <= m_pContentsLV->GetItemCount() - 1; iItem++)
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
			thePlugin.ReplaceLine(line, tContent.c_str());
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
		thePlugin.ReplaceLine(line, tContent.c_str());
	}

	LOGFUNEND;*/

	m_pDocument->NumberContents();
}

//wait to add
void CContentsViewerDialog::DelDuplicateContents()
{
	/*LOGFUNBGN;
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
			thePlugin.DelLine(line);
			LOGINFO(_T("tItemText1: %s = tItemText2: %s"), tItemText1.c_str(), tItemText2.c_str());
		}			
	} 
	LOGFUNEND;*/
	m_pDocument->DelDuplicateContents();
}

//wait to add function: 
//1. excusive level 2 up content,  ok
//2. Delete not empty content's empty mark
//3. print a to-do list
void CContentsViewerDialog::MarkEmptyContents()
{
	/*LOGFUNBGN;

	if (true == m_bHeadIndexContent) return;

	ReParseCurFile();

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
			thePlugin.ReplaceLine(line1, tItemText1.c_str());
		}
	} 
	LOGFUNEND;*/

	m_pDocument->MarkEmptyContents();
}

//wait to test
void CContentsViewerDialog::Big5ToGBK()
{
	/*LOGFUNBGN;
	tString tBig5Line, tGBKLine;
	int cchDest;
	WORD wLanguageID = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
	LCID Locale = MAKELCID(wLanguageID, SORT_CHINESE_PRC);
	for(int line = 1; line <= thePlugin.GetLineCount(); line ++)
	{
		cchDest = thePlugin.GetLine(line, tBig5Line);
		tGBKLine.resize(tBig5Line.length());
		LCMapString(Locale, LCMAP_SIMPLIFIED_CHINESE, tBig5Line.c_str(), -1, const_cast<TCHAR*>(tGBKLine.c_str()), tBig5Line.length());
		thePlugin.ReplaceLine(line, tGBKLine.c_str());
	}
	LOGFUNEND;*/

	m_pDocument->Big5ToGBK();
}

//bug fix: can't remove the bank in the paragraph.
void CContentsViewerDialog::MergeParagraphs()
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
	
	//thePlugin.ReplaceSelection(tSel);


	// 表达式选项 
	syntax_option_type flags = ECMAScript | icase;

	try
	{
		tSel = regex_replace(tSel, wregex(_T("\r\n\r\n"), flags), _T("@"));
		//int count = StringRegReplaceAll(tFile, wregex(_T("\r\n\r\n"), flags), _T("@"));
		tSel = regex_replace(tSel, wregex(_T("\r\n"), flags), _T(""));
		//count = StringRegReplaceAll(tFile, wregex(_T("\r\n"), flags), _T(""));
		tSel = regex_replace(tSel, wregex(_T("@"), flags), _T("\r\n\r\n"));
		//count = StringRegReplaceAll(tSel, wregex(_T("@"), flags), _T("\r\n\r\n"));

		//tSel = regex_replace(tSel, wregex(_T("　　")), _T(""), match_default | match_not_bol);
		//tSel = regex_replace(tSel, wregex(_T("\\B　　"), flags), _T(""));
		tSel = regex_replace(tSel, wregex(_T("^　　"), flags), _T("@"));
		tSel = regex_replace(tSel, wregex(_T("　　"), flags), _T(""));
		tSel = regex_replace(tSel, wregex(_T("^@"), flags), _T("　　"));
	}
	catch (regex_error& e)
	{
		LOGINFO(_T("%s: %d."), e.what(), e.code());
	}
	
	//thePlugin.SetDocument(tSel);
	thePlugin.ReplaceSelection(tSel);
	*/

	m_pDocument->MergeParagraphs();

	LOGFUNEND;
}

//wait to test
void CContentsViewerDialog::AlignParagraphs()
{
	LOGFUNBGN;

	/*tString tFile;

	//thePlugin.GetSelection(tFile);
	thePlugin.GetDocument(tFile);

	// 表达式选项 
	syntax_option_type flags = ECMAScript;

	try
	{
		// int count = 0;

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

	ReParseCurFile();

	OnTrimContent();

	tString tFirstLine;

	thePlugin.GetLine(1, tFirstLine);

	StringTrimLeft(tFirstLine);
	StringTrimRight(tFirstLine);

	thePlugin.ReplaceLine(1, tFirstLine.c_str());
	
	LOGFUNEND;*/

	m_pDocument->AlignParagraphs();
}

//not for advanced index
void CContentsViewerDialog::TrimContents()
{
	/*LOGFUNBGN;
	if(true == m_bHeadIndexContent) return;
	tString tContent, tKeywrod;
	int line;
	size_t pos = 0;

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
	LOGFUNEND;*/

	m_pDocument->TrimContents();
}

//wait to test
void CContentsViewerDialog::OnScriptReplace(size_t id)
{
	tString tFile = m_mapScript[id];

	TCHAR scriptFile[MAX_PATH];

	_stprintf(scriptFile, _T("%s%s%s%s"), m_tszScriptPath, _T("\\"), tFile.c_str(), _T(".jb"));

	/*CIniFile* pScriptFile = new CIniFile(scriptFile);

	//pScriptFile->GetString(_T("Common"), _T("name"), tszName);
	bool isRegularMode = pScriptFile->GetBool(_T("Common"), _T("regrex"));
	
	int amount = pScriptFile->GetInteger(_T("Common"), _T("amount"));

	TCHAR tszFindItem[MAX_PATH], tszReplaceItem[MAX_PATH];
	TCHAR tszFindString[MAX_PATH], tszReplaceString[MAX_PATH];
	
	tString tDocument;
	//thePlugin.GetSelection(tFile);
	thePlugin.GetDocument(tDocument);

	//LOGINFO(_T("m_TszAppPath: %s."), m_tszAppPath);
	//_stprintf(tszRepPath, _T("%s%s%s"), m_tszAppPath, _T("\\plugins\\Config\\"), tszRepFile);

	LOGINFO(_T("Start to exec replace script : %s."), scriptfile);

	for(int i = 1; i <= amount; i++)
	{
		_stprintf(tszFindItem, _T("%s%d"), _T("f"), i);
		_stprintf(tszReplaceItem, _T("%s%d"), _T("r"), i);
		
		pScriptFile->GetString(_T("Code"), tszFindItem, tszFindString);
		pScriptFile->GetString(_T("Code"), tszReplaceItem, tszReplaceString);

		//if (NULL == tszReplace) tszReplace = _T("");

		LOGINFO(_T("tFind：%s, tReplace：%s."), tszFindString, tszReplaceString);

		//FindAndReplace(tszFind, tszReplace);
		int count = StringReplaceAll(tDocument, tszFindString, tszReplaceString, isRegularMode);

		LOGINFO(_T("%d was found and replaced."), count);
	}

	thePlugin.SetDocument(tDocument);

	delete pScriptFile;
	pScriptFile = NULL;*/

	tString tDocument;
	thePlugin.GetDocument(tDocument);
	m_pDocument->SetDocument(tDocument);
	m_pDocument->ScriptReplace(scriptFile);
	m_pDocument->GetDocument(tDocument);
	thePlugin.SetDocument(tDocument);
}

void CContentsViewerDialog::OnPyScriptReplace(size_t id)
{
	tString tFile = m_mapScript[id];

	TCHAR scriptFile[MAX_PATH];

	_stprintf(scriptFile, _T("%s%s%s%s"), m_tszScriptPath, _T("\\"), tFile.c_str(), _T(".py"));

	tString tDocument;
	thePlugin.GetDocument(tDocument);
	m_pDocument->SetDocument(tDocument);
	m_pDocument->PyScriptReplace(scriptFile);
	m_pDocument->GetDocument(tDocument);
	thePlugin.SetDocument(tDocument);
}

int CContentsViewerDialog::AddContent(int nItem, const TCHAR* tszContentName, int line, int level, const TCHAR* tszKeyword)
{
	LOGFUNBGN;

	LOGINFO(_T("CVD AddContent: %s, Level : %d, Line: %d"), tszContentName, level, line);

	//int maxlen = 16;
	
	//TCHAR tszContent[256];

	/*if (m_mapContent.end() == m_mapContent.find(line))
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
	return -1;*/

	return m_pDocument->AddContent(nItem, tszContentName, line, level, tszKeyword);
}

size_t CContentsViewerDialog::ParseCurFile()
{
	thePlugin.UpdateCurFileCodePage();
	m_nMaxLevel = m_pDocument->Parse(m_bHeadIndexContent);
	return m_nMaxLevel;
}

size_t CContentsViewerDialog::ReParseCurFile()
{
	LOGFUNBGN;
	int totalNum = 0;

	/*thePlugin.UpdateCurFileCodePage();

	m_pContentsLV->SetRedraw(FALSE);
	m_pContentsLV->DeleteAllItems();
	//m_pContentsLV->SetRedraw(TRUE);

	ClearMapContent();

	//tString tDocument;
	//thePlugin.GetDocument(tDocument);

	if(false == m_bHeadIndexContent)
	{
		//LOGINFO(_T("Start to index 卷.\n"));
		//int level = 1;
		//tString tText = _T("^[ 　]*第[^ 　]{0, 14}卷.*");
		//LOGINFO(tText.c_str());
		//int i = thePlugin.IndexContents(tText.c_str(), level);
		//if (i > 0) level++;
		//LOGINFO(_T("End to index 卷.\n"));

		//LOGINFO(_T("Start to index 集.\n"));
		//int pos = tText.find(_T("卷"));
		//tText = tText.replace(pos, 1, _T("集"));
		//LOGINFO(tText.c_str());
		//i = thePlugin.IndexContents(tText.c_str(), level);
		//if (i > 0) level++;
		//LOGINFO(_T("End to index 集.\n"));
		//
		//LOGINFO(_T("Start to index 章.\n"));
		//tText = tText.replace(pos, 1, _T("章"));
		//LOGINFO(tText.c_str());
		//i = thePlugin.IndexContents(tText.c_str(), level);
		//if (i > 0) level++;
		//LOGINFO(_T("End to index 章.\n"));
		//
		//LOGINFO(_T("Start to index 回.\n"));
		//tText = tText.replace(pos, 1, _T("回"));
		//LOGINFO(tText.c_str());
		//i = thePlugin.IndexContents(tText.c_str(), level);
		//LOGINFO(_T("End to index 回.\n"));
		//
		//LOGINFO(_T("Start to sort.\n"));
	
		//tString tlevelItem;
		//TCHAR tszKeyword[40], X[MAX_PATH], tszLevelItem[20];
		//tString tX;

		//int totalLevel = m_pIniFile->GetInteger(_T("Content"), _T("totallevel"));
		//int CountX = m_pIniFile->GetInteger(_T("Content"), _T("CountX"));
		//m_pIniFile->GetString(_T("Content"), _T("X"), X);
		//tX = AtoT(X, CP_ACP);

		size_t totalLevel = _tcsclen(m_tszKeyword);
		LOGINFO(_T("totalLevel: %d, CountX: %d, X: %s"), totalLevel, m_uCountX, m_tszNum);
		TCHAR tt[10];
		_stprintf(tt, _T("%d"), m_uCountX*2);

		int level = 1;
		int num = 0;
		//_stprintf(tszText, _T("^[ 　]*第[%s]{0, %d}卷.*"), X, CountX*2);
		//tString tText = _T("^[ 　\t]*第[");
		tString tText = _T("^[ 　\\t]*?第[");
		//tText = tText + X;
		tText = tText + m_tszNum;
		//tText = tText + L"]{0,";
		tText = tText + _T("]{0,");
		tText = tText + tt;
		//tText = tText + L"}卷.*";
		tText = tText + _T("}卷.*");
		LOGINFO(_T("tText: %s"), tText.c_str());
		//MessageBoxW(tText.c_str(), tText.c_str(), MB_OK);
		//size_t pos = tText.find(L"卷");	
		size_t pos = tText.find(_T("卷"));
		LOGINFO(_T("pos: %d"), pos);
		LOGINFO(_T("totalLevel: %d"), totalLevel);

		tString tkeyWord;
		for (int i = 1; i <= totalLevel; i ++)
		{
			//_stprintf(tszLevelItem, _T("%s%d"), _T("level"), i);
			//m_pIniFile->GetString(_T("Content"), tszLevelItem, tszKeyword);
			tkeyWord = m_tszKeyword[i - 1];
			//LOGINFO(_T("tszLevelItem: %s, keywrod: %s."), tszLevelItem, tszKeyword);
			LOGINFO(_T("tszLevelItem: %d, keywrod: %s."), i, tkeyWord.c_str());

			//tText = tText.replace(pos, 1, tszKeyword);
			tText = tText.replace(pos, 1, tkeyWord.c_str());
			LOGINFO(_T("tText: %s"), tText.c_str());
			if(num > 0) level++;

			//num = thePlugin.IndexContents(tDocument, tText.c_str(), tszKeyword, level);
			//num = thePlugin.IndexContents(tText.c_str(), tszKeyword, level);
			num = thePlugin.IndexContents(tText.c_str(), tkeyWord.c_str(), level);

			if (num > 0) m_nMaxLevel = level;
			totalNum = totalNum + num;
			//LOGINFO(_T("End to index %s.\n"), tszKeyword);
			LOGINFO(_T("End to index %s.\n"), tkeyWord.c_str());
		}
	}
	else
	{
		//totalnum = thePlugin.IndexContents(tDocument, _T("^[^ 　\\t\\r\\n].+"), _T("回"), 1);
		totalNum = thePlugin.IndexContents(_T("^[^ 　\\t\\r\\n].+"), _T(""), 1);
		LOGINFO(_T("Head index content."));
	}

	if (totalNum >= 1)
	{
		m_pContentsLV->SortItemsByLine();

		UpdateContentListview();

		int line = thePlugin.GetCurLineNo();
		FocuseContentListviewItemByLine(line);

		//int line = thePlugin.GetLastLine();
		//if(line > 30)
		//	thePlugin.GotoLine(line);
		//m_pContentsLV->FocusLastItem();
		//ActiveContentListviewItem();
	}

	m_pContentsLV->SetRedraw(TRUE);

	LOGFUNEND;*/

	return totalNum;
}

//wait to finish
void CContentsViewerDialog::SavePos(int line)
{
	line = thePlugin.GetCurLineNo();
	//m_vecLine.push_back(line);
	m_ToolBar.SetButtonState(IDB_PREVPOS, 1);
}

void CContentsViewerDialog::UpdateContentListview()
{
	//m_pContentsLV->SetRedraw(FALSE);
	m_pContentsLV->DeleteAllItems();
	
	//map<int, ContentData*>::iterator intertr;
	mapContent_t::iterator intertr;
	mapContent_t content;
	m_pDocument->GetContent(content);
	ContentData* pContent = NULL;
	int nItem = 0;
	tString tContentName;
	int line, level;
	bool IsFistLineIndexed = false;
	//for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++)
	for(intertr = content.begin(); intertr != content.end(); intertr++)
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

	m_pContentsLV->AddListViewItem(nItem, _T("Last line!"), thePlugin.GetLineCount(), 1);

	//m_pContentsLV->SetRedraw(TRUE);
}

void CContentsViewerDialog::FocuseContentListviewItemByLine(int line)
{
	int iItem = m_pContentsLV->GetItemByLine(line);
	m_pContentsLV->FocusItem(iItem);
}

void CContentsViewerDialog::ClearMapContent()
{
	map<int, ContentData*>::iterator intertr;
	ContentData* pContent = NULL;
	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++) {
		pContent = intertr->second;
		if(pContent != NULL) delete pContent;
	}
	m_mapContent.clear();
}

void CContentsViewerDialog::ReadCfg(const TCHAR* cfgFile)
{
	CIniFile* pIniFile = new CIniFile(cfgFile);

	TCHAR tszVer[MAX_PATH];
	pIniFile->GetString(_T("Common"), _T("version"), tszVer);

	//g_bDebug = pIniFile->GetBool(_T("Debug"), _T("Enable"));
	g_bDebug = true;

	if (g_bDebug)
	{
		TCHAR tszLogPath[MAX_PATH], tszLogFullPath[MAX_PATH];
		pIniFile->GetString(_T("Debug"), _T("file"), tszLogPath);
		_stprintf(tszLogFullPath, _T("%s%s"), m_tszCfgPath, tszLogPath);
		g_pLogFile->StartLog(tszLogFullPath);
		LOGINFO(_T("Ver: %s"), tszVer);
		LOGINFO(_T("LogDir: %s"), tszLogFullPath);
	}

	size_t ulineBetweenParagraphs = pIniFile->GetInteger(_T("Style"), _T("lineBetweenParagraphs"));

	TCHAR tszNum[MAX_PATH];
	pIniFile->GetString(_T("Content"), _T("X"), tszNum);
	size_t uCountX = pIniFile->GetInteger(_T("Content"), _T("CountX"));

	LOGINFO(_T("CountX: %d, X: %s"), uCountX, tszNum);

	TCHAR tszKeyword[40];
	pIniFile->GetString(_T("Content"), _T("Level"), tszKeyword);
	LOGINFO(_T("keywrod: %s."), tszKeyword);

	m_pDocument = new CDocument();
	m_pDocument->SetParameters(ulineBetweenParagraphs, tszNum, uCountX, tszKeyword);

	TCHAR scriptPath[MAX_PATH];
	pIniFile->GetString(_T("Script"), _T("path"), scriptPath);
	_stprintf(m_tszScriptPath, _T("%s%s"), m_tszCfgPath, scriptPath);

	delete pIniFile;
}

//TO-DO: wait to test
void CContentsViewerDialog::ListScripts()
{
	m_mapScript.clear();

	//TCHAR tszName[MAX_PATH]

	size_t nScriptMenuID = IDM_SCRIPT_FIRST;
	size_t nPyScriptMenuID = IDM_PYSCRIPT_FIRST;
	//tString tNewMenu = _T("");

	//获取菜单
    //CMenu menu = GetMenu();
    //获取索引为2的菜单项，即第3列菜单
    // CMenu subMenu = mHFunMenu.GetSubMenu(2);
	//HMENU scriptHMenu = GetSubMenu(mHFunMenu, 2);
	//HMENU pyScriptHMenu = GetSubMenu(mHFunMenu, 2);

	//if (FALSE == CheckPath(m_tszScriptPath))
	//{
	//	LOGERR(_T("%s dosn't exist!"), m_tszScriptPath);
	//	return;
	//}

	TCHAR searchPath[MAX_PATH];
	_tcscpy(searchPath, m_tszScriptPath);
	_tcscat(searchPath, _T("\\*.*"));

	LOGINFO(_T("scriptPath: %s;\n"), searchPath);

	//find the first file
	_tfinddata64_t srcFileData;
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
			_stprintf(newPath, _T("%s%s%s%s"), searchPath, _T("\\"), srcFileData.name, _T("\\"));
			if (srcFileData.attrib & _A_SUBDIR)
			{
				LOGINFO(_T("Found a folder: %s.\n"), newPath);
			}
			else
			{
				// tString tFileName = srcFileData.name;
				TCHAR fileName[MAX_PATH];
				_tcscpy(fileName, srcFileData.name);
				LOGINFO(_T("Found a file: %s."), fileName);

				//pScriptFile = Load(srcFileData.name);				
				//pScriptFile->GetString(_T("Common"), _T("name"), tszName);

				//tNameExt.erase(0, tNameExt.find_last_of(_T(".")));
				//tFileName.erase(0, tNameExt.find_last_of(_T("\\")));

				//(_tcsrchr(fileName, _T('.')))[0] = 0;

				//ext
				//full_name.rfind(".") == string.npos ? full_name : full_name.substr(full_name.rfind(".")+1);

				// TCHAR *ext = _tcsrchr(fileName, _T('.'));
				// if (ext)
				// {    
					// *ext = _T('\0');    
					// ext++;
				// }

				TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fName[_MAX_FNAME], ext[_MAX_EXT];
				_tsplitpath(fileName, drive, dir, fName, ext);				
				LOGINFO(_T("Script name: %s, Ext: %s"), fName, ext);

				if(0 == _tcscmp(ext, _T(".jb")))
				{
					//pSubMenu->AppendMenu(MF_SEPARATOR); //分隔线
					//subMenu.AppendMenu(MF_STRING| MF_ENABLED, curNewMenu, fileName);
					nScriptMenuID++;
					//insert to map
					//m_mapScript[scriptMenu] = fName;
					m_mapScript.insert(pair<int, tString>(nScriptMenuID, fName));
					//::AppendMenu(m_hScriptMenu, MF_STRING| MF_ENABLED, scriptMenu, fName);
					m_ScriptMenu.AppendMenuW(MF_STRING | MF_ENABLED, nScriptMenuID, fName);
				}
				else if(0 == _tcscmp(ext, _T(".py")))
				{
					nPyScriptMenuID++;
					//insert to map
					//m_mapScript[pyScriptMenu] = fName;
					m_mapScript.insert(pair<int, tString>(nPyScriptMenuID, fName));
					//AppendMenu(m_hPyScriptMenu, MF_STRING| MF_ENABLED, pyScriptMenu, fName);
					m_PyScriptMenu.AppendMenuW(MF_STRING | MF_ENABLED, nPyScriptMenuID, fName);
				}
			}
		} while(_tfindnext64(hFile, &srcFileData) == 0);
	}

	//close search handle
	_findclose(hFile);
	
	//delete pScriptFile;
}

/*void CContentsViewerDialog::OnExecuteReplace()
{
	LOGFUNBGN;

	//bool bDebug = true;

	tString tFile;

	//thePlugin.GetSelection(tFile);
	thePlugin.GetDocument(tFile);

	//LOGINFO(_T("Selection:\n%s"), tFile.c_str());

	TCHAR tszRepFile[MAX_PATH], tszRepPath[MAX_PATH];

	m_pIniFile->GetString(_T("Script"), _T("stdFile"), tszRepFile);

	_stprintf(tszRepPath, _T("%s%s"), m_tszScriptPath, tszRepFile);

	// bool isRegularMode = m_pIniFile->GetBool(_T("Replace"), _T("RegularMode"));
	
	FILE* repFile = NULL;
	//repFile = _tfopen(tfile, _T("r"));
	repFile = _tfopen(tszRepPath, _T("r"));
	assert(repFile);

	//LOGINFO(_T("Start to replace : %s"), tfile);
	LOGINFO(_T("Start to replace : %s."), tszRepPath);

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
}*/

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