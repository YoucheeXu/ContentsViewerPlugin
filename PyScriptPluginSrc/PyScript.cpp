// PyScript.cpp

#define PY_SSIZE_T_CLEAN		  // Make "s#" use Py_ssize_t rather than int.
#include <Python.h>
#include "PyScript.h"
#include "PyScriptPlugin.h"
#include <string>
#include <codecvt>

void log2File(int level, const char* szMsg, ...)
{
	char szBuffer[1024];
	va_list pArgList;

	// The va_start macro (defined in STDARG.H) is usually equivalent to:
	// pArgList = (char *) &szFormat + sizeof (szFormat) ;
	va_start(pArgList, szMsg);

	// The last argument to wvsprintf points to the arguments

	_vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(char),
		szMsg, pArgList);

	// The va_end macro just zeroes out pArgList for no good reason
	va_end(pArgList);

	SendCVMsg(CVM_LOGA, (WPARAM)level, (LPARAM)szBuffer);
}

void log2File(int level, const wchar_t* wszMsg, ...)
{
	wchar_t wszBuffer[1024];
	va_list pArgList;

	// The va_start macro (defined in STDARG.H) is usually equivalent to:
	// pArgList = (char *) &szFormat + sizeof (szFormat) ;
	va_start(pArgList, wszMsg);

	// The last argument to wvsprintf points to the arguments

	_vsnwprintf(wszBuffer, sizeof(wszBuffer) / sizeof(wchar_t),
		wszMsg, pArgList);

	// The va_end macro just zeroes out pArgList for no good reason
	va_end(pArgList);

	SendCVMsg(CVM_LOGW, (WPARAM)level, (LPARAM)wszBuffer);
}

static PyObject * thePlugin_logInfo(PyObject *self, PyObject *args)
{
	wchar_t* msg;

	if (!PyArg_ParseTuple(args, "u", &msg))
	{
		log2File(ERR, "fail to get para of thePlugin_logInfo");
		return NULL;
	}

	log2File(INFO, msg);

	//PyMem_Free(msg);

	Py_RETURN_NONE;
}

static PyObject * thePlugin_logWarn(PyObject *self, PyObject *args)
{
	wchar_t* msg;

	if (!PyArg_ParseTuple(args, "u", &msg))
	{
		log2File(ERR, "fail to get para of thePlugin_logWarn");
		return NULL;
	}

	log2File(WARN, msg);

	//PyMem_Free(msg);

	Py_RETURN_NONE;
}

static PyObject * thePlugin_logErr(PyObject *self, PyObject *args)
{
	wchar_t* msg;

	if (!PyArg_ParseTuple(args, "u", &msg))
	{
		log2File(ERR, "fail to get para of thePlugin_logErr");
		return NULL;
	}

	log2File(ERR, msg);

	//PyMem_Free(msg);

	Py_RETURN_NONE;
}

static PyObject * thePlugin_save_document(PyObject *self, PyObject *args)
{
	SendCVMsg(CVM_SAVE_DOCUMENT, (WPARAM)NULL, (LPARAM)NULL);
	Py_RETURN_NONE;
}

static PyObject * thePlugin_open_document(PyObject *self, PyObject *args)
{
	wchar_t* name;

	if (!PyArg_ParseTuple(args, "u", &name))
	{
		log2File(ERR, "fail to get para of thePlugin_open_document");
		return NULL;
	}

	SendCVMsg(CVM_OPEN_DOCUMENT, (WPARAM)NULL, (LPARAM)name);

	//PyMem_Free(name);

	Py_RETURN_NONE;
}

static PyObject *thePlugin_fetch_document(PyObject *self, PyObject *args)
{
	//wchar_t doc[] = L"ÄãºÃhello°¡";

	int len = 0;

	int* lenPtr = &len;

	int len2 = SendCVMsg(CVM_FETCH_DOCUMENT, (WPARAM)lenPtr, (LPARAM)NULL);

	wchar_t* doc = new wchar_t[len + 1];

	SendCVMsg(CVM_FETCH_DOCUMENT, (WPARAM)(len + 1), (LPARAM)doc);

	PyObject *pyDoc = Py_BuildValue("u", doc);

	delete[] doc;

	return pyDoc;
}

static PyObject * thePlugin_update_document(PyObject *self, PyObject *args)
{
	wchar_t* doc;

	if(!PyArg_ParseTuple(args, "u", &doc))
	{
		log2File(ERR, "fail to get para of thePlugin_update_document");
		return NULL;
	}

	SendCVMsg(CVM_UPDATE_DOCUMENT, (WPARAM)NULL, (LPARAM)doc);

	//SendCVMsg(CVM_UPDATE_CONTENTS, (WPARAM)NULL, (LPARAM)NULL);

	//PyMem_Free(doc);

	Py_RETURN_NONE;
}

