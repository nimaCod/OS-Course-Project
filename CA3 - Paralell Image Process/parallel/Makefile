CC = g++ -std=c++11 -g -Wall -pthread

all: ImageFilters.out 

bmp.o: bmp.cpp bmp.h
	$(CC) -c bmp.cpp


main.o: main.cpp bmp.h
	$(CC) -c main.cpp

ImageFilters.out:  main.o bmp.o
	$(CC) -o ImageFilters.out main.o bmp.o

clean:
	rm -f *.o ImageFilters.out
