UNAME      := $(shell uname)
MAIN_FILES := main.cpp
CPP_FILES  := $(filter-out $(MAIN_FILES), $(subst ./,, $(shell find . -name "*.cpp")))
OBJ_FILES  := $(addprefix obj/,$(CPP_FILES:.cpp=.o))
OBJ_DIRS   := $(addsuffix .directory, $(dir $(OBJ_FILES)))

INCLUDES = . /usr/include/ /usr/local/include
LIB_DIRS = /usr/local/lib/
LIBS = 	   mingw32 SDLmain SDL SDL_image opengl32 glu32 glew32

INCLUDE_ARGS  = $(addprefix -I, $(INCLUDES))
LIB_DIRS_ARGS = $(addprefix -L, $(LIB_DIRS))
LIB_ARGS      = $(addprefix -l, $(LIBS))

all:  render

render:	$(OBJ_DIRS) $(OBJ_FILES) obj/main.o
	g++ -ggdb3 $(INCLUDE_ARGS) $(LIB_DIRS_ARGS) $(LIB_ARGS)  -o $@ $(OBJ_FILES) obj/main.o $(LIB_DIRS_ARGS) $(LIB_ARGS)

obj/%.o: %.cpp
	g++ -c -g $(INCLUDE_ARGS) -o  $@ $<

%.directory:
	mkdir -p $@

clean:
	rm -rf obj
	rm render.exe
