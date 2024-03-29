// ContentsViewerDialog.cpp
#include "ContentsViewerDialog.h"
#include "ContentsViewer.h"
#include "ContentsListView.h"
//#include "ContentsTreeView.h"
#include "ContentsViewerMsgs.h"
#include <regex>
#include <io.h>
#include <cstdlib>
//#include <fstream>
#include "..\third_party\Win32xx_860\include\wxx_commondlg.h"
// #include "InputBox.h"
#include "..\third_party\Win32InputBox.h"

//extern CContentsViewer thePlugin;
//extern CLogFile* g_pLogFile;
//extern bool g_bDebug;
using namespace regex_constants;
using namespace std;
//using namespace Win32xx;

#define INI_PATH L"\\ContentsViewerPlugin.ini"
#define PLUGIN_PATH L"\\plugins\\ContentsViewerPlugin\\"

CContentsViewerDialog::CContentsViewerDialog() : CDialog(IDD_CVDLG)
{
	m_bDebug = true;

	m_pIniFile = nullptr;
	m_pDocument = nullptr;
	m_pContentsLV = nullptr;

	m_nMaxLevel = 0;
	m_stIndexType = NormIndexTyp;

	m_pScriptPlugin = nullptr;
}

CContentsViewerDialog::~CContentsViewerDialog()
{
	m_mapScriptMenu.clear();

	if (nullptr != m_pIniFile)
		delete m_pIniFile;
	if (nullptr != m_pDocument)
		delete m_pDocument;
	if (nullptr != m_pContentsLV)
		delete m_pContentsLV;

	if (nullptr != m_pScriptPlugin)
		delete m_pScriptPlugin;
}

BOOL CContentsViewerDialog::OnInitDialog()
{
	//InitOptions();
	wchar_t wszTemp[MAX_PATH];

	wchar_t wszCfgPath[MAX_PATH];
	wchar_t wszIniPath[MAX_PATH];

	// _tcscpy(tszCfgPath, thePlugin.GetPluginsConfigDir());
	// _tcscat(tszCfgPath, L"\\ContentsViewerPlugin"));
	wchar_t wszPluginsCfgDir[MAX_PATH];
	thePlugin.GetPluginsConfigDir(wszPluginsCfgDir);
	_swprintf(wszCfgPath, L"%s%s", wszPluginsCfgDir, L"\\ContentsViewerPlugin");
	//_stprintf(tszCfgPath, L"%s%s"), thePlugin.GetPluginsConfigDir(), L"\\ContentsViewerPlugin"));

	//GetModuleFileName(NULL, m_tszAppPath, MAX_PATH); //得到当前模块路径
	//(_tcsrchr(m_tszAppPath, L'\\')))[0] = 0;
	//LOGINFO(L"AppDir: %s"), m_tszAppPath);

	_swprintf(wszIniPath, L"%s%s", wszCfgPath, INI_PATH);

	m_pIniFile = new CIniFile(wszIniPath);

	wchar_t wszVer[MAX_PATH];
	m_pIniFile->GetString(L"Common", L"version", wszVer);

	g_bDebug = m_pIniFile->GetBool(L"Debug", L"Enable");

	if (g_bDebug)
	{
		wchar_t wszLogPath[MAX_PATH], wszLogFullPath[MAX_PATH];
		m_pIniFile->GetString(L"Debug", L"file", wszLogPath);
		swprintf(wszLogFullPath, L"%s%s", wszCfgPath, wszLogPath);
		g_pLogFile->StartLog(wszLogFullPath);
		LOGINFO(L"Ver: %s", wszVer);
		LOGINFO(L"LogDir: %s", wszLogFullPath);
	}

	size_t ulineBetweenParagraphs = m_pIniFile->GetInteger(L"Style", L"lineBetweenParagraphs");

	wchar_t wszNum[MAX_PATH];
	m_pIniFile->GetString(L"Content", L"X", wszNum);
	size_t uCountX = m_pIniFile->GetInteger(L"Content", L"CountX");

	LOGINFO(L"CountX: %d, X: %s", uCountX, wszNum);

	wchar_t wszKeyword[40];
	m_pIniFile->GetString(L"Content", L"Level", wszKeyword);
	LOGINFO(L"wszKeyword: %s.", wszKeyword);

	m_pDocument = new CDocument();
	m_pDocument->SetParameters(ulineBetweenParagraphs, wszNum, uCountX, wszKeyword);

	CreateToolbar();

	m_ToolBar.SetButtonState(IDB_PREVPOS, 0);
	m_ToolBar.SetButtonState(IDB_NEXTPOS, 0);
	//m_ToolBar.SetButtonState(IDB_SORTLINE, 0);
	//m_ToolBar.SetButtonState(IDB_SORTNAME, 0);
	m_ToolBar.SetButtonState(IDB_VIEW_LIST, 0);
	m_ToolBar.SetButtonState(IDB_VIEW_TREE, 0);
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

	m_pContentsLV = new CContentsListView();
	//m_pContentsLV->Attach(::GetDlgItem(m_hNppWnd, IDC_LV_CV));
	m_pContentsLV->Attach(GetDlgItem(IDC_LISTVIEW));
	// LVS_EX_INFOTIP enables tooltips
	// LVS_EX_LABELTIP ensures the full tooltip is shown
	//    and is not partially hidden
	m_pContentsLV->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_LABELTIP);
	m_pContentsLV->InsertColumn(0, L"Contents", LVCFMT_LEFT, 170);
	m_pContentsLV->InsertColumn(1, L"Line", LVCFMT_RIGHT, 50);

	m_hContentContextMenu = LoadMenu(thePlugin.GetInstanceHandle(), MAKEINTRESOURCE(IDM_CONTEXT_CONTENT));
	m_hContentContextMenu = GetSubMenu(m_hContentContextMenu, 0);
	assert(m_hContentContextMenu);

	//int line = thePlugin.GetCurrentLine();
	int line = 1;
	//m_vecLine.push_back(line);
	m_ToolBar.SetButtonState(IDB_PREVPOS, 1);

	wchar_t wszScriptPath[MAX_PATH];
	wchar_t wszScriptExt[MAX_PATH];

	m_pIniFile->GetString(L"Script", L"path", wszScriptPath);
	swprintf(m_wszScriptPath, L"%s%s", wszCfgPath, wszScriptPath);
	// m_pIniFile->GetString(L"Script"), L"ScriptpluginExt"), wszScriptExt);
	// ListScripts(m_wszScriptPath, wszScriptExt);

	// int nScriptPluginNum = m_pIniFile->GetInteger(L"Script"), L"scriptPluginNum"));

	wchar_t wszPluginPath[MAX_PATH];
	wchar_t wszScriptPluginDLL[MAX_PATH];

	bool bLoadScriptPlugin = m_pIniFile->GetBool(L"Script", L"LoadScriptPlugin");
	if (true == bLoadScriptPlugin)
	{
		/*GetModuleFileName(NULL, wszPluginPath, MAX_PATH); //得到当前模块路径
		(_tcsrchr(wszPluginPath, L'\\')))[0] = 0;*/

		thePlugin.GetPluginsDir(wszPluginPath);
		LOGINFO(L"thePluginPath: %s", wszPluginPath);

		m_pIniFile->GetString(L"Script", L"ScriptPluginDLL", wszTemp);
		_swprintf(wszScriptPluginDLL, L"%s%s%s", wszPluginPath, PLUGIN_PATH, wszTemp);
		LOGINFO(L"Script Plugin DLL: %s", wszScriptPluginDLL);

		m_pScriptPlugin = new CScriptPlugin();
		m_pScriptPlugin->Load(wszScriptPluginDLL);

		m_pIniFile->GetString(L"Script", L"ScriptPluginExt", wszScriptExt);
		// ListScripts(wszScriptPath, wszScriptExt);
	}

	// list all RepScript and add it to submenu of "rep script"
	ListScripts(m_wszScriptPath);

	return FALSE;
}

