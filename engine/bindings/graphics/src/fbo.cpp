#include "../../../bindings/graphics/include/fbo.h"

FBOWrap::FBOWrap() : FBO() {

}

FBOWrap::FBOWrap(int width, int height, bool position_buffer, bool normal_buffer, bool color_buffer, bool depth_buffer) : FBO(width, height, position_buffer, normal_buffer, color_buffer, depth_buffer) {};
FBOWrap::FBOWrap(const char *path, const string &directory) : FBO(path, directory) {};

int FBOWrap::setup(int width, int height, bool position_buffer, bool normal_buffer, bool color_buffer, bool depth_buffer) {
    return FBO::setup(width, height, position_buffer, normal_buffer, color_buffer, depth_buffer);
}

void FBOWrap::Draw(StaticShader shader) {
    FBO::Draw(shader);
}

void FBOWrap::bind(bool value) {
    FBO::bind(value);
}

void BindTexture(int slot, int texture) {
    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}



void wrap_FBO() {
    python::class_<FBOWrap, boost::noncopyable>("FBO", python::init<>())
    .def(python::init<const char *, const string &>())
    .def(python::init<int,int, bool,bool,bool,bool>())
    .def("setup", &FBO::setup)
    .def("Draw", &FBO::Draw)
    .def("bind", &FBO::bind)
    .add_property("color", &FBO::getColorBuffer, &FBO::color_texture)
    .add_property("normal", &FBO::getNormalBuffer, &FBO::normal_texture)
    .add_property("position",  &FBO::getPositionBuffer, &FBO::position_texture)
    .add_property("depth", &FBO::getDepthBuffer, &FBO::depth_texture)
    ;
    def("BindTexture", BindTexture);
}
