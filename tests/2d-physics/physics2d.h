/*----------------------------------------------------------------------------

2D Physics Test Program - a cheesy test harness for 2D physics

by Chris Hecker for my Game Developer Magazine articles.  See my homepage
for more information.

NOTE: This is a hacked test program, not a nice example of Windows programming.
physics.cpp the only part of this you should look at!!!

This material is Copyright 1997 Chris Hecker, All Rights Reserved.
It's for you to read and learn from, not to put in your own articles
or books or on your website, etc.  Thank you.

Chris Hecker
checker@d6.com
http://www.d6.com/users/checker

*/

/*----------------------------------------------------------------------------

physics.h - The header for the physics demo.

10/15/96 - checker

*/

#if !defined(PHYSICS_H)
#define PHYSICS_H

// explicit dependencies

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
using namespace boost;
using namespace boost::python;


float const REAL_MAX = FLT_MAX;
float const REAL_MIN = FLT_MIN;

float const PI = 3.14159265358979323846f;
float const Epsilon = 0.00001f;


inline glm::vec2 GetPerpendicular( glm::vec2 const &A )
{
	return glm::vec2(-A.y,A.x);
}



/*----------------------------------------------------------------------------

Declarations for physics code.

*/

struct bounding_box
{
	std::vector<glm::vec2> aVertices;
	bounding_box() {
		aVertices.push_back(glm::vec2(0,0));
		aVertices.push_back(glm::vec2(0,0));
		aVertices.push_back(glm::vec2(0,0));
		aVertices.push_back(glm::vec2(0,0));
	}

} BoundingBox;

struct configuration
{
	glm::vec2 CMPosition;
	float Orientation;

	glm::vec2 CMVelocity;
	float AngularVelocity;

	glm::vec2 CMForce;
	float Torque;

	bounding_box BoundingBox;
}; 

bool operator==(configuration lhs, const configuration rhs)
{
    return (const configuration)lhs == rhs;
}

struct rigid_body
{
	float Width, Height;
	float OneOverMass, OneOverCMMomentOfInertia;
	float CoefficientOfRestitution;

	glm::vec2 position;
	float orientation;

	unsigned int id;

	enum { NumberOfConfigurations = 2 };

	rigid_body(float Density, float Width, float Height,
					float CoefficientOfRestitution, glm::vec2 pos, glm::vec2 vel, float ang_vel );

	glm::vec2 getPosition();
	void setPosition(glm::vec2 p);


	float getOrientation();
	void setOrientation(float o);

	std::vector<configuration> aConfigurations;

	static std::vector<rigid_body> aBodies;
};

bool operator==(rigid_body lhs, const rigid_body rhs)
{
    return lhs.id == rhs.id;
}

struct wall
{
	// define wall by plane equation
	glm::vec2 Normal;		// inward pointing
	float c;					// ax + by + c = 0

	// points for drawing wall
	glm::vec2 StartPoint;
	glm::vec2 EndPoint;
};

bool operator==(wall lhs, const wall rhs)
{
    return (const wall)lhs == rhs;
}

class simulation_world
{
public:
	std::vector<float> vertices;

	simulation_world( float WorldWidth, float WorldHeight );
	
	~simulation_world( void );

	int Penetrating = 0;
	int Colliding = 1;
	int Clear = 2;

	int CollisionState;

	glm::vec2 CollisionNormal;
	int CollidingBodyIndex;
	int CollidingCornerIndex;

	int SourceConfigurationIndex;
	int TargetConfigurationIndex;

	float WorldWidth, WorldHeight;

	int NumberOfWalls = 5;

	std::vector<wall> aWalls = {wall(), wall(), wall(), wall(), wall()};
	enum { NumberOfBodies = 2 };
};

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


#endif
