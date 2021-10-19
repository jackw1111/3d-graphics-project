#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

#include <numeric>

using namespace boost;
using namespace boost::python;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

#include "static_model.h"

std::vector<std::vector<glm::vec3>>* get_translated_vertices(std::vector<std::vector<glm::vec3>> *p) {

}


bool check_collision_SAT(std::vector<std::vector<glm::vec3>> *p1, std::vector<std::vector<glm::vec3>> *p2) {
	std::vector<std::vector<glm::vec3>> poly1 = *p1;
	std::vector<std::vector<glm::vec3>> poly2 = *p2;

	for (int shape = 0; shape < 2; shape++) {
		if (shape == 1) {
			poly1 = *p2;
			poly2 = *p1;
		}
		// for every face
		for (int f = 0; f < poly1.size(); f++) {
			// for every line segment
			for (int a = 0; a < poly1[f].size(); a++) {
				int b = (a + 1) % poly1[f].size();

				// project each shapes points onto normal
				// float axisProjx = -(poly1[f][b][1] - poly1[f][a][1]);
				// float axisProjy = poly1[f][b][0] - poly1[f][a][0];

				// float minR1_xy = std::numeric_limits<float>::max();
				// float maxR1_xy = std::numeric_limits<float>::min();

				// for (int p = 0; p < poly1.size(); p++) {
				// 	glm::vec2 translated_vertex = glm::vec2(poly1[f][p][0], poly1[f][p][1]);
				// 	float q = glm::dot(translated_vertex, glm::vec2(axisProjx, axisProjy));
				// 	minR1_xy = min(minR1_xy, q);
				// 	maxR1_xy = max(maxR1_xy, q);
				// }

				// float minR2_xy = std::numeric_limits<float>::max();
				// float maxR2_xy = std::numeric_limits<float>::min();

				// for (int p = 0; p < poly2.size(); p++) {
				// 	glm::vec2 translated_vertex = glm::vec2(poly2[f][p][0], poly2[f][p][1]);
				// 	float q = glm::dot(translated_vertex, glm::vec2(axisProjx, axisProjy));
				// 	minR2_xy = min(minR2_xy, q);
				// 	maxR2_xy = max(maxR2_xy, q);
				// }

				// repeat for xz
				float axisProjx = -(poly1[f][b][2] - poly1[f][a][2]);
				float axisProjz = poly1[f][b][0] - poly1[f][a][0];

				float minR1_xz = std::numeric_limits<float>::max();
				float maxR1_xz = std::numeric_limits<float>::min();

				for (int p = 0; p < poly1.size(); p++) {
					glm::vec2 translated_vertex = glm::vec2(poly1[f][p][0], poly1[f][p][2]);
					float q = glm::dot(translated_vertex, glm::vec2(axisProjx, axisProjz));
					minR1_xz = min(minR1_xz, q);
					maxR1_xz = max(maxR1_xz, q);
				}

				float minR2_xz = std::numeric_limits<float>::max();
				float maxR2_xz = std::numeric_limits<float>::min();
				for (int p = 0; p < poly2.size(); p++) {
					glm::vec2 translated_vertex = glm::vec2(poly2[f][p][0], poly2[f][p][2]);
					float q = glm::dot(translated_vertex, glm::vec2(axisProjx, axisProjz));
					minR2_xz = min(minR2_xz, q);
					maxR2_xz = max(maxR2_xz, q);
				}

				// // repeat for yz
			 //    axisProjy = -(poly1[f][b][2] - poly1[f][a][2]);
			 //    axisProjz = poly1[f][b][1] - poly1[f][a][1];

				// float minR1_yz = std::numeric_limits<float>::max();
				// float maxR1_yz = std::numeric_limits<float>::min();

				// for (int p = 0; p < poly1.size(); p++) {
				// 	glm::vec2 translated_vertex = glm::vec2(poly1[f][p][1], poly1[f][p][2]);
			 //        float q = glm::dot(translated_vertex, glm::vec2(axisProjy, axisProjz));
			 //        minR1_yz = min(minR1_yz, q);
			 //        maxR1_yz = max(maxR1_yz, q);
				// }

				// float minR2_yz = std::numeric_limits<float>::max();
				// float maxR2_yz = std::numeric_limits<float>::min();

				// for (int p = 0; p < poly2.size(); p++) {
				// 	glm::vec2 translated_vertex = glm::vec2(poly2[f][p][1], poly2[f][p][2]);
			 //        float q = glm::dot(translated_vertex, glm::vec2(axisProjy, axisProjz));
			 //        minR2_yz = min(minR2_yz, q);
			 //        maxR2_yz = max(maxR2_yz, q);
				// }	
				// if any of the projections dont overlap, no collision
				if (!(maxR2_xz >= minR1_xz && maxR1_xz >= minR2_xz
				)) {
					// vec3 collision_normal = vec3(axisProjx, axisProjy, axisProjz);
					// vec3 collision_normal = vec3(axisProjx, axisProjy, axisProjz);
					return false;
				}
			}
		}

	}

	return true;
}


