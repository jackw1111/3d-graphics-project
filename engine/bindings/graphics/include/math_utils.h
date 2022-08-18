#ifndef BINDING_MATH_UTILS_H
#define BINDING_MATH_UTILS_H
#include "engine.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;

glm::vec2* mul_mat2_vec2(glm::mat2 const &mat, glm::vec2 const &vec);
glm::mat2* mul_mat2_mat2(glm::mat2 const &m1, glm::mat2 const &m2);
glm::vec2* rotate_mat2(glm::mat2 m, float angle, glm::vec2 axis);

glm::mat4* mul_mat4(glm::mat4 const &mat1, glm::mat4 const &mat2);
glm::vec4* mul_mat4_vec4(glm::mat4 const &mat, glm::vec4 const &vec);
std::string print_mat4(glm::mat4 const &m);
glm::vec3* mul_mat3_vec3(glm::mat3 const &mat, glm::vec3 const &vec);

boost::shared_ptr<glm::mat4> make_mat4(float m);
boost::shared_ptr<glm::mat3> make_mat3( glm::vec3 right,  glm::vec3 up, glm::vec3 front);

struct vec4Wrap : glm::vec4, boost::python::wrapper<glm::vec4>
{
  vec4Wrap(float x, float y, float z, float w);
  vec4Wrap(float a);
  vec4Wrap();

};

struct vec3Wrap : glm::vec3, boost::python::wrapper<glm::vec3>
{
  vec3Wrap(float x, float y, float z);
  vec3Wrap(float a);
  vec3Wrap();

};


struct vec2Wrap : glm::vec2, boost::python::wrapper<glm::vec2>
{
  vec2Wrap(float x, float y);
  vec2Wrap(float a);
  vec2Wrap();

};

void wrap_vec2();
void wrap_vec3();
void wrap_vec4();
void wrap_mat2();
void wrap_mat4();
void wrap_mat3();
void wrap_quat();

// Declare the following in each translation unit
struct void_ {};
BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(void_);
/// @brief Type that allows for registration of conversions from
///        python iterable types.
struct iterable_converter
{
  /// @note Registers converter from a python interable type to the
  ///       provided type.
  template <typename Container>
  iterable_converter&
  from_python()
  {
    boost::python::converter::registry::push_back(
      &iterable_converter::convertible,
      &iterable_converter::construct<Container>,
      boost::python::type_id<Container>());

    // Support chaining.
    return *this;
  }

  /// @brief Check if PyObject is iterable.
  static void* convertible(PyObject* object)
  {
    return PyObject_GetIter(object) ? object : NULL;
  }

  /// @brief Convert iterable PyObject to C++ container type.
  ///
  /// Container Concept requirements:
  ///
  ///   * Container::value_type is CopyConstructable.
  ///   * Container can be constructed and populated with two iterators.
  ///     I.e. Container(begin, end)
  template <typename Container>
  static void construct(
    PyObject* object,
    boost::python::converter::rvalue_from_python_stage1_data* data)
  {
    namespace python = boost::python;
    // Object is a borrowed reference, so create a handle indicting it is
    // borrowed for proper reference counting.
    python::handle<> handle(python::borrowed(object));

    // Obtain a handle to the memory block that the converter has allocated
    // for the C++ type.
    typedef python::converter::rvalue_from_python_storage<Container>
                                                                storage_type;
    void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

    typedef python::stl_input_iterator<typename Container::value_type>
                                                                    iterator;

    // Allocate the C++ type into the converter's memory block, and assign
    // its handle to the converter's convertible variable.  The C++
    // container is populated by passing the begin and end iterators of
    // the python object to the container's constructor.
    new (storage) Container(
      iterator(python::object(handle)), // begin
      iterator());                      // end
    data->convertible = storage;
  }
};

glm::vec3 getPixel(unsigned int x, unsigned int y);

struct custom_deleter {
   glm::mat4 operator()(glm::mat4* f);
};


struct mat4Wrap : glm::mat4, boost::python::wrapper<glm::mat4>
{

  glm::mat4* operator*(glm::mat4 const &mat);
  glm::mat4* __lookAt (glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &up);
};

struct mat2Wrap : glm::mat2, boost::python::wrapper<glm::mat2>
{

  glm::mat2* operator*(glm::mat2 const &mat);
};

struct quatWrap : glm::quat, boost::python::wrapper<glm::quat>
{

};

struct custom_deleter_quat {
   glm::quat operator()(glm::quat* m);
};

struct custom_deleter_mat2 {
   glm::mat2 operator()(glm::mat2* m);
};

struct custom_deleter_mat4 {
   glm::mat4 operator()(glm::mat4* m);
};

struct custom_deleter_mat3 {
   glm::mat3 operator()(glm::mat3* m);
};

