CC = g++
CFLAGS = -Wall -O3 --std=c++11

Ex_05.exe: Ex_05.o random.o
	$(CC) random.o Ex_05.o -o Ex_05.exe

Ex_05.o: Ex_05.cpp random.h
	$(CC) -c Ex_05.cpp -o Ex_05.o $(CFLAGS)

random.o: random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

clean:
	rm -f *.o *.exe seed.out
