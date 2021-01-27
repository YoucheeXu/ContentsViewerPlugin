// Plugin.cpp
#include "ContentsViewer.h"
#include "ContentsViewerMsgs.h"
#include "resource.h"
#include "Plugin.h"
#include "AboutBox.h"

//----------------------------------------------------------------------------

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

/*std::wstring_view rc_str_view(UINT string_id) {
	HINSTANCE hInstance = GetResourceHandle();
	const wchar_t *ret = nullptr;
	// static_cast<HINSTANCE>(h_module)
	auto len = LoadString(hInstance, string_id, reinterpret_cast<LPWSTR>(&ret), 0);
	return {ret, static_cast<size_t>(len)};
}*/

// std::wstring rc_str(UINT string_id){return std::wstring{rc_str_view(string_id)};}

// std::vector<std::unique_ptr<ShortcutKey>> shortcut_storage;

void IndexContents()
{
	thePlugin.trigerAction(IDB_PARSE);
}

void HeadIndexContents()
{
	thePlugin.trigerAction(IDM_HEAD_INDEX_CONTENTS);
}

void SpecificIndexContents()
{
	thePlugin.trigerAction(IDM_SPECIFIC_INDEX_CONTENTS);
}

void ExportContents()
{
	thePlugin.trigerAction(IDM_EXPORT_CONTENTS);
}
void ImportContents()
{
	thePlugin.trigerAction(IDM_IMPORT_CONTENTS);
}

void NumberContents()
{
	thePlugin.trigerAction(IDM_SC_NUMBER_CONTENTS);
}
void AlignContentsNumber()
{
	thePlugin.trigerAction(IDM_SC_ALIGN_CONTENTS_NUMBER);
}
void TrimContents()
{
	thePlugin.trigerAction(IDM_SC_TRIM_CONTENTS);
}
void SortContents()
{
	thePlugin.trigerAction(IDM_SC_SORT_CONTENTS);
}
void DelDuplicateContents()
{
	thePlugin.trigerAction(IDM_SC_DEL_DUPLICATE_CONTENTS);
}
void MarkEmptyContents()
{
	thePlugin.trigerAction(IDM_SC_MARK_EMPTY_CONTENTS);
}

void BIG5ToGBK()
{
	thePlugin.trigerAction(IDM_BIG5_GBK);
}

void MergeParagraphs()
{
	thePlugin.trigerAction(IDM_MERGE_PARAGRAPHS);
}
void AlignParagraphs()
{
	thePlugin.trigerAction(IDM_ALIGN_PARAGRAPHS);
}

