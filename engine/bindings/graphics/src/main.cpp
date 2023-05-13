

#include "../../../bindings/graphics/include/main.h"
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


#ifdef GAME_TOOL

int inputEnabled = 0;

#include "mathmodule.h"
#include "timemodule.h"
#include "randommodule.h"
#include "binascii.h"
#include "_CStringIO.h"
#include "_collectionsmodule.h"
#include "operator.h"
#include "itertoolsmodule.h"
#include "_struct.h"
#include "_datetime.h"

#include "model.h"
#include "_camera.h"

#include <emscripten.h>
#include <emscripten/bind.h>
using namespace emscripten;
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GLES2/gl2.h>

#if PY_MAJOR_VERSION >= 3
#define MODINIT(name) PyInit_##name
#else
#define MODINIT(name) init##name
#endif

PyMODINIT_FUNC MODINIT(emscripten) (void);
PyMODINIT_FUNC MODINIT(emscripten_fetch) (void);

extern Application *app;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

PyThreadState *mainInterpreter;
PyThreadState *newInterpreter = NULL;

python::object main_module;
python::object main_namespace;

python::dict global_namespace;
python::dict local_namespace;

Camera2 camera(glm::vec3(0.0f, 0.0f, 3.0f));

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;   // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// unsigned int WIDTH = 800;
// unsigned int HEIGHT = 600;

class Application2
{
 public:

    float deltaTime = 0.0;
    float lastFrame = 0.0;
    float currentFrame = 0.0;

    float lastX;
    float lastY;

    std::string name = "Application2";

    GLFWwindow* window;

    Application2(std::string x, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen, bool _MSAA);
    virtual int _setup(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen, bool _MSAA);
    virtual void setup() { std::cout << "Hello" << std::endl; }
    virtual void draw() { std::cout << "Hello" << std::endl; }
    void gameLoop();
};

Application2::Application2(std::string title, unsigned int _WIDTH, unsigned int _HEIGHT, bool fullscreen, bool _MSAA) {

    WIDTH = _WIDTH;
    HEIGHT = _HEIGHT;

    glfwInit();
    std::cout << "initialized glfw" << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    if (glfwGetCurrentContext() == NULL) {

        if (!_MSAA) {
            glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE );
        }
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        if (_MSAA) {
            glfwWindowHint(GLFW_SAMPLES, 4);
        }
        if (!fullscreen) {
            window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), NULL, NULL);
            std::cout << window << std::endl;
            assert(window != NULL);
        } else {
            window = glfwCreateWindow(WIDTH, HEIGHT, title.c_str(), glfwGetPrimaryMonitor(), NULL);
            std::cout << window << std::endl;
            assert(window != NULL);

        }

        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        std::cout << "got here1" << std::endl;

        glfwMakeContextCurrent(window);
        std::cout << "got here2" << std::endl;

        if (!_MSAA) {
            glfwSwapInterval(0);
        }

        GLint no_of_extensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &no_of_extensions);

        std::set<std::string> ogl_extensions;
        for (int i = 0; i < no_of_extensions; i++) {
            ogl_extensions.insert((const char*)glGetStringi(GL_EXTENSIONS, i));
        }
        bool texture_storage = 
            ogl_extensions.find("GL_ARB_arrays_of_arrays") != ogl_extensions.end();
        if (texture_storage) {
             std::cout << "GL_ARB_arrays_of_arrays not supported!" << std::endl; 
        }

        texture_storage = 
            ogl_extensions.find("GL_ARB_shader_storage_buffer_object") != ogl_extensions.end();
        if (texture_storage) {
             std::cout << "GL_ARB_shader_storage_buffer_object not supported!" << std::endl; 
        }

        texture_storage = 
            ogl_extensions.find("GL_EXT_multi_draw_arrays") != ogl_extensions.end();
        if (texture_storage) {
             std::cout << "GL_EXT_multi_draw_arrays not supported!" << std::endl; 
        }

        texture_storage = 
            ogl_extensions.find("GL_ARB_multi_draw_indirect") != ogl_extensions.end();
        if (texture_storage) {
             std::cout << "GL_ARB_multi_draw_indirect not supported!" << std::endl; 
        }   

        std::cout << glGetString ( GL_SHADING_LANGUAGE_VERSION ) << std::endl;
        std::cout << "got here11" << std::endl;
    }
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, WIDTH, HEIGHT);   
    _setup(title, WIDTH, HEIGHT, fullscreen, _MSAA);


}

// Use in conjunction with Application() to create an instance but do not create a new window
// useful for drawing to different contexts/windows of OpenGL

int Application2::_setup(std::string title, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen, bool _MSAA) {

    // glfw: initialize and configure
    // ------------------------------
    // glfwInit();
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // //if (glfwGetCurrentContext() == NULL) {
    //     // glfw window creation
    //     // --------------------
    //     window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    //     if (window == NULL)
    //     {
    //         std::cout << "Failed to create GLFW window" << std::endl;
    //         glfwTerminate();
    //         return -1;
    //     }
    //     glfwMakeContextCurrent(window);      
    // //}

    // // configure global opengl state
    // // -----------------------------
    // glEnable(GL_DEPTH_TEST);

    return 0;
}

