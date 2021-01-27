#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <Python.h>

namespace emb
{
	typedef std::function<void(std::wstring)> stdCatch_write_type;

	PyObject* StdCatch_write(PyObject* self, PyObject* args);

	PyObject* StdCatch_flush(PyObject* self, PyObject* args);

	PyMODINIT_FUNC PyInit_emb(void);

	void set_stdCatch(stdCatch_write_type write);

	void reset_stdCatch();

} // namespace emb