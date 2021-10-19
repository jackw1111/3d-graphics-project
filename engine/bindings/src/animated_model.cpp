#include "../../bindings/include/animated_model.h"

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
  python::class_<AnimatedModel>("AnimatedModel", python::init<std::string>())
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

    python::class_<AnimatedObject>("AnimatedObject", python::init<std::string>())
    .add_property("model", &AnimatedObject::getModel)
    .add_property("model_matrix",&AnimatedObject::getModelMatrix, &AnimatedObject::setModelMatrix)
    .add_property("color",&AnimatedObject::getColor, &AnimatedObject::setColor)
    .def_readwrite("start_frame", &AnimatedObject::start_frame)
    .def_readwrite("end_frame", &AnimatedObject::end_frame)
    .def_readwrite("render_to_ui", &AnimatedObject::renderToUI)
    .def("set_frames", &AnimatedObject::setFrames)
    .def_readwrite("set_to_draw", &AnimatedObject::setToDraw)
    .def("remove", &AnimatedObject::remove);
}
