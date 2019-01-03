#include <sys/time.h>
#include <Python.h>

struct timeval t0, t1;

/*
 * Function to be called from Python
 */
static PyObject* py_setStartTime(PyObject* self, PyObject* args)
{
  char *s = "Hello from C!";
  gettimeofday(&t0, 0);
  return Py_BuildValue("s", s);
}

static PyObject* py_setEndTime(PyObject* self, PyObject* args)
{
  char *s = "Hello from C!";
  gettimeofday(&t1, 0);
  return Py_BuildValue("s", s);
}

static PyObject* py_getElapsedTime(PyObject* self, PyObject* args)
{
  long elapsed  = (t1.tv_sec-t0.tv_sec) * 1e6 + t1.tv_usec - t0.tv_usec;
  return Py_BuildValue("l", elapsed);
}

/*
 * Bind Python function names to our C functions
 */
static PyMethodDef timingModule_methods[] = {
  {"setStartTime", py_setStartTime, METH_VARARGS},
  {"setEndTime", py_setEndTime, METH_VARARGS},
  {"getElapsedTime", py_getElapsedTime, METH_VARARGS},
  {NULL, NULL}
};

/*
 * Python calls this to let us initialize our module
 */
PyMODINIT_FUNC inittimingModule()
{
  (void) Py_InitModule("timingModule", timingModule_methods);
}
