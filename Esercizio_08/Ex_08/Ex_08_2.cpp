#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

#include "random.h"
#include "functions.h"

using namespace std;

int main() {
    Random rnd;
    initialize_random(rnd);

    // Punto iniziale nello spazio dei parametri variazionali.
    double mu = 1.0;
    double sigma = 0.5;

    // Passo Metropolis in x.
    // Lo tengo fisso durante il SA: non voglio mischiare l'ottimizzazione
    // di mu e sigma con un adattamento continuo del campionamento in x.
    double delta_x_sa = 2.0;

    // Statistica usata durante il simulated annealing.
    // Deve essere abbastanza leggera, perché viene ripetuta tante volte.
    int sa_blocks = 100;
    int sa_steps_per_block = 1000;
    int sa_equilibration_steps = 1000;

    // Statistica più accurata usata solo per controllare i nuovi best.
    // Così evito di scegliere come migliore un punto solo perché ha avuto
    // una fluttuazione statistica favorevole nella stima veloce.
    int check_blocks = 100;
    int check_steps_per_block = 2000;
    int check_equilibration_steps = 5000;

    // Statistica finale: qui voglio una stima pulita da mettere nel notebook.
    int final_blocks = 100;
    int final_steps_per_block = 10000;
    int final_equilibration_steps = 10000;

    // Schedule del simulated annealing:
    //
    // T_i = T0 / (i+1)^alpha
    //
    // Con alpha = 1 si ha T_i = 1/(i+1).
    const int n_temperatures = 1000;
    const int steps_per_temperature = 100;

    double T0 = 10.0;
    double cooling_power = 1.0;

    // Passi iniziali di proposta per mu e sigma.
    // Questi vengono adattati durante la parte calda/intermedia del SA.
    double delta_mu = 0.08;
    double delta_sigma = 0.05;

    // Limiti sui passi nello spazio dei parametri.
    // Servono a evitare che, a bassa temperatura, i passi diventino
    // numericamente insignificanti.
    double min_delta_mu = 1.0e-3; 
    double min_delta_sigma = 1.0e-3;

    double max_delta_mu = 0.20;
    double max_delta_sigma = 0.15;

    // Prima stima veloce dell'energia al punto iniziale.
    EnergyEstimate current = estimate_energy(rnd,
                                             mu,
                                             sigma,
                                             delta_x_sa,
                                             sa_blocks,
                                             sa_steps_per_block,
                                             sa_equilibration_steps);

    // Stima più accurata del punto iniziale, usata per inizializzare
    // il best in modo più affidabile.
    EnergyEstimate checked_current = estimate_energy(rnd,
                                                     mu,
                                                     sigma,
                                                     delta_x_sa,
                                                     check_blocks,
                                                     check_steps_per_block,
                                                     check_equilibration_steps);

    double best_mu = mu;
    double best_sigma = sigma;
    double best_energy = checked_current.mean;
    double best_error = checked_current.error;

    ofstream out("../results/annealing_08_2.txt");
    if (!out.is_open()) {
        cerr << "PROBLEM: Unable to open output file" << endl;
        return 1;
    }

    out << "# step beta temperature mu sigma energy error accepted "
        << "best_energy best_error best_mu best_sigma acceptance "
        << "delta_mu delta_sigma" << endl;

    int global_step = 0;

    for (int itemp = 0; itemp < n_temperatures; itemp++) {

        double temperature = T0 / pow(double(itemp + 1), cooling_power);
        double beta = 1.0 / temperature;

        int accepted_at_temperature = 0;

        for (int istep = 0; istep < steps_per_temperature; istep++) {
            global_step++;

            double mu_new = mu + rnd.Rannyu(-delta_mu, delta_mu);
            double sigma_new = sigma + rnd.Rannyu(-delta_sigma, delta_sigma);

            bool accepted = false;

            // Mu e sigma devono rimanere positivi. Se non lo sono,
            // rifiuto direttamente la mossa.
            if (mu_new > 0.0 && sigma_new > 0.0) {

                EnergyEstimate proposed = estimate_energy(rnd,
                                                          mu_new,
                                                          sigma_new,
                                                          delta_x_sa,
                                                          sa_blocks,
                                                          sa_steps_per_block,
                                                          sa_equilibration_steps);

                double delta_energy = proposed.mean - current.mean;

                double acceptance_probability = 1.0;
                if (delta_energy > 0.0) {
                    acceptance_probability = exp(-beta * delta_energy);
                }

                if (rnd.Rannyu() < acceptance_probability) {
                    mu = mu_new;
                    sigma = sigma_new;
                    current = proposed;

                    accepted = true;
                    accepted_at_temperature++;

                    // Se la stima veloce suggerisce un punto interessante,
                    // lo ricontrollo con più statistica prima di aggiornare il best.
                    //
                    // Uso una condizione leggermente permissiva: se la stima veloce
                    // è compatibile con un miglioramento entro circa due errori,
                    // faccio comunque il controllo più accurato.
                    if (current.mean < best_energy + 2.0 * current.error) {

                        EnergyEstimate checked_candidate = estimate_energy(rnd,
                                                                           mu,
                                                                           sigma,
                                                                           delta_x_sa,
                                                                           check_blocks,
                                                                           check_steps_per_block,
                                                                           check_equilibration_steps);

                        if (checked_candidate.mean < best_energy) {
                            best_mu = mu;
                            best_sigma = sigma;
                            best_energy = checked_candidate.mean;
                            best_error = checked_candidate.error;
                        }
                    }
                }
            }

            double acceptance_rate =
                double(accepted_at_temperature) / double(istep + 1);

            out << global_step << " "
                << beta << " "
                << temperature << " "
                << mu << " "
                << sigma << " "
                << current.mean << " "
                << current.error << " "
                << accepted << " "
                << best_energy << " "
                << best_error << " "
                << best_mu << " "
                << best_sigma << " "
                << acceptance_rate << " "
                << delta_mu << " "
                << delta_sigma << endl;
        }

        double acceptance_rate =
            double(accepted_at_temperature) / double(steps_per_temperature);

        cout << "T = " << temperature
             << ", acceptance = " << acceptance_rate
             << ", mu = " << mu
             << ", sigma = " << sigma
             << ", H = " << current.mean
             << " +/- " << current.error
             << ", checked best = " << best_energy
             << " +/- " << best_error
             << " at mu = " << best_mu
             << ", sigma = " << best_sigma
             << ", delta_mu = " << delta_mu
             << ", delta_sigma = " << delta_sigma << endl;

        // Adatto solo i passi nello spazio variazionale.
        // L'adattamento serve nella parte calda/intermedia del SA.
        // A temperature molto basse l'algoritmo deve congelare, quindi non
        // continuo a ridurre artificialmente i passi se l'acceptance è zero.
        if (temperature > 0.01) {
            if (acceptance_rate > 0.60) {
                delta_mu *= 1.05;
                delta_sigma *= 1.05;
            } else if (acceptance_rate < 0.30) {
                delta_mu *= 0.90;
                delta_sigma *= 0.90;
            }

            delta_mu = max(min_delta_mu, min(delta_mu, max_delta_mu));
            delta_sigma = max(min_delta_sigma, min(delta_sigma, max_delta_sigma));
        }
    }

    out.close();

    cout << endl;
    cout << "Best point checked with larger statistics:" << endl;
    cout << "mu = " << best_mu << endl;
    cout << "sigma = " << best_sigma << endl;
    cout << "H_checked = " << best_energy << " +/- " << best_error << endl;

    // Per il risultato finale uso il miglior punto ricontrollato con più statistica.
    // Questo è più affidabile del minimo grezzo visto durante il SA.
    double final_mu = best_mu;
    double final_sigma = best_sigma;

    cout << endl;
    cout << "Final VMC run at the checked best point:" << endl;

    EnergyEstimate final_result = vmc_energy(rnd,
                                             final_mu,
                                             final_sigma,
                                             delta_x_sa,
                                             final_blocks,
                                             final_steps_per_block,
                                             final_equilibration_steps,
                                             "../results/metropolis_08_3.txt");

    sample_positions(rnd,
                     final_mu,
                     final_sigma,
                     delta_x_sa,
                     final_equilibration_steps,
                     100000,
                     "../results/sampled_positions_08_3.txt");

    cout << endl;
    cout << "Final result to report:" << endl;
    cout << "mu = " << final_mu << endl;
    cout << "sigma = " << final_sigma << endl;
    cout << "<H> = " << final_result.mean
         << " +/- " << final_result.error << endl;
    cout << "acceptance = " << final_result.acceptance << endl;

    rnd.SaveSeed();

    return 0;
}