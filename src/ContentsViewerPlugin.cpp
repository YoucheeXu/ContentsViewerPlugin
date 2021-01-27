#include "ContentsViewerPlugin.h"
#include <regex>
#include "resource.h"

using namespace regex_constants;
using namespace std;

extern FuncItem funcItem[nbFunc];
//extern NPPData nppData;

// global vars
//bool g_bDebug;
CLogFile* g_pLogFile;

CContentsViewerPlugin thePlugin;

CContentsViewerPlugin::CContentsViewerPlugin()
{
	g_pLogFile = new CLogFile();
	m_nCodePage = CP_ACP;
	m_bDebug = false;
}

CContentsViewerPlugin::~CContentsViewerPlugin()
{
	Uninitialize();
	delete g_pLogFile;
}

void CContentsViewerPlugin::Initialize(HINSTANCE hInstance)
{
	SetResourceHandle(hInstance);
	mHTabIcon = (HICON)::LoadImage(hInstance, 
		MAKEINTRESOURCE(IDB_CONTENTSVIEWER), IMAGE_ICON, 0, 0,
		LR_LOADMAP3DCOLORS| LR_LOADTRANSPARENT);
	//m_TB_Icon.hToolbarBmp = (HBITMAP)::LoadImage(hInstance,
	// MAKEINTRESOURCE(IDB_TAGSVIEW), IMAGE_BITMAP, 0, 0, 
	// LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS);
}

void CContentsViewerPlugin::Uninitialize()
{
	if (mHTabIcon)
	{
		::DestroyIcon(mHTabIcon);
		mHTabIcon = NULL;
	}
	//if ( m_TB_Icon.hToolbarBmp )
	//{
	//    ::DeleteObject(m_TB_Icon.hToolbarBmp);
	//    m_TB_Icon.hToolbarBmp = NULL;
	//}
}

BOOL CContentsViewerPlugin::IsCVDlgVisable()
{
	return mCVDlg.IsWindowVisible();
	//return mCVDlg.IsWindow();
}

bool CContentsViewerPlugin::IsCVDlgExist()
{
	//return (mCVDlg.GetHWnd() == NULL);
	return (mCVDlg.IsWindow() == TRUE);
}

void CContentsViewerPlugin::ShowCVDlg()
{
	mCVDlg.DoModeless();
}

HWND CContentsViewerPlugin::GetCVDlgHWnd()
{
	return mCVDlg.GetHwnd();
}

void CContentsViewerPlugin::SetNPPData(const NPPData& nppd)
{
	mNPPData = nppd;
	mCVDlg.SetNPPWnd(nppd._nppHandle);
}

HWND CContentsViewerPlugin::GetEditHWnd() const
{
	// Get the current scintilla
	int currentView = -1;
	SendNPPMsg(NPPM_GETCURRENTSCINTILLA, 0, (LPARAM) &currentView);
	if (currentView == -1)
	{
		LOGERR("Can't get current scintilla!");
		assert(NULL);
		return NULL;
	}
	return ((currentView == 0)? mNPPData._scintillaMainHandle: mNPPData._scintillaSecondHandle);
}

//NPPN_BUFFERACTIVATED
void CContentsViewerPlugin::OnFileActivated()
{
	if (IsCVDlgExist())
	{
		//if(IsCVDlgVisable()) thePlugin.ReParseCurFile();
		if(IsCVDlgVisable()) ParseCurFile();
	}
}

//NPPN_FILESAVED
void CContentsViewerPlugin::OnFileSaved()
{
}

//NPPN_FILEOPENED
void CContentsViewerPlugin::OnFileOpened()
{
	/*if (IsCVDlgExist())
	{
		if (IsCVDlgVisable())
			thePlugin.ReParseCurFile();
	}*/
}

//NPPN_FILEBEFORECLOSE

//NPPN_FILECLOSED
void CContentsViewerPlugin::OnFileClosed()
{
}

