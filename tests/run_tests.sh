# A set of demos that showcase some of the possibilites and functionality of the framework
# ranging from games, to 3d scientific applications, to GUI applications with 3D models viewers and even basic 2D games



# basic example
cd example/
python3 main.py
cd ..

# csgo (FPS, mouse picking, removing view matrix from 3D models, joysticks, collisions)
cd csgo/
python3 main.py
cd ..

# minecraft (collisions, modules)
cd minecraft/
python3 main2.py
cd ..

# angry-bots (animation loops, spawning enemies)
cd angry-bots/
python3 main2.py
cd ..

# Rubik's cube (3D math, integration with Python cube solver)
cd rubiks-cube/
python3 main2.py
cd ..

# solar system (3D line drawing, 3D math)
cd solar-system/
python3 main.py
cd ..

# curve-ball (basic collision detection/response physics)
cd curve-ball/
python3 main.py
cd ..

# grapher (3D math function mesher, GUI, integration with pygtk)
cd grapher/
python3 main.py
cd ..

# chess (mouse picking, Python sunfish (chess engine) integration)
cd chess/
python3 main2.py
cd ..

# tetris (basic 2D game functionality)
cd tetris/
python3 main.py
cd ..

# shadertoy (GUI, integration with pygtk)
cd shadertoy/
python3 ide2.py ./
cd ..

# C++ extension example
cd example-cpp/
make
export LD_LIBRARY_PATH=/home/me/Documents/3d-graphics-project/engine/bin:$LD_LIBRARY_PATH
./a.out
cd ..


# heli example
cd heli/
python3 main.py
cd ..

# plane example
cd plane/
python3 main.py
cd ..

# raytracer example
cd ray-sphere-intersection/
python3 main.py
cd ..

# particles example
cd particles/
python3 main.py
cd ..

# bow-and-arrow example
cd bow-and-arrow/
python3 main.py
cd ..


