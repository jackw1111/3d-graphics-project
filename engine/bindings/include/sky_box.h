#include "_engine.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;

struct SkyboxWrap : Skybox, boost::python::wrapper<Skybox>
{
    unsigned int loadCubemap(std::vector<std::string> faces);

    void Draw(StaticShader skyboxShader);

    void load(std::vector<std::string> faces);

    void bindTexture();

};

void wrap_Skybox();