//NPPN_TBMODIFICATION
void CContentsViewerPlugin::OnNPPTBModification()
{
	/*toolbarIcons g_TBHex;
	//g_TBHex.hToolbarBmp = (HBITMAP)::LoadImage((HINSTANCE)g_hModule, MAKEINTRESOURCE(IDB_TB_HEX), IMAGE_BITMAP, 0, 0, (LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS));
	g_TBHex.hToolbarBmp = mHTabIcon;
	SendNppMsg(NPPM_ADDTOOLBARICON, (WPARAM)funcItem[0]._cmdID, (LPARAM)&g_TBHex);*/
}

//NPPN_READY
void CContentsViewerPlugin::OnNPPReady()
{
	//const HWND hSciEdit = GetEditHWnd();
	//::SendMessage(hSciEdit, SCI_SETCODEPAGE, 936, 0);
}

//SCN_PAINTED
void CContentsViewerPlugin::OnSelectionChanged()
{
}

LRESULT CContentsViewerPlugin::SendNPPMsg(UINT uMsg, WPARAM wParam , LPARAM lParam ) const
{
	return ::SendMessage(mNPPData._nppHandle, uMsg, wParam, lParam);
}

LRESULT CContentsViewerPlugin::SendNPPMsg(UINT uMsg, WPARAM wParam , LPARAM lParam )
{
	return ::SendMessage(mNPPData._nppHandle, uMsg, wParam, lParam);
}

int CContentsViewerPlugin::GetLineCount()
{
	const HWND hSciEdit = GetEditHWnd();
	return (int)::SendMessage(hSciEdit, SCI_GETLINECOUNT, 0, 0);
}

void CContentsViewerPlugin::GotoLine(int line)
{
	mCVDlg.SavePos(line);
	/*if (line == GetLineCount())
		line = line + 5;
	else if(line >= 5)
		line = line - 5;*/

	const HWND hSciEdit = GetEditHWnd();
	if(NULL == hSciEdit) return;

	::SendMessage(hSciEdit, WM_SETREDRAW, (WPARAM) FALSE, 0);

	//This message sets both the anchor and the current position. If currentPos is negative, it means the end of the document. If anchorPos is negative, it means remove any selection (i.e. set the anchor to the same position as currentPos). The caret is scrolled into view after this operation.
	//::SendMessage(hSciEdit, SCI_SETSEL, selStart, selEnd);

	//::SendMessage(hSciEdit, SCI_ENSUREVISIBLE, line - 1, 0);

	LOGOUT("DesLine: %d\n", line);
	
	int curLineNo = GetCurLineNo();
	LOGOUT("curLineNo: %d\n", curLineNo);
	//int firstVisibleLine = 0;
	auto linesOnScreen = 0;
	int desLine = 0;
	linesOnScreen = (int)::SendMessage(hSciEdit, SCI_LINESONSCREEN, 0, 0);
	LOGOUT("linesOnScreen: %d\n", linesOnScreen);
	if (curLineNo < line)
	{
		desLine = linesOnScreen / 2 + line;
	}
	else
	{
		desLine = line - linesOnScreen / 2;
	}
	//This removes any selection and sets the caret at the start of line number line and scrolls the view (if needed) to make it visible. The anchor position is set the same as the current position. If line is outside the lines in the document (first line is 0), the line set is the first or last.
	::SendMessage(hSciEdit, SCI_GOTOLINE, desLine, 0);

	////These messages retrieve and set the line number of the first visible line in the Scintilla view. The first line in the document is numbered 0. The value is a visible line rather than a document line.
	//int firstVisibleLine = ::SendMessage(hSciEdit, SCI_GETFIRSTVISIBLELINE, 0, 0);
	//::SendMessage(hSciEdit, SCI_SETFIRSTVISIBLELINE, desLine, 0);

	//This will attempt to scroll the display by the number of columns and lines that you specify. Positive line values increase the line number at the top of the screen (i.e. they move the text upwards as far as the user is concerned), Negative line values do the reverse.
	//::SendMessage(hSciEdit, SCI_LINESCROLL, 0, firstLine);

	::SendMessage(hSciEdit, WM_SETREDRAW, (WPARAM) TRUE, 0);
	::InvalidateRect(hSciEdit, NULL, TRUE);
	::UpdateWindow(hSciEdit);  
}

