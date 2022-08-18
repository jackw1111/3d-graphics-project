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

#include "engine.h"


 StaticObject* uv_sphere(int n_slices, int n_stacks)
{
	StaticObject *model = new StaticObject("./data/grass_block.obj");
	// add top vertex
	auto v0 = Vertex(0, 1, 0);
	auto v1 = Vertex(0, 1, 1);
	auto v2 = Vertex(1, 1, 0);
	std::cout << model->meshes.size() << std::endl;
	model->meshes[0].vertices[0] = v0;
	model->meshes[0].vertices[1] = v1;
	model->meshes[0].vertices[2] = v2;

	model->meshes[0].indices[0] = 0;
	model->meshes[0].indices[1] = 1;
	model->meshes[0].indices[2] = 2;

	// model->meshes.push_back(mesh);
	return model;
  // // generate vertices per stack / slice
  // for (int i = 0; i < n_stacks - 1; i++)
  // {
  //   auto phi = M_PI * double(i + 1) / double(n_stacks);
  //   for (int j = 0; j < n_slices; j++)
  //   {
  //     auto theta = 2.0 * M_PI * double(j) / double(n_slices);
  //     auto x = std::sin(phi) * std::cos(theta);
  //     auto y = std::cos(phi);
  //     auto z = std::sin(phi) * std::sin(theta);
  //     mesh.vertices.push_back(Vertex(x, y, z));
  //   }
  // }

  // // add bottom vertex
  // auto v1 = Vertex(0, -1, 0);
  // mesh.vertices.push_back(v1);
  // // add top / bottom triangles
  // for (int i = 0; i < n_slices; ++i)
  // {
  //   auto i0 = i + 1;
  //   auto i1 = (i + 1) % n_slices + 1;
  //   mesh.add_triangle(v0, Vertex(i1), Vertex(i0));
  //   i0 = i + n_slices * (n_stacks - 2) + 1;
  //   i1 = (i + 1) % n_slices + n_slices * (n_stacks - 2) + 1;
  //   mesh.add_triangle(v1, Vertex(i0), Vertex(i1));
  // }

  // // add quads per stack / slice
  // for (int j = 0; j < n_stacks - 2; j++)
  // {
  //   auto j0 = j * n_slices + 1;
  //   auto j1 = (j + 1) * n_slices + 1;
  //   for (int i = 0; i < n_slices; i++)
  //   {
  //     auto i0 = j0 + i;
  //     auto i1 = j0 + (i + 1) % n_slices;
  //     auto i2 = j1 + (i + 1) % n_slices;
  //     auto i3 = j1 + i;
  //     mesh.add_quad(Vertex(i0), Vertex(i1),
  //                   Vertex(i2), Vertex(i3));
  //   }
  // }
  //model.meshes.push_back(mesh);

}



BOOST_PYTHON_MODULE(generate_sphere)
{
	python::def("uv_sphere", uv_sphere, return_value_policy<manage_new_object>());
}