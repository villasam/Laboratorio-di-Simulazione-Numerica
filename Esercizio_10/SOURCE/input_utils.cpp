#include <iostream>
#include <fstream>
#include <string>

#include "input_utils.h"

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
) {

    ifstream input("../INPUT/input.dat");

    if(!input.is_open()) {

        cerr << "ERROR: unable to open ../INPUT/input.dat"
             << endl;

        exit(EXIT_FAILURE);
    }

    string keyword;

    while(input >> keyword) {

        if(keyword == "NCITIES") {

            input >> n_cities;
        }

        else if(keyword == "NPOPULATION") {

            input >> population_size;
        }

        else if(keyword == "NGENERATIONS") {

            input >> n_generations;
        }

        else if(keyword == "PCROSSOVER") {

            input >> p_crossover;
        }

        else if(keyword == "PSWAP") {

            input >> p_swap;
        }

        else if(keyword == "PINVERSION") {

            input >> p_inversion;
        }

        else if(keyword == "PSHIFT") {

            input >> p_shift;
        }

        else if(keyword == "PBLOCKSWAP") {

            input >> p_block_swap;
        }

        else if(keyword == "MIGRATION") {
            input >> migration;
        }

        else if(keyword == "N_MIGR") {
            input >> n_migr;
        }

        else if(keyword == "PROBLEM_NAME") {

            input >> problem_name;
        }

        else if(keyword == "MAP_FILE") {

            input >> map_file;
        }

        else {

            cerr << "WARNING: unknown keyword: "
                 << keyword
                 << endl;
        }
    }

    input.close();
}