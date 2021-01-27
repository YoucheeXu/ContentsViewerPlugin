// Plugin.h
#pragma once
#ifndef _Plugin_h_
#define _Plugin_h_

#include <memory>
#include "..\third_party\Npp\PluginInterface.h"
#include "..\third_party\enum_array.h"

enum class Action
{
	openContentsViewerDlg,

	indexContents,
	headIndexContents,
	specificIndexContents,

	exportContents,
	importContents,

	intoNumber,
	alignNumber,
	trimContents,
	sortContents,
	delDuplicatecontents,
	markEmptycontents,

	big5ToGbk,

	mergeParagraphs,
	alignParagraphs,

	openAboutDlg,

	COUNT,
};

extern enum_array<Action, int> action_index;

//-------------------------------------//
//-- STEP 1. DEFINE YOUR PLUGIN NAME --//
//-------------------------------------//
// Here define your plugin name
//
const wchar_t NPP_PLUGIN_NAME[] = L"Contents Viewer";
const wchar_t PLUGIN_DLL_NAME[] = L"ContentsViewerPlugin.dll";

//-----------------------------------------------//
//-- STEP 2. DEFINE YOUR PLUGIN COMMAND NUMBER --//
//-----------------------------------------------//
//
// Here define the number of your plugin commands
//

const size_t nbFunc = static_cast<size_t> (Action::COUNT) + 6;
extern FuncItem funcItem[nbFunc];

//
// Initialization of your plugin data
// It will be called while plugin loading
//
//void pluginInit(HANDLE hModule);

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
size_t setNextCommand(const wchar_t *cmdName, PFUNCPLUGINCMD pFunc, std::unique_ptr<ShortcutKey> sk = nullptr, bool check0nInit = false);

//
// Your plugin command functions
//
void openContentsViewerDlg();
void openAboutDlg();

#endif	// _Plugin_h_