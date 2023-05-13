#RELEASE= -O3 -funroll-loops -fno-signed-zeros -fno-trapping-math -fprofile-use -fopenmp -D_GLIBCXX_PARALLEL
# video_player:
# 	g++ video_player.cpp -I /home/me/Documents/3d-graphics-project/engine/core/include/ -I /usr/include/python3.8 -I /home/me/Documents/3d-graphics-project/engine/core/lib/freetype-2.10.1/include -I /home/me/Documents/3d-graphics-project/engine/core/lib/ -L/home/me/Documents/3d-graphics-project/engine/bin  /home/me/Documents/3d-graphics-project/engine/bin/engine.so  -lpython3.8 -lboost_python38 -fPIC -shared -o video_player.so

all:
	g++ physics2d.cpp -I../../engine/core/include -I../../engine/core/lib -I/usr/include/python3.8 -L../../engine/bin -I../../engine/core/lib/freetype-2.10.1/include ../../engine/bin/engine.so  -lpython3.8 -lboost_python38 -fPIC -shared -o physics2d.so
