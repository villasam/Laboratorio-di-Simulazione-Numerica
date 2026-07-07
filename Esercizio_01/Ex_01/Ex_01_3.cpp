#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"

using namespace std;

const double d = 2.0;   // distanza tra le linee
const double L = 1.0;   // lunghezza dell'ago

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

// Controlla se un ago lanciato casualmente interseca una linea
bool hit(Random &rnd) {

    // distanza del centro dell'ago dalla linea più vicina
    double y = rnd.Rannyu(0.0, d / 2.0);
    
    // generazione di una direzione casuale senza usare pi greco
    double x1, x2, r2;

    do {
        x1 = rnd.Rannyu(-1.0, 1.0);
        x2 = rnd.Rannyu(-1.0, 1.0);
        r2 = x1 * x1 + x2 * x2;
    } while (r2 > 1.0 || r2 == 0.0);
    
    double sin_theta = fabs(x2) / sqrt(r2);

    // l'ago interseca una linea se la proiezione verticale della mezza lunghezza
    // supera la distanza del centro dalla linea più vicina
    return y <= (L / 2.0) * sin_theta;
}

// Calcola l'incertezza statistica da media progressiva e media progressiva dei quadrati
double error(double mean, double mean2, int block_index) {
    if (block_index == 0) {
        return 0.0;
    }
    return sqrt((mean2 - mean * mean) / block_index);
}

// Esperimento di Buffon con blocking method
void buffon(Random &rnd) {

    const int n_throws = 1000000;     // numero totale di lanci
    const int n_blocks = 100;         // numero di blocchi
    const int block_size = n_throws / n_blocks;
   
    ofstream out("../results/buffon.txt");
    if (!out.is_open()) {
        cerr << "PROBLEM: Unable to open output file" << endl;
        return;
    }
    
    double sum_pi = 0.0;
    double sum_pi2 = 0.0;
    
    for (int i = 0; i < n_blocks; i++) {
        
        int block_hits = 0;
        
        for (int j = 0; j < block_size; j++) {
            if (hit(rnd)) {
                block_hits++;
            }
        }

        if (block_hits == 0) {
            cerr << "WARNING: zero hits in block " << i + 1 << endl;
            continue;
        }
        
        // pi = 2 L N_thr / (d N_hit)
        double pi_estimate = 2.0 * L * block_size / (d * block_hits);
        double pi_estimate2 = pi_estimate * pi_estimate;
        
        sum_pi += pi_estimate;
        sum_pi2 += pi_estimate2;
        
        double cumulative_pi = sum_pi / (i + 1);
        double cumulative_pi2 = sum_pi2 / (i + 1);
        
        double pi_error = error(cumulative_pi, cumulative_pi2, i);
        
        int throws_so_far = (i + 1) * block_size;
        
        out << throws_so_far << " "
            << cumulative_pi << " "
            << pi_error << "\n";
    }
    
    out.close();
}

int main(int argc, char *argv[]) {

    Random rnd;
    initialize_random(rnd);

    buffon(rnd);

    rnd.SaveSeed();

    return 0;
}