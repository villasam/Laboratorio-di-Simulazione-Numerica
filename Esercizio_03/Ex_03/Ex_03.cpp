#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include "random.h"

using namespace std;

const int n_throws = 100000; //lanci
const int n_blocks = 100; //blocchi

// Parametri del problema
const double S0 = 100.0;
const double T  = 1.0;
const double K  = 100.0;
const double r  = 0.1;
const double sigma = 0.25;
const int n_steps = 100;

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

double stat_error(double mean, double mean_sq, int block_index) {
   if (block_index == 0) {
      return 0.0;
   }
   return sqrt((mean_sq - mean * mean) / block_index);
}

// Prezzo finale diretto: campionamento diretto di S(T)
double final_price_direct(Random &rnd) {
   double Z = rnd.Gauss(0.0, 1.0);

   return S0 * exp((r - 0.5*sigma*sigma) * T + sigma * Z * sqrt(T));
}

// Prezzo finale discretizzato: costruzione del cammino fino a T
double final_price_discrete(Random &rnd) {
   double dt = T / double(n_steps);
   double price = S0;

   for (int i = 0; i < n_steps; i++) {
      double Z = rnd.Gauss(0.0, 1.0);
      price *= exp((r - 0.5*sigma*sigma) * dt + sigma * Z * sqrt(dt));
   }
   
   return price;
}

// Payoff scontati
double discounted_call_payoff(double ST) {
   return exp(-r*T) * max(0.0, ST - K);
}

double discounted_put_payoff(double ST) {
    return exp(-r * T) * max(0.0, K - ST);
}

void compute_direct_blocks(Random &rnd, int n_throws, int n_blocks) {
   
   const int n_throws_per_block = n_throws / n_blocks;   // lanci per blocco

   ofstream out_call("../results/call_direct_results.txt");
   ofstream out_put("../results/put_direct_results.txt");

   if (!out_call.is_open() || !out_put.is_open()) {
      cerr << "PROBLEM: Unable to open direct output files" << endl;
      return;
   }

   double progressive_sum_call = 0.0;
   double progressive_sum_call_sq = 0.0;

   double progressive_sum_put = 0.0;
   double progressive_sum_put_sq = 0.0;
   
   for (int block = 0; block < n_blocks; block++) {

      double block_call = 0.0;
      double block_put = 0.0;

      for (int j = 0; j < n_throws_per_block; j++) {
         double ST = final_price_direct(rnd);

         block_call += discounted_call_payoff(ST);
         block_put  += discounted_put_payoff(ST);
      }

      double block_average_call = block_call / n_throws_per_block;
      double block_average_put  = block_put / n_throws_per_block;

      progressive_sum_call += block_average_call;
      progressive_sum_call_sq += block_average_call * block_average_call;

      progressive_sum_put += block_average_put;
      progressive_sum_put_sq += block_average_put * block_average_put;

      double mean_call = progressive_sum_call / (block + 1);
      double mean_call_sq = progressive_sum_call_sq / (block + 1);
      double err_call = stat_error(mean_call, mean_call_sq, block);

      double mean_put = progressive_sum_put / (block + 1);
      double mean_put_sq = progressive_sum_put_sq / (block + 1);
      double err_put = stat_error(mean_put, mean_put_sq, block);

      out_call << block + 1 << " " << mean_call << " " << err_call << "\n";
      out_put  << block + 1 << " " << mean_put  << " " << err_put  << "\n";
   }

   out_call.close();
   out_put.close();       
}

// Data blocking per metodo discretizzato
void compute_discrete_blocks(Random &rnd, int n_throws, int n_blocks) {
   const int n_throws_per_block = n_throws / n_blocks;

   ofstream out_call("../results/call_discrete_results.txt");
   ofstream out_put("../results/put_discrete_results.txt");

   if (!out_call.is_open() || !out_put.is_open()) {
      cerr << "PROBLEM: Unable to open discrete output files" << endl;
      return;
   }

   double progressive_sum_call = 0.0;
   double progressive_sum_call_sq = 0.0;

   double progressive_sum_put = 0.0;
   double progressive_sum_put_sq = 0.0;

   for (int block = 0; block < n_blocks; block++) {
      double block_call = 0.0;
      double block_put = 0.0;

      for (int j = 0; j < n_throws_per_block; j++) {
         double ST = final_price_discrete(rnd);

         block_call += discounted_call_payoff(ST);
         block_put  += discounted_put_payoff(ST);
      }

      double block_average_call = block_call / n_throws_per_block;
      double block_average_put  = block_put / n_throws_per_block;

      progressive_sum_call += block_average_call;
      progressive_sum_call_sq += block_average_call * block_average_call;

      progressive_sum_put += block_average_put;
      progressive_sum_put_sq += block_average_put * block_average_put;

      double mean_call = progressive_sum_call / (block + 1);
      double mean_call_sq = progressive_sum_call_sq / (block + 1);
      double err_call = stat_error(mean_call, mean_call_sq, block);

      double mean_put = progressive_sum_put / (block + 1);
      double mean_put_sq = progressive_sum_put_sq / (block + 1);
      double err_put = stat_error(mean_put, mean_put_sq, block);

      out_call << block + 1 << " " << mean_call << " " << err_call << "\n";
      out_put  << block + 1 << " " << mean_put  << " " << err_put  << "\n";
   }

   out_call.close();
   out_put.close();
}

int main (){

   Random rnd;
   initialize_random(rnd);

   compute_direct_blocks(rnd, n_throws, n_blocks);
   compute_discrete_blocks(rnd, n_throws, n_blocks);

   rnd.SaveSeed();
   return 0;
}

