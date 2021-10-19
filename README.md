![alt text](https://github.com/jackw1111/3d-graphics-project/master/engine.png?raw=true)
Features:

- Deferred 3D renderer (C++11, Python3, OpenGL3.3), built on Intel 3000 iGPU (bottom 2% GPU on userbenchmark.com) so compatible and performant across many GPUs

- shadow maps, multiple lights, SSAO, skeletal animations, basic particle system, skybox

- ellipsoid character collision detection with collision resolution

- basic audio

- gamepad support

- python bindings allows short iteration cycle for fast prototyping of ideas, Python and C++ API are similar enough that Python prototypes can be easily rewritten in C++ for release and allows seamless integration with PyOpenGL(shares the same OpenGL context, mostly seamless), PyGTK, numpy, and many Python libraries, seamless extentability from C++ during prototyping phase

- 10 basic game examples to demonstrate how to program against the engine (WIP)

- iPython-like console for easily switching/setting some common flags (WIP)

- program with or without included IDE (WIP)

- documentation included (WIP)

- github.io with realtime-demos with emscripten (WIP)

What I learned:

- Proficient in C++ and Python, and can rewrite sections of slow Python apps in C++
- Working knowledge in mid-scale software engineering design patterns (10k LOC project)
- Building maintainable and scalable code (Fully documented API, released) (WIP)
