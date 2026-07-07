#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

#include "functions.h"

using namespace std;

void initialize_random(Random &rnd) {
    int seed[4];
    int p1, p2;

    ifstream Primes("Primes");
    if (Primes.is_open()) {
        Primes >> p1 >> p2;
    } else {
        cerr << "PROBLEM: Unable to open Primes" << endl;
    }
    Primes.close();

    ifstream input("seed.in");
    string property;

    if (input.is_open()) {
        while (input >> property) {
            if (property == "RANDOMSEED") {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }
        input.close();
    } else {
        cerr << "PROBLEM: Unable to open seed.in" << endl;
    }
}

// Distribuzione di probabilità da campionare: |Psi_T(x)|^2.
// La normalizzazione non serve, perché in Metropolis uso solo rapporti.
double psi_squared(double x, double mu, double sigma) {
    double sigma2 = sigma * sigma;

    double gaussian_plus  = exp(-pow(x - mu, 2) / (2.0 * sigma2));
    double gaussian_minus = exp(-pow(x + mu, 2) / (2.0 * sigma2));

    double psi = gaussian_plus + gaussian_minus;
    return psi * psi;
}

double potential(double x) {
    return pow(x, 4) - 2.5 * x * x;
}

// Energia locale E_L(x) = -1/2 Psi''(x)/Psi(x) + V(x).
// Qui uso la derivata seconda analitica delle due gaussiane.
double local_energy(double x, double mu, double sigma) {
    double sigma2 = sigma * sigma;
    double sigma4 = sigma2 * sigma2;

    double gaussian_plus  = exp(-pow(x - mu, 2) / (2.0 * sigma2));
    double gaussian_minus = exp(-pow(x + mu, 2) / (2.0 * sigma2));

    double psi = gaussian_plus + gaussian_minus;

    double psi_second =
        gaussian_plus  * (pow(x - mu, 2) / sigma4 - 1.0 / sigma2) +
        gaussian_minus * (pow(x + mu, 2) / sigma4 - 1.0 / sigma2);

    double kinetic_energy = -0.5 * psi_second / psi;

    return kinetic_energy + potential(x);
}

double statistical_error(double mean, double mean2, int n) {
    if (n == 0) return 0.0;
    return sqrt((mean2 - mean * mean) / double(n));
}

// Un passo Metropolis uniforme nella variabile x.
double metropolis_step_x(double x, Random &rnd, int &accepted,
                         double mu, double sigma, double delta_x) {
    double x_new = x + rnd.Rannyu(-delta_x, delta_x);

    double acceptance = min(1.0, psi_squared(x_new, mu, sigma) /
                                 psi_squared(x,     mu, sigma));

    if (rnd.Rannyu() < acceptance) {
        accepted++;
        return x_new;
    }

    return x;
}

// Stima finale di <H> senza salvare il data blocking su file.
// La uso nel simulated annealing, dove servono tante stime dell'energia.
EnergyEstimate estimate_energy(Random &rnd,
                               double mu,
                               double sigma,
                               double delta_x,
                               int n_blocks,
                               int steps_per_block,
                               int equilibration_steps) {
    double x = 0.1;

    int accepted_equilibration = 0;
    for (int i = 0; i < equilibration_steps; i++) {
        x = metropolis_step_x(x, rnd, accepted_equilibration,
                              mu, sigma, delta_x);
    }

    int accepted = 0;
    double sum_prog = 0.0;
    double sum_prog2 = 0.0;

    for (int iblock = 0; iblock < n_blocks; iblock++) {
        double block_sum = 0.0;

        for (int istep = 0; istep < steps_per_block; istep++) {
            x = metropolis_step_x(x, rnd, accepted,
                                  mu, sigma, delta_x);
            block_sum += local_energy(x, mu, sigma);
        }

        double block_average = block_sum / double(steps_per_block);
        sum_prog += block_average;
        sum_prog2 += block_average * block_average;
    }

    double mean = sum_prog / double(n_blocks);
    double mean2 = sum_prog2 / double(n_blocks);
    double error = statistical_error(mean, mean2, n_blocks - 1);
    double acceptance = double(accepted) / double(n_blocks * steps_per_block);

    return {mean, error, acceptance};
}

// Stessa stima, ma con salvataggio della media progressiva blocco per blocco.
// Questa funzione produce i file da usare direttamente nel notebook.
EnergyEstimate vmc_energy(Random &rnd,
                          double mu,
                          double sigma,
                          double delta_x,
                          int n_blocks,
                          int steps_per_block,
                          int equilibration_steps,
                          const string &filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        cerr << "PROBLEM: Unable to open output file " << filename << endl;
        return {0.0, 0.0, 0.0};
    }

    out << "# block progressive_energy error block_energy acceptance" << endl;

    double x = 0.1;

    int accepted_equilibration = 0;
    for (int i = 0; i < equilibration_steps; i++) {
        x = metropolis_step_x(x, rnd, accepted_equilibration,
                              mu, sigma, delta_x);
    }

    int accepted = 0;
    double sum_prog = 0.0;
    double sum_prog2 = 0.0;

    double progressive_energy = 0.0;
    double progressive_error = 0.0;

    for (int iblock = 0; iblock < n_blocks; iblock++) {
        double block_sum = 0.0;
        int accepted_before_block = accepted;

        for (int istep = 0; istep < steps_per_block; istep++) {
            x = metropolis_step_x(x, rnd, accepted,
                                  mu, sigma, delta_x);
            block_sum += local_energy(x, mu, sigma);
        }

        double block_energy = block_sum / double(steps_per_block);
        sum_prog += block_energy;
        sum_prog2 += block_energy * block_energy;

        progressive_energy = sum_prog / double(iblock + 1);
        double progressive_energy2 = sum_prog2 / double(iblock + 1);
        progressive_error = statistical_error(progressive_energy,
                                              progressive_energy2,
                                              iblock);

        double block_acceptance =
            double(accepted - accepted_before_block) / double(steps_per_block);

        out << iblock + 1 << " "
            << progressive_energy << " "
            << progressive_error << " "
            << block_energy << " "
            << block_acceptance << endl;
    }

    out.close();

    double acceptance = double(accepted) / double(n_blocks * steps_per_block);

    cout << "mu = " << mu
         << ", sigma = " << sigma
         << ", <H> = " << progressive_energy
         << " +/- " << progressive_error
         << ", acceptance = " << acceptance << endl;

    return {progressive_energy, progressive_error, acceptance};
}

void sample_positions(Random &rnd,
                      double mu,
                      double sigma,
                      double delta_x,
                      int equilibration_steps,
                      int n_samples,
                      const string &filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        cerr << "PROBLEM: Unable to open output file " << filename << endl;
        return;
    }

    out << "# x" << endl;

    double x = 0.1;

    int accepted_equilibration = 0;
    for (int i = 0; i < equilibration_steps; i++) {
        x = metropolis_step_x(x, rnd, accepted_equilibration,
                              mu, sigma, delta_x);
    }

    int accepted = 0;
    for (int i = 0; i < n_samples; i++) {
        x = metropolis_step_x(x, rnd, accepted, mu, sigma, delta_x);
        out << x << endl;
    }

    out.close();

    cout << "Positions sampled with acceptance = "
         << double(accepted) / double(n_samples) << endl;
}
