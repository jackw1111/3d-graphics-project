#include "../../../bindings/graphics/include/culling.h"
#include "../../../bindings/graphics/include/math_utils.h"

void wrap_Frustum() {
    python::class_<Frustum, boost::shared_ptr<Frustum> >("Frustum", python::no_init)
    .def_readwrite("centre_near", &Frustum::Cnear)
    .def_readwrite("centre_far", &Frustum::Cfar)
    .def_readwrite("top_right_far", &Frustum::topRightFar)
    .def_readwrite("top_left_far", &Frustum::topLeftFar)
    .def_readwrite("top_right_near", &Frustum::topRightNear)
    .def_readwrite("top_left_near", &Frustum::topLeftNear)
    .def_readwrite("bottom_left_near", &Frustum::bottomLeftNear)
    .def_readwrite("bottom_left_far", &Frustum::bottomLeftFar)
    .def_readwrite("bottom_right_near", &Frustum::bottomRightNear)
    .def_readwrite("bottom_right_far", &Frustum::bottomRightFar)
    .def_readwrite("right_normal", &Frustum::rightNormal)
    .def_readwrite("left_normal", &Frustum::leftNormal)
    .def_readwrite("top_normal", &Frustum::topNormal)
    .def_readwrite("bottom_normal", &Frustum::bottomNormal)
    .def_readwrite("back_normal", &Frustum::backNormal)
    .def_readwrite("front_normal", &Frustum::frontNormal)
    ;

    python::class_<Query>("Query", python::init<float, std::vector<int>, glm::vec3>())
    .def_readwrite("max_seperation", &Query::max_seperation)
    .def_readwrite("max_index", &Query::max_index)
    .def_readwrite("best_axis", &Query::best_axis)
    .def_readwrite("type", &Query::type)
    ;

    iterable_converter()
    // Build-in type.
    .from_python<std::vector<Query> >()
    .from_python<std::vector<int> >()
    .from_python<std::vector<Face> >()
    .from_python<std::vector<Edge> >()
    ;
    boost::python::class_< std::vector < Query > >("QueryVector")
    .def(boost::python::vector_indexing_suite<std::vector< Query > >());
    boost::python::class_< std::vector < int > >("IntVector")
    .def(boost::python::vector_indexing_suite<std::vector< int > >());
    boost::python::class_< std::vector < Edge > >("EdgeVector")
    .def(boost::python::vector_indexing_suite<std::vector< Edge > >());
    boost::python::class_< std::vector < Face > >("FaceVector")
    .def(boost::python::vector_indexing_suite<std::vector< Face > >());

    python::class_<Body>("Body", python::init<>())
    .def_readwrite("cm_position", &Body::cm_position)
    .def_readwrite("faces", &Body::faces)
    .def_readwrite("edges", &Body::edges)
    .def("update", &Body::update)
    ;


    python::class_<Edge>("Edge", python::init<glm::vec3, glm::vec3>())
    .def_readwrite("v0", &Edge::v0)
    .def_readwrite("v1", &Edge::v1)
    .def_readwrite("faces", &Edge::face_indices)
    ;


    python::class_<Face>("Face", python::init<glm::vec3, glm::vec3, glm::vec3>())
    .def_readwrite("vertices", &Face::vertices)
    .def_readwrite("position", &Face::position)
    .def_readwrite("normal", &Face::normal)
    ;   


    python::def("SAT", &SAT);
    // python::def("get_reference_faces", &get_reference_faces);
    // python::def("get_incident_faces", &get_incident_faces);
    // python::def("get_orthogonal_faces", &get_orthogonal_faces);


}