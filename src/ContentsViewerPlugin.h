#pragma once
#ifndef _CCONTENTSVIEWERPLUGIN_H_
#define _CCONTENTSVIEWERPLUGIN_H_

// All difinitions of plugin interface
#include "help.h"
#include "PluginInterface.h"
#include "LogFile.h"
#include "ContentsViewerDialog.h"

//using Win32xx::CWinApp;
using namespace std;

class CContentsViewerPlugin : public CWinApp
{
public:
	CContentsViewerPlugin();
	~CContentsViewerPlugin();

private:
	DISALLOW_COPY_AND_ASSIGN(CContentsViewerPlugin);

public:	
	void Initialize(HINSTANCE hInstance);
	void Uninitialize();

	/*const CContentsViewerDialog& GetCVDlg() const { return mCVDlg; }
    CContentsViewerDialog& GetCVDlg() { return mCVDlg; }*/

	BOOL IsCVDlgVisable();
	bool IsCVDlgExist();
	void ShowCVDlg();
	HWND GetCVDlgHWnd();

	const NPPData& GetNPPData() const {return mNPPData;}
	void SetNPPData(const NPPData& nppd);

	const HWND GetMainHWnd() const { return mHMainWnd; }
	void SetMainHWnd(HWND hWnd) { mHMainWnd = hWnd; }

	HWND GetEditHWnd() const;

	//NPP MSG
	void OnFileActivated();
	void OnFileSaved();
	void OnFileOpened();
	void OnFileClosed();
	void OnNPPTBModification();
	void OnNPPReady();
	void OnSelectionChanged();

	HICON GetTabIcon() const {return mHTabIcon;}

	int IndexContents(const TCHAR* tszText, const TCHAR* tszKeyword, int level);
	int FindAndReplace(const TCHAR* tszFind, const TCHAR* tszReplace, bool isRegularMode);
	void GotoLine(int line);
	void DelLine(int line);

	void CutLines(int lineStart, int lineEnd);
	void PasteBeforeLine(int line);
	void ReplaceLine(int line, const TCHAR* tszTxt);
	void ReparseCurFile();
	int GetLine(int line, tString& tLine);
	int GetLineCount();
	int GetCurLineNo();
	bool SelectLines(int fromline, int endline);
	bool GetSelection(tString &str);
	bool ReplaceSelection(tString tStr);
	bool GetDocument(tString &file);
	bool SetDocument(const tString &file);

	LRESULT SendNPPMsg(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0) const;
	LRESULT SendNPPMsg(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);

	TCHAR* GetNPPDirectory();
	TCHAR* GetPluginsConfigDir();
	void SaveCurFile();

	void UpdateCurFileCodePage();
	int GetCurFileEncoding();
	void SetCurFileEncoding(int encoding);

	void CallNPPCmd(int commandID);

	void ActiveContentListviewItem();

private:	
	//void MultiByte2WideChar(const char* mb, tString& wc);
	//void WideChar2MultiByte(const TCHAR* wc, string& mb);
	bool m_bDebug;

protected:
	CContentsViewerDialog mCVDlg;
	NPPData      mNPPData;
	HWND		mHMainWnd;
	HICON        mHTabIcon;

	UINT m_nCodePage;
};

extern CContentsViewerPlugin thePlugin;

//-------------------------------------//
//-- STEP 1. DEFINE YOUR PLUGIN NAME --//
//-------------------------------------//
// Here define your plugin name
//
const TCHAR NPP_PLUGIN_NAME[] = TEXT("Contents Viewer");

//-----------------------------------------------//
//-- STEP 2. DEFINE YOUR PLUGIN COMMAND NUMBER --//
//-----------------------------------------------//
//
// Here define the number of your plugin commands
//
const int nbFunc = 2;

//
// Initialization of your plugin data
// It will be called while plugin loading
//
void pluginInit(HANDLE hModule);

//
// Cleaning of your plugin
// It will be called while plugin unloading
//
void pluginCleanUp();

//
//Initialization of your plugin commands
//
void commandMenuInit();

//
//Clean up your plugin commands allocation (if any)
//
void commandMenuCleanUp();

//
// Function which sets your command 
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = NULL, bool check0nInit = false);

//
// Your plugin command functions
//
void openContentsViewerDlg();
void openAboutDlg();

#endif	//_CCONTENTSVIEWERPLUGIN_H_