BOOL CContentsViewerDialog::CreateToolbar()
{
	HWND hToolBar = m_ToolBar.CreateEx(
		0,					   //WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE,
		TOOLBARCLASSNAME, L"", //WS_TABSTOP |
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
	m_ToolBar.AddButton(IDB_VIEW_LIST);
	m_ToolBar.AddButton(IDB_VIEW_TREE);
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
	case WM_SIZE:
	{
		OnSize();
		break;
	}
	case WM_NOTIFY:
	{
		NMHDR *pnmh = (NMHDR *)lParam;
		/*if (pnmh->hwndFrom == m_hNPPWnd)
		{
			if (LOWORD(pnmh->code) == DMN_CLOSE)
			{
				// ::SendMessage(m_hNPPWnd, NPPM_SETMENUITEMCHECK, funcItem[0]._cmdID, FALSE);

				return 0;
			}
		}
		break;*/
		if (FALSE == thePlugin.NotifyFromCVD(pnmh->hwndFrom, LOWORD(pnmh->code)))
		{
			return FALSE;
		}
		break;
	}
	case CVM_LOGA:
	{
		int level = static_cast<int>(wParam);

		char *msgPtr = CharPtrFromSPtr(lParam);

		switch (level)
		{
		case INFO:
			LOGINFO(msgPtr);
			break;

		case WARN:
			LOGWARN(msgPtr);
			break;

		case ERR:
			LOGERR(msgPtr);
			break;

		default:
			LOGMSG(msgPtr);
			break;
		}
		return TRUE;
	}
	case CVM_LOGW:
	{
		int level = static_cast<int>(wParam);

		wchar_t *msgPtr = WCharPtrFromSPtr(lParam);

		switch (level)
		{
		case INFO:
			LOGINFO(msgPtr);
			break;

		case WARN:
			LOGWARN(msgPtr);
			break;

		case ERR:
			LOGERR(msgPtr);
			break;

		default:
			LOGMSG(msgPtr);
			break;
		}
		return TRUE;
	}
	case CVM_SAVE_DOCUMENT:
	{
		LOGERR("Not implement CVM_SAVEDOCUMENT");
		return FALSE;
	}
	case CVM_OPEN_DOCUMENT:
	{
		if (lParam == 0)
			return 0;
		wchar_t *wcharPtr = WCharPtrFromSPtr(lParam);
		LOGERR("Not implement CVM_OPENDOCUMENT");
		return FALSE;
	}
	case CVM_RENAME_DOCUMENT:
	{
		if (lParam == 0)
			return 0;
		wchar_t *wcharPtr = WCharPtrFromSPtr(lParam);
		LOGERR("Not implement CVM_RENAME_DOCUMENT");
		return FALSE;
	}
	/*case CVM_FETCH_DOCUMENT:
	{
		thePlugin.GetDocument(wStr);
		str = WtoA(wStr.c_str());
		number = str.length();

		if (lParam == 0)
		{
			//wParam = (WPARAM)number + 1;
			int* intPtr = (int*)wParam;
			memcpy(intPtr, &number, sizeof(int));
			return number + 1;
		}
		else
		{
			charPtr = CharPtrFromSPtr(lParam);
			if (number) {
				memcpy(charPtr, str.c_str(), number);
				charPtr[number++] = '\0';
			}
			else {
				charPtr[0] = '\0';
			}
			return number;
		}
		break;
	}*/
	case CVM_FETCH_DOCUMENT:
	{
		wstring wDoc;

		thePlugin.GetDocument(wDoc);
		//const wchar_t* wzStr = wStr.c_str();
		int number = wDoc.length();

		if (lParam == 0)
		{
			//wParam = (WPARAM)number + 1;
			int *intPtr = (int *)wParam;
			memcpy(intPtr, &number, sizeof(int));
			return number;
		}
		else
		{
			wchar_t *wcharPtr = WCharPtrFromSPtr(lParam);
			if (number)
			{
				memcpy(wcharPtr, wDoc.c_str(), sizeof(wchar_t) * number);
				wcharPtr[number++] = L'\0';
			}
			else
			{
				wcharPtr[0] = L'\0';
			}
		}
		return number;
	}
	case CVM_UPDATE_DOCUMENT:
	{
		if (lParam == 0)
			return 0;
		wchar_t *wcharPtr = WCharPtrFromSPtr(lParam);
		//wStr = AtoW(charPtr);
		thePlugin.SetDocument(wcharPtr);
		return TRUE;
	}
	case CVM_FETCH_SELECTION:
	{
		wstring wSel;
		thePlugin.GetSelection(wSel);
		int number = wSel.length();

		if (lParam == 0)
		{
			//wParam = (WPARAM)number + 1;
			int *intPtr = (int *)wParam;
			memcpy(intPtr, &number, sizeof(int));
			return number;
		}
		else
		{
			wchar_t *wcharPtr = WCharPtrFromSPtr(lParam);
			if (number)
			{
				memcpy(wcharPtr, wSel.c_str(), sizeof(wchar_t) * number);
				wcharPtr[number++] = L'\0';
			}
			else
			{
				wcharPtr[0] = L'\0';
			}
		}
		return number;
	}
	case CVM_UPDATE_SELECTION:
	{
		if (lParam == 0)
			return 0;
		wchar_t *wcharPtr = WCharPtrFromSPtr(lParam);
		thePlugin.ReplaceSelection(wcharPtr);
		return TRUE;
	}
	case CVM_UPDATE_CONTENTSVIEW:
	{
		UpdateContentListview();
		return TRUE;
	}
	case CVM_GET_DOCUMENT:
	{
		wstring wDoc;
		m_pDocument->GetDocument(wDoc);
		int number = wDoc.length();

		if (lParam == 0)
		{
			int *intPtr = (int *)wParam;
			memcpy(intPtr, &number, sizeof(int));
			return number;
		}
		else
		{
			wchar_t *wcharPtr = WCharPtrFromSPtr(lParam);
			if (number)
			{
				memcpy(wcharPtr, wDoc.c_str(), sizeof(wchar_t) * number);
				wcharPtr[number++] = L'\0';
			}
			else
			{
				wcharPtr[0] = L'\0';
			}
		}
		return number;
	}
	case CVM_SET_DOCUMENT:
	{
		if (lParam == 0)
			return 0;
		wchar_t *wcharPtr = WCharPtrFromSPtr(lParam);
		m_pDocument->SetDocument(wcharPtr);
		return TRUE;
	}
	case CVM_PARSE:
	{
		m_stIndexType = (stIndexType_t)(int)wParam;

		m_wIndex = L"";

		if (NormIndexTyp == m_stIndexType)
		{
			CheckMenuItem(m_hExtraMenu, IDM_SPECIFIC_INDEX_CONTENTS, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(m_hExtraMenu, IDM_HEAD_INDEX_CONTENTS, MF_BYCOMMAND | MF_UNCHECKED);
		}
		else if (HeadIndexTyp == m_stIndexType)
		{
			CheckMenuItem(m_hExtraMenu, IDM_SPECIFIC_INDEX_CONTENTS, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(m_hExtraMenu, IDM_HEAD_INDEX_CONTENTS, MF_BYCOMMAND | MF_CHECKED);
		}
		else if (SpecificIndexTyp == m_stIndexType)
		{
			m_wIndex = WCharPtrFromSPtr(lParam);

			CheckMenuItem(m_hExtraMenu, IDM_SPECIFIC_INDEX_CONTENTS, MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(m_hExtraMenu, IDM_HEAD_INDEX_CONTENTS, MF_BYCOMMAND | MF_UNCHECKED);
		}

		m_pDocument->Parse(m_stIndexType, m_wIndex.c_str());
		return TRUE;
	}
	case CVM_DEFAULT_REPLACE:
	{
		wchar_t scriptFile[MAX_PATH];
		_swprintf(scriptFile, L"%s%s", m_wszScriptPath, L"\\stdRep.jb");
		m_pDocument->ScriptReplace(scriptFile);
		return TRUE;
	}
	case CVM_UPDATE_CONTENTS:
	{
		m_pDocument->UpdateContents();
		return 1;
	}
	case CVM_EXPORT_CONTENTS:
	{
		LOGERR("Not implement CVM_EXPORT_CONTENTS");
		return FALSE;
	}
	case CVM_IMPORT_CONTENTS:
	{
		LOGERR("Not implement CVM_IMPORT_CONTENTS");
		return FALSE;
	}
	case CVM_NUMBER_CONTENTS:
	{
		m_pDocument->NumberContents();
		return TRUE;
	}
	case CVM_ALIGN_CONTENTSNUMBER:
	{
		return FALSE;
	}
	case CVM_TRIM_CONTENTS:
	{
		m_pDocument->TrimContents();
		return TRUE;
	}
	case CVM_SORT_CONTENTS:
	{
		// m_pDocument->();
		LOGERR("Not implement CVM_SORT_CONTENTS");
		return FALSE;
	}
	case CVM_DEL_DUPLICATECONTENTS:
	{
		m_pDocument->DelDuplicateContents();
		return TRUE;
	}
	case CVM_MARK_EMPTYCONTENTS:
	{
		m_pDocument->MarkEmptyContents();
		return TRUE;
	}
	case CVM_BIG5_GBK:
	{
		// m_pDocument->();
		LOGERR("Not implement CVM_BIG5_GBK");
		return FALSE;
	}
	case CVM_MERGE_PARAGRAPHS:
	{
		m_pDocument->MergeParagraphs();
		return TRUE;
	}
	case CVM_ALIGN_PARAGRAPHS:
	{
		m_pDocument->AlignParagraphs();
		return TRUE;
	}
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
	::MoveWindow(m_ToolBar, left, top, width - 2 * left, r.Height(), TRUE);
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

	int nLine = thePlugin.GetCurLineNum();

	if ((uMsg >= IDM_SCRIPT_FIRST) && (uMsg <= IDM_SCRIPT_LAST))
	{
		OnScriptReplace(uMsg);
		ParseCurFile();
		UpdateContentListview();
		thePlugin.GotoLine(nLine);
		return FALSE;
	}
	else if ((uMsg >= IDM_PYSCRIPT_FIRST) && (uMsg <= IDM_PYSCRIPT_LAST))
	{
		OnPyScriptReplace(uMsg);
		//ParseCurFile();
		//UpdateContentListview();
		//thePlugin.GotoLine(nLine);
		return FALSE;
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

	case IDB_VIEW_LIST:
		MessageBox(L"IDB_VIEWLIST was clicked", L"Contents Viewer", MB_OK);
		break;

	case IDB_VIEW_TREE:
		MessageBox(L"IDB_VIEWTREE was clicked", L"Contents Viewer", MB_OK);
		break;

	case IDB_PARSE:
		ParseCurFile();
		UpdateContentListview();
		LOGOUT("CurLine: %d", nLine);
		FocuseContentListviewItemByLine(nLine);
		break;

	case IDB_EXTRA_MENU:
		OnExtraMenu();
		break;

	case IDM_HEAD_INDEX_CONTENTS:
		HeadIndex();
		ParseCurFile();
		UpdateContentListview();
		break;

	case IDM_SPECIFIC_INDEX_CONTENTS:
		SpecificIndex();
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

	case IDM_SC_NUMBER_CONTENTS:
		//MessageBox(L"IDM_SC_INTO_NUMBER was clicked"), L"Contents Viewer"), MB_OK);
		//if (true == m_bHeadIndexContent) HeadIndexContents();
		//ReParseCurFile();
		ParseCurFile();
		NumberContents();
		//ReParseCurFile();
		ParseCurFile();
		UpdateContentListview();
		break;

	case IDM_SC_ALIGN_CONTENTS_NUMBER:
		MessageBox(L"IDM_SC_ALIGN_NUMBER was clicked", L"Contents Viewer", MB_OK);
		break;

	case IDM_SC_TRIM_CONTENTS:
		//if(true == m_bHeadIndexContent) HeadIndexContents();
		//ReParseCurFile();
		ParseCurFile();
		TrimContents();
		//ReParseCurFile();
		ParseCurFile();
		UpdateContentListview();
		break;

	case IDM_SC_SORT_CONTENTS:
		MessageBox(L"IDM_SC_SORT_PARAGRAPH was clicked", L"Contents Viewer", MB_OK);
		break;

	case IDM_SC_DEL_DUPLICATE_CONTENTS:
		//ReParseCurFile();
		ParseCurFile();
		DelDuplicateContents();
		//ReParseCurFile();
		ParseCurFile();
		UpdateContentListview();
		break;

	case IDM_SC_MARK_EMPTY_CONTENTS:
	{
		//ReParseCurFile();
		ParseCurFile();
		MarkEmptyContents();
		//ReParseCurFile();
		ParseCurFile();
		UpdateContentListview();
		break;
	}

		/*case IDM_REPLACE_WORDS :
		//MessageBox(L"IDM_REPLACE_PINYINWORD was clicked"), L"Contents Viewer"), MB_OK);
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

void CContentsViewerDialog::OnExtraMenu()
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	TrackPopupMenu(m_hExtraMenu, TPM_LEFTBUTTON, point.x, point.y, 0, this->GetHwnd(), NULL);
}

void CContentsViewerDialog::OnScriptReplace(size_t id)
{
	wstring wFile = m_mapScriptMenu[id];

	wchar_t wszScriptFile[MAX_PATH];

	_swprintf(wszScriptFile, L"%s%s%s%s", m_wszScriptPath, L"\\", wFile.c_str(), L".jb");

	/*CIniFile* pScriptFile = new CIniFile(scriptFile);

	//pScriptFile->GetString(L"Common"), L"name"), tszName);
	bool isRegularMode = pScriptFile->GetBool(L"Common"), L"regrex"));

	int amount = pScriptFile->GetInteger(L"Common"), L"amount"));

	wchar_t tszFindItem[MAX_PATH], tszReplaceItem[MAX_PATH];
	wchar_t tszFindString[MAX_PATH], tszReplaceString[MAX_PATH];

	tString tDocument;
	//thePlugin.GetSelection(tFile);
	thePlugin.GetDocument(tDocument);

	//LOGINFO(L"m_TszAppPath: %s."), m_tszAppPath);
	//_stprintf(tszRepPath, L"%s%s%s"), m_tszAppPath, L"\\plugins\\Config\\"), tszRepFile);

	LOGINFO(L"Start to exec replace script : %s."), scriptfile);

	for(int i = 1; i <= amount; i++)
	{
		_stprintf(tszFindItem, L"%s%d"), L"f"), i);
		_stprintf(tszReplaceItem, L"%s%d"), L"r"), i);

		pScriptFile->GetString(L"Code"), tszFindItem, tszFindString);
		pScriptFile->GetString(L"Code"), tszReplaceItem, tszReplaceString);

		//if (NULL == tszReplace) tszReplace = L"");

		LOGINFO(L"tFind：%s, tReplace：%s."), tszFindString, tszReplaceString);

		//FindAndReplace(tszFind, tszReplace);
		int count = StringReplaceAll(tDocument, tszFindString, tszReplaceString, isRegularMode);

		LOGINFO(L"%d was found and replaced."), count);
	}

	thePlugin.SetDocument(tDocument);

	delete pScriptFile;
	pScriptFile = NULL;*/

	wstring wDocument;
	thePlugin.GetDocument(wDocument);
	m_pDocument->SetDocument(wDocument.c_str());
	m_pDocument->ScriptReplace(wszScriptFile);
	m_pDocument->GetDocument(wDocument);
	thePlugin.SetDocument(wDocument.c_str());
}

void CContentsViewerDialog::OnPyScriptReplace(size_t id)
{
	wstring wFile = m_mapScriptMenu[id];

	wchar_t wszScriptFile[MAX_PATH];

	_swprintf(wszScriptFile, L"%s%s%s%s", m_wszScriptPath, L"\\", wFile.c_str(), L".py");

	// tString tDocument;
	// thePlugin.GetDocument(tDocument);
	// m_pDocument->SetDocument(tDocument);
	// m_pDocument->PyScriptReplace(scriptFile);
	// m_pDocument->GetDocument(tDocument);
	// thePlugin.SetDocument(tDocument);

	int ret = m_pScriptPlugin->Execute(WtoA(wszScriptFile));
	if (-1 == ret)
	{
		MessageBox(wszScriptFile, L"Fail to Execute", MB_OK);
	}
}

size_t CContentsViewerDialog::ParseCurFile()
{
	thePlugin.UpdateCurFileCodePage();
	m_nMaxLevel = m_pDocument->Parse(m_stIndexType, m_wIndex.c_str());
	return m_nMaxLevel;
}

size_t CContentsViewerDialog::ReParseCurFile()
{
	int nTotalNum = 0;

	/*thePlugin.UpdateCurFileCodePage();

	m_pContentsLV->SetRedraw(FALSE);
	m_pContentsLV->DeleteAllItems();
	//m_pContentsLV->SetRedraw(TRUE);

	ClearMapContent();

	//tString tDocument;
	//thePlugin.GetDocument(tDocument);

	if(false == m_bHeadIndexContent)
	{
		//LOGINFO(L"Start to index 卷.\n"));
		//int level = 1;
		//tString tText = L"^[ 　]*第[^ 　]{0, 14}卷.*");
		//LOGINFO(tText.c_str());
		//int i = thePlugin.IndexContents(tText.c_str(), level);
		//if (i > 0) level++;
		//LOGINFO(L"End to index 卷.\n"));

		//LOGINFO(L"Start to index 集.\n"));
		//int pos = tText.find(L"卷"));
		//tText = tText.replace(pos, 1, L"集"));
		//LOGINFO(tText.c_str());
		//i = thePlugin.IndexContents(tText.c_str(), level);
		//if (i > 0) level++;
		//LOGINFO(L"End to index 集.\n"));
		//
		//LOGINFO(L"Start to index 章.\n"));
		//tText = tText.replace(pos, 1, L"章"));
		//LOGINFO(tText.c_str());
		//i = thePlugin.IndexContents(tText.c_str(), level);
		//if (i > 0) level++;
		//LOGINFO(L"End to index 章.\n"));
		//
		//LOGINFO(L"Start to index 回.\n"));
		//tText = tText.replace(pos, 1, L"回"));
		//LOGINFO(tText.c_str());
		//i = thePlugin.IndexContents(tText.c_str(), level);
		//LOGINFO(L"End to index 回.\n"));
		//
		//LOGINFO(L"Start to sort.\n"));

		//tString tlevelItem;
		//wchar_t tszKeyword[40], X[MAX_PATH], tszLevelItem[20];
		//tString tX;

		//int totalLevel = m_pIniFile->GetInteger(L"Content"), L"totallevel"));
		//int CountX = m_pIniFile->GetInteger(L"Content"), L"CountX"));
		//m_pIniFile->GetString(L"Content"), L"X"), X);
		//tX = AtoT(X, CP_ACP);

		size_t totalLevel = _tcsclen(m_wszKeyword);
		LOGINFO(L"totalLevel: %d, CountX: %d, X: %s"), totalLevel, m_uCountX, m_wszNum);
		wchar_t tt[10];
		_stprintf(tt, L"%d"), m_uCountX*2);

		int level = 1;
		int num = 0;
		//_stprintf(tszText, L"^[ 　]*第[%s]{0, %d}卷.*"), X, CountX*2);
		//tString tText = L"^[ 　\t]*第[");
		tString tText = L"^[ 　\\t]*?第[");
		//tText = tText + X;
		tText = tText + m_wszNum;
		//tText = tText + L"]{0,";
		tText = tText + L"]{0,");
		tText = tText + tt;
		//tText = tText + L"}卷.*";
		tText = tText + L"}卷.*");
		LOGINFO(L"tText: %s"), tText.c_str());
		//MessageBoxW(tText.c_str(), tText.c_str(), MB_OK);
		//size_t pos = tText.find(L"卷");
		size_t pos = tText.find(L"卷"));
		LOGINFO(L"pos: %d"), pos);
		LOGINFO(L"totalLevel: %d"), totalLevel);

		tString tkeyWord;
		for (int i = 1; i <= totalLevel; i ++)
		{
			//_stprintf(tszLevelItem, L"%s%d"), L"level"), i);
			//m_pIniFile->GetString(L"Content"), tszLevelItem, tszKeyword);
			tkeyWord = m_wszKeyword[i - 1];
			//LOGINFO(L"tszLevelItem: %s, keywrod: %s."), tszLevelItem, tszKeyword);
			LOGINFO(L"tszLevelItem: %d, keywrod: %s."), i, tkeyWord.c_str());

			//tText = tText.replace(pos, 1, tszKeyword);
			tText = tText.replace(pos, 1, tkeyWord.c_str());
			LOGINFO(L"tText: %s"), tText.c_str());
			if(num > 0) level++;

			//num = thePlugin.IndexContents(tDocument, tText.c_str(), tszKeyword, level);
			//num = thePlugin.IndexContents(tText.c_str(), tszKeyword, level);
			num = thePlugin.IndexContents(tText.c_str(), tkeyWord.c_str(), level);

			if (num > 0) m_nMaxLevel = level;
			totalNum = totalNum + num;
			//LOGINFO(L"End to index %s.\n"), tszKeyword);
			LOGINFO(L"End to index %s.\n"), tkeyWord.c_str());
		}
	}
	else
	{
		//totalnum = thePlugin.IndexContents(tDocument, L"^[^ 　\\t\\r\\n].+"), L"回"), 1);
		totalNum = thePlugin.IndexContents(L"^[^ 　\\t\\r\\n].+"), L""), 1);
		LOGINFO(L"Head index content."));
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

	return nTotalNum;
}

int CContentsViewerDialog::AddContent(int nItem, const wchar_t *wszContentName, int nLine, int nLevel, const wchar_t *wszKeyword)
{
	LOGFUNBGN;

	LOGINFO(L"CVD AddContent: %s, nLevel : %d, nLine: %d", wszContentName, nLevel, nLine);

	//int maxlen = 16;

	//wchar_t tszContent[256];

	/*if (m_mapContent.end() == m_mapContent.find(line))
	{
		ContentData_t* pContent = new ContentData_t;
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

	return m_pDocument->AddContent(nItem, wszContentName, nLine, nLevel, wszKeyword);
}

void CContentsViewerDialog::UpdateContentListview()
{
	//m_pContentsLV->SetRedraw(FALSE);
	m_pContentsLV->DeleteAllItems();

	//mapContent_t::iterator intertr;
	// mapContent_t::iterator intertr;
	mapContent_t content;
	m_pDocument->GetContent(content);
	ContentData_t *pContent = nullptr;
	int nItem = 0;
	wstring wContentName;
	int nLine, nLevel;
	bool bIsFistLineIndexed = false;

	for (auto intertr = content.begin(); intertr != content.end(); intertr++)
	{
		nLine = intertr->first;
		pContent = intertr->second;
		wContentName = pContent->wContent;
		nLevel = pContent->nLevel;

		if (1 == nLine)
			bIsFistLineIndexed = true;
		nItem++;

		m_pContentsLV->AddListViewItem(nItem, wContentName.c_str(), nLine, nLevel);
		nItem++;
	}

	if (false == bIsFistLineIndexed)
	{
		wstring wFisrtLine;
		thePlugin.GetLine(1, wFisrtLine);
		m_pContentsLV->AddListViewItem(0, wFisrtLine.c_str(), 1, 1);
		//m_pContentsLV->SortItemsByLine();
		nItem++;
	}

	m_pContentsLV->AddListViewItem(nItem, L"Last line!", thePlugin.GetLineCount(), 1);

	//m_pContentsLV->SetRedraw(TRUE);
}

void CContentsViewerDialog::FocuseContentListviewItemByLine(int nLine)
{
	int nItem = m_pContentsLV->GetItemByLine(nLine);
	m_pContentsLV->FocusItem(nItem);
}

//
void CContentsViewerDialog::HeadIndex()
{
	if (HeadIndexTyp == m_stIndexType)
	{
		CheckMenuItem(m_hExtraMenu, IDM_HEAD_INDEX_CONTENTS, MF_BYCOMMAND | MF_UNCHECKED);
		m_stIndexType = NormIndexTyp;
	}
	else
	{
		CheckMenuItem(m_hExtraMenu, IDM_SPECIFIC_INDEX_CONTENTS, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(m_hExtraMenu, IDM_HEAD_INDEX_CONTENTS, MF_BYCOMMAND | MF_CHECKED);
		m_stIndexType = HeadIndexTyp;
	}
}

//
void CContentsViewerDialog::SpecificIndex()
{
	if (SpecificIndexTyp == m_stIndexType)
	{
		CheckMenuItem(m_hExtraMenu, IDM_SPECIFIC_INDEX_CONTENTS, MF_BYCOMMAND | MF_UNCHECKED);
		m_stIndexType = NormIndexTyp;
	}
	else
	{
		CheckMenuItem(m_hExtraMenu, IDM_HEAD_INDEX_CONTENTS, MF_BYCOMMAND | MF_UNCHECKED);

		// CInputBox inputbox; //定义一个inputbox
		// if(IDOK == inputbox.DoModal())
		// {
		// inputbox.getInput(m_tIndex);
		// }
		wchar_t wszBuf[100] = {0};
		INT_PTR ret = CWin32InputBox::InputBox(L"CContents Viewer", L"Specific Index:", wszBuf, 100);
		if (IDOK == ret)
		{
			CheckMenuItem(m_hExtraMenu, IDM_SPECIFIC_INDEX_CONTENTS, MF_BYCOMMAND | MF_CHECKED);
			m_wIndex = wszBuf;
			m_stIndexType = SpecificIndexTyp;
		}
	}
}

// TODO: test
void CContentsViewerDialog::ExportContents()
{
	Win32xx::CFileDialog myDlg(FALSE, NULL, NULL, 0, L"Text|*.txt||");

	if (myDlg.DoModal() != IDOK)
		return;

	wstring wContentPath = myDlg.GetPathName();

	/*FILE* pContentFile = NULL;

	pContentFile = _tfopen(tContentPath.c_str(), L"w+"));

	if(NULL == pContentFile) {
		LOGERR(L"Can't open %s to Export!"), tContentPath.c_str());
		return;
	}

	// mapContent_t::iterator intertr;
	// ContentData_t* pContent = NULL;

	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++) {
		line = intertr->first;
		pContent = intertr->second;
		tItemText = pContent->tContent;

		_stprintf(lineBuffer, L"%d"), line);

		_fputts(tItemText.c_str(), pContentFile);
		_fputts(L"\t"), pContentFile);
		_fputts(lineBuffer, pContentFile);
		_fputts(L"\n"), pContentFile);
	}

	fflush(pContentFile);
	fclose(pContentFile);
	pContentFile = NULL;*/

	m_pDocument->ExportContents(wContentPath);
}

// TODO: test
void CContentsViewerDialog::ImportContents()
{
	CFileDialog myDlg(TRUE, NULL, NULL, 0, L"Text|*.txt||");

	if (myDlg.DoModal() != IDOK)
		return;

	wstring wContentPath = myDlg.GetPathName();

	/*FILE* contentFile = NULL;

	contentFile = _tfopen(tContentPath.c_str(), L"r"));

	if(NULL == contentFile) {
		LOGERR(L"Can't open %s to Import!"), tContentPath.c_str());
		return;
	}

	assert(contentFile);

	wchar_t tszLine[256];

	wchar_t* tszContent;
	wchar_t* tszLineNo;

	int lineNo = 0;

	while(_fgetts(tszLine, 256, contentFile))
	{
		int i = _tcslen(tszLine);

		tszLine[i - 1] = 0;

		LOGINFO(L"Line：%s."), tszLine);

		tszContent = _tcstok(tszLine, L"\t"));
		tszLineNo = _tcstok(NULL, L"\t"));

		LOGINFO(L"Content: %s, tszLineNo：%s."), tszContent, tszLineNo);

		lineNo = (int)(_ttoi(tszLineNo));

		thePlugin.ReplaceLine(lineNo, tszContent);
	}

	fclose(contentFile);
	contentFile = NULL;*/

	m_pDocument->ImportContents(wContentPath);
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
	mapContent_t::iterator intertr;
	ContentData_t* pContent = NULL;
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
		LOGINFO(L"Content to number : %s, keyword: %s, content: %s."), tNumber.c_str(), pContent->tKeyword.c_str(), tContent.c_str());
		pos = 0;

		if (0 == _tcscmp(tNumber.c_str(), L"十")))
		{
			tNumber = L"10");
			tContent = tContent.replace(1, lastpos - 1, tNumber);
			LOGINFO(L"Numbered content: %s."), tContent.c_str());
			thePlugin.ReplaceLine(line, tContent.c_str());
			continue;
		}

		if (0 == _tcscmp(tNumber.substr(0, 1).c_str(), L"十")))
		{
			tNumber = tNumber.replace(0, 1, L"1"));
		}

		pos = tNumber.find(L"0"));
		while (pos != tString::npos)
		{
			tNumber = tNumber.replace(pos, 1, L"零"));
			pos = tNumber.find(L"0"));
		}
		LOGINFO(L"Numbered one: %s."), tNumber.c_str());

		pos = tNumber.find(L"o"));
		while (pos != tString::npos)
		{
			tNumber = tNumber.replace(pos, 1, L"零"));
			pos = tNumber.find(L"o"));
		}
		LOGINFO(L"Numbered two: %s."), tNumber.c_str());

		wchar_t last[2];
		_tcscpy(last, tNumber.substr(lastpos - 2 ,1).c_str());
		if(0 == _tcscmp(last, L"十")))
			tNumber = tNumber.replace(lastpos - 2, 1, L"0"));
		else if(0 == _tcscmp(last, L"百")))
			tNumber = tNumber.replace(lastpos - 2, 1, L"00"));
		else if(0 == _tcscmp(last, L"千")))
			tNumber = tNumber.replace(lastpos - 2, 1, L"000"));
		LOGINFO(L"Numbered three: %s."), tNumber.c_str());

		pos = tNumber.find(L"十零"));
		if(pos != tString::npos) tNumber.erase(pos, 2);
		pos = tNumber.find(L"十"));
		if (pos != tString::npos) tNumber.erase(pos, 1);
		pos = tNumber.find(L"百零"));
		if(pos != tString::npos) tNumber = tNumber.replace(pos, 2, L"0"));
		pos = tNumber.find(L"百"));
		if (pos != tString::npos) tNumber = tNumber.erase(pos, 1);
		pos = tNumber.find(L"千零"));
		if (pos != tString::npos)
		{
			if (lastpos - pos <= 4)
				tNumber = tNumber.replace(pos, 2, L"00"));
			else
				tNumber = tNumber.replace(pos, 2, L"0"));
		}
		pos = tNumber.find(L"千"));
		if (pos != tString::npos) tNumber = tNumber.erase(pos, 1);

		LOGINFO(L"Numbered four: %s."), tNumber.c_str());

		tString lefts = L"一二三四五六七八九零");
		tString rights = L"1234567890");
		tString left, right;
		LOGINFO(L"lefts: %s, Rights: %s."), lefts.c_str(), rights.c_str());
		for(int i = 0; i < 10; i++)
		{
			left = lefts.substr(i, 1);
			right = rights.substr(i, 1);

			pos = tNumber.find(left.c_str());

			while (pos != tString::npos)
			{
				tNumber = tNumber.replace(pos, 1, right);
				pos = tNumber.find(left.c_str());
				LOGINFO(L"left: %s, Right: %s."), left.c_str(), right.c_str());
			}
		}
		LOGINFO(L"Numbered five: %s."), tNumber.c_str());

		//remove left banks
		tContent = tContent.replace(1, lastpos - 1, tNumber);
		LOGINFO(L"Numbered content: %s."), tContent.c_str());
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

		LOGINFO(L"tItemText1: %s, tItemText2: %s"), tItemText1.c_str(), tItemText2.c_str());

		if (0 == _tcscmp(tItemText1.c_str(), tItemText2.c_str()))
		{
			thePlugin.DelLine(line);
			LOGINFO(L"tItemText1: %s = tItemText2: %s"), tItemText1.c_str(), tItemText2.c_str());
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
	mapContent_t::iterator intertr1, intertr2;
	ContentData_t* pContent1 = NULL;
	ContentData_t* pContent2 = NULL;

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

		size_t pos = tItemText1.rfind(L"+"));

		if (pos != string::npos) LOGINFO(L"pos of '+' : %d; length: %d."), pos, tItemText1.length());
		if (tItemText1.rfind(L"+")) == tItemText1.length() - 3) return;

		if (line2 - line1 <= 5 && level1 >= level2)
		{
			LOGINFO(L"Empty content : %s"), tItemText1.c_str());

			tItemText1.insert(tItemText1.length() - 2, L"+"));

			LOGINFO(L"Line1: %d"), line1);
			LOGINFO(L"Line2: %d"), line2);
			LOGINFO(L"Marked content : %s"), tItemText1.c_str());
			LOGINFO(L"\n"));
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
		LCMapString(Locale, LCMAP_SIMPLIFIED_CHINESE, tBig5Line.c_str(), -1, const_cast<wchar_t*>(tGBKLine.c_str()), tBig5Line.length());
		thePlugin.ReplaceLine(line, tGBKLine.c_str());
	}
	LOGFUNEND;*/

	wstring wDoc;
	thePlugin.GetDocument(wDoc);

	m_pDocument->SetDocument(wDoc.c_str());
	m_pDocument->Big5ToGBK();
	m_pDocument->GetDocument(wDoc);

	thePlugin.SetDocument(wDoc.c_str());
}