void CContentsViewerPlugin::DelLine(int line)
{
	line = line - 1;
	const HWND hSciEdit = GetEditHWnd();
	int pos = (int)::SendMessage(hSciEdit, SCI_POSITIONFROMLINE, line, 0);
	unsigned int len = ::SendMessage(hSciEdit, SCI_GETLINE, line, 0);
	::SendMessage(hSciEdit, SCI_DELETERANGE, pos, len);	
}

void CContentsViewerPlugin::CutLines(int lineStart, int lineEnd)
{
	const HWND hSciEdit = GetEditHWnd();
	int anchorPos = (int)::SendMessage(hSciEdit, SCI_POSITIONFROMLINE, lineStart - 1, 0);
	int currentPos = (int)::SendMessage(hSciEdit, SCI_POSITIONFROMLINE, lineEnd - 1 + 1, 0);
	//unsigned int len = ::SendMessage(hSciEdit, SCI_GETLINE, lineEnd - 1, 0);
	//currentPos = currentPos + len;
	::SendMessage(hSciEdit, SCI_SETSEL, anchorPos, currentPos);
	::SendMessage(hSciEdit, SCI_CUT, 0, 0);
	//SCI_POSITIONFROMLINE(int line)
	//SCI_SETSEL(int anchorPos, int currentPos)
	//SCI_CUT
}

void CContentsViewerPlugin::PasteBeforeLine(int line)
{
	const HWND hSciEdit = GetEditHWnd();
	int pos = (int)::SendMessage(hSciEdit, SCI_POSITIONFROMLINE, line - 1, 0);
	::SendMessage(hSciEdit, SCI_GOTOPOS, pos, 0);
	::SendMessage(hSciEdit, SCI_PASTE, 0, 0);
}

void CContentsViewerPlugin::ReplaceLine(int line, const TCHAR* tszTxt)
{
	LOGFUNBGN;
	const HWND hSciEdit = GetEditHWnd();

	string sLine = TtoA(tszTxt, m_nCodePage);
	char* szLine = const_cast<char*>(sLine.c_str());

	int currentPos = ::SendMessage(hSciEdit, SCI_POSITIONFROMLINE, line - 1, 0);
	//int currentPos = ::SendMessage(hSciEdit, SCI_POSITIONFROMLINE, line + 1, 0);
	unsigned int len = ::SendMessage(hSciEdit, SCI_GETLINE, line - 1, 0);
	int anchorPos = currentPos + len;
	LOGINFO(_T("currentPos = %d, anchorPos = %d"), currentPos, anchorPos);
	::SendMessage(hSciEdit, SCI_SETSEL, anchorPos, currentPos);
	//MessageBox(NULL, _T("haha!"), _T(""), MB_OK);
	::SendMessage(hSciEdit, SCI_REPLACESEL, 0, (LPARAM)szLine);
	LOGFUNEND;
}

void CContentsViewerPlugin::ParseCurFile()
{
	//LOGINFO(_T("CContentsViewerPlugin::ParseCurFile()"));
	//mCVDlg.OnParse();
	mCVDlg.ParseCurFile();
	mCVDlg.UpdateContentListview();
}

void CContentsViewerPlugin::ReParseCurFile()
{
	LOGFUNBGN;
	LOGINFO(_T("CContentsViewerPlugin::ReParseCurFile()"));
	//mCVDlg.ReParseCurFile();
	//mCVDlg.ParseCurFile();
	mCVDlg.ParseCurFile();
	mCVDlg.UpdateContentListview();
	LOGFUNEND;
}

