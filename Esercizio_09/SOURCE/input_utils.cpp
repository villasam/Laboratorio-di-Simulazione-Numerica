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

    string& map_file,
    string& progress_file,
    string& best_path_file,
    string& best_coord_file
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

        else if(keyword == "MAP_FILE") {

            input >> map_file;
        }

        else if(keyword == "PROGRESS_FILE") {

            input >> progress_file;
        }

        else if(keyword == "BEST_PATH_FILE") {

            input >> best_path_file;
        }

        else if(keyword == "BEST_COORD_FILE") {

            input >> best_coord_file;
        }

        else {

            cerr << "WARNING: unknown keyword: "
                 << keyword
                 << endl;
        }
    }

    input.close();
}