//bug fix: can't remove the bank in the paragraph.
void CContentsViewerDialog::MergeParagraphs()
{
	/*
	//thePlugin.FindAndReplace(L"\r\n\r\n"), L"@"), true);
	//thePlugin.FindAndReplace("\r\n\r\n", "@", true);
	tString tSel;
	thePlugin.GetSelection(tSel);

	// tr1::cmatch res1;
	// tr1::regex
	// tr1::regex_search
	//thePlugin.FindAndReplace(L"　　"), L"@"), false);

	//thePlugin.FindAndReplace(L"\r\n"), L""), true);
	//thePlugin.FindAndReplace("\r\n", "", true);

	//thePlugin.FindAndReplace(L"@"), L"\r\n\r\n"), true);
	//thePlugin.FindAndReplace("@", "\r\n\r\n", true);

	//thePlugin.ReplaceSelection(tSel);


	// 表达式选项
	syntax_option_type flags = ECMAScript | icase;

	try
	{
		tSel = regex_replace(tSel, wregex(L"\r\n\r\n"), flags), L"@"));
		//int count = StringRegReplaceAll(tFile, wregex(L"\r\n\r\n"), flags), L"@"));
		tSel = regex_replace(tSel, wregex(L"\r\n"), flags), L""));
		//count = StringRegReplaceAll(tFile, wregex(L"\r\n"), flags), L""));
		tSel = regex_replace(tSel, wregex(L"@"), flags), L"\r\n\r\n"));
		//count = StringRegReplaceAll(tSel, wregex(L"@"), flags), L"\r\n\r\n"));

		//tSel = regex_replace(tSel, wregex(L"　　")), L""), match_default | match_not_bol);
		//tSel = regex_replace(tSel, wregex(L"\\B　　"), flags), L""));
		tSel = regex_replace(tSel, wregex(L"^　　"), flags), L"@"));
		tSel = regex_replace(tSel, wregex(L"　　"), flags), L""));
		tSel = regex_replace(tSel, wregex(L"^@"), flags), L"　　"));
	}
	catch (regex_error& e)
	{
		LOGINFO(L"%s: %d."), e.what(), e.code());
	}

	//thePlugin.SetDocument(tSel);
	thePlugin.ReplaceSelection(tSel);
	*/

	wstring wSel;
	thePlugin.GetSelection(wSel);

	m_pDocument->SetDocument(wSel.c_str());
	m_pDocument->MergeParagraphs();
	m_pDocument->GetDocument(wSel);

	thePlugin.ReplaceSelection(wSel.c_str());
}

