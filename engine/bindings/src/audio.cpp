
#include "../../bindings/include/audio.h"

AudioWindowWrap::AudioWindowWrap() : AudioWindow() {

}


void wrap_Audio() {


  python::class_<AudioWindowWrap, boost::noncopyable>("AudioWindow", python::init<>())
  .def("play", &AudioWindow::playAudio)
  .def("stop", &AudioWindow::stopAudio)
  .def("set_volume", &AudioWindow::setVolume)
  ;

	boost::python::object audioModule(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("engine.core.audio"))));
	boost::python::scope().attr("core.audio") = audioModule;
	boost::python::scope io_scope4 = audioModule;
}
