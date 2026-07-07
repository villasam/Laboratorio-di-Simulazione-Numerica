#include <iostream>
#include <fstream>

#include "random_utils.h"
#include "map.h"
#include "population.h"
#include "input_utils.h"

using namespace std;

int main() {

    int n_cities;
    int population_size;
    int n_generations;

    double p_crossover;
    double p_swap;
    double p_inversion;
    double p_shift;
    double p_block_swap;
    string map_file;
    string progress_file;
    string best_path_file;
    string best_coord_file;

    ReadInput(n_cities, population_size, n_generations, p_crossover, p_swap, p_inversion, p_shift, p_block_swap, map_file, progress_file, best_path_file, best_coord_file);

    Random rnd = InitializeRandom();
    
    Map map; 
    map.LoadFromFile(map_file);

    if(map.GetNumberCities() != n_cities) {
        cerr << "ERROR: number of cities in file is different from n_cities"
             << endl;

        return 1;
    }

    Population pop(population_size, n_cities, rnd);

    pop.ComputeLengths(map);
    pop.Sort();

    ofstream fout(progress_file);
    
    // Apro file
    if(!fout.is_open()) {
        cerr << "ERROR: unable to open file " << progress_file << endl;

        return 1;
    }

    fout << "# generation best_length mean_best_half" << endl;

    // Stampo a file la generazione 0
    fout << 0 << " " << pop.GetBestLength() << " " << pop.GetMeanBestHalf() << endl;

    // Stampo a file le successive generazioni
    for(int generation = 1; generation <= n_generations; generation++) {

        pop.NewGeneration(rnd, map, p_crossover, p_swap, p_inversion, p_shift, p_block_swap);
        fout << generation << " " << pop.GetBestLength() << " " << pop.GetMeanBestHalf() << endl;
    }

    fout.close();

    pop.SaveBestPath(best_path_file);
    pop.SaveBestPathCoordinates(best_coord_file, map);
    
    cout << "Best length = " <<  pop.GetBestLength() << endl;

    rnd.SaveSeed();

    return 0;
}