void CContentsViewerDialog::AlignParagraphs()
{
	/*tString tFile;

	//thePlugin.GetSelection(tFile);
	thePlugin.GetDocument(tFile);

	// 表达式选项
	syntax_option_type flags = ECMAScript;

	try
	{
		// int count = 0;

		//remove all bank at the head of paragraph
		//theplugin.findandreplace(L"^[ 　	]+"), L""), true);
		tFile = regex_replace(tFile, wregex(L"^[ 　\t]+"), flags), L""));

		//remove all bank at the tail of paragraph
		//theplugin.findandreplace(L"[ 　	]+$"), L""), true);
		tFile = regex_replace(tFile, wregex(L"[ 　	\t]+$"), flags), L""));

		if (0 == line)
		{
			// do
			// {
				// count = theplugin.findandreplace(L"\r\n\r\n"), L"\r\n"), true);
			// } while (count >= 1);
			tFile = regex_replace(tFile, wregex(L"\r\n\r\n"), flags), L"\r\n"));
		}
		else if (1 == line)
		{
			//theplugin.findandreplace(L"\r\n"), L"\r\n\r\n"), true);
			tFile = regex_replace(tFile, wregex(L"\r\n"), flags), L"\r\n\r\n"));
			// do
			// {
				// count = theplugin.findandreplace(L"\r\n\r\n\r\n"), L"\r\n\r\n"), true);
			// } while (count >= 1);
			tFile = regex_replace(tFile, wregex(L"\r\n\r\n\r\n"), flags), L"\r\n\r\n"));
		}

		//first line indent
		//theplugin.findandreplace(L"^"), L"　　"), true);
		//count = theplugin.findandreplace("^", "　　", true);
		tFile = regex_replace(tFile, wregex(L"^"), flags), L"　　"));
		//LOGINFO(L"%d line was indented."), count);

	}
	catch (regex_error& e)
	{
		LOGINFO(L"%s: %d."), e.what(), e.code());
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

	wstring wDoc;
	thePlugin.GetDocument(wDoc);

	m_pDocument->SetDocument(wDoc.c_str());
	m_pDocument->AlignParagraphs();
	m_pDocument->GetDocument(wDoc);

	thePlugin.SetDocument(wDoc.c_str());
}

void CContentsViewerDialog::TrimContents()
{
	/*LOGFUNBGN;
	if(true == m_bHeadIndexContent) return;
	tString tContent, tKeywrod;
	int line;
	size_t pos = 0;

	mapContent_t::iterator intertr;
	ContentData_t* pContent = NULL;
	//for (int iItem = 0; iItem <= m_pContentsLV->GetItemCount() - 1; iItem++)
	//intertr = m_mapContent.end();
	for (intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++)
	//for (intertr++; intertr == m_mapContent.begin(); intertr-- )
	{
		line = intertr->first;
		pContent = intertr->second;

		LOGINFO(L"line: %d"), line);

		if(NULL == pContent)
		{
			LOGINFO(L"error in line: %d"), line);
			continue;
		}

		tContent = pContent->tContent;
		LOGINFO(L"raw tContent: '%s'"), tContent.c_str());
		tKeywrod = pContent->tKeyword;
		LOGINFO(L"keyword: %s"), tKeywrod.c_str());

		//tContent = m_pContentsLV->GetContentByItem(iItem);
		//line = m_pContentsLV->GetLineByItem(iItem);

		StringTrimLeft(tContent);
		StringTrimRight(tContent);

		LOGINFO(L"trimed tContent: '%s'"), tContent.c_str());

		pos = tContent.find(L"　"));
		while (pos != tString::npos)
		{
			tContent = tContent.replace(pos, 1, L" "));
			pos = tContent.find(L"　"));
		}
		LOGINFO(L"banked 1: '%s'"), tContent.c_str());

		pos = tContent.find(tKeywrod.c_str());
		if (tString::npos != pos)
		{
			//tBank = tContent.substr(pos + 1, 1);
			//if (_Tcscmp(tBank.c_str(), L" ")) != 0)
			//	tContent.insert(pos + 1, 1, L' '));
			tContent.insert(pos + 1, L" "));
			LOGINFO(L"banked 2: '%s'"), tContent.c_str());

			pos = tContent.find(L"  "));
			while (string::npos != pos)
			{
				tContent.replace(pos, 2, L" "));
				pos = tContent.find(L"  "));
			}
			LOGINFO(L"banked 3: '%s'"), tContent.c_str());
		}
		LOGINFO(L"Going to replace %d line by '%s'"), line, tContent.c_str());
		thePlugin.ReplaceLine(line, tContent.c_str());
	}
	LOGFUNEND;*/

	m_pDocument->TrimContents();
}