int CContentsViewerPlugin::IndexContents(const TCHAR* tText, const TCHAR* tszKeyword, int level)
{
	LOGFUNBGN;

	//LOGINFO(_T("REGEXP: %s, keyword %s, level: %d"), tText, tszKeyword, level);
	const HWND hSciEdit = GetEditHWnd();
	int searchFlags = SCFIND_REGEXP + SCFIND_POSIX;
	//int searchFlags = SCFIND_MATCHCASE;

	TextToFind ttf;
	int pos = 0;
	// int n = 0;
	int i = 0;
	ttf.chrg.cpMin = 0;
	ttf.chrg.cpMax = ::SendMessage(hSciEdit, SCI_GETLENGTH, 0, 0);

	string text;

	//if (936 == m_nCodePage) text = T2A(tText, CP_ACP);
	//else text = T2A(tText, 65001);
	text = TtoA(tText, m_nCodePage);

	//int lenText = text.length();

	//char* lpstrText = new char[256];
	//memset(lpstrText, 0, 256);
	//strcpy(lpstrText, text.c_str());

	ttf.lpstrText = const_cast<char*>(text.c_str());
	//ttf.lpstrText = lpstrText;
	//ttf.lpstrText = szText;

	//strcpy(ttf.lpstrText, text.c_str());

	LOGINFO("ttf.lpstrText: %s.\n", ttf.lpstrText);

	tString tContent;

	do{
		pos = ::SendMessage(hSciEdit, SCI_FINDTEXT, searchFlags, (LPARAM)&ttf);

		if(pos != -1)
		{
			i++;
			LOGINFO(_T("Content is found at: %d"), pos);
			int line = ::SendMessage(hSciEdit, SCI_LINEFROMPOSITION, pos, 0);

			unsigned int len = ::SendMessage(hSciEdit, SCI_GETLINE, line, 0);
			ttf.chrg.cpMin = pos + len;
			char *chText = new char[len + 1];
			memset(chText, 0, len + 1);

			::SendMessage(hSciEdit, SCI_GETLINE, line, (LPARAM)chText);

			tContent = AtoT(chText, m_nCodePage);

			LOGINFO(_T("Length of %s be cuted : %u"), tContent.c_str(), tContent.size());

			mCVDlg.AddContent(i, tContent.c_str(), line + 1, level, tszKeyword);
			delete [] chText;

		}
	}while(pos != -1);

	//delete[] lpstrText;

	LOGFUNEND;

	return i;
}

void  CContentsViewerPlugin::ActiveContentListviewItem()
{
	if(IsCVDlgExist())
	{
		if(IsCVDlgVisable())
		{
			int line = GetCurLineNo();
			mCVDlg.FocuseContentListviewItemByLine(line);
		}
	}
}

/*******************************************************************************
 *  desc: find and replace text file
 *------------------------------------------------------------------------------
 * param: const TCHAR* tFind  -- string to find
 * const TCHAR* tReplace  -- string to replace
 * bool isRegularMode -- find in regular mode or not
 *------------------------------------------------------------------------------
 * return: int  -- number of find and replace
*******************************************************************************/
//wait to test
int CContentsViewerPlugin::FindAndReplace(const TCHAR* tszFind, const TCHAR* tszReplace, bool isRegularMode)
{
	LOGFUNBGN;

	LOGINFO(_T("tFind: %s, tReplace: %s. regularMode: %d."), tszFind, tszReplace, isRegularMode);

	const HWND hSciEdit = GetEditHWnd();
	int searchFlags;

	/*#define SCFIND_WHOLEWORD 2
	#define SCFIND_MATCHCASE 4
	#define SCFIND_WORDSTART 0x00100000
	#define SCFIND_REGEXP 0x00200000
	#define SCFIND_POSIX 0x00400000*/
	if(true == isRegularMode)
		searchFlags = SCFIND_REGEXP + SCFIND_POSIX;
		//searchFlags = SCFIND_CXX11REGEX;
	else
		//searchFlags = SCFIND_WORDSTART;
		searchFlags = SCFIND_MATCHCASE;

	int count = 0;
	TextToFind ttf;
	int pos = 0;
	// int i = 0;
	ttf.chrg.cpMin = 0;
	ttf.chrg.cpMax = ::SendMessage(hSciEdit, SCI_GETLENGTH, 0, 0);

	string sFind = TtoA(tszFind, m_nCodePage);

	string sReplace = TtoA(tszReplace, m_nCodePage);

	//char* szReplace = const_cast<char*>(sReplace.c_str());

	char szReplace[] = "$";

	int iFindLen = _tcslen(tszFind);
	int iReplaceLen = _tcslen(tszReplace);
	
	LOGINFO(_T("iFindLen: %d, iReplaceLen: %d."), iFindLen, iReplaceLen);
	
	ttf.lpstrText = const_cast<char*>(sFind.c_str());

	do
	{
		pos = ::SendMessage(hSciEdit, SCI_FINDTEXT, searchFlags, (LPARAM)&ttf);

		if(pos != -1)
		{
			count++;
			LOGINFO(_T("found in pos: %d"), pos);
			//pos = pos - iFindLen;
			//::SendMessage(hSciEdit, SCI_SETTARGETSTART, pos, 0);
			//::SendMessage(hSciEdit, SCI_SETTARGETEND, pos + iFindLen, 0);
			//::SendMessage(hSciEdit, SCI_REPLACETARGETRE, -1, (LPARAM)replace);

			::SendMessage(hSciEdit, SCI_SETSEL, pos, pos + iFindLen);
			::SendMessage(hSciEdit, SCI_REPLACESEL, 0, (LPARAM)szReplace);
			//::SendMessage(hSciEdit, SCI_REPLACESEL, 0, (LPARAM)sReplace.c_str());
			
			ttf.chrg.cpMin = pos - iReplaceLen;
		}
	}while(pos != -1);

	//delete[] find;
	//delete[] replace;
	LOGINFO(_T("%d was found and replaced!\n"), count);
	LOGFUNEND;
	return count;
}

