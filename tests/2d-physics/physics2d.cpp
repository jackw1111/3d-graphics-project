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

physics.cpp - This file implements the 2D physics simulator.

10/15/96 - checker

*/

#include <assert.h>
#include <iostream>

#include "physics2d.h"


#include <numeric>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>


static float StartTime = 0.f;

float GetTime( void )
{
    // @todo how do you get an accurate high precision time on glut?
    // @todo MSVC's clock() sucks completely
#if 0
    static clock_t StartTime = clock();
    assert(StartTime != (clock_t)(-1));
    return ((float)(clock()-StartTime))/((float)CLOCKS_PER_SEC);
#else
    StartTime += 0.005;
    return StartTime;
#endif  
}


// weird CodeWarrior bug with global initialization forces me to do this
#define WIDTH 400
#define HEIGHT 400

int WorldWidth = WIDTH;
int WorldHeight = HEIGHT;


int GravityActive = 1;
glm::vec2 Gravity(0.0f, -10.0f);


rigid_body::rigid_body(float Density, float _Width, float _Height,
					float _CoefficientOfRestitution, glm::vec2 pos, glm::vec2 vel, float ang_vel)
{				

	float const Mass = Density * _Width * _Height;

	CoefficientOfRestitution = _CoefficientOfRestitution;

	Width = _Width;
	Height = _Height;

	OneOverMass = 1.0f / Mass;

	// integrate over the body to find the moment of inertia

	OneOverCMMomentOfInertia = 1.0f / ((Mass / 12.0f) *
		(_Width * _Width + _Height * _Height));

	aConfigurations.push_back(configuration());
	aConfigurations.push_back(configuration());

	// 0-out non-vector quantities
	aConfigurations[0].Orientation = 0.0f;
	aConfigurations[0].AngularVelocity = 0.0f;
	aConfigurations[0].Torque = 0.0f;

	aConfigurations[0].CMPosition = glm::vec2(pos.x, pos.y);
	aConfigurations[0].CMVelocity = glm::vec2(vel.x, vel.y);
	aConfigurations[0].AngularVelocity = ang_vel;

	id = rigid_body::aBodies.size();

	rigid_body::aBodies.push_back(*this);
}

glm::vec2 rigid_body::getPosition() {
	return rigid_body::aBodies[id].position;
}

void rigid_body::setPosition(glm::vec2 p) {
	rigid_body::aBodies[id].position = p;
}


float rigid_body::getOrientation() {
	return rigid_body::aBodies[id].orientation;
}

void rigid_body::setOrientation(float o) {
	rigid_body::aBodies[id].orientation = o;
}


std::vector<rigid_body> rigid_body::aBodies = {};

/*----------------------------------------------------------------------------

simulation_world ctor

*/

simulation_world::simulation_world( float WorldWidth_, float WorldHeight_ ) :
	WorldWidth(WorldWidth_), WorldHeight(WorldHeight_),
	SourceConfigurationIndex(0), TargetConfigurationIndex(1)
{

}

/*----------------------------------------------------------------------------

simulation_world dtor

*/

simulation_world::~simulation_world( void )
{

}



struct SimulationWorldWrap : simulation_world, boost::python::wrapper<simulation_world>
{

	SimulationWorldWrap(float WorldWidth, float WorldHeight) : simulation_world(WorldWidth, WorldHeight) {}

};

struct RigidBodyWrap : rigid_body, boost::python::wrapper<rigid_body> {
	RigidBodyWrap(float Density, float Width, float Height, float CoefficientOfRestitution, glm::vec2 pos, glm::vec2 vel, float ang_vel) : rigid_body(Density, Width, Height, CoefficientOfRestitution, pos, vel, ang_vel) {};
};

struct BoundingBoxWrap : bounding_box, boost::python::wrapper<bounding_box> {
	BoundingBoxWrap() : bounding_box() {};
};

struct ConfigurationWrap : configuration, boost::python::wrapper<configuration> {
	ConfigurationWrap() : configuration() {};
};

struct WallWrap : wall, boost::python::wrapper<wall> {
	WallWrap() : wall() {};
};


