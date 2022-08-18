#include "../../../bindings/physics/include/collisionSAT.h"

bool operator==(Face2 f1, const Face2 f2) {
    return f1.verts_ws.at(0) == f2.verts_ws.at(0) &&
           f1.verts_ws.at(1) == f2.verts_ws.at(1) &&
           f1.verts_ws.at(2) == f2.verts_ws.at(2);
}

bool operator==(Query2 q1, const Query2 q2) {
    return q1.best_distance == q2.best_distance &&
           q1.best_index == q2.best_index &&
           q1.best_axis == q2.best_axis;
}
bool operator==(Edge2 e1, const Edge2 e2) {
    return e1.v1_ws == e2.v1_ws &&
           e1.v2_ws == e2.v2_ws;
}

void wrap_collisionSAT() {

    python::class_<Query2>("Query2", python::init<float, vec2, vec3>())
    .def_readwrite("best_distance", &Query2::best_distance)
    .def_readwrite("best_index", &Query2::best_index)
    .def_readwrite("best_axis", &Query2::best_axis)
    .def_readwrite("id", &Query2::id)
    ;

    python::class_<Edge2>("Edge2", python::init<vector<int>, vector<int>>())
    .def_readwrite("v1_index", &Edge2::v1_index)
    .def_readwrite("v2_index", &Edge2::v2_index)
    .def_readwrite("v1_ws", &Edge2::v1_ws)
    .def_readwrite("v2_ws", &Edge2::v2_ws)
    .def_readwrite("faces_indices", &Edge2::faces_indices)
    .def_readwrite("faces", &Edge2::faces)
    .def("set_data", &Edge2::set_data)
    ;

    python::class_<Face2>("Face2", python::init<vector<int>>())
    .def_readwrite("verts_indices", &Face2::verts_indices)
    .def_readwrite("verts_ws", &Face2::verts_ws)
    .def_readwrite("position_ws", &Face2::position_ws)
    .def_readwrite("normal_ws", &Face2::normal_ws)
    .def("set_data", &Face2::set_data)
    ;   

    iterable_converter()
    // Build-in type.
    .from_python<vector<Face2> >()
    .from_python<vector<Edge2> >()
    .from_python<vector<Query2> >()
    ;

    boost::python::class_< vector < Face2 > >("FaceVector")
    .def(boost::python::vector_indexing_suite<vector< Face2 > >());
    boost::python::class_< vector < Edge2 > >("EdgeVector")
    .def(boost::python::vector_indexing_suite<vector< Edge2 > >());
    boost::python::class_< vector < Query2 > >("QueryVector")
    .def(boost::python::vector_indexing_suite<vector< Query2 > >());

    // collision
    python::def("get_support", &get_support);
    python::def("query_edge_directions", &query_edge_directions);
    python::def("query_face_directions", &query_face_directions);
    python::def("SAT2", &SAT2);
    python::def("closest_point_on_cylinder", &closest_point_on_cylinder);
    python::def("sphere_cylinder_collision", &sphere_cylinder_collision);
    python::def("sphere_sphere_collision", &sphere_sphere_collision);
    python::def("sphere_box_collision", &sphere_box_collision);
    python::def("ray_triangle_collision", &ray_triangle_collision);
}
