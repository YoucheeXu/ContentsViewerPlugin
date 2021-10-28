// ContentsViewer.cpp
#include <regex>
#include "ContentsViewer.h"
#include "resource.h"
#include "ContentsViewerMsgs.h"
#include "Plugin.h"

using namespace regex_constants;
using namespace std;

//extern NPPData nppData;
// #define CMD_OPENCVD 0

// global vars
//bool g_bDebug;
CLogFile *g_pLogFile;

//static HBITMAP s_hContentsViewerBitmap = NULL;

CContentsViewer thePlugin;

CContentsViewer::CContentsViewer()
{
	g_pLogFile = new CLogFile();
	m_nCodePage = CP_ACP;
	m_bDebug = true;
	//m_hTabIcon = NULL;
	m_hSciEdit = NULL;
}

CContentsViewer::~CContentsViewer()
{
	Uninitialize();
	delete g_pLogFile;
}

void CContentsViewer::Initialize(HINSTANCE hInstance)
{
	SetResourceHandle(hInstance);
	m_hTabIcon = (HICON)::LoadImage(hInstance,
									MAKEINTRESOURCE(IDB_CONTENTSVIEWER), IMAGE_BITMAP, 0, 0,
									LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
	//m_TB_Icon.hToolbarBmp = (HBITMAP)::LoadImage(hInstance,
	// MAKEINTRESOURCE(IDB_TAGSVIEW), IMAGE_BITMAP, 0, 0,
	// LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS);

	/*s_hContentsViewerBitmap = (HBITMAP)::LoadImage(hInstance, MAKEINTRESOURCE(IDB_BITMAP_JSONVIEW), IMAGE_BITMAP,
		0, 0, (LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS));*/
}

void CContentsViewer::Uninitialize()
{
	/*if (m_hTabIcon)
	{
		::DestroyIcon(m_hTabIcon);
		m_hTabIcon = NULL;
	}*/

	//if ( m_TB_Icon.hToolbarBmp )
	//{
	//    ::DeleteObject(m_TB_Icon.hToolbarBmp);
	//    m_TB_Icon.hToolbarBmp = NULL;
	//}
}

BOOL CContentsViewer::IsCVDlgVisable()
{
	if (false == IsCVDlgExist())
		return false;
	else
		return m_CVDlg.IsWindowVisible();
	//return m_CVDlg.IsWindow();
}

bool CContentsViewer::IsCVDlgExist()
{
	//return (mCVDlg.GetHWnd() == NULL);
	return (m_CVDlg.IsWindow() == TRUE);
}

void CContentsViewer::ShowCVDlg()
{
	m_CVDlg.DoModeless();
}

HWND CContentsViewer::GetCVDlgHWnd()
{
	return m_CVDlg.GetHwnd();
}

void CContentsViewer::SetNPPData(const NPPData &nppd)
{
	m_NPPData = nppd;
	//m_CVDlg.SetNPPWnd(nppd._nppHandle);
}

HWND CContentsViewer::GetEditHWnd() const
{
	// Get the current scintilla
	int nCurrentView = -1;
	SendNPPMsg(NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&nCurrentView);
	if (nCurrentView == -1)
	{
		LOGERR("Can't get current scintilla!");
		assert(NULL);
		return NULL;
	}
	return ((nCurrentView == 0) ? m_NPPData._scintillaMainHandle : m_NPPData._scintillaSecondHandle);
}

//NPPN_BUFFERACTIVATED
void CContentsViewer::OnFileActivated()
{
	if (IsCVDlgExist())
	{
		m_hSciEdit = GetEditHWnd();
		//if(IsCVDlgVisable()) thePlugin.ReParseCurFile();
		if (IsCVDlgVisable())
			ParseCurFile();
	}
}

//NPPN_FILESAVED
void CContentsViewer::OnFileSaved()
{
}

//NPPN_FILEOPENED
void CContentsViewer::OnFileOpened()
{
	/*if (IsCVDlgExist())
	{
		if (IsCVDlgVisable())
			thePlugin.ReParseCurFile();
	}*/
}

//NPPN_FILEBEFORECLOSE

//NPPN_FILECLOSED
void CContentsViewer::OnFileClosed()
{
}

//NPPN_TBMODIFICATION
void CContentsViewer::OnNPPTBModification()
{
	/*toolbarIcons g_TBHex;
	//g_TBHex.hToolbarBmp = (HBITMAP)::LoadImage((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDB_TB_HEX), IMAGE_BITMAP, 0, 0, (LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS));
	g_TBHex.hToolbarBmp = mHTabIcon;
	SendNppMsg(NPPM_ADDTOOLBARICON, (WPARAM)funcItem[0]._cmdID, (LPARAM)&g_TBHex);*/

	//HWND hNppToolbar = NppToolbarHandleGetter::get();

	UINT style = (LR_SHARED | LR_LOADTRANSPARENT | LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS);
	m_tbiJS.hToolbarBmp = (HBITMAP)::LoadImage(GetInstanceHandle(), MAKEINTRESOURCE(IDB_CONTENTSVIEWER), IMAGE_BITMAP, 0, 0, style);
	;
	m_tbiJS.hToolbarIcon = m_hTabIcon;
	SendNPPMsg(NPPM_ADDTOOLBARICON, (WPARAM)funcItem[action_index[Action::openContentsViewerDlg]]._cmdID, (LPARAM)&m_tbiJS);
	//SendNPPMsg(NPPM_ADDTOOLBARICON, (WPARAM)funcItem[5]._cmdID, (LPARAM)&m_hTabIcon);
}

//NPPN_READY
void CContentsViewer::OnNPPReady()
{
	//const HWND hSciEdit = GetEditHWnd();
	//::SendMessage(hSciEdit, SCI_SETCODEPAGE, 936, 0);
	m_hSciEdit = GetEditHWnd();
}

//SCN_PAINTED
void CContentsViewer::OnSelectionChanged()
{
}

LRESULT CContentsViewer::SendNPPMsg(UINT uMsg, WPARAM wParam, LPARAM lParam) const
{
	return ::SendMessage(m_NPPData._nppHandle, uMsg, wParam, lParam);
}

LRESULT CContentsViewer::SendNPPMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::SendMessage(m_NPPData._nppHandle, uMsg, wParam, lParam);
}