bool CContentsViewerPlugin::SelectLines(int fromline, int endline)
{
	LOGFUNBGN;	
	
	const HWND hSciEdit = GetEditHWnd();
	
	int currentPos = ::SendMessage(hSciEdit, SCI_POSITIONFROMLINE, fromline - 1, 0);
	//int currentPos = ::SendMessage(hSciEdit, SCI_POSITIONFROMLINE, line + 1, 0);
	//unsigned int len = ::SendMessage(hSciEdit, SCI_GETLINE, line - 1, 0);
	
	int anchorPos = ::SendMessage(hSciEdit, SCI_POSITIONFROMLINE, endline - 1, 0);
	
	::SendMessage(hSciEdit, SCI_SETSEL, currentPos, anchorPos);

	LOGFUNEND;
	return true;	
}

int CContentsViewerPlugin::GetLine(int line, tString& tLine)
{
	LOGFUNBGN;
	const HWND hSciEdit = GetEditHWnd();

	int len = ::SendMessage(hSciEdit, SCI_GETLINE, line - 1, 0);

	char *chText = new char[len + 1];
	memset(chText, 0, len + 1);

	::SendMessage(hSciEdit, SCI_GETLINE, line - 1, (LPARAM)chText);

	tLine = AtoT(chText, m_nCodePage);
	//MultiByte2WideChar(chText, tLine);

	int linelen = _tcslen(tLine.c_str());

	LOGINFO(_T("curretn line : %s"), tLine.c_str());
	delete [] chText;
	
	//delete szLine;
	LOGFUNEND;
	return linelen + 1;
}

int CContentsViewerPlugin::GetCurLineNo()
{
	return SendNPPMsg(NPPM_GETCURRENTLINE, 0, 0) + 1;
}

bool CContentsViewerPlugin::GetSelection(tString &str)
{
	// bool bDebug = true;
	//int anchorPos = 0, currentPos = 0;
	////SendNPPMsg(SCI_GETCURRENTPOS, 0, currentPos);
	//SendNPPMsg(SCI_GETSELECTIONSTART, 0, anchorPos);
	////SendNPPMsg(SCI_GETANCHOR, 0, anchorPos);
	//SendNPPMsg(SCI_GETSELECTIONEND, 0, currentPos);
	//char* text = new char[abs(currentPos - anchorPos)];

	const HWND hSciEdit = GetEditHWnd();

	int len = ::SendMessage(hSciEdit, SCI_GETSELTEXT, 0, 0);

	char* text = new char[len];
	::SendMessage(hSciEdit, SCI_GETSELTEXT, (WPARAM)0, (LPARAM)text);

	str = AtoT(text, m_nCodePage);
	delete [] text;
	return true;
}

