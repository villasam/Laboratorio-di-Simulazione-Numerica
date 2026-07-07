#include <iostream>
#include <fstream>
#include <string>
#include "random_utils.h"

using namespace std;

Random InitializeRandom(int rank) {

    Random rnd;

    int seed[4];
    int p1, p2;

    ifstream Primes("INPUT/Primes");

    if(Primes.is_open()) {

        // Ogni rank salta le righe precedenti e legge una riga diversa
        for(int i = 0; i <= rank; i++) {
            if(!(Primes >> p1 >> p2)) {
                cerr << "ERROR: not enough rows in Primes for rank "
                     << rank << endl;
                exit(EXIT_FAILURE);
            }
        }

    } else {
        cerr << "ERROR: unable to open Primes" << endl;
        exit(EXIT_FAILURE);
    }

    Primes.close();

    ifstream input("INPUT/seed.in");

    string property;

    if(input.is_open()) {

        while(!input.eof()) {
            input >> property;

            if(property == "RANDOMSEED") {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }

    } else {
        cerr << "ERROR: unable to open seed.in" << endl;
        exit(EXIT_FAILURE);
    }

    input.close();

    return rnd;
}