#RELEASE= -O3 -funroll-loops -fno-signed-zeros -fno-trapping-math -fprofile-use -fopenmp -D_GLIBCXX_PARALLEL
INCLUDE = -I /home/me/Documents/3d-graphics-project/engine/core/include/ -I../../engine/core/lib/include/ -I../../engine/core/lib/ -I/usr/lib/x86_64-linux-gnu -I../../engine/include/ -I../../engine/lib -I../../engine/core/lib/freetype-2.10.1/include
RELEASE = -O3 -fopenmp -D_GLIBCXX_PARALLEL
generate_sphere:
	g++ generate_sphere.cpp -I../../engine/include -I../../engine/lib -I/usr/include/python3.8 -L../../engine/bin -I../../engine/lib/freetype-2.10.1/include ../../engine/bin/engine.so  -lpython3.8 -lboost_python38 -fPIC -shared -o generate_sphere.so

cpp:
	@echo "export LD_LIBRARY_PATH=/home/me/Documents/3d-graphics-project/engine/bin:$(LD_LIBRARY_PATH)"
	g++ main.cpp $(INCLUDE)  -L./usr/lib/ -ldl /home/me/Documents/3d-graphics-project/engine/core/lib/libassimp.so -lfreetype -lglfw -lopenal -laudio -std=c++11 -L../../engine/bin -lengine

all:
	make cpp