bool CContentsViewerPlugin::ReplaceSelection(tString tStr)
{
	string str;
	//WideChar2MultiByte(tStr.c_str(), str);
	str = TtoA(tStr.c_str(), m_nCodePage);

	const HWND hSciEdit = GetEditHWnd();
	::SendMessage(hSciEdit, SCI_REPLACESEL, (WPARAM)0, (LPARAM)str.c_str());

	return true;
}

bool CContentsViewerPlugin::GetDocument(tString &file)
{
	// bool bDebug = true;

	const HWND hSciEdit = GetEditHWnd();

	int len = ::SendMessage(hSciEdit, SCI_GETLENGTH, 0, 0);

	char* text = new char[len + 1];
	::SendMessage(hSciEdit, SCI_GETTEXT, (WPARAM)(len + 1), (LPARAM)text);

	file = AtoT(text, m_nCodePage);
	delete[] text;
	return true;
}

bool CContentsViewerPlugin::SetDocument(const tString &file)
{
	// bool bDebug = true;

	const HWND hSciEdit = GetEditHWnd();

	int len = ::SendMessage(hSciEdit, SCI_GETLENGTH, 0, 0);

	string text = TtoA(file.c_str(), m_nCodePage);

	::SendMessage(hSciEdit, SCI_SETTEXT, 0, (LPARAM)text.c_str());

	return true;
}

//void CContentsViewerPlugin::MultiByte2WideChar(const char* mb, tString& wc)
//{
//	int len = ::MultiByteToWideChar(CP_ACP, 0, mb, -1, NULL, 0);
//	wchar_t * twc = new wchar_t[len];
//
//	::MultiByteToWideChar(CP_ACP, 0, mb, -1, twc, len);
//	wc = twc;
//	delete [] twc;
//}
//
//void CContentsViewerPlugin::WideChar2MultiByte(const TCHAR* wc, string& mb)
//{
//	int len = ::WideCharToMultiByte(CP_ACP, 0, wc, -1, NULL, NULL, NULL, NULL);
//
//	char* tmb = new char[len];
//
//	::WideCharToMultiByte(CP_ACP, 0, wc, -1, tmb, len, NULL, NULL);
//	mb = tmb;
//	delete[] tmb;
//}

TCHAR* CContentsViewerPlugin::GetNPPDirectory()
{
	LOGFUNBGN;
	TCHAR nppDir[MAX_PATH];
	SendNPPMsg(NPPM_GETNPPDIRECTORY, (WPARAM)MAX_PATH, (LPARAM)nppDir);
	LOGINFO(_T("nppDir: %s"), nppDir);
	return nppDir;
}

TCHAR* CContentsViewerPlugin::GetPluginsConfigDir()
{
	// TCHAR pluginsConfDir[MAX_PATH];
	char pluginsConfDir[MAX_PATH];
	SendNPPMsg(NPPM_GETPLUGINSCONFIGDIR, (WPARAM)MAX_PATH, (LPARAM)pluginsConfDir);
	LOGINFO(_T("pluginsConfDir: %s"), pluginsConfDir);
	return (TCHAR*)pluginsConfDir;
}

void CContentsViewerPlugin::SaveCurFile()
{
	SendNPPMsg(NPPM_SAVECURRENTFILE, 0, 0);
}

void CContentsViewerPlugin::UpdateCurFileCodePage()
{
	const HWND hSciEdit = GetEditHWnd();
	m_nCodePage = ::SendMessage(hSciEdit, SCI_GETCODEPAGE, 0, 0);
	LOGINFO(_T("Cur File codepage: %d."), m_nCodePage);
	//::SendMessage(hSciEdit, SCI_SETCODEPAGE, 936, 0);
}

int CContentsViewerPlugin::GetCurFileEncoding()
{
	//Returns active document buffer ID
	int bufferID = SendNPPMsg(NPPM_GETCURRENTBUFFERID, 0 ,0);
	//Get document's encoding from given buffer ID.

	int encoding = SendNPPMsg(NPPM_GETBUFFERENCODING, bufferID, 0);
	LOGINFO(_T("Cur File encoding: %d."), encoding);

	return encoding;
}

