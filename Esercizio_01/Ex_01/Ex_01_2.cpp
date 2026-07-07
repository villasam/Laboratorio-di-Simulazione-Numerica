#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"

using namespace std;

// lanci per Sn
const int n1 = 1;
const int n2 = 2;
const int n3 = 10;
const int n4 = 100;
const int realizations = 10000;

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

// funzione che calcola la media di N lanci di dadi
double average_standard_dice(Random &rnd, int N) {
    double sum = 0.0;
    for(int i = 0; i < N; i++) {
        int dice = int(rnd.Rannyu()*6) + 1; // lancia un dado standard a 6 facce
        sum += dice;
    }
    return sum / N;
}

// funzione che calcola la media di N variabili distribuite come un esponenziale
double average_exponential(Random &rnd, int N, double lambda) {
    double sum = 0.0;
    for(int i = 0; i < N; i++) {
        double exp = rnd.Exponential(lambda);
        sum += exp;
    }
    return sum / N;
}

// funzione che calcola la media di N variabili estratte da una distribuzione di cauchy
double average_cauchy(Random &rnd, int N, double mu, double gamma) {
    double sum = 0.0;
    for(int i = 0; i < N; i++) {
        double cauchy = rnd.Cauchy(mu, gamma);
        sum += cauchy;
    }
    return sum / N;
}

// funzione che esegue diverse realizzazioni di Sn e le salva su un file per il dado
void results_standard_dice(Random &rnd, int n1, int n2, int n3, int n4, int realizations) {

    ofstream out("../results/standard_dice_results.txt");
    if (!out.is_open()) {
        cerr << "PROBLEM: Unable to open output file" << endl;
        return;
    }

    for(int i = 0; i < realizations; i++) {
        out << average_standard_dice(rnd, n1) << " " 
        << average_standard_dice(rnd, n2) << " " 
        << average_standard_dice(rnd, n3) << " " 
        << average_standard_dice(rnd, n4) << " " << endl;
    }

    out.close();
}

// funzione che esegue diverse realizzazioni di Sn e le salva su un file per l'esponenziale
void results_exponential(Random &rnd, double lambda, int n1, int n2, int n3, int n4, int realizations) {

    ofstream out("../results/exponential_results.txt");
    if (!out.is_open()) {
        cerr << "PROBLEM: Unable to open output file" << endl;
        return;
    }

    for(int i = 0; i < realizations; i++) {
        out << average_exponential(rnd, n1, lambda) << " " 
        << average_exponential(rnd, n2, lambda) << " " 
        << average_exponential(rnd, n3, lambda) << " " 
        << average_exponential(rnd, n4, lambda) << " " << endl;
    }

    out.close();
}

// funzione che esegue diverse realizzazioni di Sn e le salva su un file per cauchy
void results_cauchy(Random &rnd, double mu, double gamma, int n1, int n2, int n3, int n4, int realizations) {

    ofstream out("../results/cauchy_results.txt");
    if (!out.is_open()) {
        cerr << "PROBLEM: Unable to open output file" << endl;
        return;
    }

    for(int i = 0; i < realizations; i++) {
        out << average_cauchy(rnd, n1, mu, gamma) << " " 
        << average_cauchy(rnd, n2, mu, gamma) << " " 
        << average_cauchy(rnd, n3, mu, gamma) << " " 
        << average_cauchy(rnd, n4, mu, gamma) << " " << endl;
    }

    out.close();
}

int main(int argc, char *argv[]) {

   double lambda = 1.0;
   double mu = 0.0;
   double gamma = 1.0;

   Random rnd;
   initialize_random(rnd);
   results_standard_dice(rnd, n1, n2, n3, n4, realizations);
   results_exponential(rnd, lambda, n1, n2, n3, n4, realizations);
   results_cauchy(rnd, mu, gamma, n1, n2, n3, n4, realizations);

   rnd.SaveSeed();

   return 0;
}