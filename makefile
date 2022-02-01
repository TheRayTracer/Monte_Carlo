LIBS = -Wall
CC   = g++

monte_carlo: main.o image.o scene.o object.o perlin.o pathtracer.o
	$(CC) $(LIBS) -o monte_carlo main.o image.o scene.o object.o perlin.o pathtracer.o

main.o: main.cpp
	$(CC) -c main.cpp

image.o: image.cpp
	$(CC) -c image.cpp

scene.o: scene.cpp
	$(CC) -c scene.cpp

object.o: object.cpp
	$(CC) -c object.cpp

perlin.o: perlin.cpp
	$(CC) -c perlin.cpp

pathtracer.o: pathtracer.cpp
	$(CC) -c pathtracer.cpp

all: monte_carlo clean

clean:
	rm *.o

