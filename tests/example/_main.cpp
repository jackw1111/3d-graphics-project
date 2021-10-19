#include "_engine.h"

class App : Application {

};

Application *getApplication() {
	return new Application("example", 800, 600, false);
}