LRESULT CContentsViewer::SendSciMsg(UINT uMsg, WPARAM wParam, LPARAM lParam) const
{
	if (NULL == m_hSciEdit)
		return FALSE;
	return ::SendMessage(m_hSciEdit, uMsg, wParam, lParam);
}

LRESULT CContentsViewer::SendSciMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_hSciEdit)
		return FALSE;
	return ::SendMessage(m_hSciEdit, uMsg, wParam, lParam);
}

int CContentsViewer::GetLineCount()
{
	return (int)SendSciMsg(SCI_GETLINECOUNT);
}

void CContentsViewer::GotoLine(int nLine)
{
	m_CVDlg.SavePos(nLine);
	/*if (line == GetLineCount())
		line = nLine + 5;
	else if(line >= 5)
		line = nLine - 5;*/

	SendSciMsg(WM_SETREDRAW, (WPARAM)FALSE);

	//This message sets both the anchor and the current position. If currentPos is negative, it means the end of the document. If anchorPos is negative, it means remove any selection (i.e. set the anchor to the same position as currentPos). The caret is scrolled into view after this operation.
	//::SendMessage(hSciEdit, SCI_SETSEL, selStart, selEnd);

	//::SendMessage(hSciEdit, SCI_ENSUREVISIBLE, nLine - 1, 0);

	//LOGOUT("DesLine: %d\n", nLine);

	int nCurLineNum = GetCurLineNum();
	LOGOUT("curLineNo: %d\n", nCurLineNum);
	//int firstVisibleLine = 0;
	auto nLinesOnScreen = 0;
	int nDesLine = 0;
	nLinesOnScreen = (int)SendSciMsg(SCI_LINESONSCREEN);
	LOGOUT("linesOnScreen: %d\n", nLinesOnScreen);
	if (nCurLineNum < nLine)
	{
		nDesLine = nLinesOnScreen / 2 + nLine;
	}
	else
	{
		nDesLine = nLine - nLinesOnScreen / 2;
	}

	SendSciMsg(SCI_GOTOLINE, nDesLine);

	////These messages retrieve and set the nLine number of the first visible nLine in the Scintilla view. The first nLine in the document is numbered 0. The value is a visible nLine rather than a document nLine.
	//int firstVisibleLine = ::SendMessage(hSciEdit, SCI_GETFIRSTVISIBLELINE, 0, 0);
	//::SendMessage(hSciEdit, SCI_SETFIRSTVISIBLELINE, desLine, 0);

	//This will attempt to scroll the display by the number of columns and nLines that you specify. Positive nLine values increase the nLine number at the top of the screen (i.e. they move the text upwards as far as the user is concerned), Negative nLine values do the reverse.
	//::SendMessage(hSciEdit, SCI_LINESCROLL, 0, firstLine);

	//::SendMessage(hSciEdit, SCI_ENSUREVISIBLE, nLine - 1, 0);
	//::SendMessage(hSciEdit, SCI_GOTOLINE, nLine - 1, 0);

	SendSciMsg(WM_SETREDRAW, (WPARAM)TRUE);
	::InvalidateRect(m_hSciEdit, NULL, TRUE);
	::UpdateWindow(m_hSciEdit);
}

