
########### default engine build ############

# set this to the root directory of game-tool, like below:
PROJ_PATH=/home/jack/game-tool/

CC =g++
CC2 =gcc
EMSC_INCLUDES1=
EMSC_INCLUDES2=
EMSC_INCLUDES3=
ARGS1=-shared -o engine/bin/libengine.so  $(CPPLIBS) 
DEFAULT_INCLUDES=-I/usr/include/python3.8 -I/usr/lib/x86_64-linux-gnu 
USE_GLAD_O=engine/core/graphics/obj/glad.o
LIBS = -ldl -lassimp -lfreetype -lpython3.8 -lboost_python38 -lglfw -lopenal -laudio
PYTHON_LIB=-L/usr/lib/python3.8/config-3.7m-x86_64-linux-gnu/
BUILD1=core
ENGINE_FILE=engine.so
CPP_FLAG=-DUSE_CPP
CPP_FLAG2=$(CPPLIBS)
ENGINE_PATH=$(PROJ_PATH)3d-graphics-project/
BUILD_TYPE=


# Variables
###############################################]
SRC=$(ENGINE_PATH)engine/core/graphics/src/
SRC2=$(ENGINE_PATH)engine/core/physics/src/
INCLUDE= $(EMSC_INCLUDES3) -I$(ENGINE_PATH)engine/core/graphics/lib/include -I$(ENGINE_PATH)engine/core/graphics/lib/ $(DEFAULT_INCLUDES) -I$(ENGINE_PATH)engine/core/graphics/include/ -I$(ENGINE_PATH)engine/core/physics/include/ -I$(ENGINE_PATH)engine/core/graphics/lib/ -I$(ENGINE_PATH)engine/core/graphics/lib/freetype-2.10.1/include
CPPLIBS=  -L$(ENGINE_PATH)engine/core/graphics/lib/ $(PYTHON_LIB) -L./usr/lib/ $(LIBS) -std=c++11
FLAGS =-Wall -fPIC #-msse2 -Wno-missing-profile# -Wl,--no-undefined  #-Werror
DEBUG= #-g -O0  
RELEASE= -O3 #-funroll-loops -fno-signed-zeros -fno-trapping-math -fprofile-use -fopenmp -D_GLIBCXX_PARALLEL
BUILD = $(DEBUG) $(BUILD_TYPE)


# Make C++ Library
###############################################]

INC=$(ENGINE_PATH)engine/core/graphics/include/
INCS = $(wildcard $(INC)*.h)

OBJ=$(ENGINE_PATH)engine/core/graphics/obj/
_OBJS = $(INCS:.h=.o)
OBJS = $(sort $(subst $(ENGINE_PATH)engine/core/graphics/include/, $(ENGINE_PATH)engine/core/graphics/obj/, $(_OBJS)) $(USE_GLAD_O))

INC2=$(ENGINE_PATH)engine/core/physics/include/
INCS2 = $(wildcard $(INC2)*.h)
OBJ2=$(ENGINE_PATH)engine/core/physics/obj/
_OBJS2 = $(INCS2:.h=.o)
OBJS2 = $(sort $(subst $(ENGINE_PATH)engine/core/physics/include/, $(ENGINE_PATH)engine/core/physics/obj/, $(_OBJS2)))

NO_MAIN_OBJS = $(filter-out $(ENGINE_PATH)engine/core/graphics/obj/main.o, $(OBJS) $(OBJS2))

cpp: $(NO_MAIN_OBJS)
	echo source /home/me/Downloads/emsdk/emsdk_env.sh
	@echo "building engine..."
	@$(CC) -DUSE_CPP $(BUILD) -c $(INCLUDE) $(FLAGS) $(ENGINE_PATH)engine/core/graphics/src/main.cpp -o $(ENGINE_PATH)engine/core/graphics/obj/main.o  $(EMSC_INCLUDES2)
	@$(CC) $(BUILD) $(INCLUDE) $(ENGINE_PATH)engine/core/graphics/obj/main.o $(NO_MAIN_OBJS) $(FLAGS) $(ARGS1) 
	@echo "Finished."

$(OBJ)%.o : $(SRC)%.cpp
	@echo $(CC)
	@echo building $* ...
	@$(CC) $(BUILD) -c $(INCLUDE) $(FLAGS) $< -o $@ 

$(OBJ2)%.o : $(SRC2)%.cpp
	@echo building $* ...
	@$(CC) $(BUILD) -c $(INCLUDE) $(FLAGS) $< -o $@ 

# Make Python Library
###############################################

BINDINGS_INC=$(ENGINE_PATH)engine/bindings/graphics/include/
BINDINGS_SRC=$(ENGINE_PATH)engine/bindings/graphics/src/
BINDINGS_OBJ=$(ENGINE_PATH)engine/bindings/graphics/obj/

