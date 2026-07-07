#include <iostream>
#include <vector>
#include <mpi.h>

#include "migration.h"

using namespace std;

void MigrateBestIndividuals(Population& pop,
                            const Map& map,
                            Random& rnd,
                            int rank,
                            int size) {

    // Con un solo processo non ha senso fare migrazione
    if(size <= 1) return;

    // Ordino la popolazione per avere il miglior individuo in posizione 0
    pop.Sort();

    // Vettore che conterrà l'ordine casuale dei continenti
    vector<int> permutation(size);

    if(rank == 0) {

        // Inizializzo la permutazione con i rank ordinati
        for(int i = 0; i < size; i++) {
            permutation[i] = i;
        }

        // Mescolo casualmente i rank con Fisher-Yates shuffle
        for(int i = size - 1; i > 0; i--) {

            int j = int(rnd.Rannyu(0, i + 1));

            int temp = permutation[i];
            permutation[i] = permutation[j];
            permutation[j] = temp;
        }
    }

    // Il rank 0 comunica a tutti la stessa permutazione
    MPI_Bcast(permutation.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

    // Posizione del processo corrente nella permutazione
    int my_position = -1;

    // Cerco dove si trova il mio rank nella permutazione
    for(int i = 0; i < size; i++) {
        if(permutation[i] == rank) {
            my_position = i;
        }
    }

    // Controllo di sicurezza sulla permutazione
    if(my_position == -1) {
        cerr << "ERROR rank " << rank
             << ": rank not found in migration permutation"
             << endl;
        return;
    }

    // Mando il mio best al continente successivo nella permutazione
    int dest = permutation[(my_position + 1) % size];

    // Ricevo un best dal continente precedente nella permutazione
    int source = permutation[(my_position - 1 + size) % size];

    // Estraggo il miglior cammino della popolazione locale
    vector<int> send_path = pop.GetBestPath();

    // Preparo il vettore in cui riceverò il cammino migrante
    vector<int> recv_path(send_path.size());

    // Numero di città, cioè lunghezza del cromosoma
    int n_cities = send_path.size();

    // Tag MPI usato per identificare questi messaggi
    int tag = 10;

    MPI_Request req;
    MPI_Status stat_send;
    MPI_Status stat_recv;

    // Invio non bloccante del mio miglior cammino
    MPI_Isend(send_path.data(), n_cities, MPI_INT, dest, tag, MPI_COMM_WORLD, &req);

    // Ricezione bloccante del cammino proveniente dal continente sorgente
    MPI_Recv(recv_path.data(), n_cities, MPI_INT, source, tag, MPI_COMM_WORLD, &stat_recv);

    // Attendo che l'invio non bloccante sia completato
    MPI_Wait(&req, &stat_send);

    // Inserisco nella popolazione locale il cammino ricevuto
    pop.ReplaceWorstPath(recv_path, map);
}