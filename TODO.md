# 3D Graphics Project
### TO DO
- Improve Python API call standards (camelCase in C++, lowercase_seperated_by_underscore for Python)
- Uninclude wrappers in Doxygen build, include Python API in Doxygen somehow
- embed console in C++ side
- fix the lastFrame/currentFrame reference problem, they are the same pointer, when they should be two seperate ones,
which is causing problems in the python binding

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
