#!/bin/bash

# Questo script esegue uno scan della temperatura iniziale per la dinamica molecolare.
# L'obiettivo è capire quale temperatura iniziale T_input produce, dopo l'evoluzione,
# una temperatura media vicina al valore desiderato, nel nostro caso circa T = 1.1.

# Se un comando fallisce, lo script si interrompe subito.
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

SOURCE_DIR="$ROOT_DIR/SOURCE"

cd "$SOURCE_DIR"

for T in 1.93 1.935 1.94 1.945 1.95 1.955 1.96 1.965 1.97 1.975 1.98 1.985 1.99 1.995 2.00 2.005 2.01 2.015 2.02 2.025 2.03; do

    # T=1.94  ->  RUN=temp_tuning/T1_94
    RUN=temp_tuning/T${T//./_}

    # Cancello eventuali vecchi input/output della stessa run.
    rm -rf ../INPUT/runs/$RUN ../OUTPUT/$RUN

    # Creo la cartella di input della run.
    mkdir -p ../INPUT/runs/$RUN/CONFIG

    # Creo la cartella di output in cui il simulatore scriverà i risultati.
    mkdir -p ../OUTPUT/$RUN
    mkdir -p ../OUTPUT/$RUN/CONFIG

    # Copio i file necessari al generatore di numeri casuali e config iniziale.
    cp ../INPUT/CONFIG/Primes ../INPUT/runs/$RUN/
    cp ../INPUT/CONFIG/seed.in ../INPUT/runs/$RUN/
    cp ../INPUT/CONFIG/config.fcc ../INPUT/runs/$RUN/CONFIG/config.xyz

    # SIMULATION_TYPE 0        -> dinamica molecolare
    # RESTART 0               -> partenza da configurazione iniziale, non da restart
    # CONFIG_MODE STANDARD    -> lettura standard della configurazione
    # INIT_VELOCITIES MAXWELL -> inizializzazione delle velocità da Maxwell-Boltzmann
    # TEMP $T                 -> temperatura iniziale testata in questo ciclo
    # NPART 108               -> numero di particelle
    # RHO 0.8                 -> densità del liquido Lennard-Jones
    # R_CUT 2.5               -> raggio di cut-off del potenziale
    # DELTA 0.001             -> passo temporale della dinamica molecolare
    # NBLOCKS 5000, NSTEPS 1  -> salvo una misura per blocco, ottenendo una serie lunga

    printf "SIMULATION_TYPE 0\nRESTART 0\nCONFIG_MODE STANDARD\nINIT_VELOCITIES MAXWELL\nTEMP $T\nNPART 108\nRHO 0.8\nR_CUT 2.5\nDELTA 0.001\nNBLOCKS 5000\nNSTEPS 1\nENDINPUT\n" > ../INPUT/runs/$RUN/input.dat

    # Creo il file properties.dat, cioè la lista delle osservabili da misurare.
    printf "KINETIC_ENERGY\nTEMPERATURE\nENDPROPERTIES\n" > ../INPUT/runs/$RUN/properties.dat

    # Stampo il nome della run corrente, così posso seguire l'avanzamento dello script.
    echo "Running $RUN"

    # Lancio il simulatore passando il nome della run.
    ./simulator.exe $RUN
done