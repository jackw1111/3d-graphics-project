

# Variables
###############################################]
SRC=engine/core/src/
LIBS = -ldl -lassimp -lfreetype -lpython3.8 -lboost_python38 -lglfw -lopenal -laudio
INCLUDE= -I/home/me/Documents/3d-graphics-project/engine/core/lib/include -I./engine/core/lib/ -I/usr/include/python3.8 -I/usr/lib/x86_64-linux-gnu -I./engine/core/include/ -I./engine/core/lib/ -I./engine/core/lib/freetype-2.10.1/include
CPPLIBS=  -L/home/me/Documents/3d-graphics-project/engine/core/lib/ -L/usr/lib/python3.8/config-3.7m-x86_64-linux-gnu/ -L./usr/lib/ $(LIBS) -std=c++11
FLAGS =-Wall -Wextra -fPIC -Wno-unused-parameter -Wno-unused-function #-Wno-missing-profile# -Wl,--no-undefined  #-Werror
DEBUG= -g -O0  
RELEASE= -O3 #-funroll-loops -fno-signed-zeros -fno-trapping-math -fprofile-use -fopenmp -D_GLIBCXX_PARALLEL
BUILD = $(RELEASE)


# Make C++ Library
###############################################]

INC=engine/core/include/
INCS = $(wildcard $(INC)*.h)

OBJ=engine/core/obj/
_OBJS = $(INCS:.h=.o)
OBJS = $(sort $(subst engine/core/include/, engine/core/obj/, $(_OBJS)) engine/core/obj/glad.o)

NO_MAIN_OBJS = $(filter-out engine/core/obj/main.o, $(OBJS))

cpp: $(NO_MAIN_OBJS)
	@echo "building engine..."
	@g++ -DUSE_CPP $(BUILD) -c $(INCLUDE) $(FLAGS) engine/core/src/main.cpp -o engine/core/obj/main.o -lGL
	@g++ $(BUILD) -shared $(INCLUDE) -o engine/bin/libengine.so  engine/core/obj/main.o $(NO_MAIN_OBJS) $(CPPLIBS)
	@echo "Finished."

$(OBJ)%.o : $(SRC)%.cpp
	@echo building $* ...
	@g++ $(BUILD) -c $(INCLUDE) $(FLAGS) $< -o $@ -lGL

# Make Python Library
###############################################

BINDINGS_INC=engine/bindings/include/
BINDINGS_SRC=engine/bindings/src/
BINDINGS_OBJ=engine/bindings/obj/

BINDINGS_INCS=$(wildcard $(BINDINGS_INC)*.h)

_BINDINGS_OBJS = $(BINDINGS_INCS:.h=.o)
BINDINGS_OBJS = $(sort $(subst engine/bindings/include/, engine/bindings/obj/, $(_BINDINGS_OBJS)))

python: $(BINDINGS_OBJS)
	@echo $(NO_MAIN_OBJS)
	@echo $(BINDINGS_OBJS)

	@echo "building engine..."
	@g++ $(BUILD) -c $(INCLUDE) $(FLAGS) engine/core/src/main.cpp -o engine/core/obj/main.o -lGL
	@g++ $(BUILD) -shared $(INCLUDE) -o engine/bin/engine.so  engine/core/obj/main.o  $(NO_MAIN_OBJS) $(BINDINGS_OBJS) $(CPPLIBS)


$(BINDINGS_OBJ)%.o : $(BINDINGS_SRC)%.cpp
	@echo building binding for $* ...
	@g++ $(BUILD) -c $(INCLUDE) -I $(BINDINGS_INC) $(FLAGS) $< -o $@ -lGL


###############################################


engine/core/obj/glad.o : engine/core/lib/glad/glad.c
	@echo building glad ...
	@gcc -c $(INCLUDE) $(FLAGS) $< -o $@

bin:
	@g++ -DUSE_CPP $(BUILD) -c $(INCLUDE) $(FLAGS) engine/core/src/main.cpp -o engine/core/obj/main.o -lGL
	@g++ $(BUILD) $(INCLUDE) -o engine/bin/engine  engine/core/obj/main.o $(NO_MAIN_OBJS) $(CPPLIBS)

all:
	make cpp
	make python

clean:
	@echo "cleaning engine..."
	@rm engine/bin/engine.so engine/core/obj/*.o example

docs:
	doxygen Doxyfile