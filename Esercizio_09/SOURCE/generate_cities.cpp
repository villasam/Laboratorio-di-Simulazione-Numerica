#include <iostream>
#include <fstream>
#include <cmath>
#include "random.h"
#include "random_utils.h"

using namespace std;

const int NCITIES = 34;

void GenerateCircle(Random& rnd) {

    ofstream fout("../INPUT/cities_circle.dat");

    for(int i = 0; i < NCITIES; i++) {
        double theta = rnd.Rannyu(0., 2. * M_PI);
        double x = cos(theta);
        double y = sin(theta);

        fout << i << " " << x << " " << y << endl;
    }

    fout.close();
}

void GenerateSquare(Random& rnd) {

    ofstream fout("../INPUT/cities_square.dat");

    for(int i = 0; i < NCITIES; i++) {
        double x = rnd.Rannyu();
        double y = rnd.Rannyu();

        fout << i << " " << x << " " << y << endl;
    }

    fout.close();
}

int main() {

    Random rnd = InitializeRandom();

    GenerateCircle(rnd);
    GenerateSquare(rnd);

    rnd.SaveSeed();

    return 0;
}