struct custom_deleter_vec3 {
   glm::vec3 operator()(glm::vec3* f);
};

struct custom_deleter_vec2 {
   glm::vec2 operator()(glm::vec2* f);
};

struct custom_deleter_vec4 {
   glm::vec4 operator()(glm::vec4* f);
};

glm::vec4* mul_vec4(glm::vec4 const &v1, float const &f);
glm::vec4* add_vec4(glm::vec4 const &v1, glm::vec4 const &v2);

boost::shared_ptr<glm::vec4> make_vec4(float x, float y, float z, float w);

glm::vec3* mul_vec3(glm::vec3 const &v1, float const &f);
glm::vec3* add_vec3(glm::vec3 const &v1, glm::vec3 const &v2);

boost::shared_ptr<glm::vec3> make_vec3(float x, float y, float z);

glm::vec2* mul_vec2(glm::vec2 const &v1, float const &f);
glm::vec2* add_vec2(glm::vec2 const &v1, glm::vec2 const &v2);

boost::shared_ptr<glm::vec2> make_vec2(float x, float y);

struct custom_deleter_float {
   float operator()(float* f);
};

struct custom_deleter_vecvecVec3 {
  vector<vector<vec3>> operator()(vector<vector<vec3>>* f);
};


glm::vec3* __add__(glm::vec3 const &v1, glm::vec3 const &v2);
glm::vec3* _mul(glm::vec3 const &v1, glm::vec3 const &v2);


glm::vec4* newVec4(float x, float y, float z, float w);
std::string print_vec4(glm::vec4 const &v);

glm::vec3* newVec3(float x, float y, float z);
std::string print_vec3(glm::vec3 const &v);

glm::vec2* newVec2(float x, float y);
std::string print_vec2(glm::vec2 const &v);

/*glm::vec3* _cross(glm::vec3 const &v1,  glm::vec3 const &v2)
float* _dot(glm::vec3 const &v1,  glm::vec3 const &v2)*/

glm::mat4* _inverse(glm::mat4 const &m);
glm::vec3* _normalize(glm::vec3 const &v);

glm::vec2* _normalize_vec2(glm::vec2 const &m);

glm::mat4* _translate(glm::mat4 const &m, glm::vec3 const &v);
glm::mat4* _rotate (glm::mat4 const &m, const float &rot, glm::vec3 const &v);

glm::mat4* _scale(glm::mat4 const &m, glm::vec3 const &v);

glm::mat4* _lookAt (glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &up);
glm::mat4* _perspective(float const &fovy, float const &aspect, float const &near, float const &far);

int setCursorVisible(GLFWwindow* window, bool isVisible);

int isJoystickPresent();

std::vector<float> getJoystickAxes();

int loadGL();


glm::vec4* get_index_mat4(glm::mat4 const &mat, int index);

void set_index_mat4(glm::mat4 &mat, int index, glm::vec4 value);


glm::vec3* get_index_mat3(glm::mat3 const &mat, int index);

void set_index_mat3(glm::mat3 &mat, int index, glm::vec3 value);


glm::quat* _normalize_quat(glm::quat const &m);
glm::mat4* quat_mat4_cast(glm::quat const &q);
glm::quat* mul_quat_float(glm::quat const &mat1, float v);

glm::quat* add_quat(glm::quat const &mat1, glm::quat const &mat2);
glm::quat* mul_quat(glm::quat const &mat1, glm::quat const &mat2);
glm::mat4* mul_mat4(glm::mat4 const &mat1, glm::mat4 const &mat2);
glm::vec4* mul_mat4_vec4(glm::mat4 const &mat, glm::vec4 const &vec);

glm::mat3* mul_mat3(glm::mat3 const &mat1, glm::mat3 const &mat2);
glm::mat3* add_mat3(glm::mat3 const &mat1, glm::mat3 const &mat2);
glm::vec3* mul_mat3_vec3(glm::mat3 const &mat, glm::vec3 const &vec);
glm::mat3* mul_mat3_float(glm::mat3 const &mat, float f);

glm::mat3* _inverse_mat3(glm::mat3 const &m);
glm::mat3* _transpose_mat3(glm::mat3 const &m);
glm::mat3* _orthonormalize_orientation(glm::mat3 m);
glm::mat3* _form_orthogonal_basis(glm::vec3 const &vector);
glm::mat3* _skew_symmetric(glm::vec3 const &vector);

glm::mat3* _mat3_cast (glm::mat4 const &m);
std::string print_mat3(glm::mat3 const &m);

std::string print_mat4(glm::mat4 const &m);

boost::shared_ptr<glm::mat2> make_mat2(float v1, float v2, float v3, float v4);
boost::shared_ptr<glm::mat4> make_mat4(float m);

