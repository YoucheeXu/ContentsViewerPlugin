// emb.cpp
#include "emb.h"

namespace emb
{
	struct StdCatch
	{
		PyObject_HEAD
			stdCatch_write_type write;
	};

	PyObject* StdCatch_write(PyObject* self, PyObject* args)
	{
		std::size_t written(0);
		StdCatch* selfimpl = reinterpret_cast<StdCatch*>(self);
		if (selfimpl->write)
		{
			wchar_t* wszData;
			if (!PyArg_ParseTuple(args, "u", &wszData))
				return 0;

			std::wstring wStr(wszData);
			selfimpl->write(wStr);
			written = wStr.size();
		}
		return PyLong_FromSize_t(written);
	}

	PyObject* StdCatch_flush(PyObject* self, PyObject* args)
	{
		// no-op
		return Py_BuildValue("");
	}

	PyMethodDef StdCatch_methods[] =
	{
		{ "write", StdCatch_write, METH_VARARGS, "sys.stdout.write" },
		{ "flush", StdCatch_flush, METH_VARARGS, "sys.stdout.flush" },
		{ "write", StdCatch_write, METH_VARARGS, "sys.stderr.write" },
		{ "flush", StdCatch_flush, METH_VARARGS, "sys.stderr.flush" },
		{ 0, 0, 0, 0 } // sentinel
	};

	PyTypeObject StdCatchType =
	{
		PyVarObject_HEAD_INIT(0, 0)
		"emb.StdCatchType",     /* tp_name */
		sizeof(StdCatch),     /* tp_basicsize */
		0,                    /* tp_itemsize */
		0,                    /* tp_dealloc */
		0,                    /* tp_print */
		0,                    /* tp_getattr */
		0,                    /* tp_setattr */
		0,                    /* tp_reserved */
		0,                    /* tp_repr */
		0,                    /* tp_as_number */
		0,                    /* tp_as_sequence */
		0,                    /* tp_as_mapping */
		0,                    /* tp_hash  */
		0,                    /* tp_call */
		0,                    /* tp_str */
		0,                    /* tp_getattro */
		0,                    /* tp_setattro */
		0,                    /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT,   /* tp_flags */
		"emb.StdCatch objects", /* tp_doc */
		0,                    /* tp_traverse */
		0,                    /* tp_clear */
		0,                    /* tp_richcompare */
		0,                    /* tp_weaklistoffset */
		0,                    /* tp_iter */
		0,                    /* tp_iternext */
		StdCatch_methods,      /* tp_methods */
		0,                    /* tp_members */
		0,                    /* tp_getset */
		0,                    /* tp_base */
		0,                    /* tp_dict */
		0,                    /* tp_descr_get */
		0,                    /* tp_descr_set */
		0,                    /* tp_dictoffset */
		0,                    /* tp_init */
		0,                    /* tp_alloc */
		0,                    /* tp_new */
	};

	PyModuleDef embmodule =
	{
		PyModuleDef_HEAD_INIT,
		"emb", 0, -1, 0,
	};

	// Internal state
	PyObject* g_stdout;
	PyObject* g_stdout_saved;
	PyObject* g_stderr;
	PyObject* g_stderr_saved;

	PyMODINIT_FUNC PyInit_emb(void)
	{
		g_stdout = 0;
		g_stdout_saved = 0;

		g_stderr = 0;
		g_stderr_saved = 0;

		StdCatchType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&StdCatchType) < 0)
			return 0;

		PyObject* m = PyModule_Create(&embmodule);
		if (m)
		{
			Py_INCREF(&StdCatchType);
			PyModule_AddObject(m, "StdCatch", reinterpret_cast<PyObject*>(&StdCatchType));
		}
		return m;
	}

	void set_stdCatch(stdCatch_write_type write)
	{
		if (!g_stdout)
		{
			g_stdout_saved = PySys_GetObject("stdout"); // borrowed
			g_stdout = StdCatchType.tp_new(&StdCatchType, 0, 0);
		}

		if (!g_stderr)
		{
			g_stderr_saved = PySys_GetObject("stderr"); // borrowed
			g_stderr = StdCatchType.tp_new(&StdCatchType, 0, 0);
		}

		StdCatch* implOut = reinterpret_cast<StdCatch*>(g_stdout);
		implOut->write = write;
		PySys_SetObject("stdout", g_stdout);

		StdCatch* implErr = reinterpret_cast<StdCatch*>(g_stderr);
		implErr->write = write;
		PySys_SetObject("stderr", g_stderr);
	}

	void reset_stdCatch()
	{
		if (g_stdout_saved)
			PySys_SetObject("stdout", g_stdout_saved);

		Py_XDECREF(g_stdout);
		g_stdout = 0;

		if (g_stderr_saved)
			PySys_SetObject("stderr", g_stderr_saved);

		Py_XDECREF(g_stderr);
		g_stderr = 0;
	}

} // namespace emb