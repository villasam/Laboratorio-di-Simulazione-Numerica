#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include "random.h"

using namespace std;

struct EnergyEstimate {
    double mean;
    double error;
    double acceptance;
};

void initialize_random(Random &rnd);

double psi_squared(double x, double mu, double sigma);
double potential(double x);
double local_energy(double x, double mu, double sigma);

double statistical_error(double mean, double mean2, int n);

double metropolis_step_x(double x, Random &rnd, int &accepted,
                         double mu, double sigma, double delta_x);

EnergyEstimate vmc_energy(Random &rnd,
                          double mu,
                          double sigma,
                          double delta_x,
                          int n_blocks,
                          int steps_per_block,
                          int equilibration_steps,
                          const string &filename);

EnergyEstimate estimate_energy(Random &rnd,
                               double mu,
                               double sigma,
                               double delta_x,
                               int n_blocks,
                               int steps_per_block,
                               int equilibration_steps);

void sample_positions(Random &rnd,
                      double mu,
                      double sigma,
                      double delta_x,
                      int equilibration_steps,
                      int n_samples,
                      const string &filename);

#endif
