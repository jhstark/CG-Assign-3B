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

CC = g++ --std=c++11
EXE = assign3_part2
OBJS = main.o stb_image.o tiny_obj_loader.o Camera.o shader.o Object.o Skybox.o Plane.o HeightMap.o
libDir = libraries/

.PHONY:  clean
test: ; $(info $$var is [$(PLATFORM)])echo Hello world
all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

main.o : main.cpp $(LINK)
	$(CC) $(CPPFLAGS) -c main.cpp
	
stb_image.o : $(libDir)stb_image.cpp
	$(CC) -c -o stb_image.o $(libDir)stb_image.cpp
	
openglH.o : $(libDir)openglH.cpp
	$(CC) -c -o openglH.o $(libDir)openglH.cpp
	
tiny_obj_loader.o : $(libDir)tiny_obj_loader.cpp
	$(CC) -c -o tiny_obj_loader.o $(libDir)tiny_obj_loader.cpp
	
Camera.o : Camera.cpp Camera.hpp
	$(CC) $(CPPFLAGS) -c Camera.cpp

shader.o : $(libDir)shader.cpp $(libDir)shader.hpp
	$(CC) $(CPPFLAGS) -c $(libDir)shader.cpp
	
Object.o : worldGen/Object.cpp worldGen/Object.hpp Skybox.o Plane.o HeightMap.o
	$(CC) $(CPPFLAGS) -c worldGen/Object.cpp
	
Skybox.o : worldGen/Skybox.cpp worldGen/Skybox.hpp
	$(CC) $(CPPFLAGS) -c worldGen/Skybox.cpp
	
Plane.o : worldGen/Plane.cpp worldGen/Plane.hpp
	$(CC) $(CPPFLAGS) -c worldGen/Plane.cpp

HeightMap.o : worldGen/HeightMap.cpp worldGen/HeightMap.hpp
	$(CC) $(CPPFLAGS) -c worldGen/HeightMap.cpp
	
clean:
	rm -f *.o run$(EXT)
