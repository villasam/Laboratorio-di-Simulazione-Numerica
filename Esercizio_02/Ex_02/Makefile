CC = g++
CFLAGS = -Wall -O3 --std=c++11

all: Ex_02_1.exe Ex_02_2.exe

Ex_02_1.exe : Ex_02_1.o random.o
	$(CC) random.o Ex_02_1.o -o Ex_02_1.exe

Ex_02_1.o : Ex_02_1.cpp
	$(CC) -c Ex_02_1.cpp -o Ex_02_1.o $(CFLAGS)

Ex_02_2.exe : Ex_02_2.o random.o
	$(CC) random.o Ex_02_2.o -o Ex_02_2.exe

Ex_02_2.o : Ex_02_2.cpp
	$(CC) -c Ex_02_2.cpp -o Ex_02_2.o $(CFLAGS)

random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean :
	rm -f *.o Ex_02_1.exe Ex_02_2.exe seed.out