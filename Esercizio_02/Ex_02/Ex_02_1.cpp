#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"

using namespace std;

const int n_throws = 10000; //lanci
const int n_blocks = 100; //blocchi
const int block_size = n_throws/n_blocks; //lanci per blocco

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

double integrand(double x) {
   return (M_PI/2.)*cos(M_PI*x/2);
}

double montecarlo_unif(int block_size, Random &rnd){
   double mean = 0.0;
   for(int i = 0; i < block_size; i++) {
      double x = rnd.Rannyu();
      mean += integrand(x);
   }
   return mean/block_size;
}

double integrand_impsamp_taylor(double x) {
   return ((M_PI/2.)*cos(M_PI*x/2)/(2-2*x));
}

double random_impsamp_taylor(Random &rnd) {
   double random_number = rnd.Rannyu();
   return 1 - sqrt(1-random_number);
}

double montecarlo_impsamp_taylor(int block_size, Random &rnd){
   double mean = 0.0;
   for(int i = 0; i < block_size; i++) {
      double x = random_impsamp_taylor(rnd);
      mean += integrand_impsamp_taylor(x);
   }
   return mean/block_size;
}

double stat_error(double mean, double mean_sq, int n) {
   if (n == 0) {
      return 0.0;
   }
   return sqrt((mean_sq - mean * mean) / n);
}

void montecarlo_unif_blocks(Random &rnd, int n_throws, int n_blocks) {
   
   const int block_size = n_throws / n_blocks;   // lanci per blocco

   ofstream out("../results/montecarlo_unif_results.txt");
   if (!out.is_open()) {
      cerr << "PROBLEM: Unable to open output file" << endl;
      return;
   }

   double block_sum = 0.0;
   double block_sum_sq = 0.0;
   for (int i = 0; i < n_blocks; i++) {
      
      double integral = montecarlo_unif(block_size, rnd);

      block_sum += integral;
      block_sum_sq += integral*integral;

      double mean = block_sum / (i + 1);
      double mean_sq = block_sum_sq / (i + 1);

      double err = stat_error(mean, mean_sq, i);

      int block = (i + 1);

      out << block << " " << mean << " " << err << "\n";
   }

   out.close();
}

void montecarlo_impsamp_taylor_blocks(Random &rnd, int n_throws, int n_blocks) {
   
   const int block_size = n_throws / n_blocks;   // lanci per blocco

   ofstream out("../results/montecarlo_impsamp_taylor_results.txt");
   if (!out.is_open()) {
      cerr << "PROBLEM: Unable to open output file" << endl;
      return;
   }

   double block_sum = 0.0;
   double block_sum_sq = 0.0;
   for (int i = 0; i < n_blocks; i++) {
      
      double integral = montecarlo_impsamp_taylor(block_size, rnd);

      block_sum += integral;
      block_sum_sq += integral*integral;

      double mean = block_sum / (i + 1);
      double mean_sq = block_sum_sq / (i + 1);

      double err = stat_error(mean, mean_sq, i);

      int block = (i + 1);

      out << block << " " << mean << " " << err << "\n";
   }

   out.close();
}

int main (int argc, char *argv[]){

   Random rnd;
   initialize_random(rnd);
   montecarlo_unif_blocks(rnd, n_throws, n_blocks);

   initialize_random(rnd);
   montecarlo_impsamp_taylor_blocks(rnd, n_throws, n_blocks);

   rnd.SaveSeed();
   return 0;
}

