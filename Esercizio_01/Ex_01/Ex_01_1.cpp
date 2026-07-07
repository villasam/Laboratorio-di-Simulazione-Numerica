#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"

using namespace std;

// Calcola l'incertezza statistica da media e media dei quadrati
double error(double block_mean, double block_mean2, int block_index) {
    if (block_index == 0) {
        return 0.0;
    }
    return sqrt((block_mean2 - block_mean * block_mean) / block_index);
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
   } 
   else {
      cerr << "PROBLEM: Unable to open seed.in" << endl;
   }
}

double compute_variance(double Random_Numbers[], double block_mean, int block_size){
   double var = 0.0;

   // somma degli scarti quadratici dalla media
   for(int i = 0; i < block_size; i++) {
      var += (Random_Numbers[i]-block_mean)*(Random_Numbers[i]-block_mean);
   }
    
   // stima della varianza campionaria (divisione per N-1)
   return var / (block_size - 1);
}


// Esercizio: stima di media e varianza con metodo a blocchi
void exercise_mean_variance(Random &rnd) {

   const int n_throws = 10000;      // numero totale di estrazioni
   const int n_blocks = 100;        // numero di blocchi
   const int block_size = n_throws / n_blocks;   // estrazioni per blocco

   ofstream out("../results/mean_variance_results.txt");
   if (!out.is_open()) {
      cerr << "PROBLEM: Unable to open output file" << endl;
      return;
   }

   // somme progressive per la media
   double progressive_mean = 0.0;
   double progressive_mean2 = 0.0;

   // somme progressive per la varianza
   double progressive_variance = 0.0;
   double progressive_variance2 = 0.0;

   for (int i = 0; i < n_blocks; i++) {

      double block_sum = 0.0;

      // array per salvare i numeri casuali del blocco
      double random_numbers[block_size] = {0.0};

      for (int j = 0; j < block_size; j++) {

         double random_number = rnd.Rannyu();   // numero uniforme in [0,1)
         block_sum += random_number;            // somma per calcolare la media
         random_numbers[j] = random_number;     // salvataggio per la varianza
      }

      // media e quadrato della media del blocco
      double block_mean = block_sum / block_size;
      double block_mean2 = block_mean * block_mean;

      // varianza e quadrato della varianza del blocco
      double block_variance = compute_variance(random_numbers, block_mean, block_size);
      double block_variance2 = block_variance * block_variance;

      // aggiornamento somme progressive media
      progressive_mean += block_mean;
      progressive_mean2 += block_mean2;
      
      // aggiornamento somme progressive varianza
      progressive_variance += block_variance;
      progressive_variance2 += block_variance2;

      // media cumulativa fino al blocco i
      double mean = progressive_mean / (i + 1);
      double mean2 = progressive_mean2 / (i + 1);

      // varianza cumulativa fino al blocco i
      double var_mean = progressive_variance / (i + 1);
      double var_mean2 = progressive_variance2 / (i + 1);

      // errore statistico sulla media
      double mean_error = error(mean, mean2, i);

      // errore statistico sulla varianza
      double variance_error = error(var_mean, var_mean2, i);

      // numero totale di lanci considerati fino a questo blocco
      int throws_so_far = (i + 1) * block_size;

      // output: lanci, errore sulla media, errore statistico, errore sulla varianza
      out << i+1 << " " << throws_so_far << " " << (mean - 0.5) << " " << mean_error << " " << (var_mean - 1.0 / 12.0) << " " << variance_error << "\n";
   }

   out.close();
}

// Calcola Chi Quadro
double compute_chi2(int observed[], double expected, int n_bins){
   double chi2 = 0.0;
   for(int i = 0; i < n_bins; i++) {
      chi2 += ((observed[i]-expected)*(observed[i]-expected))/expected;
   }
   return chi2;
}

void generate_bins(Random &rnd, int n_throws, int n_bins, int bins[]) {

   // inizializza i conteggi a zero
   for(int i = 0; i < n_bins; i++) {
      bins[i] = 0;
   }

   // genera n numeri casuali
   for(int i = 0; i < n_throws; i++) {
      double r = rnd.Rannyu();
      int bin = int(r * n_bins);   // individua il bin
      bins[bin] += 1;
   }
}

// Test di chi quadro per verificare se i numeri generati sono uniformemente distribuiti
void chi2_test(Random &rnd){

   const int n_bins = 100;
   const int n_throws = 10000;
   const int n_tests = 100;

   int bins[n_bins];

   double expected = double(n_throws)/n_bins;

   ofstream out("../results/chi2_results.txt");
   if (!out.is_open()) {
      cerr << "PROBLEM: Unable to open chi2 output file" << endl;
      return;
   }

   for(int j = 0; j < n_tests; j++){

      generate_bins(rnd, n_throws, n_bins, bins);
      double value = compute_chi2(bins, expected, n_bins);

      out << j << " " << value << endl;
   }

   out.close();
}

// Funzione principale
int main(int argc, char *argv[]) {

   Random rnd;
   initialize_random(rnd);
   exercise_mean_variance(rnd);

   chi2_test(rnd);

   rnd.SaveSeed();

   return 0;
}