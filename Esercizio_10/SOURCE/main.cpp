#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <mpi.h>
#include <vector>

#include "random_utils.h"
#include "map.h"
#include "population.h"
#include "input_utils.h"
#include "migration.h"

using namespace std;

int main(int argc, char* argv[]) {

    // Inizializzo l'ambiente MPI
    MPI_Init(&argc, &argv);

    // Ogni processo conosce il proprio rank e il numero totale di processi
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Parametri principali della simulazione
    int n_cities;
    int population_size;
    int n_generations;

    // Probabilità degli operatori genetici
    double p_crossover;
    double p_swap;
    double p_inversion;
    double p_shift;
    double p_block_swap;

    // Parametri della migrazione
    int migration;
    int n_migr;

    // File di input/output letti da input.dat
    string problem_name;
    string map_file;

    // Lettura dei parametri dal file di input
    ReadInput(n_cities, population_size, n_generations,
              p_crossover, p_swap, p_inversion, p_shift, p_block_swap,
              migration, n_migr,
              problem_name, map_file);

    // Inizializzo il generatore random usando semi diversi per ogni rank
    Random rnd = InitializeRandom(rank);

    // Carico la mappa delle città
    Map map;
    map.LoadFromFile(map_file);

    // Controllo che il numero di città letto sia quello atteso
    if(map.GetNumberCities() != n_cities) {

        cerr << "ERROR rank " << rank
             << ": number of cities in file is different from n_cities"
             << endl;

        MPI_Finalize();
        return 1;
    }

    // Definisco il tipo di run in base al parametro migration
    string run_type = "independent";

    if(migration == 1) {
        run_type = "migration";
    }

    // Cartella di output associata al problema considerato
    string output_folder =
        "../OUTPUT/" + problem_name;

    // Costruisco il nome del file in cui salvo il miglior cammino del rank
    string best_path_file_rank =
        output_folder + "/bestpath_" + problem_name + "_" +
        run_type + "_rank_" + to_string(rank) + ".dat";

    // Costruisco il nome del file in cui salvo le coordinate del miglior cammino
    string best_coord_file_rank =
        output_folder + "/bestpath_coordinates_" + problem_name + "_" +
        run_type + "_rank_" + to_string(rank) + ".dat";

    // Costruisco il nome del file in cui salvo l'andamento del rank 0
    string history_file_rank =
        output_folder + "/history_" + problem_name + "_" +
        run_type + "_rank_" + to_string(rank) + ".dat";

    // Costruisco il nome del file riassuntivo finale della run
    string summary_file_run =
        output_folder + "/final_summary_" + problem_name + "_" +
        run_type + ".dat";

    // Creo la popolazione iniziale del continente associato a questo rank
    Population pop(population_size, n_cities, rnd);

    // Calcolo le lunghezze iniziali e ordino la popolazione
    pop.ComputeLengths(map);
    pop.Sort();

    // File per salvare best length e media della metà migliore
    ofstream history;

    // Salvo la storia solo del rank 0, come riferimento per i grafici
    if(rank == 0) {

        history.open(history_file_rank);

        // Controllo che il file history sia stato aperto correttamente
        if(!history.is_open()) {

            cerr << "ERROR rank " << rank
                 << ": unable to open file "
                 << history_file_rank
                 << endl;

            MPI_Finalize();
            return 1;
        }

        // Intestazione del file history
        history << "# generation best_length mean_best_half" << endl;

        // Fisso il numero di cifre decimali nell'output
        history << fixed << setprecision(10);

        // Salvo anche la generazione iniziale
        history << 0 << " "
                << pop.GetBestLength() << " "
                << pop.GetMeanBestHalf() << endl;
    }

    // Ciclo principale dell'algoritmo genetico
    for(int generation = 1; generation <= n_generations; generation++) {

        // Creo una nuova generazione tramite selezione, crossover e mutazioni
        pop.NewGeneration(rnd, map,
                          p_crossover,
                          p_swap,
                          p_inversion,
                          p_shift,
                          p_block_swap);

        // Se richiesto, ogni n_migr generazioni faccio migrare i migliori individui
        if(migration == 1 && n_migr > 0 && generation % n_migr == 0) {

            MigrateBestIndividuals(pop, map, rnd, rank, size);
        }

        // Salvo l'andamento del rank 0 a ogni generazione
        if(rank == 0) {

            history << generation << " "
                    << pop.GetBestLength() << " "
                    << pop.GetMeanBestHalf() << endl;
        }
    }

    // Chiudo il file history del rank 0
    if(rank == 0) {
        history.close();
    }

    // Preparo i dati finali del rank corrente
    double local_summary[2];

    local_summary[0] = pop.GetBestLength();
    local_summary[1] = pop.GetMeanBestHalf();

    // Vettore che conterrà i risultati raccolti da tutti i rank
    vector<double> global_summary(2 * size);

    // Raccolgo best length e best half di tutti i rank sul rank 0
    MPI_Gather(local_summary, 2, MPI_DOUBLE,
            global_summary.data(), 2, MPI_DOUBLE,
            0, MPI_COMM_WORLD);

    // Il rank 0 scrive il file riassuntivo finale
    if(rank == 0) {

        ofstream summary(summary_file_run);

        if(!summary.is_open()) {

            cerr << "ERROR rank " << rank
                << ": unable to open file "
                << summary_file_run
                << endl;

            MPI_Finalize();
            return 1;
        }

        summary << "# rank best_length mean_best_half" << endl;
        summary << fixed << setprecision(10);

        for(int i = 0; i < size; i++) {

            summary << i << " "
                    << global_summary[2 * i] << " "
                    << global_summary[2 * i + 1] << endl;
        }

        summary.close();
    }

    // Salvo il miglior cammino finale trovato da questo rank
    pop.SaveBestPath(best_path_file_rank);

    // Salvo le coordinate del miglior cammino finale trovato da questo rank
    pop.SaveBestPathCoordinates(best_coord_file_rank, map);

    // Stampo a terminale il risultato finale di ogni rank
    cout << "Rank " << rank
         << " / " << size
         << " - Best length = "
         << pop.GetBestLength()
         << endl;

    // Salvo il seed solo dal rank 0 per evitare scritture contemporanee
    if(rank == 0) {
        rnd.SaveSeed();
    }

    // Chiudo l'ambiente MPI
    MPI_Finalize();

    return 0;
}   