void CContentsViewerPlugin::SetCurFileEncoding(int encoding)
{
	//Returns active document buffer ID
	int bufferID = SendNPPMsg(NPPM_GETCURRENTBUFFERID, 0 ,0);
	//Get document's encoding from given buffer ID.
	SendNPPMsg(NPPM_SETBUFFERENCODING, bufferID, encoding);
}

void CContentsViewerPlugin::CallNPPCmd(int commandID)
{
	//call any the Notepad++ menu commands.
	SendNPPMsg(NPPM_MENUCOMMAND, 0, commandID);
	//IDM_FORMAT_CONV2_ANSI
}

//----------------------------------------------------------------------------

//// global vars
//CContentsViewerPlugin thePlugin;
//CLogFile* g_pLogFile;

//HANDLE g_hMod;
//CHAR* curText = NULL;
//CContentsViewerDialog contentsViewerDlg;

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
//NPPData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
//void pluginInit(HANDLE /*hModule*/)
//{
//	g_hMod = hModule;
//	contentsViewerDlg.init((HINSTANCE)g_hMod, nppData._nppHandle);
//}

//

// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

	//--------------------------------------------//
	//-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
	//--------------------------------------------//
	// with function :
	// setCommand(int index,                      // zero based number to indicate the order of command
	//            TCHAR *commandName,             // the command name that you want to see in plugin menu
	//            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
	//            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
	//            bool check0nInit                // optional. Make this menu item be checked visually
	//            );
	ShortcutKey *sk = new ShortcutKey();
	sk->_isAlt = TRUE;
	sk->_isCtrl = TRUE;
	sk->_isShift = TRUE;
	sk->_key = 0x4A;
	setCommand(0, TEXT("Show &Contents Viewer"), openContentsViewerDlg, sk, false);
	setCommand(1, TEXT("&About"), openAboutDlg, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
	delete funcItem[0]._pShKey;
}

//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
	if (index >= nbFunc) return false;

	if (!pFunc) return false;

	lstrcpy(funcItem[index]._itemName, cmdName);
	funcItem[index]._pFunc = pFunc;
	funcItem[index]._init2Check = check0nInit;
	funcItem[index]._pShKey = sk;

	return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void openContentsViewerDlg()
{
	static tTbData dockData;
	static TCHAR szPluginName[64];

	//bool bParseFile = false;

	if (thePlugin.IsCVDlgExist())
	{
		UINT uMsg = NPPM_DMMSHOW;
		BOOL bCheck = TRUE;
		if (thePlugin.IsCVDlgVisable())
		{
			uMsg = NPPM_DMMHIDE;
			bCheck = FALSE;
			//bParseFile = true;
		}

		//show the dialog
		thePlugin.SendNPPMsg(uMsg, 0, (LPARAM) dockData.hClient );
		//set the check on menu item
		//thePlugin.SendNPPMsg(NPPM_SETMENUITEMCHECK, 
		//CContentsViewerDialog::FUNC_ARRAY[CContentsViewerDialog::EFI_TAGSVIEW]._cmdID, bCheck );
		thePlugin.SendNPPMsg(NPPM_SETMENUITEMCHECK, funcItem[0]._cmdID,  bCheck);
	}
	else
	{
		lstrcpy(szPluginName, NPP_PLUGIN_NAME);
		thePlugin.ShowCVDlg();

		//dockData.hClient = thePlugin.GetCVDlg().GetHWnd();
		dockData.hClient = thePlugin.GetCVDlgHWnd();
		dockData.pszName = szPluginName;
		dockData.dlgID = -1;
		dockData.uMask = DWS_DF_CONT_LEFT | DWS_ICONTAB;
		dockData.hIconTab = thePlugin.GetTabIcon();
		dockData.pszAddInfo = NULL;
		dockData.rcFloat.left = 0;
		dockData.rcFloat.top = 0;
		dockData.rcFloat.right = 0;
		dockData.rcFloat.bottom = 0;
		dockData.iPrevCont = -1;
		dockData.pszModuleName = NPP_PLUGIN_NAME;

		//set dialog dockable
		thePlugin.SendNPPMsg(NPPM_DMMREGASDCKDLG, 0, (LPARAM) &dockData);
		//register dialog
		thePlugin.SendNPPMsg(NPPM_MODELESSDIALOG, MODELESSDIALOGADD, (LPARAM) dockData.hClient);
		//show the dialog
		thePlugin.SendNPPMsg(NPPM_DMMSHOW, 0, (LPARAM) dockData.hClient);
		//set the check on menu item
		//thePlugin.SendNPPMsg(NPPM_SETMENUITEMCHECK, 
		//CContentsViewerDialog::FUNC_ARRAY[CContentsViewerDialog::EFI_TAGSVIEW]._cmdID, TRUE);
		thePlugin.SendNPPMsg(NPPM_SETMENUITEMCHECK, funcItem[0]._cmdID, TRUE);
	}

	//if (bParseFile)
	//{
	//	//theLogFile.LogOutA(__FUNCTION__);
	//	thePlugin.ReParseCurFile();
	//}
	thePlugin.ParseCurFile();
}