static PyObject *thePlugin_fetch_selection(PyObject *self, PyObject *args)
{
	int len = 0;

	int* lenPtr = &len;

	int len2 = SendCVMsg(CVM_FETCH_SELECTION, (WPARAM)lenPtr, (LPARAM)NULL);

	wchar_t* sel = new wchar_t[len + 1];

	SendCVMsg(CVM_FETCH_SELECTION, (WPARAM)(len + 1), (LPARAM)sel);

	PyObject *pyDoc = Py_BuildValue("u", sel);

	delete[] sel;

	return pyDoc;
}

static PyObject * thePlugin_update_selection(PyObject *self, PyObject *args)
{
	wchar_t* sel;

	if(!PyArg_ParseTuple(args, "u", &sel))
	{
		log2File(ERR, "fail to get para of thePlugin_update_selection");
		return NULL;
	}

	SendCVMsg(CVM_UPDATE_SELECTION, (WPARAM)NULL, (LPARAM)sel);

	//PyMem_Free(doc);

	Py_RETURN_NONE;
}

static PyMethodDef thePlugin_module_Methods[] =
{
	{
		"logInfo",
		thePlugin_logInfo,
		METH_VARARGS,
		""
	},
	{
		"logWarn",
		thePlugin_logWarn,
		METH_VARARGS,
		""
	},
	{
		"logErr",
		thePlugin_logErr,
		METH_VARARGS,
		""
	},	
	{
		"save_document",
		thePlugin_save_document,
		METH_VARARGS,
		""
	},
	{
		"open_document",
		thePlugin_open_document,
		METH_VARARGS,
		""
	},	
	{
		"fetch_document",
		thePlugin_fetch_document,
		METH_VARARGS,
		""
	},
	{
		"update_document",
		thePlugin_update_document,
		METH_VARARGS,
		""
	},
	{
		"fetch_selection",
		thePlugin_fetch_selection,
		METH_VARARGS,
		"update selection from Npp"
	},
	{
		"update_selection",
		thePlugin_update_selection,
		METH_VARARGS,
		"update selection to Npp"
	},
	{ NULL, NULL, 0, NULL }		/* Sentinel */
};

static struct PyModuleDef thePlugin_module =
{
	PyModuleDef_HEAD_INIT,
	"thePlugin",				/* name of module */
	NULL,						/* module documentation, may be NULL */
	-1,							/* size of per-interpreter state of the module, 
									or -1 if the module keeps state in global variables. */
	thePlugin_module_Methods
};

PyMODINIT_FUNC PyInit_thePlugin_module(void)
{
	PyObject *m;

	m = PyModule_Create(&thePlugin_module);
	if (m == NULL)
	{
		log2File(ERR, "fail to init thePlugin module");
		return NULL;
	}

	log2File(INFO, "Init thePlugin module OK!");
	return m;
}


static PyObject * theCViewerDlg_update_contentsview(PyObject *self, PyObject *args)
{
	SendCVMsg(CVM_UPDATE_CONTENTSVIEW, (WPARAM)NULL, (LPARAM)NULL);

	Py_RETURN_NONE;
}

