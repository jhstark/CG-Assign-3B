PLATFORM := $(shell uname)

# Options provided by pkg-config where possible
GL_LIBS = `pkg-config --static --libs glfw3 glew`
CPPFLAGS = `pkg-config --cflags glfw3`

# Platform specific options
ifneq (, $(findstring Darwin, $(PLATFORM)))
	GL_LIBS += -framework OpenGL
endif

ifneq (, $(findstring MSYS, $(PLATFORM)))
	GL_LIBS = `pkg-config --static --libs glfw3 glew freeglut`
endif

ifneq (, $(findstring MINGW, $(PLATFORM)))
	GL_LIBS = `pkg-config --static --libs glfw3 glew freeglut`
endif

ifneq (, $(findstring CYGWIN, $(PLATFORM)))
	GL_LIBS = -lopengl32 -lglfw3 -lglew32
	EXT = .exe
	CPPFLAGS = -DWIN32
endif

CC = g++
EXE = assign3_part2
OBJS = main.o shader.o Object.o Landscape.o

.PHONY:  clean
test: ; $(info $$var is [$(PLATFORM)])echo Hello world
all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

main.o : main.cpp $(LINK)
	$(CC) $(CPPFLAGS) -c main.cpp

shader.o : shader.cpp shader.hpp
	$(CC) $(CPPFLAGS) -c shader.cpp
	
Object.o : worldGen/Object.cpp worldGen/Object.hpp
	$(CC) $(CPPFLAGS) -c worldGen/Object.cpp
Landscape.o : worldGen/Landscape.cpp worldGen/Landscape.hpp
	$(CC) $(CPPFLAGS) -c worldGen/Landscape.cpp
	
clean:
	rm -f *.o run$(EXT)