BOOST_PYTHON_MODULE(physics2d)
{

 //    python::class_<SimulationWorldWrap, boost::noncopyable>("SimulationWorld", python::init<float, float>())
	// .def_readwrite("collision_normal", &SimulationWorldWrap::CollisionNormal)
	// .def_readwrite("colliding_body_index", &SimulationWorldWrap::CollidingBodyIndex)
	// .def_readwrite("colliding_corner_index", &SimulationWorldWrap::CollidingCornerIndex)
	// .def_readwrite("source_configuration_index", &SimulationWorldWrap::SourceConfigurationIndex)
	// .def_readwrite("target_configuration_index", &SimulationWorldWrap::TargetConfigurationIndex)
	// .def_readwrite("number_of_walls", &SimulationWorldWrap::NumberOfWalls)
	// .def_readwrite("aWalls", &SimulationWorldWrap::aWalls)
	// .def_readwrite("collision_state", &SimulationWorldWrap::CollisionState)
	// .def_readwrite("vertices", &SimulationWorldWrap::vertices)
	// ;

 //    python::class_<BoundingBoxWrap, boost::noncopyable>("bounding_box", python::init<>())
	// .def_readwrite("aVertices", &BoundingBoxWrap::aVertices)
 //    ;


 //    python::class_<RigidBodyWrap, boost::noncopyable>("RigidBody", python::init<float, float, float, float, glm::vec2, glm::vec2, float>())
 //    .add_property("position", &RigidBodyWrap::getPosition, &RigidBodyWrap::setPosition)
 //    .add_property("orientation", &RigidBodyWrap::getOrientation, &RigidBodyWrap::setOrientation)
	// .def_readwrite("aBodies", &RigidBodyWrap::aBodies)
	// .def_readwrite("aConfigurations", &RigidBodyWrap::aConfigurations)
	// .def_readwrite("OneOverMass", &RigidBodyWrap::OneOverMass)
	// .def_readwrite("OneOverCMMomentOfInertia", &RigidBodyWrap::OneOverCMMomentOfInertia)
	// .def_readwrite("width", &RigidBodyWrap::Width)
	// .def_readwrite("height", &RigidBodyWrap::Height)
	// .def_readwrite("coefficient_of_restitution", &RigidBodyWrap::CoefficientOfRestitution)
 //    ;

 //    python::class_<ConfigurationWrap, boost::noncopyable>("Configuration", python::init<>())
	// .def_readwrite("bounding_box", &ConfigurationWrap::BoundingBox)
	// .def_readwrite("CMPosition", &ConfigurationWrap::CMPosition)
	// .def_readwrite("Orientation", &ConfigurationWrap::Orientation)
	// .def_readwrite("CMVelocity", &ConfigurationWrap::CMVelocity)
	// .def_readwrite("AngularVelocity", &ConfigurationWrap::AngularVelocity)
	// .def_readwrite("CMForce", &ConfigurationWrap::CMForce)
	// .def_readwrite("Torque", &ConfigurationWrap::Torque)
	// ;

 //    iterable_converter()
 //    .from_python<std::vector<configuration> >()
 //    ;

 //    boost::python::class_<std::vector< configuration > >("ConfigurationVector")
 //        .def(boost::python::vector_indexing_suite<std::vector< configuration > >());


 //    iterable_converter()
 //    .from_python<std::vector<rigid_body> >()
 //    ;

 //    boost::python::class_<std::vector< rigid_body > >("RigidBodyVector")
 //        .def(boost::python::vector_indexing_suite<std::vector< rigid_body > >());

 //    iterable_converter()
 //    .from_python<std::vector<glm::vec2> >()
 //    ;

 //    boost::python::class_<std::vector< glm::vec2 > >("Vec2Vector")
 //        .def(boost::python::vector_indexing_suite<std::vector< glm::vec2 > >());


 //    python::class_<WallWrap, boost::noncopyable>("wall", python::init<>())
 //    .def_readwrite("normal", &WallWrap::Normal)
 //    .def_readwrite("c", &WallWrap::c)
 //    .def_readwrite("start_point", &WallWrap::StartPoint)
 //    .def_readwrite("end_point", &WallWrap::EndPoint)
 //    ;

 //    iterable_converter()
 //    .from_python<std::vector<wall> >()
 //    ;

 //    boost::python::class_<std::vector< wall > >("WallVector")
 //        .def(boost::python::vector_indexing_suite<std::vector< wall > >());
}


