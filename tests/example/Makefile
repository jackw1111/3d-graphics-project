#RELEASE= -O3 -funroll-loops -fno-signed-zeros -fno-trapping-math -fprofile-use -fopenmp -D_GLIBCXX_PARALLEL
# video_player:
# 	g++ video_player.cpp -I /home/me/Documents/3d-graphics-project/engine/core/include/ -I /usr/include/python3.8 -I /home/me/Documents/3d-graphics-project/engine/core/lib/freetype-2.10.1/include -I /home/me/Documents/3d-graphics-project/engine/core/lib/ -L/home/me/Documents/3d-graphics-project/engine/bin  /home/me/Documents/3d-graphics-project/engine/bin/engine.so  -lpython3.8 -lboost_python38 -fPIC -shared -o video_player.so

generate_sphere:
	g++ generate_sphere.cpp -I../../engine/core/include -I../../engine/core/lib -I/usr/include/python3.8 -L../../engine/bin -I../../engine/core/lib/freetype-2.10.1/include ../../engine/bin/engine.so  -lpython3.8 -lboost_python38 -fPIC -shared -o generate_sphere.so

all:
	@echo "export LD_LIBRARY_PATH=/home/me/Documents/3d-graphics-project/engine/bin:$LD_LIBRARY_PATH"
	g++  main.cpp generate_sphere.so -I./engine/core/lib/ -I/usr/lib/x86_64-linux-gnu -I../../engine/core/include/ -I../../engine/core/lib -I../../engine/core/lib/freetype-2.10.1/include -L./usr/lib/ -ldl -lassimp -lfreetype -lglfw -lopenal -laudio -std=c++11 -L../../engine/bin -lengine