enum_array<Action, int> action_index = []() {
	enum_array<Action, int> val;
	val.fill(-1);
	return val;
}();

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
	// ShortcutKey *sk = new ShortcutKey();

	auto sk = std::make_unique<ShortcutKey>();
	sk->_isAlt = true;
	sk->_isCtrl = true;
	sk->_isShift = false;
	sk->_key = 'C';
	action_index[Action::openContentsViewerDlg] = setNextCommand(TEXT("Show &Contents Viewer"), openContentsViewerDlg, std::move(sk));

	setNextCommand(L"---", nullptr);

	action_index[Action::indexContents] = setNextCommand(Win32xx::LoadStringW(IDS_INDEX_CONTENTS).c_str(), IndexContents);
	action_index[Action::headIndexContents] = setNextCommand(Win32xx::LoadStringW(IDS_HEAD_INDEX_CONTENTS).c_str(), HeadIndexContents);
	action_index[Action::specificIndexContents] = setNextCommand(Win32xx::LoadString(IDS_SPECIFIC_INDEX_CONTENTS).c_str(), SpecificIndexContents);

	setNextCommand(L"---", nullptr);

	action_index[Action::exportContents] = setNextCommand(Win32xx::LoadString(IDS_EXPORT_CONTENTS).c_str(), ExportContents);
	action_index[Action::importContents] = setNextCommand(Win32xx::LoadString(IDS_IMPORT_CONTENTS).c_str(), ImportContents);

	setNextCommand(L"---", nullptr);

	action_index[Action::intoNumber] = setNextCommand(Win32xx::LoadString(IDS_NUMBER_CONTENTS).c_str(), NumberContents);
	action_index[Action::alignNumber] = setNextCommand(Win32xx::LoadString(IDS_ALIGN_CONTENTS_NUMBER).c_str(), AlignContentsNumber);
	action_index[Action::trimContents] = setNextCommand(Win32xx::LoadString(IDS_TRIM_CONTENTS).c_str(), TrimContents);
	action_index[Action::sortContents] = setNextCommand(Win32xx::LoadString(IDS_SORT_CONTENTS).c_str(), SortContents);
	action_index[Action::delDuplicatecontents] = setNextCommand(Win32xx::LoadString(IDS_DEL_DUPLICATE_CONTENTS).c_str(), DelDuplicateContents);
	action_index[Action::markEmptycontents] = setNextCommand(Win32xx::LoadString(IDS_MARK_EMPTY_CONTENTS).c_str(), MarkEmptyContents);

	setNextCommand(L"---", nullptr);

	action_index[Action::big5ToGbk] = setNextCommand(Win32xx::LoadString(IDS_BIG5_GBK).c_str(), BIG5ToGBK);

	setNextCommand(L"---", nullptr);

	action_index[Action::mergeParagraphs] = setNextCommand(Win32xx::LoadString(IDS_MERGE_PARAGRAPHS).c_str(), MergeParagraphs);
	action_index[Action::alignParagraphs] = setNextCommand(Win32xx::LoadString(IDS_ALIGN_PARAGRAPHS).c_str(), AlignParagraphs);

	setNextCommand(L"---", nullptr);

	action_index[Action::openAboutDlg] = setNextCommand(TEXT("&About"), openAboutDlg);
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
//size_t setNextCommand(const TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = nullptr, bool check0nInit = false)
static int counter = 0;
size_t setNextCommand(const wchar_t *cmdName, PFUNCPLUGINCMD pFunc, std::unique_ptr<ShortcutKey> sk, bool check0nInit)
{
	if (counter >= nbFunc)
	{
		assert(false); // Less actions specified in nb_func constant than added
		return -1;
	}

	if (pFunc == nullptr)
	{
		counter++;
		return counter - 1;
	}

	lstrcpy(funcItem[counter]._itemName, cmdName);
	funcItem[counter]._pFunc = pFunc;
	funcItem[counter]._init2Check = false;
	funcItem[counter]._pShKey = nullptr;
	counter++;

	return counter - 1;
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
		bool bCheck = true;
		if (thePlugin.IsCVDlgVisable())
		{
			uMsg = NPPM_DMMHIDE;
			bCheck = false;
			//bParseFile = true;
		}

		//show/hide the dialog
		thePlugin.SendNPPMsg(uMsg, 0, (LPARAM)dockData.hClient);
		//set the check on menu item
		//thePlugin.SendNPPMsg(NPPM_SETMENUITEMCHECK, 
		//CContentsViewerDialog::FUNC_ARRAY[CContentsViewerDialog::EFI_TAGSVIEW]._cmdID, bCheck );
		//Action::openContentsViewerDlg
		//thePlugin.SendNPPMsg(NPPM_SETMENUITEMCHECK, funcItem[0action_index[Action::openContentsViewerDlg]]._cmdID, bCheck);
		thePlugin.CheckOpenCVDOrNot(bCheck);
	}
	else
	{
		lstrcpy(szPluginName, NPP_PLUGIN_NAME);

		//static AboutBox aboutbox2;
		//aboutbox2.DoModeless();
		//dockData.hClient = aboutbox2.GetHwnd();

		thePlugin.ShowCVDlg();
		dockData.hClient = thePlugin.GetCVDlgHWnd();

		dockData.pszName = szPluginName;
		dockData.dlgID = -1;
		dockData.uMask = DWS_DF_CONT_LEFT | DWS_ICONTAB;
		dockData.hIconTab = thePlugin.GetTabIcon();
		dockData.pszAddInfo = NULL;
		// dockData.rcFloat.left = 0;
		// dockData.rcFloat.top = 0;
		// dockData.rcFloat.right = 0;
		// dockData.rcFloat.bottom = 0;
		// dockData.iPrevCont = -1;
		// dockData.pszModuleName = PLUGIN_DLL_NAME;
		dockData.pszModuleName = szPluginName;

		//set dialog dockable
		thePlugin.SendNPPMsg(NPPM_DMMREGASDCKDLG, 0, (LPARAM)&dockData);
		//register dialog
		thePlugin.SendNPPMsg(NPPM_MODELESSDIALOG, MODELESSDIALOGADD, (LPARAM)dockData.hClient);
		//show the dialog
		thePlugin.SendNPPMsg(NPPM_DMMSHOW, 0, (LPARAM)dockData.hClient);
		//set the check on menu item
		thePlugin.CheckOpenCVDOrNot(true);
	}

	thePlugin.ParseCurFile();
}

void openAboutDlg()
{
	//::CreateDialog((HINSTANCE)g_hMod, MAKEINTRESOURCE(IDD_ABOUTDLG), thePlugin.GetMainHWnd(), abtDlgProc);
	// ::MessageBox(
		// thePlugin.GetMainHWnd(),
		// _T("Haha!"),
		// NPP_PLUGIN_NAME,
		// MB_OK);
	static AboutBox aboutbox;
	aboutbox.OnHelpAbout(L"v0.6");
}

// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}


//-----------------------------------------------

extern "C" __declspec(dllexport) void setInfo(NPPData notpadPlusData)
{
	//nppData = notpadPlusData;
	commandMenuInit();
	thePlugin.SetNPPData(notpadPlusData);
	// thePlugin.SetMainHWnd(notpadPlusData._nppHandle);
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