void openAboutDlg()
{
	//::CreateDialog((HINSTANCE)g_hMod, MAKEINTRESOURCE(IDD_ABOUTDLG), thePlugin.GetMainHWnd(), abtDlgProc);
	::MessageBox(
		thePlugin.GetMainHWnd(),
		_T("Haha!"),
		NPP_PLUGIN_NAME,
		MB_OK);
}

//-----------------------------------------------

extern "C" __declspec(dllexport) void setInfo(NPPData notpadPlusData)
{
	//nppData = notpadPlusData;
	commandMenuInit();
	thePlugin.SetNPPData(notpadPlusData);
	thePlugin.SetMainHWnd(notpadPlusData._nppHandle);
}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
	return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
	*nbF = nbFunc;
	return funcItem;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *pscn)
{
	if (pscn->nmhdr.hwndFrom == thePlugin.GetNPPData()._nppHandle)
	{
		switch (pscn->nmhdr.code)
		{
		case NPPN_BUFFERACTIVATED:
			thePlugin.OnFileActivated();
			break;

		case NPPN_FILESAVED:
			thePlugin.OnFileSaved();
			break;

		case NPPN_FILEOPENED:
			thePlugin.OnFileOpened();
			break;

		case NPPN_FILEBEFORECLOSE:
			//fileToBeClosed = thePlugin.GetFilePathName();
			break;

		case NPPN_FILECLOSED:
			thePlugin.OnFileClosed();
			//fileToBeClosed.clear();
			break;

		case NPPN_TBMODIFICATION:
			thePlugin.OnNPPTBModification();
			break;

		case NPPN_READY:
			thePlugin.OnNPPReady();
			break;
		}
		//LOGOUT("Notification code: %d\n", pscn->nmhdr.code);
	}
	else
	{
		switch (pscn->nmhdr.code)
		{
		case SCN_PAINTED:
			thePlugin.OnSelectionChanged();
			break;
		}
	}
}

// Here you can process the NPP Messages 
// I will make the messages accessible little by little, according to the need of plugin development.
// Please let me know if you need to access to some messages :
// http://sourceforge.net/forum/forum.php?forum_id=482781
//
extern "C" __declspec(dllexport) LRESULT messageProc(UINT message, WPARAM wParam, LPARAM lParam)
{/*
	if (Message == WM_MOVE)
	{
		::MessageBox(NULL, "move", "", MB_OK);
	}
*/ 
	switch (message)
	{
		case WM_MOUSEWHEEL:
			thePlugin.ActiveContentListviewItem();
			break;

		case WM_KEYUP:
			switch (wParam)
			{
				case VK_PRIOR:
				case VK_NEXT:
					thePlugin.ActiveContentListviewItem();
					break;
			}
		break;
	}
	return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
	return TRUE;
}
#endif //UNICODE

//BOOL APIENTRY DllMain( HANDLE hModule, 
//					  DWORD  reasonForCall, 
//					  LPVOID lpReserved )
extern "C" BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	//switch (reasonForCall)
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		//pluginInit(hModule);
		thePlugin.Initialize(hInstance);
		break;

	case DLL_PROCESS_DETACH:
		commandMenuCleanUp();
		//pluginCleanUp();
		thePlugin.Uninitialize();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}