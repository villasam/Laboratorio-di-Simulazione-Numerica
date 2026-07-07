#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <cstdlib>

#include "population.h"

using namespace std;

// Costruttori
Population::Population() {}

Population::Population(int population_size, int n_cities, Random& rnd) {

    _individuals.resize(population_size);

    for(int i = 0; i < population_size; i++) {

        _individuals[i] = Individual(n_cities);
        _individuals[i].RandomInitialize(rnd);
    }
}

// Calcolo lunghezza di ciascun individuo
void Population::ComputeLengths(const Map& map) {

    for(int i = 0; i < int(_individuals.size()); i++) {

        _individuals[i].ComputeLength(map);
    }
}


bool CompareIndividuals(const Individual& a, const Individual& b) {

    return a.GetLength() < b.GetLength();
}

// sort(a, b, comparator)
void Population::Sort() {

    sort(_individuals.begin(), _individuals.end(), CompareIndividuals);
}


int Population::Select(Random& rnd) const {

    // Aumentando p si rende il tutto più selettivo
    double p = 1.5;

    int index = int(_individuals.size() * pow(rnd.Rannyu(), p));

    return index;
}


pair<Individual, Individual> Population::Crossover(const Individual& parent1, const Individual& parent2, Random& rnd) {

    int n_cities = parent1.GetNumberCities();

    Individual child1(n_cities);
    Individual child2(n_cities);

    vector<int> path1(n_cities, -1);
    vector<int> path2(n_cities, -1);

    // città iniziale fissata
    path1[0] = 0;
    path2[0] = 0;

    // punto di taglio
    int cut = int(rnd.Rannyu(1, n_cities));

    // copia prima parte
    for(int i = 1; i < cut; i++) {

        path1[i] = parent1.GetCityIndex(i);
        path2[i] = parent2.GetCityIndex(i);
    }

    // completa child1
    int index1 = cut;

    for(int i = 1; i < n_cities; i++) {

        int city = parent2.GetCityIndex(i);

        bool found = false;

        for(int j = 1; j < cut; j++) {
            if(path1[j] == city) {
                found = true;
                break;
            }
        }

        if(!found) {
            path1[index1] = city;
            index1++;
        }
    }

    // completa child2
    int index2 = cut;

    for(int i = 1; i < n_cities; i++) {

        int city = parent1.GetCityIndex(i);

        bool found = false;

        for(int j = 1; j < cut; j++) {
            if(path2[j] == city) {
                found = true;
                break;
            }
        }

        if(!found) {
            path2[index2] = city;
            index2++;
        }
    }

    child1.SetPath(path1);
    child2.SetPath(path2);

    return make_pair(child1, child2);
}

// MUTAZIONI
void Population::Mutate(Individual& individual, Random& rnd, double p_swap, double p_inversion, double p_shift, double p_block_swap) {

    if(rnd.Rannyu() < p_swap) {
        individual.SwapMutation(rnd);
    }

    if(rnd.Rannyu() < p_inversion) {
        individual.InversionMutation(rnd);
    }

    if(rnd.Rannyu() < p_shift) {
        individual.ShiftMutation(rnd);
    }

    if(rnd.Rannyu() < p_block_swap) {
        individual.BlockSwapMutation(rnd);
    }
}


void Population::NewGeneration(Random& rnd, const Map& map, double p_crossover, double p_swap, double p_inversion, double p_shift, double p_block_swap) {

    vector<Individual> new_population;
    new_population.reserve(_individuals.size());

    int n_elite = 0;

    for(int i = 0; i < n_elite; i++) {
        new_population.push_back(_individuals[i]);
    }


    while(new_population.size() < _individuals.size()) {

        int i1 = Select(rnd);
        int i2 = Select(rnd);
        
        while(i2 == i1 && _individuals.size() > 1) {
            i2 = Select(rnd);
        }

        Individual parent1 = _individuals[i1];
        Individual parent2 = _individuals[i2];

        Individual child1 = parent1;
        Individual child2 = parent2;

        // crossover
        if(rnd.Rannyu() < p_crossover) {

            auto children =
                Crossover(parent1, parent2, rnd);

            child1 = children.first;
            child2 = children.second;
        }

        // mutazioni
        Mutate(child1, rnd, p_swap, p_inversion, p_shift, p_block_swap);
        Mutate(child2, rnd, p_swap, p_inversion, p_shift, p_block_swap);

        // ricalcolo lunghezze
        child1.ComputeLength(map);
        child2.ComputeLength(map);

        // controlli
        if(!child1.Check()) {
            cerr << "ERROR: invalid child1" << endl;
            exit(EXIT_FAILURE);
        }

        if(!child2.Check()) {
            cerr << "ERROR: invalid child2" << endl;
            exit(EXIT_FAILURE);
        }

        new_population.push_back(child1);

        if(new_population.size() < _individuals.size()) {
            new_population.push_back(child2);
        }
    }

    _individuals = new_population;

    Sort();
}

int Population::GetSize() const {

    return _individuals.size();
}


Individual Population::GetIndividual(int index) const {

    return _individuals[index];
}


Individual Population::GetBest() const {

    return _individuals[0];
}


double Population::GetBestLength() const {

    return _individuals[0].GetLength();
}


double Population::GetMeanBestHalf() const {

    int half = _individuals.size() / 2;

    double sum = 0.0;

    for(int i = 0; i < half; i++) {
        sum += _individuals[i].GetLength();
    }

    return sum / double(half);
}

void Population::PrintBest() const {

    _individuals[0].Print();
}

void Population::SaveBestPath(string filename) const {

    ofstream fout(filename);

    if(!fout.is_open()) {
        cerr << "ERROR: unable to open file " << filename << endl;
        return;
    }

    Individual best = GetBest();

    for(int i = 0; i < best.GetNumberCities(); i++) {

        fout << i << " " << best.GetCityIndex(i) << endl;
    }


    fout << best.GetNumberCities() << " " << best.GetCityIndex(0) << endl;

    fout.close();
}

void Population::SaveBestPathCoordinates(string filename, const Map& map) const {

    ofstream fout(filename);

    if(!fout.is_open()) {
        cerr << "ERROR: unable to open file " << filename << endl;
        return;
    }

    Individual best = GetBest();

    for(int i = 0; i < best.GetNumberCities(); i++) {

        int city_index = best.GetCityIndex(i);
        City city = map.GetCity(city_index);

        fout << city.GetX() << " " << city.GetY() << endl;
    }

    // chiusura del cammino
    int first_city_index = best.GetCityIndex(0);
    City first_city = map.GetCity(first_city_index);

    fout << first_city.GetX() << " " << first_city.GetY() << endl;

    fout.close();
}