void CContentsViewer::DelLine(int nLine)
{
	nLine = nLine - 1;
	int nPos = (int)SendSciMsg(SCI_POSITIONFROMLINE, nLine);
	unsigned int uLen = SendSciMsg(SCI_GETLINE, nLine);
	SendSciMsg(SCI_DELETERANGE, nPos, uLen);
}

void CContentsViewer::CutLines(int nLineStart, int nLineEnd)
{
	int nAnchorPos = (int)SendSciMsg(SCI_POSITIONFROMLINE, nLineStart - 1);
	int nCurrentPos = (int)SendSciMsg(SCI_POSITIONFROMLINE, nLineEnd - 1 + 1);

	SendSciMsg(SCI_SETSEL, nAnchorPos, nCurrentPos);
	SendSciMsg(SCI_CUT);
}

void CContentsViewer::PasteBeforeLine(int nLine)
{
	int nPos = (int)SendSciMsg(SCI_POSITIONFROMLINE, nLine - 1);
	SendSciMsg(SCI_GOTOPOS, nPos);
	SendSciMsg(SCI_PASTE);
}

void CContentsViewer::ReplaceLine(int nLine, const wchar_t *wszTxt)
{
	string aLine = WtoA(wszTxt, m_nCodePage);
	char *szLine = const_cast<char *>(aLine.c_str());

	int nStartPos = SendSciMsg(SCI_POSITIONFROMLINE, nLine - 1);
	//int currentPos = SendSciMsg(SCI_POSITIONFROMLINE, nLine + 1);
	unsigned int uLen = SendSciMsg(SCI_GETLINE, nLine - 1);
	int nEndPos = nStartPos + uLen;
	LOGINFO("nStartPos = %d, nEndPos = %d", nStartPos, nEndPos);
	SendSciMsg(SCI_SETSEL, nEndPos, nStartPos);

	SendSciMsg(SCI_REPLACESEL, 0, (LPARAM)szLine);
}

void CContentsViewer::ParseCurFile()
{
	//LOGINFO(_T("CContentsViewer::ParseCurFile()"));
	//mCVDlg.OnParse();
	m_CVDlg.ParseCurFile();
	m_CVDlg.UpdateContentListview();
}

void CContentsViewer::ReParseCurFile()
{
	LOGINFO("CContentsViewer::ReParseCurFile()");
	ParseCurFile();
}

