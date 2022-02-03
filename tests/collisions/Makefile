INCLUDE = -I /home/me/Documents/3d-graphics-project/engine/core/include/ -I../../engine/core/lib/include/ -I../../engine/core/lib/ -I/usr/lib/x86_64-linux-gnu -I../../engine/include/ -I../../engine/lib -I../../engine/core/lib/freetype-2.10.1/include

collision2:
	g++ collision2.cpp $(INCLUDE) -I../../engine/include -I../../engine/lib -I/usr/include/python3.8 -L../../engine/bin -I../../engine/lib/freetype-2.10.1/include ../../engine/bin/engine.so  -lpython3.8 -lboost_python38 -fPIC -shared -o collision2.so