LRESULT CContentsViewerDialog::OnNotify(WPARAM wParam, LPARAM lParam)
{
	switch (((LPNMHDR)lParam)->code)
	{
	case TTN_GETDISPINFO:
	{
		// code from Win32++
		//HINSTANCE hInstance = GetApp().GetInstanceHandle();
		LPNMTTDISPINFOW lpDispInfo = (LPNMTTDISPINFOW)lParam;
		//lpDispInfo->hinst = hInstance;
		//UINT nID = lpDispInfo->hdr.idFrom;
		UINT nID = lpDispInfo->hdr.idFrom;

		//lpDispInfo->lpszText = (LPTSTR)Win32xx::LoadString(nID).c_str();
		//lpDispInfo->lpszText = MAKEINTRESOURCE(nID);
		//wchar_t buffer[80];
		//::LoadString(hInstance, nID, buffer, 80);
		//lpDispInfo->lpszText = buffer;
		wcscpy(lpDispInfo->szText, Win32xx::LoadString(nID + 10000).c_str());
		break;
	}

	case LVN_GETINFOTIP:
	{
		LPNMLVGETINFOTIPW lpNmlvgetinfotip = (LPNMLVGETINFOTIPW)lParam;
		int nItem = lpNmlvgetinfotip->iItem;
		int nLine = m_pContentsLV->GetLineByItem(nItem);
		_swprintf(lpNmlvgetinfotip->pszText, L"%d", nLine);
		break;
	}

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
			OnListViewItemRClicked((NMHDR *)lParam, 0);
		break;
	}

	return 0;
}

