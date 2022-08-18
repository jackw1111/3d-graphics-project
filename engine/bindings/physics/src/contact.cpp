#include "../../../bindings/physics/include/contact.h"

bool operator==(const Contact f1, const Contact f2) {
    return f1.body_a == f2.body_a && f1.body_b == f2.body_b;
}
bool operator==(const Tri t1, const Tri t2) {
    return &t1 == &t2;
}
bool operator==(const TriOctree t1, const TriOctree t2) {
    return &t1 == &t2;
}
void wrap_Contact() {

    iterable_converter()
    // Build-in type.
    .from_python<vector<Contact> >()
    .from_python<vector<Tri> >()
    .from_python<vector<TriOctree> >()

    ;

    boost::python::class_< vector < Contact > >("ContactVector")
    .def(boost::python::vector_indexing_suite<vector< Contact > >());

    boost::python::class_< vector < Tri > >("TriVector")
    .def(boost::python::vector_indexing_suite<vector< Tri > >());

    boost::python::class_< vector < TriOctree > >("TriOctreeVector")
    .def(boost::python::vector_indexing_suite<vector< TriOctree > >());

    python::class_<Contact>("Contact", python::init<RigidBody2&, RigidBody2&>())
    .def_readwrite("normal", &Contact::normal)
    .def_readwrite("collision_point", &Contact::collision_point)
    .def_readwrite("penetration", &Contact::penetration)
    .def_readwrite("bias", &Contact::bias)
    .def_readwrite("impulse_denominator_bitangent", &Contact::impulse_denominator_bitangent)
    .def_readwrite("impulse_denominator_tangent", &Contact::impulse_denominator_tangent)
    .def_readwrite("impulse_denominator_normal", &Contact::impulse_denominator_normal)
    .def_readwrite("Pn", &Contact::Pn)
    .def_readwrite("Pt", &Contact::Pt)
    .def_readwrite("Pbt", &Contact::Pbt)
    .def("apply_impulse", &Contact::apply_impulse)
    .def("pre_step", &Contact::pre_step)
    ;

    python::def("get_orthogonal_faces", &get_orthogonal_faces);
    python::def("get_most_parallel_face", &get_most_parallel_face);
    python::def("get_most_antiparallel_face", &get_most_antiparallel_face);
    python::def("create_minimal_contacts", &create_minimal_contacts);
    python::def("sphere_sphere_contacts", &sphere_sphere_contacts);
    python::def("sphere_box_contacts", &sphere_box_contacts);
    python::def("sphere_cylinder_contacts", &sphere_cylinder_contacts);
    python::def("box_box_contacts", &box_box_contacts);
    python::def("create_contacts", &create_contacts);
    python::def("simulate", &simulate);
    python::def("closest_point_on_tri", &closest_point_on_tri);

}