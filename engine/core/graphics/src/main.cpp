#include "app.h"
#include "engine.h"
#include "main.h"

extern unsigned int WIDTH;
extern unsigned int HEIGHT;


// check if building for python or cpp application
#ifndef USE_CPP
extern Application *app; // exported from python file
#endif
#ifdef USE_CPP
Application *app;
extern Application* getApplication();
#endif


// std::string runPythonCommand(std::string cmd) {
//   try
//   {
//     Py_Initialize();
//     PyObject *pModule = PyImport_AddModule("__main__"); //create main module
//     std::string stdOutErr = "import sys\nclass CatchOutErr:\n\tdef __init__(self):\n\t\tsys.stdout.write('\\r')\n\t\tself.value = ''\n\tdef write(self, txt):\n\t\tself.value += txt\ncatchOutErr = CatchOutErr()\nsys.stdout = catchOutErr\nsys.stderr = catchOutErr\n";

//     PyRun_SimpleString(stdOutErr.c_str()); //invoke code to redirect
//     std::cout << "start command..." << std::endl;
//     PyRun_SimpleString(cmd.c_str());
//     std::cout << "finish command." << std::endl;
//     PyObject *catcher = PyObject_GetAttrString(pModule, "catchOutErr"); //get our catchOutErr created above
//     PyObject *output = PyObject_GetAttrString(catcher,"value"); //get the stdout and stderr from our catchOutErr object
//     PyObject *encodedData = PyUnicode_AsEncodedString(output, "ascii", NULL); //it's not in our C++ portion
//     char* buf;
//     Py_ssize_t len;
//     PyBytes_AsStringAndSize(encodedData, &buf, &len);
//     std::cout << std::string(buf) << std::endl;
//     Py_DECREF(output);
//     Py_DECREF(encodedData);
//     return std::string(buf);
//   }
//   catch (const python::error_already_set&)
//   {
//     PyObject *ptype, *pvalue, *ptraceback;
//     PyErr_Fetch(&ptype, &pvalue, &ptraceback);
//     std::string strErrorMessage = extract<string>(pvalue);
//     return strErrorMessage;
//     PyErr_Print();
//   }
//   return std::string("");
// }