int CContentsViewer::IndexContents(const wchar_t *wszText, const wchar_t *wszKeyword, int nLevel)
{
	int nSearchFlags = SCFIND_REGEXP + SCFIND_POSIX;
	//int searchFlags = SCFIND_MATCHCASE;

	TextToFind aTTF;
	int nPos = 0;
	// int n = 0;
	int i = 0;
	aTTF.chrg.cpMin = 0;
	aTTF.chrg.cpMax = SendSciMsg(SCI_GETLENGTH);

	string aText = WtoA(wszText, m_nCodePage);

	//int lenText = text.length();

	//char* lpstrText = new char[256];
	//memset(lpstrText, 0, 256);
	//strcpy(lpstrText, text.c_str());

	aTTF.lpstrText = const_cast<char *>(aText.c_str());
	//ttf.lpstrText = lpstrText;
	//ttf.lpstrText = szText;

	//strcpy(ttf.lpstrText, text.c_str());

	LOGINFO("ttf.lpstrText: %s.\n", aTTF.lpstrText);

	wstring wContent;

	do
	{
		nPos = SendSciMsg(SCI_FINDTEXT, nSearchFlags, (LPARAM)&aTTF);

		if (nPos != -1)
		{
			i++;
			//LOGINFO(_T("Content is found at Pos: %d\n"), nPos);
			int nLine = SendSciMsg(SCI_LINEFROMPOSITION, nPos);

			unsigned int uLen = SendSciMsg(SCI_GETLINE, nLine);
			aTTF.chrg.cpMin = nPos + uLen;
			char *szText = new char[uLen + 1];
			memset(szText, 0, uLen + 1);

			SendSciMsg(SCI_GETLINE, nLine, (LPARAM)szText);

			wContent = AtoW(szText, m_nCodePage);

			StringTrimLeft(wContent);
			StringTrimRight(wContent);
			m_CVDlg.AddContent(i, wContent.c_str(), nLine + 1, nLevel, wszKeyword);
			delete[] szText;
		}
	} while (nPos != -1);

	return i;
}

void CContentsViewer::ActiveContentListviewItem()
{
	if (IsCVDlgExist())
	{
		if (IsCVDlgVisable())
		{
			int nLine = GetCurLineNum();
			m_CVDlg.FocuseContentListviewItemByLine(nLine);
		}
	}
}

/*******************************************************************************
 *  desc: find and replace text file
 *------------------------------------------------------------------------------
 * param: const wchar_t* tFind  -- string to find
 * const wchar_t* tReplace  -- string to replace
 * bool isRegularMode -- find in regular mode or not
 *------------------------------------------------------------------------------
 * return: int  -- number of find and replace
*******************************************************************************/
//wait to test
int CContentsViewer::FindAndReplace(const wchar_t *wszFind, const wchar_t *wszReplace, bool bRegularMode)
{
	LOGINFO(L"Find: %s, Replace: %s. regularMode: %d.", wszFind, wszReplace, bRegularMode);

	int nSearchFlags;

	/*#define SCFIND_WHOLEWORD 2
	#define SCFIND_MATCHCASE 4
	#define SCFIND_WORDSTART 0x00100000
	#define SCFIND_REGEXP 0x00200000
	#define SCFIND_POSIX 0x00400000*/
	if (true == bRegularMode)
		nSearchFlags = SCFIND_REGEXP + SCFIND_POSIX;
	//searchFlags = SCFIND_CXX11REGEX;
	else
		//searchFlags = SCFIND_WORDSTART;
		nSearchFlags = SCFIND_MATCHCASE;

	int nCount = 0;
	TextToFind aTTF;
	int nPos = 0;
	// int i = 0;
	aTTF.chrg.cpMin = 0;
	aTTF.chrg.cpMax = SendSciMsg(SCI_GETLENGTH);

	string aFind = WtoA(wszFind, m_nCodePage);

	string aReplace = WtoA(wszReplace, m_nCodePage);

	//char* szReplace = const_cast<char*>(sReplace.c_str());

	char szReplace[] = "$";

	int nFindLen = wcslen(wszFind);
	int nReplaceLen = wcslen(wszReplace);

	LOGINFO("nFindLen: %d, nReplaceLen: %d.", nFindLen, nReplaceLen);

	aTTF.lpstrText = const_cast<char *>(aFind.c_str());

	do
	{
		nPos = SendSciMsg(SCI_FINDTEXT, nSearchFlags, (LPARAM)&aTTF);

		if (nPos != -1)
		{
			nCount++;
			LOGINFO("found in pos: %d", nPos);
			//pos = pos - iFindLen;
			//SendSciMsg(SCI_SETTARGETSTART, pos, 0);
			//SendSciMsg(SCI_SETTARGETEND, pos + iFindLen, 0);
			//SendSciMsg(SCI_REPLACETARGETRE, -1, (LPARAM)replace);

			SendSciMsg(SCI_SETSEL, nPos, nPos + nFindLen);
			SendSciMsg(SCI_REPLACESEL, NULL, (LPARAM)szReplace);
			//SendSciMsg(SCI_REPLACESEL, 0, (LPARAM)sReplace.c_str());

			aTTF.chrg.cpMin = nPos - nReplaceLen;
		}
	} while (nPos != -1);

	LOGINFO("%d was found and replaced!", nCount);

	return nCount;
}

