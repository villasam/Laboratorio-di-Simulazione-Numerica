CC = g++
CFLAGS = -Wall -O3 --std=c++11

all: Ex_08_1.exe Ex_08_2.exe

Ex_08_1.exe : Ex_08_1.o functions.o random.o
	$(CC) Ex_08_1.o functions.o random.o -o Ex_08_1.exe

Ex_08_2.exe : Ex_08_2.o functions.o random.o
	$(CC) Ex_08_2.o functions.o random.o -o Ex_08_2.exe

Ex_08_1.o : Ex_08_1.cpp functions.h random.h
	$(CC) -c Ex_08_1.cpp -o Ex_08_1.o $(CFLAGS)

Ex_08_2.o : Ex_08_2.cpp functions.h random.h
	$(CC) -c Ex_08_2.cpp -o Ex_08_2.o $(CFLAGS)

functions.o : functions.cpp functions.h random.h
	$(CC) -c functions.cpp -o functions.o $(CFLAGS)

random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean :
	rm -f *.o Ex_08_1.exe Ex_08_2.exe seed.out