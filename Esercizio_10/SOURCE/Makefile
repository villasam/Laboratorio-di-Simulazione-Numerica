CXX = mpicxx
GEN_CXX = g++

CXXFLAGS = -Wall -O3 --std=c++11
INC = -I../INCLUDE

SRC = main.cpp \
      random.cpp \
      random_utils.cpp \
      city.cpp \
      individual.cpp \
      population.cpp \
      map.cpp \
      input_utils.cpp \
      migration.cpp

GEN_SRC = generate_cities.cpp \
          random.cpp \
          random_utils.cpp \
          input_utils.cpp

EXE = tsp_mpi.exe
GEN_EXE = generate_cities.exe

all: $(EXE) $(GEN_EXE)

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $(INC) $(SRC) -o $(EXE)

$(GEN_EXE): $(GEN_SRC)
	$(GEN_CXX) $(CXXFLAGS) $(INC) $(GEN_SRC) -o $(GEN_EXE)

run4: $(EXE)
	mkdir -p ../OUTPUT/square ../OUTPUT/italy
	mpirun -np 4 ./$(EXE)

run11: $(EXE)
	mkdir -p ../OUTPUT/square ../OUTPUT/italy
	mpirun -np 11 ./$(EXE)

generate: $(GEN_EXE)
	./$(GEN_EXE)

clean:
	rm -f $(EXE) $(GEN_EXE)

clean_output:
	rm -f ../OUTPUT/*.out
	rm -f ../OUTPUT/*.dat
	rm -f ../OUTPUT/square/*.dat
	rm -f ../OUTPUT/italy/*.dat

.PHONY: all run4 run11 generate clean clean_output