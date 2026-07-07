CXX = g++
CXXFLAGS = -Wall -O3 --std=c++11
INC = -I../INCLUDE

MAIN_SRC = \
	main.cpp \
	random.cpp \
	random_utils.cpp \
	city.cpp \
	individual.cpp \
	population.cpp \
	map.cpp \
	input_utils.cpp

GEN_SRC = \
	generate_cities.cpp \
	random.cpp \
	random_utils.cpp \
	input_utils.cpp

all: main.exe generate_cities.exe

main.exe: $(MAIN_SRC)
	$(CXX) $(MAIN_SRC) -o main.exe $(CXXFLAGS) $(INC)

generate_cities.exe: $(GEN_SRC)
	$(CXX) $(GEN_SRC) -o generate_cities.exe $(CXXFLAGS) $(INC)

# Run from the project root, so paths like INPUT/ and OUTPUT/ are resolved correctly.
run: main.exe
	./main.exe

generate: generate_cities.exe
	./generate_cities.exe

clean:
	rm -f main.exe generate_cities.exe

clean_output:	
	rm -f ../OUTPUT/*.out ../OUTPUT/*.dat

.PHONY: all run generate clean clean_output