void CContentsViewerDialog::OnListViewItemClicked()
{
	int nItem = m_pContentsLV->GetSelectionMark();
	if (nItem >= 0)
	{
		int nState = m_pContentsLV->GetItemState(nItem, LVIS_FOCUSED | LVIS_SELECTED);
		if (nState & (LVIS_FOCUSED | LVIS_SELECTED))
		{
			m_pContentsLV->SetItemState(nItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			m_pContentsLV->SetSelectionMark(nItem);
			//m_pContentsLV->SetLastItem(nItem);

			int nLine = m_pContentsLV->GetLineByItem(nItem);
			if (nLine >= 1)
			{
				thePlugin.GotoLine(nLine);
			}
		}
	}
}

void CContentsViewerDialog::OnListViewItemRClicked(NMHDR *lParam, LRESULT *pResult)
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	TrackPopupMenu(m_hContentContextMenu, TPM_LEFTBUTTON, point.x, point.y, 0, this->GetHwnd(), NULL);

	*pResult = NULL;
}

void CContentsViewerDialog::ClearMapContent()
{
	// mapContent_t::iterator intertr;
	ContentData_t *pContent = nullptr;
	for (auto intertr = m_mapContent.begin(); intertr != m_mapContent.end(); intertr++)
	{
		pContent = intertr->second;
		if (pContent != nullptr)
			delete pContent;
	}
	m_mapContent.clear();
}

