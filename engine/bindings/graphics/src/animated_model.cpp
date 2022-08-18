#include "../../../bindings/graphics/include/animated_model.h"

std::shared_ptr<AnimatedModel> createAnimatedModel(std::string fileName)
{
  return std::shared_ptr<AnimatedModel>(
    new AnimatedModel(fileName),
    std::mem_fn(&AnimatedModel::remove));
}

int AnimatedModelWrap::Draw()
{
  return AnimatedModel::Draw();
}

int AnimatedModelWrap::DrawInstanced(std::vector<mat4> modelTransforms){
  return AnimatedModelWrap::DrawInstanced(modelTransforms);
}

void AnimatedModelWrap::setFrames(float start, float end) {
  return AnimatedModel::setFrames(start, end);
}


void wrap_AnimatedModel() {
    // python::class_<AnimatedModel, boost::noncopyable>("AnimatedModel", python::init<std::string>())
    python::class_<AnimatedModel, boost::shared_ptr<AnimatedModel> >("AnimatedModel", python::no_init)
    .def("__init__", python::make_constructor(&createAnimatedModel))
    .def("Draw", &AnimatedModel::Draw)
    .def("DrawInstanced", &AnimatedModel::DrawInstanced)
    .def("setFrames", &AnimatedModel::setFrames)

    .def_readwrite("animationIndex", &AnimatedModel::animationIndex)
    .def_readwrite("meshes", &AnimatedModel::meshes)
    .def_readwrite("shader", &AnimatedModel::shader)
    .def_readwrite("model_matrix", &AnimatedModel::model)
    .def_readwrite("start_frame", &AnimatedModel::start_frame)
    .def_readwrite("end_frame", &AnimatedModel::end_frame)
    .def("getAABB", &StaticModel::getAABB)

    ;
    python::register_ptr_to_python< std::shared_ptr<AnimatedModel> >();

    python::class_<AnimatedObject>("AnimatedObject", python::init<std::string>())
    .add_property("model", &AnimatedObject::getModel)
    .add_property("model_matrix",&AnimatedObject::getModelMatrix, &AnimatedObject::setModelMatrix)
    .add_property("shininess", &AnimatedObject::getShininess, &AnimatedObject::setShininess)
    .add_property("color",&AnimatedObject::getColor, &AnimatedObject::setColor)
    .def_readwrite("start_frame", &AnimatedObject::start_frame)
    .def_readwrite("end_frame", &AnimatedObject::end_frame)
    .def_readwrite("animated_vertices", &AnimatedObject::getAnimatedVertices)
    .def_readwrite("render_to_ui", &AnimatedObject::renderToUI)
    .def_readwrite("bone_transforms", &AnimatedObject::getBoneTransforms)
    .add_property("use_custom_shader", &AnimatedObject::getUseCustomShader, &AnimatedObject::setUseCustomShader)
    .def("set_frames", &AnimatedObject::setFrames)
    .add_property("set_to_draw", &AnimatedObject::getToDraw, &AnimatedObject::setToDraw)
    .def_readwrite("bounding_box", &AnimatedObject::boundingBox)
    .add_property("draw_bounding_box",&AnimatedObject::getDrawBoundingBox, &AnimatedObject::setDrawBoundingBox)
    .def("remove", &AnimatedObject::remove);
}
