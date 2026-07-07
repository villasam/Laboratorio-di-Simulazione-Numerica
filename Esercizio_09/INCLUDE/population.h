#ifndef __POPULATION_H__
#define __POPULATION_H__

#include <vector>
#include <string>


#include "individual.h"
#include "map.h"
#include "random.h"

using namespace std;

bool CompareIndividuals(const Individual& a, const Individual& b);

class Population {

private:

    vector<Individual> _individuals;

public:

    Population();

    Population(
        int population_size,
        int n_cities,
        Random& rnd
    );

    // valutazione
    void ComputeLengths(const Map& map);

    // ordinamento
    void Sort();

    // selezione
    int Select(Random& rnd) const;

    // crossover
    pair<Individual, Individual> Crossover(const Individual& parent1, const Individual& parent2, Random& rnd);

    // mutazioni
    void Mutate(Individual& individual, Random& rnd, double p_swap, double p_inversion, double p_shift, double p_block_swap);

    // evoluzione
    void NewGeneration(Random& rnd, const Map& map, double p_crossover, double p_swap, double p_inversion, double p_shift, double p_block_swap);

    // getter
    int GetSize() const;
    Individual GetIndividual(int index) const;
    Individual GetBest() const;
    double GetBestLength() const;
    double GetMeanBestHalf() const;   
    
    void SaveBestPath(string filename) const;
    void SaveBestPathCoordinates(string filename, const Map& map) const;

    // debug
    void PrintBest() const;
};

#endif