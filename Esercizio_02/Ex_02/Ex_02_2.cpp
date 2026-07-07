#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"

using namespace std;

const int n_blocks = 100;
const int n_walks_per_block = 1000;
const int n_steps = 100;

struct Position {
    double x, y, z;
};

struct WalkResult {
    double mean[n_steps + 1];
    double err[n_steps + 1];
};

double squared_distance(const Position &p) {
    return p.x * p.x + p.y * p.y + p.z * p.z;
}

double stat_error(double mean, double mean_sq, int n) {
    if (n == 0) return 0.0;
    return sqrt((mean_sq - mean * mean) / n);
}

void take_lattice_step(Position &p, Random &rnd) {
    int dir = int(rnd.Rannyu(0, 6));

    if (dir == 0) p.x += 1.0;
    else if (dir == 1) p.x -= 1.0;
    else if (dir == 2) p.y += 1.0;
    else if (dir == 3) p.y -= 1.0;
    else if (dir == 4) p.z += 1.0;
    else p.z -= 1.0;
}

void take_continuum_step(Position &p, Random &rnd) {
    double phi = rnd.Rannyu(0.0, 2.0 * M_PI);
    double cos_theta = rnd.Rannyu(-1.0, 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    p.x += sin_theta * cos(phi);
    p.y += sin_theta * sin(phi);
    p.z += cos_theta;
}

WalkResult simulate_random_walk(Random &rnd, bool is_continuum) {
    WalkResult res;

    double sum_distances[n_steps + 1] = {0.0};
    double sum_distances_sq[n_steps + 1] = {0.0};

    for (int block = 0; block < n_blocks; block++) {

        double block_squared_distance_sum[n_steps + 1] = {0.0};

        for (int walk = 0; walk < n_walks_per_block; walk++) {

            Position p = {0.0, 0.0, 0.0};

            // passo 0
            block_squared_distance_sum[0] += squared_distance(p);

            for (int step = 1; step <= n_steps; step++) {

                if (is_continuum) {
                    take_continuum_step(p, rnd);
                } else {
                    take_lattice_step(p, rnd);
                }

                block_squared_distance_sum[step] += squared_distance(p);
            }
        }

        for (int step = 0; step <= n_steps; step++) {
            double block_avg_squared_distance = block_squared_distance_sum[step] / double(n_walks_per_block);
            double block_distance = sqrt(block_avg_squared_distance);

            sum_distances[step] += block_distance;
            sum_distances_sq[step] += block_distance * block_distance;
        }
    }

    for (int step = 0; step <= n_steps; step++) {
        double mean = sum_distances[step] / double(n_blocks);
        double mean_sq = sum_distances_sq[step] / double(n_blocks);

        res.mean[step] = mean;
        res.err[step] = stat_error(mean, mean_sq, n_blocks - 1);
    }

    return res;
}


// Inizializza il generatore leggendo Primes e seed.in
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

void save_walk_results(const string &filename, const WalkResult &res) {
    ofstream out(filename);

    for (int i = 0; i <= n_steps; i++) {
        out << i << " " << res.mean[i] << " " << res.err[i] << endl;
    }

    out.close();
}

int main (int argc, char *argv[]){

    Random rnd;
    initialize_random(rnd);

    WalkResult lattice = simulate_random_walk(rnd, false);
    WalkResult continuum = simulate_random_walk(rnd, true);

    save_walk_results("../results/rw_lattice.txt", lattice);
    save_walk_results("../results/rw_continuum.txt", continuum);

    rnd.SaveSeed();
    return 0;
}

