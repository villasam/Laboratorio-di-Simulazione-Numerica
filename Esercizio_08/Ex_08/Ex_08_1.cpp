#include <iostream>

#include "random.h"
#include "functions.h"

using namespace std;

int main() {
    Random rnd;
    initialize_random(rnd);

    // Primo test VMC richiesto dall'esercizio: parametri fissati.
    // Non sto ancora cercando il minimo: voglio solo stimare <H> con data blocking.
    double mu = 1.0;
    double sigma = 0.5;
    double delta_x = 1.2;

    int n_blocks = 100;
    int steps_per_block = 1000;
    int equilibration_steps = 10000;

    EnergyEstimate result = vmc_energy(rnd,
                                       mu,
                                       sigma,
                                       delta_x,
                                       n_blocks,
                                       steps_per_block,
                                       equilibration_steps,
                                       "../results/metropolis_08_1.txt");

    cout << endl;
    cout << "Exercise 08.1" << endl;
    cout << "<H> = " << result.mean << " +/- " << result.error << endl;
    cout << "acceptance = " << result.acceptance << endl;

    rnd.SaveSeed();
    return 0;
}
