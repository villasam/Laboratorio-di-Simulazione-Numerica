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

    int& migration,
    int& n_migr,

    string& problem_name,
    string& map_file
);

#endif