BINDINGS_INCS=$(wildcard $(BINDINGS_INC)*.h)

_BINDINGS_OBJS = $(BINDINGS_INCS:.h=.o)
BINDINGS_OBJS = $(sort $(subst $(ENGINE_PATH)engine/bindings/graphics/include/, $(ENGINE_PATH)engine/bindings/graphics/obj/, $(_BINDINGS_OBJS)))


BINDINGS_INC2=$(ENGINE_PATH)engine/bindings/physics/include/
BINDINGS_SRC2=$(ENGINE_PATH)engine/bindings/physics/src/
BINDINGS_OBJ2=$(ENGINE_PATH)engine/bindings/physics/obj/

BINDINGS_INCS2=$(wildcard $(BINDINGS_INC2)*.h)

_BINDINGS_OBJS2 = $(BINDINGS_INCS2:.h=.o)
BINDINGS_OBJS2 = $(sort $(subst $(ENGINE_PATH)engine/bindings/physics/include/, $(ENGINE_PATH)engine/bindings/physics/obj/, $(_BINDINGS_OBJS2)))


python: $(NO_MAIN_OBJS) $(BINDINGS_OBJS) $(BINDINGS_OBJS2) 
	@echo $(NO_MAIN_OBJS)
	@echo $(BINDINGS_OBJS)
	@echo "building engine..."
	@$(CC) $(BUILD) -c $(INCLUDE) $(FLAGS) $(ENGINE_PATH)engine/core/graphics/src/main.cpp -o $(ENGINE_PATH)engine/core/graphics/obj/main.o
	@echo "got here"
	@$(CC) $(BUILD) -c $(INCLUDE) $(FLAGS) $(ENGINE_PATH)engine/$(BUILD1)/graphics/src/main.cpp -o $(ENGINE_PATH)engine/$(BUILD1)/graphics/obj/main.o $(EMSC_INCLUDES2)
	@$(CC) $(BUILD) -shared $(INCLUDE) -o $(ENGINE_PATH)engine/bin/$(ENGINE_FILE) $(ENGINE_PATH)engine/core/graphics/obj/main.o  $(NO_MAIN_OBJS) $(BINDINGS_OBJS) $(BINDINGS_OBJS2) $(CPPLIBS) $(EMSC_INCLUDES2)

exe: $(BINDINGS_OBJS)
	@echo $(NO_MAIN_OBJS)
	@echo $(BINDINGS_OBJS)

	@echo "building engine..."
	@$(CC) $(BUILD) -c $(INCLUDE) $(FLAGS) $(ENGINE_PATH)engine/core/graphics/src/main.cpp -o $(ENGINE_PATH)engine/core/graphics/obj/main.o
	@$(CC) $(BUILD) $(INCLUDE) -o $(ENGINE_PATH)engine/bin/engine  $(ENGINE_PATH)engine/core/graphics/obj/main.o  $(NO_MAIN_OBJS) $(CPPLIBS)


$(BINDINGS_OBJ)%.o : $(BINDINGS_SRC)%.cpp 
	@echo building binding for $* ...
	@$(CC) $(BUILD) -c $(INCLUDE) -I $(BINDINGS_INC) $(FLAGS) $< -o $@ $(EMSC_INCLUDES2)


$(BINDINGS_OBJ2)%.o : $(BINDINGS_SRC2)%.cpp
	@echo building binding for $* ...
	@$(CC) $(BUILD) -c $(INCLUDE) -I $(BINDINGS_INC2) $(FLAGS) $< -o $@ 

###############################################


$(ENGINE_PATH)engine/core/graphics/obj/glad.o : $(ENGINE_PATH)engine/core/graphics/lib/glad/glad.c
	@echo building glad ...
	@$(CC2) -c $(INCLUDE) $(FLAGS) $< -o $@

bin:
	@$(CC) $(CPP_FLAG) $(BUILD) -c $(INCLUDE) $(FLAGS) $(ENGINE_PATH)engine/core/graphics/src/main.cpp -o $(ENGINE_PATH)engine/core/graphics/obj/main.o 
	@$(CC) $(BUILD) $(INCLUDE) -o $(ENGINE_PATH)engine/bin/engine $(ENGINE_PATH)engine/core/graphics/obj/main.o $(NO_MAIN_OBJS) $(CPP_FLAG2)

all:
	@echo "making"
	make cpp
	make python

clean:
	@echo "cleaning engine..."
	@rm $(ENGINE_PATH)engine/bin/engine.so $(ENGINE_PATH)engine/core/graphics/obj/*.o example

docs:
	doxygen Doxyfile