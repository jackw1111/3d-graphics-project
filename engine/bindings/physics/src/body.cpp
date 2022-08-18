
#include "../../../bindings/physics/include/body.h"


bool operator==(RigidBody2 r1, const RigidBody2 r2) {
    return &r1 == &r2;
}

void wrap_RigidBody() {

    iterable_converter()
    // Build-in type.
    .from_python<vector<RigidBody2> >()
    .from_python<vector<vec4> >()
    ;

    boost::python::class_< vector < RigidBody2 > >("RigidBodyVector")
    .def(boost::python::vector_indexing_suite<vector< RigidBody2 > >());
    boost::python::class_< vector < vec4 > >("Vec4Vector")
    .def(boost::python::vector_indexing_suite<vector< vec4 > >());

    python::class_<RigidBody2>("RigidBody2", python::init<>())
    .def_readwrite("octree_id", &RigidBody2::octreeID)
    .def_readwrite("tri_ids", &RigidBody2::triIDs)
    //.def_readwrite("object", &RigidBody2::object)
    .def_readwrite("coefficient_of_restitution", &RigidBody2::coefficient_of_restitution)
    .def_readwrite("mass", &RigidBody2::mass)
    .def_readwrite("one_over_mass", &RigidBody2::one_over_mass)
    .def_readwrite("density", &RigidBody2::density)
    .def_readwrite("friction", &RigidBody2::friction)
    .def_readwrite("cm_position", &RigidBody2::cm_position)
    .def_readwrite("orientation", &RigidBody2::orientation)
    .def_readwrite("cm_velocity", &RigidBody2::cm_velocity)
    .def_readwrite("angular_momentum", &RigidBody2::angular_momentum)
    .def_readwrite("cm_force", &RigidBody2::cm_force)
    .def_readwrite("torque", &RigidBody2::torque)
    .def_readwrite("inverse_world_inertia_tensor", &RigidBody2::inverse_world_inertia_tensor)
    .def_readwrite("inverse_body_inertia_tensor", &RigidBody2::inverse_body_inertia_tensor)
    .def_readwrite("angular_velocity", &RigidBody2::angular_velocity)
    .def_readwrite("a_bounding_vertices", &RigidBody2::a_bounding_vertices)
    .def("integrate_forces", &RigidBody2::integrate_forces)
    .def("integrate_velocities", &RigidBody2::integrate_velocities)
    .def("get_model_matrix", &RigidBody2::get_model_matrix)
    .def("update", &RigidBody2::update)
    .def_readwrite("faces", &RigidBody2::faces)
    .def_readwrite("edges", &RigidBody2::edges)
    .def("calculate_vertices", &RigidBody2::calculate_vertices)
    .def_readwrite("a_body_bounding_vertices", &RigidBody2::a_body_bounding_vertices)
    .def_readwrite("number_of_bounding_vertices", &RigidBody2::number_of_bounding_vertices)
    .def_readwrite("dX2", &RigidBody2::dX2)
    .def_readwrite("dY2", &RigidBody2::dY2)
    .def_readwrite("dZ2", &RigidBody2::dZ2)
    .def_readwrite("offset", &RigidBody2::offset)
    .def_readwrite("id", &RigidBody2::id)
    //.def_readwrite("aabb_min_ws", &RigidBody2::aabb_min_ws)
    //.def_readwrite("aabb_max_ws", &RigidBody2::aabb_max_ws)
    ;

    boost::python::scope().attr("NONE") = NONE; 
    boost::python::scope().attr("TRIANGLE") = TRIANGLE; 
    boost::python::scope().attr("BOX") = BOX; 
    boost::python::scope().attr("CYLINDER") = CYLINDER; 
    boost::python::scope().attr("SPHERE") = SPHERE; 
    boost::python::scope().attr("PLANE") = PLANE; 
    boost::python::scope().attr("CAPSULE") = CAPSULE; 

    python::def("init_cylinder", &init_cylinder);
    python::def("init_sphere", &init_sphere);
    python::def("init_box", &init_box);
    python::def("init_triangle", &init_triangle);
    python::def("init_triangle_mesh", &init_triangle_mesh);
    python::def("init_plane", &init_plane);
    python::def("init_capsule", &init_capsule);


    python::class_<Tri>("Tri", python::init<int, std::vector<vec3>>())
    .def_readwrite("data", &Tri::data)
    .def_readwrite("id", &Tri::id)
    ;

    python::class_<TriOctree>("TriOctree", python::init<>())
    .def_readwrite("index", &TriOctree::index)
    .def_readwrite("position", &TriOctree::position)
    .def_readwrite("hw", &TriOctree::hw)
    .def_readwrite("triangles", &TriOctree::triangles)
    .def_readwrite("xMin", &TriOctree::xMin)
    .def_readwrite("yMin", &TriOctree::yMin)
    .def_readwrite("zMin", &TriOctree::zMin)
    .def_readwrite("xMax", &TriOctree::xMax)
    .def_readwrite("yMax", &TriOctree::yMax)
    .def_readwrite("zMax", &TriOctree::zMax)
    .def_readwrite("children", &TriOctree::children)
    .def_readwrite("MAX_TRIANGLES", &TriOctree::MAX_TRIANGLES)
    ;

    python::def("is_point_in_cube", &is_point_in_cube);
    python::def("is_triangle_in_cube", &is_triangle_in_cube);
    python::def("get_triangle_set", &get_triangle_set);
    python::def("create_children", &create_children);
    python::def("init_octree", &init_octree);
    python::def("get_all_nodes", &getAllNodes);

}
