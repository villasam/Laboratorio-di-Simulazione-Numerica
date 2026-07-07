CC = g++
CFLAGS = -Wall -O3 --std=c++11

all: Ex_01_1.exe Ex_01_2.exe Ex_01_3.exe

Ex_01_1.exe : Ex_01_1.o random.o
	$(CC) random.o Ex_01_1.o -o Ex_01_1.exe

Ex_01_1.o : Ex_01_1.cpp
	$(CC) -c Ex_01_1.cpp -o Ex_01_1.o $(CFLAGS)

Ex_01_2.exe : Ex_01_2.o random.o
	$(CC) random.o Ex_01_2.o -o Ex_01_2.exe

Ex_01_2.o : Ex_01_2.cpp
	$(CC) -c Ex_01_2.cpp -o Ex_01_2.o $(CFLAGS)

Ex_01_3.exe : Ex_01_3.o random.o
	$(CC) random.o Ex_01_3.o -o Ex_01_3.exe

Ex_01_3.o : Ex_01_3.cpp
	$(CC) -c Ex_01_3.cpp -o Ex_01_3.o $(CFLAGS)

random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean :
	rm -f *.o Ex_01_1.exe Ex_01_2.exe Ex_01_3.exe seed.out