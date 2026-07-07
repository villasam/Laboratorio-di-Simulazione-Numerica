CC = g++
CFLAGS = -O3 --std=c++11
AFLAGS = -larmadillo
all: simulator.exe
simulator.exe : system.o Ex_06.o particle.o random.o
	$(CC) system.o particle.o random.o Ex_06.o -o simulator.exe $(AFLAGS)
Ex_06.o : Ex_06.cpp system.h particle.h
	$(CC) -c Ex_06.cpp -o Ex_06.o $(CFLAGS)
system.o : system.cpp system.h random.h particle.h
	$(CC) -c system.cpp -o system.o $(CFLAGS)
particle.o : particle.cpp particle.h
	$(CC) -c particle.cpp -o particle.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm -f *.o *.exe
remove:
	rm ../OUTPUT/*.*
	rm ../OUTPUT/CONFIG/*.*
