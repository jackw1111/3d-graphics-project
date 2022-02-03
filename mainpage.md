# 3D-graphics-project

\image{inline} html engine.png "caption"

### Features:

- 3D renderer (C++11, Python3, OpenGL3.3), built on Intel 3000 iGPU (bottom 2% GPU on userbenchmark.com) so compatible and performant across many GPUs (every rect and line is batched, every model is instanced, vertex lighting to save on perf, frustum culling, caching)
- shadow maps, multiple lights, skeletal animations, basic particle system, skybox
- ellipsoid character collision detection with collision resolution
- basic audio
- gamepad support

- python bindings allows short iteration cycle for fast prototyping of ideas, Python and C++ API are similar enough that Python prototypes can be easily rewritten in C++ for release and allows seamless integration with PyOpenGL(shares the same OpenGL context, mostly seamless), PyGTK, numpy, and many Python libraries, seamless extentability from C++ during prototyping phase


- 10 basic game examples to demonstrate how to program against the engine (WIP)
- iPython-like console for easily switching/setting some common flags (WIP)
- program with or without included IDE (WIP)
- documentation included (WIP)
- github.io with realtime-demos with emscripten (WIP)

# Resume clout
- Proficient in C++ and Python, and can rewrite sections of slow Python apps in C++
- Working knowledge in mid-scale software engineering design patterns (10k LOC project)
- Building maintainable and scalable code (Fully documented API, released) (WIP)

# troubleshoot
- there is a strange bug when exporting .obj files for `StaticObject`'s, you will need to flip forward and up axis in the export settings for the object to appear the correct orientation in the engine. Eg. if you want to export -z forward and y up, you will have to apply y forward and -z up in the blender export settings.
- TO DO example on how to export .dae/.fbx for `AnimatedObject`'s.