void Application2::gameLoop() {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // render
    // ------
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


    draw();
    // glBindVertexArray(0); // no need to unbind it every time 

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    // glfwSwapBuffers(window);
    // glfwPollEvents();

}

struct Application2Wrap : Application2
{
    // hello constructor storing initial self parameter
    Application2Wrap(PyObject *p, std::string x, unsigned int WIDTH, unsigned int HEIGHT, bool fullscreen, bool _MSAA) // 2
        : Application2(x, WIDTH, HEIGHT, fullscreen, _MSAA), self(p) {}

    // In case hello is returned by-value from a wrapped function
    Application2Wrap(PyObject *p, const Application2& x) // 3
        : Application2(x), self(p) {}

    // Override draw to call back into Python
    void draw() // 4
    { 
        call_method<void>(self, "draw"); 
    }
    // void setup() // 4
    // {   
    //     call_method<void>(self, "setup");
    // }

 private:
    PyObject* self; // 1
};


Application2* app2;
GLFWwindow* window2;



void main_loop2() {
    app2->gameLoop();
}
bool mainLoopSet2 = false;
void run2(python::object obj) {

    if (mainLoopSet2)  {
        emscripten_cancel_main_loop();
    }
    mainLoopSet2 = true;
    app2 = python::extract<Application2Wrap*>(obj);
    emscripten_set_main_loop(main_loop2, 0, true);
}

#endif


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
    #ifdef GAME_TOOL
    //
    #else
    // Allow Python to load modules from the current directory.
    setenv("PYTHONPATH", ".", 1);
    // Initialize Python.
    Py_Initialize();
    #endif

    // Initialize internal python interpretter
    namespace python = boost::python;
    try
    {
      python::object main_module = import("__main__");
      python::object main_namespace = main_module.attr("__dict__");

    #ifdef GAME_TOOL
    //
    #else
      python::object ignored = exec("print ('engine loaded: True')",
                            main_namespace);
    #endif
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
    wrap_Skybox();
    wrap_Vertex();
    wrap_VertexTransform();
    wrap_FBO();
    wrap_Frustum();
    wrap_Audio();
    wrap_Line3D();
    wrap_Line2D();
    wrap_Light();
    wrap_Label2D();
    wrap_Label3D();
    wrap_Particle();
    wrap_Rect2D();
    wrap_mathUtils();
    wrap_vec2();
    wrap_vec3();
    wrap_vec4();
    wrap_mat2();
    wrap_mat4();
    wrap_mat3();
    wrap_quat();


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
    //boost::python::def("run", &run);


    // math library wrappers

    python::def("intersectPlane", intersectPlane);
    #ifdef GAME_TOOL
    // Create the Python type object for our extension class
    boost::python::class_<Application2,Application2Wrap> ("Application2", init<std::string, unsigned int, unsigned int, bool, bool>())
    // Add a virtual member function
    //.def("setup", &Application2Wrap::setup)
    .def("draw", &Application2Wrap::draw)
    ;

    boost::python::def("run2", &run2);


    boost::python::class_<Model>("Model", python::init<>())
    .def("setup", &Model::setup)
    .def("draw", &Model::Draw)
    .def_readwrite("model", &Model::model)
    ;
    #endif
    // collision2
    //python::def("get_position", getPosition);
    // python::def("sphere_plane_collision", spherePlaneCollision);
    // python::def("ray_plane_intersection", rayPlaneIntersection);
    // python::def("get_world_space", getWorldSpace);
    // python::def("ray_triangle_collision", rayTriangleCollision);
    // python::def("closest_point_on_triangle", closestPointOnTriangle);
    // python::def("sphere_triangle_collision", sphereTriangleCollision);
    // python::def("closest_point_on_line_segment", closestPointOnLineSegment2); 


    boost::python::object physicsModule(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("engine.physics"))));
    boost::python::scope().attr("physics") = physicsModule;
    boost::python::scope io_scope2 = physicsModule;

    wrap_collisionSAT();
    wrap_Contact();
    wrap_RigidBody();
    wrap_Joint();
    wrap_Math3D();
    // boost::python::object physicsModule(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("engine.core.physics"))));
    // boost::python::scope().attr("core.physics") = physicsModule;
    // boost::python::scope io_scope3 = physicsModule;

    // python::def("collision_SAT", collision_SAT);

    // bp::def("cpp_method", &cpp_method);

}

#ifdef GAME_TOOL

