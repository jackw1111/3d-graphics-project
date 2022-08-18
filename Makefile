

# Variables
###############################################]
SRC=engine/core/graphics/src/
SRC2=engine/core/physics/src/
LIBS = -ldl -lassimp -lfreetype -lpython3.8 -lboost_python38 -lglfw -lopenal -laudio
INCLUDE= -I/home/me/Documents/3d-graphics-project/engine/core/graphics/lib/include -I./engine/core/graphics/lib/ -I/usr/include/python3.8 -I/usr/lib/x86_64-linux-gnu -I./engine/core/graphics/include/ -I./engine/core/physics/include/ -I./engine/core/graphics/lib/ -I./engine/core/graphics/lib/freetype-2.10.1/include
CPPLIBS=  -L/home/me/Documents/3d-graphics-project/engine/core/graphics/lib/ -L/usr/lib/python3.8/config-3.7m-x86_64-linux-gnu/ -L./usr/lib/ $(LIBS) -std=c++11
FLAGS =-Wall -fPIC -msse2 #-Wno-missing-profile# -Wl,--no-undefined  #-Werror
DEBUG= -g -O0  
RELEASE= -O3 #-funroll-loops -fno-signed-zeros -fno-trapping-math -fprofile-use -fopenmp -D_GLIBCXX_PARALLEL
BUILD = $(RELEASE)


# Make C++ Library
###############################################]

INC=engine/core/graphics/include/
INCS = $(wildcard $(INC)*.h)

OBJ=engine/core/graphics/obj/
_OBJS = $(INCS:.h=.o)
OBJS = $(sort $(subst engine/core/graphics/include/, engine/core/graphics/obj/, $(_OBJS)) engine/core/graphics/obj/glad.o)

INC2=engine/core/physics/include/
INCS2 = $(wildcard $(INC2)*.h)
OBJ2=engine/core/physics/obj/
_OBJS2 = $(INCS2:.h=.o)
OBJS2 = $(sort $(subst engine/core/physics/include/, engine/core/physics/obj/, $(_OBJS2)))

NO_MAIN_OBJS = $(filter-out engine/core/graphics/obj/main.o, $(OBJS) $(OBJS2))


cpp: $(NO_MAIN_OBJS)
	@echo "building engine..."
	@g++ -DUSE_CPP $(BUILD) -c $(INCLUDE) $(FLAGS) engine/core/graphics/src/main.cpp -o engine/core/graphics/obj/main.o -lGL
	@g++ $(BUILD) -shared $(INCLUDE) -o engine/bin/libengine.so  engine/core/graphics/obj/main.o $(NO_MAIN_OBJS) $(CPPLIBS) $(FLAGS)
	@echo "Finished."

$(OBJ)%.o : $(SRC)%.cpp
	@echo building $* ...
	@g++ $(BUILD) -c $(INCLUDE) $(FLAGS) $< -o $@ -lGL

$(OBJ2)%.o : $(SRC2)%.cpp
	@echo building $* ...
	@g++ $(BUILD) -c $(INCLUDE) $(FLAGS) $< -o $@ -lGL

# Make Python Library
###############################################

BINDINGS_INC=engine/bindings/graphics/include/
BINDINGS_SRC=engine/bindings/graphics/src/
BINDINGS_OBJ=engine/bindings/graphics/obj/

BINDINGS_INCS=$(wildcard $(BINDINGS_INC)*.h)

_BINDINGS_OBJS = $(BINDINGS_INCS:.h=.o)
BINDINGS_OBJS = $(sort $(subst engine/bindings/graphics/include/, engine/bindings/graphics/obj/, $(_BINDINGS_OBJS)))


BINDINGS_INC2=engine/bindings/physics/include/
BINDINGS_SRC2=engine/bindings/physics/src/
BINDINGS_OBJ2=engine/bindings/physics/obj/

BINDINGS_INCS2=$(wildcard $(BINDINGS_INC2)*.h)

_BINDINGS_OBJS2 = $(BINDINGS_INCS2:.h=.o)
BINDINGS_OBJS2 = $(sort $(subst engine/bindings/physics/include/, engine/bindings/physics/obj/, $(_BINDINGS_OBJS2)))


python: $(BINDINGS_OBJS) $(BINDINGS_OBJS2)
	@echo $(NO_MAIN_OBJS)
	@echo $(BINDINGS_OBJS)

	@echo "building engine..."
	@g++ $(BUILD) -c $(INCLUDE) $(FLAGS) engine/core/graphics/src/main.cpp -o engine/core/graphics/obj/main.o -lGL
	@g++ $(BUILD) -shared $(INCLUDE) -o engine/bin/engine.so engine/core/graphics/obj/main.o  $(NO_MAIN_OBJS) $(BINDINGS_OBJS) $(BINDINGS_OBJS2) $(CPPLIBS)

exe: $(BINDINGS_OBJS)
	@echo $(NO_MAIN_OBJS)
	@echo $(BINDINGS_OBJS)

	@echo "building engine..."
	@g++ $(BUILD) -c $(INCLUDE) $(FLAGS) engine/core/graphics/src/main.cpp -o engine/core/graphics/obj/main.o -lGL
	@g++ $(BUILD) $(INCLUDE) -o engine/bin/engine  engine/core/graphics/obj/main.o  $(NO_MAIN_OBJS) $(CPPLIBS)


$(BINDINGS_OBJ)%.o : $(BINDINGS_SRC)%.cpp
	@echo building binding for $* ...
	@g++ $(BUILD) -c $(INCLUDE) -I $(BINDINGS_INC) $(FLAGS) $< -o $@ -lGL


$(BINDINGS_OBJ2)%.o : $(BINDINGS_SRC2)%.cpp
	@echo building binding for $* ...
	@g++ $(BUILD) -c $(INCLUDE) -I $(BINDINGS_INC2) $(FLAGS) $< -o $@ -lGL

###############################################


engine/core/graphics/obj/glad.o : engine/core/graphics/lib/glad/glad.c
	@echo building glad ...
	@gcc -c $(INCLUDE) $(FLAGS) $< -o $@

bin:
	@g++ -DUSE_CPP $(BUILD) -c $(INCLUDE) $(FLAGS) engine/core/graphics/src/main.cpp -o engine/core/graphics/obj/main.o -lGL
	@g++ $(BUILD) $(INCLUDE) -o engine/bin/engine engine/core/graphics/obj/main.o $(NO_MAIN_OBJS) $(CPPLIBS)

all:
	make cpp
	make python

clean:
	@echo "cleaning engine..."
	@rm engine/bin/engine.so engine/core/graphics/obj/*.o example

docs:
	doxygen Doxyfile