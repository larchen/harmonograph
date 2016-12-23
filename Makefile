OBJS = harmonograph.o lodepng.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c -std=c++11 $(DEBUG)
LFLAGS = -Wall $(DEBUG)

harmonograph : harmonograph.o lodepng.o
	$(CC) $(LFLAGS) harmonograph.o lodepng.o -o harmonograph

harmonograph.o : harmonograph.cpp lodepng/lodepng.h lodepng/lodepng.cpp
	$(CC) $(CFLAGS) harmonograph.cpp

lodepng.o : lodepng/lodepng.h lodepng/lodepng.cpp
	$(CC) $(CFLAGS) lodepng/lodepng.cpp

clean:
	rm *.o && rm harmonograph
