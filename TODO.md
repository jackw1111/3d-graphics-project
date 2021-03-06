# 3D Graphics Project
### TO DO
- Seperate core engine components from wrapper functions for the Python API so that the engine can built with or without bindings.
- Delete or improve useless files (collisions2, physics, quad_renderer)
- Improve Python API call standards (camelCase in C++, lowercase_seperated_by_underscore for Python)
- Uninclude wrappers in Doxygen build, include Python API in Doxygen somehow
- MSAA
- ~~audio~~
- normal mapping
- embed console in C++ side
- generate wrappers for all vector<T> automatically
- Vertex and VertexTransform ?
- fixed memory leaks with valgrind
- ~~move button from C++ to python as a utility~~
- ~~remove poly class~~
- remove new shader, new mesh, new model classes
- split all wrap code from bindings.cpp into each class file
- ~~fix makefile~~
- write tests
- ~~make examples MVCE (remove all unneccessary data)~~
- make filepath class to dynamically fill in the absolute directory of an absolute file name
- fix alpha blending for Rect class
- fix prototype for `texture_from_file`
- build all scene effects into c++ side, including shadows, ssao, skybox, msaa, and draw calls draw directly to the gbuffer, can be turned on and off from the python console
- model spawner c++ class ?
- convert application to a registry based approach (maybe change it to scene?) and each c++ class type is subclassed from a class with onMouseMoved, onKeyPress, to make scripting easier
- modularize render code so eventual vulkan backend swap
- ~~frustum culling~~
- problems with rect class model matrix, simplify api
- make all file paths relative paths, not absolute paths
- vec3(value) constructor
- add a util py file for circles, and add possibility for axis defined circles

Possible engine conversion to C:

### Pros
- Automatic (runtime) bindings generation (gintrospection)
- codebase is less fragmented
- no need for Boost::Python
- learn how to manage memory and use valgrind (forcefully)
- use GtkDoc instead of Doxygen
- learn how alot of GNOME/GTK works
- language bindings for other languages for free (Rust, Javascript, Python, C++)

### Cons
- would require full conversion of engine to C.
- C code is harder to write/maintain
- pointers, memory management
- hard to find modern documentation for (eventual conversion to Vulkan will be made that much harder)