static PyMethodDef theCViewerDlg_module_Methods[] =
{
	{
		"update_contentsview",
		theCViewerDlg_update_contentsview,
		METH_VARARGS,
		"update contents to viwer dialog"
	},
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef theCViewerDlg_module =
{
	PyModuleDef_HEAD_INIT,
	"theCViewerDlg",
	NULL,
	-1,
	theCViewerDlg_module_Methods
};

PyMODINIT_FUNC PyInit_theCViewerDlg_module(void)
{
	PyObject *m;
	m = PyModule_Create(&theCViewerDlg_module);
	if (m == NULL)
	{
		log2File(ERR, "fail to init theCViewerDlg module");
		return NULL;
	}

	log2File(INFO, "Init theCViewerDlg module OK!\n");
	return m;
}

/*static PyObject * theDocument_fetch_document(PyObject *self, PyObject *args)
{
	SendCVMsg(CVM_FETCH_DOCUMENT, (WPARAM)NULL, (LPARAM)NULL);

	Py_RETURN_NONE;
}*/

/*static PyObject *theDocument_update_document(PyObject *self, PyObject *args)
{
	SendCVMsg(CVM_UPDATE_DOCUMENT, (WPARAM)NULL, (LPARAM)NULL);

	Py_RETURN_NONE;
}*/

static PyObject *theDocument_get_document(PyObject *self, PyObject *args)
{
	int len = 0;

	int* lenPtr = &len;

	int len2 = SendCVMsg(CVM_GET_DOCUMENT, (WPARAM)lenPtr, (LPARAM)NULL);

	wchar_t* doc = new wchar_t[len + 1];

	SendCVMsg(CVM_GET_DOCUMENT, (WPARAM)(len + 1), (LPARAM)doc);

	PyObject *pyDoc = Py_BuildValue("u", doc);

	delete[] doc;

	return pyDoc;
}

static PyObject * theDocument_set_document(PyObject *self, PyObject *args)
{
	wchar_t* doc;

	if(!PyArg_ParseTuple(args, "u", &doc))
	{
		log2File(ERR, "fail to get para of theDocument_set_document");
		return NULL;
	}

	SendCVMsg(CVM_SET_DOCUMENT, (WPARAM)NULL, (LPARAM)doc);

	//PyMem_Free(doc);

	Py_RETURN_NONE;
}

static PyObject * theDocument_parse(PyObject *self, PyObject *args)
{
	int num = SendCVMsg(CVM_PARSE, (WPARAM)0, (LPARAM)NULL);

	return Py_BuildValue("i", num);
}

static PyObject * theDocument_headParse(PyObject *self, PyObject *args)
{
	int num = SendCVMsg(CVM_PARSE, (WPARAM)1, (LPARAM)NULL);

	return Py_BuildValue("i", num);
}

static PyObject * theDocument_specificParse(PyObject *self, PyObject *args)
{
	wchar_t* index;

	if(!PyArg_ParseTuple(args, "u", &index))
	{
		log2File(ERR, "fail to get para of theDocument_specificParse");
		return NULL;
	}

	int num = SendCVMsg(CVM_PARSE, (WPARAM)2, (LPARAM)index);

	return Py_BuildValue("i", num);
}

static PyObject * theDocument_merge_paragraphs(PyObject *self, PyObject *args)
{
	SendCVMsg(CVM_MERGE_PARAGRAPHS, (WPARAM)NULL, (LPARAM)NULL);

	Py_RETURN_NONE;
}

static PyObject * theDocument_default_replace(PyObject *self, PyObject *args)
{
	SendCVMsg(CVM_DEFAULT_REPLACE, (WPARAM)NULL, (LPARAM)NULL);

	Py_RETURN_NONE;
}

static PyObject * theDocument_update_contents(PyObject *self, PyObject *args)
{
	int ret = SendCVMsg(CVM_UPDATE_CONTENTS, (WPARAM)NULL, (LPARAM)NULL);

	Py_RETURN_NONE;
}

static PyObject * theDocument_number_contents(PyObject *self, PyObject *args)
{
	SendCVMsg(CVM_NUMBER_CONTENTS, (WPARAM)NULL, (LPARAM)NULL);

	Py_RETURN_NONE;
}

static PyObject * theDocument_trim_contents(PyObject *self, PyObject *args)
{
	int ret = SendCVMsg(CVM_TRIM_CONTENTS, (WPARAM)NULL, (LPARAM)NULL);

	Py_RETURN_NONE;
}

static PyMethodDef theDocument_module_Methods[] =
{
	/*{
		"fetch_document",
		theDocument_get_document,
		METH_VARARGS,
		""
	},
	{
		"update_document",
		theDocument_get_document,
		METH_VARARGS,
		""
	},*/
	{
		"get_document",
		theDocument_get_document,
		METH_VARARGS,
		"get doc from document"
	},
	{
		"set_document",
		theDocument_set_document,
		METH_VARARGS,
		"set doc to document"
	},
	{
		"parse",
		theDocument_parse,
		METH_VARARGS,
		"norm parse document"
	},
	{
		"headParse",
		theDocument_headParse,
		METH_VARARGS,
		"head parse document"
	},
	{
		"specificParse",
		theDocument_specificParse,
		METH_VARARGS,
		"specific parse document"
	},	
	{
		"merge_paragraphs",
		theDocument_merge_paragraphs,
		METH_VARARGS,
		""
	},
	{
		"default_replace",
		theDocument_default_replace,
		METH_VARARGS,
		""
	},
	{
		"update_contents",
		theDocument_update_contents,
		METH_VARARGS,
		""
	},
	{
		"number_contents",
		theDocument_number_contents,
		METH_VARARGS,
		""
	},
	{
		"trim_contents",
		theDocument_trim_contents,
		METH_VARARGS,
		""
	},	
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef theDocument_module =
{
	PyModuleDef_HEAD_INIT,
	"theDocument",
	NULL,
	-1,
	theDocument_module_Methods
};

PyMODINIT_FUNC PyInit_theDocument_module(void)
{
	PyObject *m;
	m = PyModule_Create(&theDocument_module);
	if (m == NULL)
	{
		log2File(ERR, "fail to init theDocument module");
		return NULL;
	}

	log2File(INFO, "Init theDocument module OK!");
	return m;
}