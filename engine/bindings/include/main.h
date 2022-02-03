#ifndef MAIN_BINDINGS_H
#define MAIN_BINDINGS_H
#define PY

#include "engine.h"

#include "../../bindings/include/static_shader.h"
#include "../../bindings/include/static_mesh.h"
#include "../../bindings/include/static_model.h"

#include "../../bindings/include/animated_mesh.h"
#include "../../bindings/include/animated_model.h"
#include "../../bindings/include/animated_shader.h"

#include "../../bindings/include/camera.h"
#include "../../bindings/include/fbo.h"
#include "../../bindings/include/entity.h"
#include "../../bindings/include/app.h"
#include "../../bindings/include/light.h"
#include "../../bindings/include/particle.h"
#include "../../bindings/include/rect2d.h"
#include "../../bindings/include/label.h"
#include "../../bindings/include/line3d.h"
#include "../../bindings/include/sky_box.h"
#include "../../bindings/include/audio.h"

#include "../../bindings/include/math_utils.h"
#include "../../bindings/include/physics.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/detail/operator_id.hpp>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace boost::python;


#endif