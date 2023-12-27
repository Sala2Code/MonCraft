SRC := $(filter-out main.cpp test.cpp, $(wildcard *.cpp))

compile:
	g++ -I"C:\\librairies\\OpenGL\\include" main.cpp glad.c $(SRC) -L"C:\librairies\OpenGL\lib" -lglfw3 -lopengl32 -lgdi32

clean:
	rm -f main *.o
