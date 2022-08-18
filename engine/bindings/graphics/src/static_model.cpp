#include "../../../bindings/graphics/include/static_model.h"



std::shared_ptr<StaticModel> createStaticModel(std::string fileName)
{
  return std::shared_ptr<StaticModel>(
    new StaticModel(fileName),
    std::mem_fn(&StaticModel::remove));
}

// void wrap_StaticModel() {
//     python::class_<StaticModel>("StaticModel", python::init<>())
//     .def(python::init<std::string>())
void wrap_StaticModel() {
    python::class_<StaticModel, boost::shared_ptr<StaticModel> >("StaticModel", python::no_init)
    .def("__init__", python::make_constructor(&createStaticModel))
    .def("Draw", &StaticModel::Draw)
    .def("DrawInstanced", &StaticModel::DrawInstanced)
    .def_readwrite("meshes", &StaticModel::meshes)
    .def_readwrite("textures_loaded", &StaticModel::textures_loaded)
    .def_readwrite("shader", &StaticModel::shader)
    .def("getAABB", &StaticModel::getAABB)
    ;

    python::register_ptr_to_python< std::shared_ptr<StaticModel> >();


    python::def("texture_from_data", &TextureFromData);


    //python::class_<StaticObject, boost::shared_ptr<StaticObject> >("StaticObject", python::no_init)
    //.def("__init__", python::make_constructor(&create_staticobject))

    python::class_<StaticObject>("StaticObject", python::init<std::string>())
    .def_readwrite("render_to_ui", &StaticObject::renderToUI)
    .add_property("color",&StaticObject::getColor, &StaticObject::setColor)
    .add_property("model_matrix",&StaticObject::getModelMatrix, &StaticObject::setModelMatrix)
    .add_property("model", &StaticObject::getModel)
    .add_property("shininess", &StaticObject::getShininess, &StaticObject::setShininess)
    .add_property("set_to_draw", &StaticObject::getToDraw, &StaticObject::setToDraw)
    .add_property("aabb_min", &StaticObject::getAABBMin, &StaticObject::setAABBMin)
    .add_property("aabb_max", &StaticObject::getAABBMax, &StaticObject::setAABBMax)
    .add_property("draw_bounding_box",&StaticObject::getDrawBoundingBox, &StaticObject::setDrawBoundingBox)
    .def("remove", &StaticObject::remove);

    //.def_readwrite("color", &StaticModel::color)
    //.def("translate", &StaticModel::translate)
    //.def("onKeyPressed", &StaticModel::onKeyPressed)
    // update
    // etc..
}

