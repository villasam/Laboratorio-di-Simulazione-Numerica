CC = g++
CFLAGS = -Wall -O3 --std=c++11

Ex_03.exe : Ex_03.o random.o
	$(CC) random.o Ex_03.o -o Ex_03.exe
Ex_03.o : Ex_03.cpp
	$(CC) -c Ex_03.cpp -o Ex_03.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm -f *.o *.exe seed.out
