#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>
#include "random.h"

using namespace std;

const int steps_per_block = 10000;
const int n_blocks = 100;
const int n_equilibration = 10000;
const int trajectory_stride = 100;

enum class State {
   Ground1s,
   Excited2p
};

enum class Proposal {
   Uniform,
   Gaussian
};

struct Position {
   double x;
   double y;
   double z;

   double norm() const {
      return sqrt(x*x + y*y + z*z);
   }
};

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

// Densità di probabilità dell'orbitale scelto.
double probability_density(Position p, State state) {
   double r = p.norm();

   if (state == State::Ground1s) {
      return (1.0 / M_PI) * exp(-2.0 * r);
   }

   if (state == State::Excited2p) {
      return (1.0 / (32.0 * M_PI)) * p.z * p.z * exp(-r);
   }

   return 0.0;
}

// Proposta di una nuova posizione.
Position propose(Position p, Random &rnd, Proposal proposal, double step) {
   Position p_new = p;

   if (proposal == Proposal::Uniform) {
      p_new.x += rnd.Rannyu(-step, step);
      p_new.y += rnd.Rannyu(-step, step);
      p_new.z += rnd.Rannyu(-step, step);
   }

   if (proposal == Proposal::Gaussian) {
      p_new.x += rnd.Gauss(0.0, step);
      p_new.y += rnd.Gauss(0.0, step);
      p_new.z += rnd.Gauss(0.0, step);
   }

   return p_new;
}

// Singolo step Metropolis.
bool metropolis_step(Position &p, Random &rnd, State state, Proposal proposal, double step) {
   Position p_new = propose(p, rnd, proposal, step);

   double p_old = probability_density(p, state);
   double p_trial = probability_density(p_new, state);

   if (p_old <= 0.0) {
      if (p_trial > 0.0) {
         p = p_new;
         return true;
      } else {
         return false;
      }
   }

   double acceptance_probability = min(1.0, p_trial / p_old);

   if (rnd.Rannyu() < acceptance_probability) {
      p = p_new;
      return true;
   }

   return false;
}

double stat_error(double mean, double mean2, int n) {
   if (n == 0) {
      return 0.0;
   }

   return sqrt((mean2 - mean*mean) / n);
}

void equilibrate(
   Position &p,
   Random &rnd,
   State state,
   Proposal proposal,
   double step,
   int n_steps
) {
   int accepted_dummy = 0;

   for (int i = 0; i < n_steps; i++) {
      bool accepted = metropolis_step(p, rnd, state, proposal, step);

      if (accepted) {
         accepted_dummy++;
      }
   }
}

void run_blocks(
   Random &rnd, 
   const string &tag, 
   State state, 
   Proposal proposal, 
   double step, 
   Position start
) {
   ofstream out("../results/metropolis_" + tag + ".txt");
   ofstream trajectory("../results/trajectory_" + tag + ".txt");

   if (!out.is_open()) {
      cerr << "PROBLEM: Unable to open output file for " << tag << endl;
      return;
   }

   if (!trajectory.is_open()) {
      cerr << "PROBLEM: Unable to open trajectory file for " << tag << endl;
      return;
   }

   out << "# block block_average progressive_average error acceptance\n";
   trajectory << "# x y z\n";

   Position p = start;

   equilibrate(p, rnd, state, proposal, step, n_equilibration);

   double progressive_sum = 0.0;
   double progressive_sum_sq = 0.0;

   int global_step = 0;
   int total_accepted = 0;

   for (int i = 0; i < n_blocks; i++) {
      double block_sum = 0.0;
      int block_accepted = 0;

      for (int j = 0; j < steps_per_block; j++) {
         bool accepted = metropolis_step(p, rnd, state, proposal, step);

         if (accepted) {
            block_accepted++;
            total_accepted++;
         }

         block_sum += p.norm();

         if (global_step % trajectory_stride == 0) {
            trajectory << p.x << " " << p.y << " " << p.z << "\n";
         }

         global_step++;
      }

      double block_average = block_sum / double(steps_per_block);

      progressive_sum += block_average;
      progressive_sum_sq += block_average * block_average;

      double progressive_average = progressive_sum / double(i + 1);
      double progressive_average_sq = progressive_sum_sq / double(i + 1);
      double progressive_error = stat_error(progressive_average, progressive_average_sq, i);

      double block_acceptance = double(block_accepted) / double(steps_per_block);

      out << i + 1 << " "
            << block_average << " "
            << progressive_average << " "
            << progressive_error << " "
            << block_acceptance << "\n";
   }

   double acceptance_rate = double(total_accepted) / double(n_blocks * steps_per_block);

   cout << tag << " acceptance rate = " << acceptance_rate << endl;

   out.close();
   trajectory.close();
}

void run_equilibration(
   Random &rnd,
   const string &tag,
   State state,
   Proposal proposal,
   double step,
   Position start,
   int n_steps
) {
   ofstream out("../results/equilibration_" + tag + ".txt");

   if (!out.is_open()) {
      cerr << "PROBLEM: Unable to open equilibration file for " << tag << endl;
      return;
   }

   out << "# step r x y z\n";

   Position p = start;

   for (int i = 0; i < n_steps; i++) {
      metropolis_step(p, rnd, state, proposal, step);

      out << i + 1 << " "
          << p.norm() << " "
          << p.x << " "
          << p.y << " "
          << p.z << "\n";
   }

   out.close();
}

int main() {
   Random rnd;
   initialize_random(rnd);

   Position start_1s = {0.1, 0.1, 0.1};
   Position start_2p = {0.0, 0.0, 2.0};
   Position far_start = {100.0, 100.0, 100.0};

   run_blocks(rnd, "1s_uniform", State::Ground1s, Proposal::Uniform, 1.2, start_1s);
   run_blocks(rnd, "2p_uniform", State::Excited2p, Proposal::Uniform, 3.0, start_2p);

   run_blocks(rnd, "1s_gaussian", State::Ground1s, Proposal::Gaussian, 0.75, start_1s);
   run_blocks(rnd, "2p_gaussian", State::Excited2p, Proposal::Gaussian, 1.85, start_2p);

   run_equilibration(rnd, "1s_far_uniform", State::Ground1s, Proposal::Uniform, 1.2, far_start, 50000);
   run_equilibration(rnd, "2p_far_uniform", State::Excited2p, Proposal::Uniform, 3.0, far_start, 50000);

   rnd.SaveSeed();

   return 0;
}