//ScriptPlugin.cpp
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>

#include <Python.h>
#include "PyScriptPlugin.h"
#include "PyScript.h"
#include "emb.h"

using namespace std;

static HWND s_hParentWnd;
//static StdCapture s_stdCpature;

LRESULT SendCVMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::SendMessageW(s_hParentWnd, uMsg, wParam, lParam);
}

// Initialization of your plugin data
// It will be called while plugin loading
//
void pluginInitialize(HANDLE hModule)
{
	s_hParentWnd = NULL;

	/* Add a built-in module, before Py_Initialize */
	PyImport_AppendInittab("emb", emb::PyInit_emb);
	PyImport_AppendInittab("thePlugin", PyInit_thePlugin_module);
	PyImport_AppendInittab("theDocument", PyInit_theDocument_module);
	PyImport_AppendInittab("theCViewerDlg", PyInit_theCViewerDlg_module);

	Py_Initialize();
	if (!Py_IsInitialized())
	{
		//cout << "python init fail" << endl;
		PyErr_Print();
		return;
	}

	//PyInit_thePlugin_module();
	//PyInit_theDocument_module();
}

// Here you can do the clean up, save the parameters (if any) for the next session
//
// Cleaning of your plugin
// It will be called while plugin unloading
//
void pluginCleanUp()
{
	Py_Finalize();
}

//-----------------------------------------------

DLL_EXPORT int SetHost(HWND hWnd)
{
	s_hParentWnd = hWnd;
	if (s_hParentWnd != NULL)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int ExecutePyCmd(const char* cmd)
{
	try
	{
		int ret = PyRun_SimpleString(cmd);

		if (-1 == ret)
		{
			//log2File(ERR, "fail to %s", cmd);
			cout << "fail to " << cmd << endl;
			return -1;
		}
	}
	catch (exception &e)
	{
		log2File(ERR, e.what());
		return -1;
	}
	return 0;
}

int ExecutePyScript(const char *szPyFile)
{

	/* Optionally import the module; alternatively,
	import can be deferred until the embedded script
	imports it. */
	/*PyImport_ImportModule("thePlugin");
	PyImport_ImportModule("theDocument");*/

	/*ExecutePyCmd("import thePlugin");

	ExecutePyCmd("thePlugin.log(0, 'haha!')");

	ExecutePyCmd("thePlugin.save_document()");

	ExecutePyCmd("thePlugin.open_document('abc.txt')");

	ExecutePyCmd("doc = thePlugin.fetch_document()");

	ExecutePyCmd("doc = 'we are here!'");
	ExecutePyCmd("thePlugin.update_document(doc)");

	ExecutePyCmd("selc = thePlugin.fetch_selection()");

	ExecutePyCmd("sel = 'i am here!'");
	ExecutePyCmd("thePlugin.update_selection(sel)");

	ExecutePyCmd("import theDocument");

	ExecutePyCmd("doc = theDocument.get_document()");

	ExecutePyCmd("theDocument.set_document('he is here')");

	ExecutePyCmd("theDocument.parse()");

	ExecutePyCmd("theDocument.merge_paragraphs()");
	ExecutePyCmd("theDocument.default_replace()");
	ExecutePyCmd("theDocument.trim_Contents()");
	ExecutePyCmd("theDocument.number_contents()");
	ExecutePyCmd("theDocument.update_contents()");*/

	/*char simpleStr[MAX_PATH];
	sprintf(simpleStr, "%s%s%s", "execfile('", pyFile, "')");
	PyRun_SimpleString(simpleStr);*/

	//redirect stdout, stderr

	PyObject *obj = Py_BuildValue("s", szPyFile);
	FILE *file = _Py_fopen_obj(obj, "r+");
	if (file != NULL)
	{
		return PyRun_SimpleFile(file, szPyFile);
	}

	//FILE *fp = fopen(pyFile, "rb");
	/*FILE *fp = nullptr;
	int ret = 0;
	errno_t err = fopen_s(&fp, pyFile, "rb");*/

	/*FILE* fp = fopen(pyFile, "rb");

	//if (err != 0)
	if(nullptr == fp)
	{
	return -1;
	}

	PyRun_SimpleFile(fp, pyFile);*/

	return -1;
}

DLL_EXPORT int ExecuteA(const char *szPyFile)
{
	try
	{
		log2File(0, "ExecuteA: %s", szPyFile);

		PyImport_ImportModule("emb");

		std::wstring wBuffer;

		emb::stdCatch_write_type write = [&wBuffer](std::wstring wStr) { wBuffer += wStr; };
		emb::set_stdCatch(write);

		int ret = 0;

		ret = ExecutePyScript(szPyFile);

		emb::reset_stdCatch();

		if (-1 == ret)
		{
			log2File(ERR, wBuffer.c_str());
			log2File(ERR, "fail to execute python script!");
			return -1;
		}

		log2File(INFO, "success to execute python script!");
	}
	catch (exception& e)
	{
		log2File(ERR, e.what());
		return -1;
	}

	return 0;
}