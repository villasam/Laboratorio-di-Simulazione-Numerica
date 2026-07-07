#ifndef __INPUT_UTILS_H__
#define __INPUT_UTILS_H__

#include <string>

using namespace std;

void ReadInput(

    int& n_cities,
    int& population_size,
    int& n_generations,

    double& p_crossover,
    double& p_swap,
    double& p_inversion,
    double& p_shift,
    double& p_block_swap,

    string& map_file,
    string& progress_file,
    string& best_path_file,
    string& best_coord_file
);

#endif