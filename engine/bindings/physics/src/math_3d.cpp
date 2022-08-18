#include "../../../bindings/physics/include/math_3d.h"

void wrap_Math3D() {
    python::def("dist_point_to_triangle", distPointToTriangle);
    python::def("line_segment_intersection", &lineSegmentIntersection);
    python::def("point_plane_distance", &pointPlaneDistance2);
    python::def("point_line_intersection", &pointLineIntersection);
    python::def("closest_points_on_line_segments", &closestPointsOnLineSegments);
    python::def("closest_point_on_circle", &closest_point_on_circle);
    python::def("clip_polygon_to_plane", &clipPolygonToPlane);
    python::def("is_minkowski_face2", &isMinkowskiFace2);
    python::def("matrix_multiply", &matrixMultiply);
	python::def("get_aabb", &getAABB);
    python::def("get_aabb_vertices", &getAABBVertices);
    python::def("aabb_overlap", &AABBOverlap);
    python::def("closest_point_on_obb", &closest_point_on_obb);
    python::def("ray_obb_intersection", &ray_obb_intersection);
}