#include "../../bindings/include/static_model.h"


boost::shared_ptr<StaticObject> create_staticobject(std::string fileName)
{
  return boost::shared_ptr<StaticObject>(
    new StaticObject(fileName),
    boost::mem_fn(&StaticObject::remove));
}

void wrap_StaticModel() {
    python::class_<StaticModel>("StaticModel", python::init<>())
    .def(python::init<std::string>())
    .def("Draw", &StaticModel::Draw)
    .def("DrawInstanced", &StaticModel::DrawInstanced)
    .def_readwrite("meshes", &StaticModel::meshes)
    .def_readwrite("textures_loaded", &StaticModel::textures_loaded)
    .def_readwrite("shader", &StaticModel::shader)
    .def("getAABB", &StaticModel::getAABB)
    ;
    python::def("texture_from_data", &TextureFromData);


    //python::class_<StaticObject, boost::shared_ptr<StaticObject> >("StaticObject", python::no_init)
    //.def("__init__", python::make_constructor(&create_staticobject))

    python::class_<StaticObject>("StaticObject", python::init<std::string>())
    .def_readwrite("render_to_ui", &StaticObject::renderToUI)
    .add_property("color",&StaticObject::getColor, &StaticObject::setColor)
    .add_property("model_matrix",&StaticObject::getModelMatrix, &StaticObject::setModelMatrix)
    .add_property("model", &StaticObject::getModel)
    .def_readwrite("set_to_draw", &StaticObject::setToDraw)
    .def("remove", &StaticObject::remove);

    //.def_readwrite("color", &StaticModel::color)
    //.def("translate", &StaticModel::translate)
    //.def("onKeyPressed", &StaticModel::onKeyPressed)
    // update
    // etc..
}