//TO-DO: wait to test
void CContentsViewerDialog::ListScripts(const wchar_t *wszScriptPath)
{
	m_mapScriptMenu.clear();

	size_t nScriptMenuID = IDM_SCRIPT_FIRST;
	size_t nPyScriptMenuID = IDM_PYSCRIPT_FIRST;

	//获取菜单
	//CMenu menu = GetMenu();
	//获取索引为2的菜单项，即第3列菜单
	// CMenu subMenu = mHFunMenu.GetSubMenu(2);
	//HMENU scriptHMenu = GetSubMenu(mHFunMenu, 2);
	//HMENU pyScriptHMenu = GetSubMenu(mHFunMenu, 2);

	//if (FALSE == CheckPath(m_wszScriptPath))
	//{
	//	LOGERR(L"%s dosn't exist!"), m_wszScriptPath);
	//	return;
	//}

	wchar_t wszSearchPath[MAX_PATH];
	wcscpy(wszSearchPath, wszScriptPath);
	wcscat(wszSearchPath, L"\\*.*");

	LOGINFO(L"scriptPath: %s;\n", wszSearchPath);

	//find the first file
	_wfinddata64_t wSrcFileData;
	intptr_t hFile = _wfindfirst64(wszSearchPath, &wSrcFileData);
	if (-1 != hFile)
	{
		//search all files
		do
		{
			if (wcslen(wSrcFileData.name) == 1 && wSrcFileData.name[0] == L'.' ||
				wcslen(wSrcFileData.name) == 2 && wSrcFileData.name[0] == L'.' && wSrcFileData.name[1] == L'.')
			{
				continue;
			}

			wchar_t wszNewPath[MAX_PATH];
			swprintf(wszNewPath, L"%s%s%s%s", wszSearchPath, L"\\", wSrcFileData.name, L"\\");
			if (wSrcFileData.attrib & _A_SUBDIR)
			{
				LOGINFO(L"Found a folder: %s.\n", wszNewPath);
			}
			else
			{
				// tString tFileName = wSrcFileData.name;
				wchar_t wszFileName[MAX_PATH];
				wcscpy(wszFileName, wSrcFileData.name);
				LOGINFO(L"Found a file: %s.", wszFileName);

				//pScriptFile = Load(wSrcFileData.name);
				//pScriptFile->GetString(L"Common"), L"name"), tszName);

				//tNameExt.erase(0, tNameExt.find_last_of(L".")));
				//tFileName.erase(0, tNameExt.find_last_of(L"\\")));

				//(_tcsrchr(fileName, L'.')))[0] = 0;

				//ext
				//full_name.rfind(".") == string.npos ? full_name : full_name.substr(full_name.rfind(".")+1);

				// wchar_t *ext = _tcsrchr(fileName, L'.'));
				// if (ext)
				// {
				// *ext = L'\0');
				// ext++;
				// }

				wchar_t wszDrive[_MAX_DRIVE], wszDir[_MAX_DIR], wszFName[_MAX_FNAME], wszExt[_MAX_EXT];
				_wsplitpath(wszFileName, wszDrive, wszDir, wszFName, wszExt);
				LOGINFO(L"Script name: %s, Ext: %s", wszFName, wszExt);

				if (0 == wcscmp(wszExt, L".jb"))
				{
					//pSubMenu->AppendMenu(MF_SEPARATOR); //分隔线
					//subMenu.AppendMenu(MF_STRING| MF_ENABLED, curNewMenu, fileName);
					nScriptMenuID++;
					//insert to map
					//m_mapScriptMenu[scriptMenu] = wszFName;
					m_mapScriptMenu.insert(pair<int, wstring>(nScriptMenuID, wszFName));
					//::AppendMenu(m_hScriptMenu, MF_STRING| MF_ENABLED, scriptMenu, wszFName);
					m_ScriptMenu.AppendMenu(MF_STRING | MF_ENABLED, nScriptMenuID, wszFName);
				}
				else if (0 == wcscmp(wszExt, L".py"))
				{
					nPyScriptMenuID++;
					//insert to map
					//m_mapScriptMenu[pyScriptMenu] = wszFName;
					m_mapScriptMenu.insert(pair<int, wstring>(nPyScriptMenuID, wszFName));
					//AppendMenu(m_hPyScriptMenu, MF_STRING| MF_ENABLED, pyScriptMenu, wszFName);
					m_PyScriptMenu.AppendMenu(MF_STRING | MF_ENABLED, nPyScriptMenuID, wszFName);
				}
			}
		} while (_wfindnext64(hFile, &wSrcFileData) == 0);
	}

	//close search handle
	_findclose(hFile);
}