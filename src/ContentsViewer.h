// ContentsViewer.h
#pragma once
#ifndef _CONTENTSVIEWER_H_
#define _CONTENTSVIEWER_H_

// All difinitions of plugin interface
#include "help.h"
#include "LogFile.h"
#include "ContentsViewerDialog.h"

//using Win32xx::CWinApp;
using namespace std;

class CContentsViewer: public CWinApp
{
public:
	CContentsViewer();
	~CContentsViewer();

private:
	DISALLOW_COPY_AND_ASSIGN(CContentsViewer);

public:	
	void Initialize(HINSTANCE hInstance);
	void Uninitialize();

	/*const CContentsViewerDialog& GetCVDlg() const { return mCVDlg; }
    CContentsViewerDialog& GetCVDlg() { return mCVDlg; }*/

	BOOL IsCVDlgVisable();
	bool IsCVDlgExist();
	void ShowCVDlg();
	HWND GetCVDlgHWnd();

	INT_PTR NotifyFromCVD(HWND hWndFrom, UINT uCode);

	const NPPData& GetNPPData() const {return m_NPPData;}
	void SetNPPData(const NPPData& nppd);

	// const HWND GetMainHWnd() const {return m_hMainWnd;}
	// void SetMainHWnd(HWND hWnd) {m_hMainWnd = hWnd;}

	HWND GetEditHWnd() const;

	//NPP MSG
	void OnFileActivated();
	void OnFileSaved();
	void OnFileOpened();
	void OnFileClosed();
	void OnNPPTBModification();
	void OnNPPReady();
	void OnSelectionChanged();

	HICON GetTabIcon() const {return m_hTabIcon;}

	int IndexContents(const wchar_t* wszText, const wchar_t* wszKeyword, int nLevel);
	int FindAndReplace(const wchar_t* wszFind, const wchar_t* wszReplace, bool bRegularMode);
	void GotoLine(int nLine);
	void DelLine(int nLine);

	void CutLines(int nLineStart, int nLineEnd);
	void PasteBeforeLine(int nLine);
	void ReplaceLine(int nLine, const wchar_t* wszTxt);
	void ParseCurFile();
	void ReParseCurFile();
	int GetLine(int nLine, wstring& wLine);
	int GetLineCount();
	int GetCurLineNum();
	bool SelectLines(int nFromline, int nEndline);
	bool GetSelection(wstring& wStr);
	bool ReplaceSelection(const wchar_t* wszStr);
	bool GetDocument(wstring& wDoc);
	bool SetDocument(const wchar_t* wszDoc);

	LRESULT SendNPPMsg(UINT uMsg, WPARAM wParam = NULL, LPARAM lParam = NULL) const;
	LRESULT SendNPPMsg(UINT uMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);

	LRESULT SendSciMsg(UINT uMsg, WPARAM wParam = NULL, LPARAM lParam = NULL) const;
	LRESULT SendSciMsg(UINT uMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);

	void GetNPPDirectory(wchar_t* wszNPPDir);
	void GetPluginsDir(wchar_t* wszPluginsDir);
	void GetPluginsConfigDir(wchar_t* wszPluginsCfgDir);
	void SaveCurFile();

	void UpdateCurFileCodePage();
	int GetCurFileEncoding();
	void SetCurFileEncoding(int nEncoding);

	void CallNPPCmd(int nCommandID);

	void ActiveContentListviewItem();

	void trigerAction(UINT uID);

	void CheckOpenCVDOrNot(bool bCheck);

private:	
	//void MultiByte2WideChar(const char* mb, wstring& wc);
	//void WideChar2MultiByte(const wchar_t* wc, string& mb);
	bool m_bDebug;

protected:
	HWND m_hSciEdit;
	CContentsViewerDialog m_CVDlg;
	NPPData      m_NPPData;
	HWND		m_hMainWnd;
	HICON        m_hTabIcon;
	toolbarIcons m_tbiJS;
	UINT m_nCodePage;
};

extern CContentsViewer thePlugin;

#endif // _CONTENTSVIEWER_H_