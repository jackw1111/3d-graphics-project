#include "../../bindings/include/main.h"
#include <sstream>
#include <stdio.h>
#include <cstdlib> // setenv
#include <GLFW/glfw3.h>

//typedef struct GLFWwindow* _GLFWwindow;
extern unsigned int WIDTH;
extern unsigned int HEIGHT;

#include <boost/python.hpp>
#include <iostream>
using namespace std;
namespace python = boost::python;
using namespace boost;
using namespace boost::python;

BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(GLFWwindow)

std::string runPythonCommand(std::string cmd) {
  try
  {
    Py_Initialize();
    PyObject *pModule = PyImport_AddModule("__main__"); //create main module
    std::string stdOutErr = "import sys\nclass CatchOutErr:\n\tdef __init__(self):\n\t\tsys.stdout.write('\\r')\n\t\tself.value = ''\n\tdef write(self, txt):\n\t\tself.value += txt\ncatchOutErr = CatchOutErr()\nsys.stdout = catchOutErr\nsys.stderr = catchOutErr\n";

    PyRun_SimpleString(stdOutErr.c_str()); //invoke code to redirect
    std::cout << "start command..." << std::endl;
    PyRun_SimpleString(cmd.c_str());
    std::cout << "finish command." << std::endl;
    PyObject *catcher = PyObject_GetAttrString(pModule, "catchOutErr"); //get our catchOutErr created above
    PyObject *output = PyObject_GetAttrString(catcher,"value"); //get the stdout and stderr from our catchOutErr object
    PyObject *encodedData = PyUnicode_AsEncodedString(output, "ascii", NULL); //it's not in our C++ portion
    char* buf;
    Py_ssize_t len;
    PyBytes_AsStringAndSize(encodedData, &buf, &len);
    std::cout << std::string(buf) << std::endl;
    Py_DECREF(output);
    Py_DECREF(encodedData);
    return std::string(buf);
  }
  catch (const python::error_already_set&)
  {
    PyObject *ptype, *pvalue, *ptraceback;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    std::string strErrorMessage = extract<string>(pvalue);
    return strErrorMessage;
    PyErr_Print();
  }
  return std::string("");
}

namespace bp = boost::python;
 
std::string handle_pyerror()
{
    namespace bp = boost::python;
 
    PyObject *exc, *val, *tb;
    bp::object formatted_list, formatted;
    PyErr_Fetch(&exc, &val, &tb);
    bp::handle<> hexc(exc), hval(bp::allow_null(val)), htb(bp::allow_null(tb));
    bp::object traceback(bp::import("traceback"));
    if (!tb) {
        bp::object format_exception_only(traceback.attr("format_exception_only"));
        formatted_list = format_exception_only(hexc, hval);
    } else {
        bp::object format_exception(traceback.attr("format_exception"));
        formatted_list = format_exception(hexc, hval, htb);
    }
    formatted = bp::str("\n").join(formatted_list);
    return bp::extract<std::string>(formatted);
}


BOOST_PYTHON_MODULE(engine)
{
    // Allow Python to load modules from the current directory.
    setenv("PYTHONPATH", ".", 1);
    // Initialize Python.
    Py_Initialize();


    // Initialize internal python interpretter
    namespace python = boost::python;
    try
    {
      python::object main_module = import("__main__");
      python::object main_namespace = main_module.attr("__dict__");

      python::object ignored = exec("print ('engine loaded: True')",
                            main_namespace);
        }
    catch (const python::error_already_set&)
    {
      PyErr_Print();
    }

    boost::python::object package = boost::python::scope();
    package.attr("__path__") = "engine";

    boost::python::object graphicsModule(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("engine.graphics"))));
    boost::python::scope().attr("graphics") = graphicsModule;
    boost::python::scope io_scope = graphicsModule;

    // engine classes
    wrap_Application();
    wrap_StaticShader();
    wrap_StaticMesh();
    wrap_StaticModel();
    wrap_AnimatedShader();
    wrap_AnimatedModel();
    wrap_AnimatedMesh();
    wrap_Camera();
    wrap_CharacterEntity();
    wrap_Skybox();
    wrap_Vertex();
    wrap_VertexTransform();
    wrap_FBO();
    wrap_Audio();
    wrap_Line3D();
    wrap_Light();
    wrap_Label();
    wrap_Rect();
    wrap_mathUtils();
    wrap_vec2();
    wrap_vec3();
    wrap_vec4();
    wrap_mat4();
    wrap_mat3();

    // still some to wrap TO DO
    python::class_<Texture, boost::noncopyable>("Texture")
    ;
    python::class_<Texture, boost::shared_ptr<Texture>>("Texture")
    .def_readwrite("id", &Texture::id)
    .def_readwrite("type", &Texture::type)
    .def_readwrite("path", &Texture::path)
    ;
    python::def("texture_from_file", TextureFromFile);

    python::def("run_command", runPythonCommand);


    // math library wrappers

    python::def("check_collision", check_collision);
    python::def("checkTriangle", checkTriangle);
    python::def("intersectPlane", intersectPlane);

    // collision2
    python::def("get_position", getPosition);
    // python::def("sphere_plane_collision", spherePlaneCollision);
    // python::def("ray_plane_intersection", rayPlaneIntersection);
    // python::def("get_world_space", getWorldSpace);
    // python::def("ray_triangle_collision", rayTriangleCollision);
    // python::def("closest_point_on_triangle", closestPointOnTriangle);
    // python::def("sphere_triangle_collision", sphereTriangleCollision);
    // python::def("closest_point_on_line_segment", closestPointOnLineSegment2); 


    boost::python::object coreModule(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("engine.core"))));
    boost::python::scope().attr("core") = coreModule;
    boost::python::scope io_scope2 = coreModule;

    // boost::python::object physicsModule(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("engine.core.physics"))));
    // boost::python::scope().attr("core.physics") = physicsModule;
    // boost::python::scope io_scope3 = physicsModule;

    // python::def("collision_SAT", collision_SAT);

    // bp::def("cpp_method", &cpp_method);



}