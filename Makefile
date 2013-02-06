all: 3dmaze.c
	cc -framework GLUT -framework OpenGL -o 3dmaze 3dmaze.c
cpp: 3dmaze.cpp
	c++ -framework GLUT -framework OpenGL -o 3dmaze 3dmaze.cpp
clean:
	rm 3dmaze