bool CContentsViewer::SelectLines(int nFromline, int nEndline)
{
	int nCurrentPos = SendSciMsg(SCI_POSITIONFROMLINE, nFromline - 1);

	int nAnchorPos = SendSciMsg(SCI_POSITIONFROMLINE, nEndline - 1);

	SendSciMsg(SCI_SETSEL, nCurrentPos, nAnchorPos);

	return true;
}

int CContentsViewer::GetLine(int nLine, wstring &wLine)
{
	int nLen = SendSciMsg(SCI_GETLINE, nLine - 1);

	char *szText = new char[nLen + 1];
	memset(szText, 0, nLen + 1);

	SendSciMsg(SCI_GETLINE, nLine - 1, (LPARAM)szText);

	wLine = AtoW(szText, m_nCodePage);

	int nLinelen = wcslen(wLine.c_str());

	LOGINFO(L"curretn nLine : %s", wLine.c_str());
	delete[] szText;

	return nLinelen + 1;
}

int CContentsViewer::GetCurLineNum()
{
	return SendNPPMsg(NPPM_GETCURRENTLINE) + 1;
}

bool CContentsViewer::GetSelection(wstring &wStr)
{
	int nLen = SendSciMsg(SCI_GETSELTEXT);

	char *szText = new char[nLen];
	SendSciMsg(SCI_GETSELTEXT, NULL, (LPARAM)szText);

	wStr = AtoW(szText, m_nCodePage);
	delete[] szText;
	return true;
}

bool CContentsViewer::ReplaceSelection(const wchar_t *wszStr)
{
	string aStr = WtoA(wszStr, m_nCodePage);

	SendSciMsg(SCI_REPLACESEL, NULL, (LPARAM)aStr.c_str());

	return true;
}

bool CContentsViewer::GetDocument(wstring &wDoc)
{
	int nLen = SendSciMsg(SCI_GETTEXT);

	char *szText = new char[nLen + 1];
	SendSciMsg(SCI_GETTEXT, (WPARAM)(nLen + 1), (LPARAM)szText);

	wDoc = AtoW(szText, m_nCodePage);
	delete[] szText;
	return true;
}

bool CContentsViewer::SetDocument(const wchar_t *wszDoc)
{
	// int len = SendSciMsg(SCI_GETLENGTH, 0, 0);

	string aText = WtoA(wszDoc, m_nCodePage);

	SendSciMsg(SCI_SETTEXT, NULL, (LPARAM)aText.c_str());

	return true;
}

//void CContentsViewer::MultiByte2WideChar(const char* mb, wstring& wc)
//{
//	int len = ::MultiByteToWideChar(CP_ACP, 0, mb, -1, NULL, 0);
//	wchar_t * twc = new wchar_t[len];
//
//	::MultiByteToWideChar(CP_ACP, 0, mb, -1, twc, len);
//	wc = twc;
//	delete [] twc;
//}
//
//void CContentsViewer::WideChar2MultiByte(const wchar_t* wc, string& mb)
//{
//	int len = ::WideCharToMultiByte(CP_ACP, 0, wc, -1, NULL, NULL, NULL, NULL);
//
//	char* tmb = new char[len];
//
//	::WideCharToMultiByte(CP_ACP, 0, wc, -1, tmb, len, NULL, NULL);
//	mb = tmb;
//	delete[] tmb;
//}

