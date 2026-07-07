#include <iostream>
#include <fstream>
#include <string>
#include "random_utils.h"

using namespace std;

Random InitializeRandom() {

    Random rnd;

    int seed[4];
    int p1, p2;

    ifstream Primes("../INPUT/Primes");

    if (Primes.is_open()) {
        Primes >> p1 >> p2;
    } else {
        cerr << "PROBLEM: Unable to open ../INPUT/Primes" << endl;
        exit(EXIT_FAILURE);
    }

    Primes.close();

    ifstream input("../INPUT/seed.in");

    string property;

    if (input.is_open()) {
        while (!input.eof()) {
            input >> property;

            if (property == "RANDOMSEED") {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }
    } else {
        cerr << "PROBLEM: Unable to open ../INPUT/seed.in" << endl;
        exit(EXIT_FAILURE);
    }

    input.close();

    return rnd;
}