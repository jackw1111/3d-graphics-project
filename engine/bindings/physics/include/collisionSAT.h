#ifndef _COLLISIONS2_H
#define _COLLISIONS2_H
#include "../../../bindings/graphics/include/math_utils.h"
#include "../../../core/physics/include/collisionSAT.h"

// #include <boost/python/numpy.hpp>
// #include <boost/python/suite/indexing/vector_indexing_suite.hpp>
// #include <boost/python/detail/operator_id.hpp>
// #include <boost/make_shared.hpp>
// #include <boost/python.hpp>
// #include <boost/shared_ptr.hpp>

// #include <numeric>

// using namespace boost;
// using namespace boost::python;

// #define GLM_ENABLE_EXPERIMENTAL
// #include <glm/ext.hpp>

// #include <iostream>
// #include <vector>
// #include <glm/glm.hpp>


#include "engine.h"

void wrap_collisionSAT();

// struct iterable_converter
// {
//   /// @note Registers converter from a python interable type to the
//   ///       provided type.
//   template <typename Container>
//   iterable_converter&
//   from_python()
//   {
//     boost::python::converter::registry::push_back(
//       &iterable_converter::convertible,
//       &iterable_converter::construct<Container>,
//       boost::python::type_id<Container>());

//     // Support chaining.
//     return *this;
//   }

//   /// @brief Check if PyObject is iterable.
//   static void* convertible(PyObject* object)
//   {
//     return PyObject_GetIter(object) ? object : NULL;
//   }

//   /// @brief Convert iterable PyObject to C++ container type.
//   ///
//   /// Container Concept requirements:
//   ///
//   ///   * Container::value_type is CopyConstructable.
//   ///   * Container can be constructed and populated with two iterators.
//   ///     I.e. Container(begin, end)
//   template <typename Container>
//   static void construct(
//     PyObject* object,
//     boost::python::converter::rvalue_from_python_stage1_data* data)
//   {
//     namespace python = boost::python;
//     // Object is a borrowed reference, so create a handle indicting it is
//     // borrowed for proper reference counting.
//     python::handle<> handle(python::borrowed(object));

//     // Obtain a handle to the memory block that the converter has allocated
//     // for the C++ type.
//     typedef python::converter::rvalue_from_python_storage<Container>
//                                                                 storage_type;
//     void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

//     typedef python::stl_input_iterator<typename Container::value_type>
//                                                                     iterator;

//     // Allocate the C++ type into the converter's memory block, and assign
//     // its handle to the converter's convertible variable.  The C++
//     // container is populated by passing the begin and end iterators of
//     // the python object to the container's constructor.
//     new (storage) Container(
//       iterator(python::object(handle)), // begin
//       iterator());                      // end
//     data->convertible = storage;
//   }
// };

#endif