boost::shared_ptr<glm::quat> make_quat(float w, float x, float y, float z);

glm::vec4* mul_vec4(glm::vec4 const &v1, float const &f);
glm::vec4* mul_vec4_mat4(glm::vec4 const &vec, glm::mat4 const &mat);


glm::vec3* mul_vec3(glm::vec3 const &v1, float const &f);

glm::vec2* mul_vec2(glm::vec2 const &v1, float const &f);
vector<vector<vec3>>* add_vecvecVec3(vector<vector<vec3>> const &v1, vector<vector<vec3>> const &v2);

glm::vec4* div_vec4(glm::vec4 const &v1, glm::vec4 const &v2);
glm::vec4* add_vec4(glm::vec4 const &v1, glm::vec4 const &v2);
glm::mat4* copy_mat4(glm::mat4 v1);
glm::vec2* copy_vec2(glm::vec2 v1);
glm::vec3* copy_vec3(glm::vec3 v1);



boost::shared_ptr<glm::vec4> make_vec4(float x, float y, float z, float w);

boost::shared_ptr<glm::vec4> make_vec4_from_vec3(glm::vec3 v, float w);
glm::vec3* div_vec3(glm::vec3 const &v1, glm::vec3 const &v2);

glm::vec3* add_vec3(glm::vec3 const &v1, glm::vec3 const &v2);

glm::vec3* sub_vec3(glm::vec3 const &v1, glm::vec3 const &v2);
boost::shared_ptr<glm::vec3> make_vec3(float x, float y, float z);

glm::vec2* div_vec2(glm::vec2 const &v1, glm::vec2 const &v2);

glm::vec2* add_vec2(glm::vec2 const &v1, glm::vec2 const &v2);

glm::vec2* sub_vec2(glm::vec2 const &v1, glm::vec2 const &v2);
boost::shared_ptr<glm::vec2> make_vec2(float x, float y);

glm::vec3* __add__(glm::vec3 const &v1, glm::vec3 const &v2);

glm::vec3* _mul(glm::vec3 const &v1, glm::vec3 const &v2);
glm::vec4* newVec4(float x, float y, float z, float w);

std::string print_vec4(glm::vec4 const &v);
float get_index_vec4(glm::vec4 const &v, int index);
void set_index_vec4(glm::vec4 &v, int index, float value);
bool eq_vec3(glm::vec3 rhs, glm::vec3 lhs);
bool eq_vec2(glm::vec2 rhs, glm::vec2 lhs);
float get_index_vec3(glm::vec3 const &v, int index);

void set_index_vec3(glm::vec3 &v, int index, float value);


glm::vec3* newVec3(float x, float y, float z);
std::string print_vec3(glm::vec3 const &v);
glm::vec2* newVec2(float x, float y);
std::string print_vec2(glm::vec2 const &v);

glm::vec3* _cross(glm::vec3 const &v1,  glm::vec3 const &v2);

float _dot3(glm::vec3 const &v1,  glm::vec3 const &v2);

float _length3(glm::vec3 const &v1);
float _length2(glm::vec2 const &v1);


glm::vec3* _normalize(glm::vec3 const &m);

float _dot2(glm::vec2 const &v1,  glm::vec2 const &v2);
float _distance(glm::vec3 const &v1,  glm::vec3 const &v2);

float _distance2(glm::vec2 const &v1,  glm::vec2 const &v2);
// TO DO:
// does unique ptr need releasing? Check answers by Tanner Sansbury on SO

glm::mat4* _interpolate(glm::mat4 const &m1, glm::mat4 const &m2, float delta);
glm::mat4* _inverse(glm::mat4 const &m);
glm::mat4* _transpose(glm::mat4 const &m);
glm::mat4* mat3_mat4_cast(glm::mat3 const &m);
glm::mat4* _translate(glm::mat4 const &m, glm::vec3 const &v);
glm::mat4* _rotate (glm::mat4 const &m, const float &rot, glm::vec3 const &v);
glm::mat4* _scale(glm::mat4 const &m, glm::vec3 const &v);

glm::mat4* _lookAt (glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &up);
glm::mat4* _perspective(float const &fovy, float const &aspect, float const &near, float const &far);
glm::mat4* _ortho(float const &left, float const &right, float const &bottom, float const &top, float const &near, float const &far);

int setVsync(int value);

int setCullFace(int value);

int setDepthTest(int value);

int clearDepthBuffer();
int clearColorBuffer();

int clearColor(float r, float g, float b);

int setAlphaTransparency(int value);
/// @brief checks the state (pressed, released, held, etc...) of the queried button 
/// @param window the current window
/// @param
int getMouseButton(GLFWwindow* window, int button);

void wrap_mathUtils();

#endif