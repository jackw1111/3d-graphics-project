#include "../../../bindings/physics/include/joint.h"


bool operator==(const SphericalJoint f1, const SphericalJoint f2) {
    return f1.body_a == f2.body_a && f1.body_b == f2.body_b;
}

void wrap_Joint() {

	iterable_converter()
    .from_python<vector<SphericalJoint> >()
    ;

    boost::python::class_< vector < SphericalJoint > >("SphericalJointVector")
    .def(boost::python::vector_indexing_suite<vector< SphericalJoint > >());

    python::class_<SphericalJoint>("SphericalJoint", python::init<unsigned int, vec3, unsigned int, vec3 >())
    // .def_readwrite("body_a", &SphericalJoint::get_body_a)
    // .def_readwrite("body_b", &SphericalJoint::get_body_b)
    .def_readwrite("anchor_a", &SphericalJoint::anchor_a)
    .def_readwrite("anchor_b", &SphericalJoint::anchor_a)
    .def_readwrite("anchor_a_ws", &SphericalJoint::anchor_a_ws)
    .def_readwrite("anchor_b_ws", &SphericalJoint::anchor_b_ws)
    .def_readwrite("normal", &SphericalJoint::normal)
    .def_readwrite("collision_point", &SphericalJoint::collision_point)
    .def_readwrite("penetration", &SphericalJoint::penetration)
    .def_readwrite("bias", &SphericalJoint::bias)
    .def_readwrite("impulse_denominator_bitangent", &SphericalJoint::impulse_denominator_bitangent)
    .def_readwrite("impulse_denominator_tangent", &SphericalJoint::impulse_denominator_tangent)
    .def_readwrite("impulse_denominator_normal", &SphericalJoint::impulse_denominator_normal)
    .def_readwrite("Pn", &SphericalJoint::Pn)
    .def_readwrite("Pt", &SphericalJoint::Pt)
    .def_readwrite("Pbt", &SphericalJoint::Pbt)
    .def("apply_impulse", &SphericalJoint::apply_impulse)
    .def("pre_step", &SphericalJoint::pre_step)
    ;

}