BOOST_PYTHON_MODULE(collision)
{
	python::def("check_collision_SAT", check_collision_SAT);
}

// def check_collision(p1, p2):
//     poly1 = p1
//     poly2 = p2
//     for shape in range(2):
//         if (shape == 1):
//             poly1 = p2
//             poly2 = p1
 
//         for a in range(len(poly1.translated_vertices)):
//             b = (a + 1) % len(poly1.translated_vertices)
 
//             axisProjx = -(poly1.translated_vertices[b][1] - poly1.translated_vertices[a][1])
//             axisProjy = poly1.translated_vertices[b][0] - poly1.translated_vertices[a][0]
 
//             minR1_xy = inf
//             maxR1_xy = -inf
//             for p in range(len(poly1.translated_vertices)):
//                 translated_vertex = vec2(poly1.translated_vertices[p].x, poly1.translated_vertices[p].y)
//                 q = dot(translated_vertex, vec2(axisProjx, axisProjy))
//                 minR1_xy = min(minR1_xy, q)
//                 maxR1_xy = max(maxR1_xy, q)
 
//             minR2_xy = inf
//             maxR2_xy = -inf
//             for p in range(len(poly2.translated_vertices)):
//                 translated_vertex = vec2(poly2.translated_vertices[p].x, poly2.translated_vertices[p].y)
//                 q = dot(translated_vertex, vec2(axisProjx, axisProjy))
//                 minR2_xy = min(minR2_xy, q)
//                 maxR2_xy = max(maxR2_xy, q)
 
 
//             axisProjx = -(poly1.translated_vertices[b][2] - poly1.translated_vertices[a][2])
//             axisProjz = poly1.translated_vertices[b][0] - poly1.translated_vertices[a][0]
//             minR1_xz = inf
//             maxR1_xz = -inf
//             for p in range(len(poly1.translated_vertices)):
//                 translated_vertex = vec2(poly1.translated_vertices[p].x, poly1.translated_vertices[p].z)
//                 q = dot(translated_vertex, vec2(axisProjx, axisProjz))
//                 minR1_xz = min(minR1_xz, q)
//                 maxR1_xz = max(maxR1_xz, q)
 
//             minR2_xz = inf
//             maxR2_xz = -inf
//             for p in range(len(poly2.translated_vertices)):
//                 translated_vertex = vec2(poly2.translated_vertices[p].x, poly2.translated_vertices[p].z)
//                 q = dot(translated_vertex, vec2(axisProjx, axisProjz))
//                 minR2_xz = min(minR2_xz, q)
//                 maxR2_xz = max(maxR2_xz, q)
 
 
//             axisProjy = -(poly1.translated_vertices[b][2] - poly1.translated_vertices[a][2])
//             axisProjz = poly1.translated_vertices[b][1] - poly1.translated_vertices[a][1]
 
//             minR1_yz = inf
//             maxR1_yz = -inf
//             for p in range(len(poly1.translated_vertices)):
//                 translated_vertex = vec2(poly1.translated_vertices[p].y, poly1.translated_vertices[p].z)
//                 q = dot(translated_vertex, vec2(axisProjy, axisProjz))
//                 minR1_yz = min(minR1_yz, q)
//                 maxR1_yz = max(maxR1_yz, q)
 
//             minR2_yz = inf
//             maxR2_yz = -inf
//             for p in range(len(poly2.translated_vertices)):
//                 translated_vertex = vec2(poly2.translated_vertices[p].y, poly2.translated_vertices[p].z)
//                 q = dot(translated_vertex, vec2(axisProjy, axisProjz))
//                 minR2_yz = min(minR2_yz, q)
//                 maxR2_yz = max(maxR2_yz, q)
 
//             # seperation between shadows of objects found => no collision possible, exit early
//             if not (maxR2_xy >= minR1_xy and maxR1_xy >= minR2_xy \
//                 and maxR2_xz >= minR1_xz and maxR1_xz >= minR2_xz \
//                 and maxR2_yz >= minR1_yz and maxR1_yz >= minR2_yz):
//                 p1.collision_normal = vec3(axisProjx, axisProjy, axisProjz);
//                 p2.collision_normal = vec3(axisProjx, axisProjy, axisProjz);
//                 return False
//     return True