void initializePyEnv() {
    std::cout << "initializing startup sequence...loading" << std::endl;
    Py_OptimizeFlag = 2; // look for .pyo rather than .pyc
    Py_FrozenFlag   = 1; // drop <exec_prefix> warnings
    Py_VerboseFlag  = 1; // trace modules loading
    static struct _inittab builtins[] = {
    { "emscripten", MODINIT(emscripten) },
    { "emscripten_fetch", MODINIT(emscripten_fetch) },
    {NULL, NULL}
    };
    PyImport_ExtendInittab(builtins);
    // Import your module to embedded Python
    PyImport_AppendInittab("math", &initmath);
    PyImport_AppendInittab("time", &inittime);
    PyImport_AppendInittab("_random", &init_random);
    PyImport_AppendInittab("binascii", &initbinascii);
    PyImport_AppendInittab("cStringIO", &initcStringIO);
    PyImport_AppendInittab("_collections", &init_collections);
    PyImport_AppendInittab("operator", &initoperator);
    PyImport_AppendInittab("itertools", &inititertools);
    PyImport_AppendInittab("_struct", &init_struct);
    PyImport_AppendInittab("datetime", &initdatetime);

    PyImport_AppendInittab("engine", &initengine);
    setenv("PYTHONPATH", "./data/", 0);
    //PyEval_InitThreads();
    Py_InitializeEx(0);  // 0 = get rid of 'Calling stub instead of sigaction()'

    main_module = python::import("__main__");
    std::cout << "got here1" << std::endl;
    main_namespace = main_module.attr("__dict__");
    std::cout << "got here2" << std::endl;

    // Do something in first interpreter 

    mainInterpreter = PyThreadState_Get();

    // try {
    //     exec("import API");
    // } catch (int exceptionPtr) {
    //     std::cout << "ptr: " << exceptionPtr << std::endl;
    // }
}
extern "C" {
    int EMSCRIPTEN_KEEPALIVE runPythonCode(const char* code) {
        std::cout << "RUNNING PYTHON CODE9" << std::endl;

        global_namespace.clear();
        //local_namespace.clear();     
        global_namespace["__builtins__"] = main_namespace["__builtins__"];

        PyRun_String(code, Py_file_input, global_namespace.ptr(), global_namespace.ptr());
        PyRun_SimpleString("\n");

        // char * pch = strtok((char*)code, "\n");
        // std::vector<std::string> strings = {};
        // while(pch != NULL)
        // {
        //     strings.push_back(std::string(pch));
        //     pch = strtok(NULL, "\n");
        // }
        // for (unsigned int i = 0; i < strings.size(); i++) {
        //     std::cout << strings.at(i) << std::endl;
        // }

        // empty the interpreters namespaces

        // for (unsigned int i = 0; i < strings.size(); i++) {
        //     PyRun_String(strings.at(i).c_str(), Py_single_input, main_namespace.ptr(), local_namespace.ptr());
        // }
        /**/
        //PyRun_SimpleString("\n");

        //PyRun_SimpleString(code);
        // PyCodeObject* _code = (PyCodeObject*) Py_CompileString(code, "test", Py_file_input);
        // PyEval_EvalCode(_code, main_namespace.ptr(), local_namespace.ptr());

        /*
        if (newInterpreter != NULL) {
            // destroy sub interpreter
            Py_EndInterpreter(newInterpreter);
        }
        newInterpreter = Py_NewInterpreter();
        //PyThreadState_Swap(newInterpreter);

        // run python code in sub interpreter
        PyRun_SimpleString(code);
        */
        //PyRun_SimpleString(code);
        return 0;
    }

    int EMSCRIPTEN_KEEPALIVE resetState(const char* code) {
        StaticObject::modelRegistry = {};
        StaticObject::uniqueObjectCount = 0;
        AnimatedObject::modelRegistry = {};
        AnimatedObject::uniqueObjectCount = 0;
        // if (app->window != NULL) {
        //     glfwDestroyWindow(app->window);
        // }
        return 0;
    }

    int EMSCRIPTEN_KEEPALIVE toggleInput(int input) {
        // toggle some global value to allow/disallow keycallbacks, mouse callbacks, etc...
        if (input) {
            std::cout << "fullscreen: enable input" << std::endl;
            inputEnabled = 1;
        } else {
            std::cout << "fullscreen: disable input" << std::endl;
            inputEnabled = 0;
        }
        return 0;
    }
}


int main(int argc, char** argv)
{
    initializePyEnv();

    std::cout << "hello engine" << std::endl;
    // #ifdef USE_CPP
    //     app = getApplication();
    //     glfwSetJoystickCallback(joystick_callback);
    //     glfwSetKeyCallback(app->window, onKeyPressed);
    //     glfwSetCharCallback(app->window, onCharPressed);
    //     glfwSetCursorPosCallback(app->window, onMouseMoved);
    //     glfwSetMouseButtonCallback(app->window, onMouseClicked);
    //     glfwSetFramebufferSizeCallback(app->window, onWindowResized);
    //     glfwSetWindowCloseCallback(app->window, onWindowClose);
    //     while(!glfwWindowShouldClose(app->window))
    //     {
    //         app->gameLoop();
    //         glfwSwapBuffers(app->window);
    //         glFinish();
    //         glfwPollEvents();    
    //     }
    //     glfwDestroyWindow(app->window);
    //     glfwTerminate();
    // #endif   


    std::ifstream file;
    std::stringstream fileStream;
    //file.open(argv[1]);
    fileStream << argv[1];
    std::string cmd = fileStream.str();
    runPythonCode(cmd.c_str());

}

#endif