void CContentsViewer::GetNPPDirectory(wchar_t *wszNPPDir)
{
	SendNPPMsg(NPPM_GETNPPDIRECTORY, (WPARAM)MAX_PATH, (LPARAM)wszNPPDir);
	PrintDebugString(L"nppDir: %s", wszNPPDir);
}

void CContentsViewer::GetPluginsDir(wchar_t *wszPluginsDir)
{
	SendNPPMsg(NPPM_GETNPPDIRECTORY, (WPARAM)MAX_PATH, (LPARAM)wszPluginsDir);
	PrintDebugString(L"pluginsConfDir: %s", wszPluginsDir);
}

void CContentsViewer::GetPluginsConfigDir(wchar_t *wszPluginsCfgDir)
{
	SendNPPMsg(NPPM_GETPLUGINSCONFIGDIR, (WPARAM)MAX_PATH, (LPARAM)wszPluginsCfgDir);
	PrintDebugString(L"pluginsConfDir: %s", wszPluginsCfgDir);
}

void CContentsViewer::SaveCurFile()
{
	SendNPPMsg(NPPM_SAVECURRENTFILE, 0, 0);
}

void CContentsViewer::UpdateCurFileCodePage()
{
	m_nCodePage = SendSciMsg(SCI_GETCODEPAGE);
	LOGINFO("Cur File codepage: %d.", m_nCodePage);
	//SendSciMsg(SCI_SETCODEPAGE, 936, 0);
}

int CContentsViewer::GetCurFileEncoding()
{
	//Returns active document buffer ID
	int nBufferID = SendNPPMsg(NPPM_GETCURRENTBUFFERID);
	//Get document's encoding from given buffer ID.

	int nEncoding = SendNPPMsg(NPPM_GETBUFFERENCODING, nBufferID);
	LOGINFO("Cur File encoding: %d.", nEncoding);

	return nEncoding;
}

void CContentsViewer::SetCurFileEncoding(int nEncoding)
{
	//Returns active document buffer ID
	int nBufferID = SendNPPMsg(NPPM_GETCURRENTBUFFERID);
	//Get document's encoding from given buffer ID.
	SendNPPMsg(NPPM_SETBUFFERENCODING, nBufferID, nEncoding);
}

void CContentsViewer::CallNPPCmd(int nCommandID)
{
	//call any the Notepad++ menu commands.
	SendNPPMsg(NPPM_MENUCOMMAND, NULL, nCommandID);
	//IDM_FORMAT_CONV2_ANSI
}

void CContentsViewer::trigerAction(UINT uID)
{
	if (IsCVDlgVisable())
	{
		WPARAM wParam = (WPARAM)uID;
		//UINT uMsg = LOWORD(wParam);
		//::SendMessage(thePlugin.GetCVDlgHWnd(), uID, NULL, NULL);
		::SendMessage(thePlugin.GetCVDlgHWnd(), WM_COMMAND, wParam, NULL);
	}
	else
	{
		::MessageBoxW(
			m_hMainWnd,
			L"Please open contents viewer plugin first!",
			NPP_PLUGIN_NAME,
			MB_OK);
	}
}

INT_PTR CContentsViewer::NotifyFromCVD(HWND hWndFrom, UINT uCode)
{
	if (hWndFrom == m_NPPData._nppHandle)
	{
		if (uCode == DMN_CLOSE)
		{
			//SendNPPMsg(NPPM_SETMENUITEMCHECK, funcItem[action_index[Action::openContentsViewerDlg]]._cmdID, FALSE);
			CheckOpenCVDOrNot(false);
			return FALSE;
		}
	}
	return TRUE;
}

void CContentsViewer::CheckOpenCVDOrNot(bool bCheck)
{
	SendNPPMsg(NPPM_SETMENUITEMCHECK, funcItem[action_index[Action::openContentsViewerDlg]]._cmdID, bCheck);
}