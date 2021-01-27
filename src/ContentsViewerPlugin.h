#ifndef CCONTENTSVIEWERPLUGIN_H
#define CCONTENTSVIEWERPLUGIN_H

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
	HWND GetCVDlgHwnd();

	const NppData& GetNppData() const { return mNppData; }
	void SetNppData(const NppData& nppd);

	const HWND GetMainHwnd() const { return mHMainWnd; }
	void SetMainHwnd(HWND hWnd) { mHMainWnd = hWnd; }

	HWND GetEditHwnd() const;

	//NPP MSG
	void OnFileActivated();
	void OnFileSaved();
	void OnFileOpened();
	void OnFileClosed();
	void OnNppTBModification();
	void OnNppReady();
	void OnSelectionChanged();

	HICON GetTabIcon() const {return mHTabIcon;}

	int IndexContents(const TCHAR* tszText, const TCHAR* tszKeyword, int level);
	int FindAndReplace(const TCHAR* tszFind, const TCHAR* tszReplace, bool isRegularMode);
	void GotoLine(int line);
	void DeleteLine(int line);

	void CutLines(int lineStart, int lineEnd);
	void PasteBeforeLine(int line);
	void ReplaceLine(int line, const TCHAR* tszTxt);
	void ReparseCurrentFile();
	int GetLine(int line, tString& tLine);
	int GetMaxLine();
	int GetCurrentLine();
	bool SelectLines(int fromline, int endline);
	bool GetSelection(tString &str);
	bool ReplaceSelection(tString tStr);
	bool GetDocument(tString &file);
	bool SetDocument(const tString &file);

	LRESULT SendNppMsg(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0) const;
	LRESULT SendNppMsg(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);

	TCHAR* GetNppDirectory();
	TCHAR* GetPluginsConfigDir();
	void SaveCurrentFile();

	void UpdateCurrentFileCodePage();
	int GetCurrentFileEncoding();
	void SetCurrentFileEncoding(int encoding);

	void CallNppCommand(int commandID);

	void ActiveContentListviewItem();

private:	
	//void MultiByte2WideChar(const char* mb, tString& wc);
	//void WideChar2MultiByte(const TCHAR* wc, string& mb);
	bool m_bDebug;

protected:
	CContentsViewerDialog mCVDlg;
	NppData      mNppData;
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

#endif//CCONTENTSVIEWERPLUGIN_H