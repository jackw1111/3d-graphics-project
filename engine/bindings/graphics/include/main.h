#ifndef MAIN_BINDINGS_H
#define MAIN_BINDINGS_H
#define PY

#include "engine.h"

#include "../../../bindings/graphics/include/static_shader.h"
#include "../../../bindings/graphics/include/static_mesh.h"
#include "../../../bindings/graphics/include/static_model.h"

#include "../../../bindings/graphics/include/animated_mesh.h"
#include "../../../bindings/graphics/include/animated_model.h"
#include "../../../bindings/graphics/include/animated_shader.h"

#include "../../../bindings/graphics/include/camera.h"
#include "../../../bindings/graphics/include/culling.h"
#include "../../../bindings/graphics/include/fbo.h"
#include "../../../bindings/graphics/include/app.h"
#include "../../../bindings/graphics/include/light.h"
#include "../../../bindings/graphics/include/particle.h"
#include "../../../bindings/graphics/include/rect2d.h"
#include "../../../bindings/graphics/include/label2d.h"
#include "../../../bindings/graphics/include/label3d.h"
#include "../../../bindings/graphics/include/line3d.h"
#include "../../../bindings/graphics/include/line2d.h"
#include "../../../bindings/graphics/include/sky_box.h"
#include "../../../bindings/graphics/include/audio.h"

#include "../../../bindings/graphics/include/math_utils.h"

#include "../../../bindings/physics/include/collisionSAT.h"
#include "../../../bindings/physics/include/contact.h"
#include "../../../bindings/physics/include/body.h"
#include "../../../bindings/physics/include/joint.h"
#include "../../../bindings/physics/include/math_3d.h"

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