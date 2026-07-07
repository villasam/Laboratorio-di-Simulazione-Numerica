#!/bin/bash

# Questo script esegue automaticamente tutte le simulazioni richiesteper l'esercizio 06.

# set -e fa terminare immediatamente lo script se un comando dà errore.
set -e

# Mi sposto automaticamente nella cartella principale Ex_06.
# "$(dirname "$0")" indica la cartella in cui si trova questo script,
# cioè scripts/.
# Con /.. risalgo alla cartella principale dell'esercizio.
# In questo modo lo script funziona anche se viene lanciato da una posizione diversa.
cd "$(dirname "$0")/.."

# Lista delle temperature da simulare.
# Sono 16 valori: 0.50, 0.60, ..., 2.00.
TEMPERATURES="0.50 0.60 0.70 0.80 0.90 1.00 1.10 1.20 1.30 1.40 1.50 1.60 1.70 1.80 1.90 2.00"

# Ciclo sulle temperature.
# A ogni iterazione T assume uno dei valori elencati sopra.
for T in $TEMPERATURES
do

    # Per ogni temperatura eseguo quattro tipi di simulazione:
    # metro_h0     -> Metropolis, h = 0
    # gibbs_h0     -> Gibbs, h = 0
    # metro_h002   -> Metropolis, h = 0.02
    # gibbs_h002   -> Gibbs, h = 0.02
    #
    # Le simulazioni con h = 0 servono per energia, calore specifico e suscettività.
    # Le simulazioni con h = 0.02 servono per la magnetizzazione.
    for RUN in metro_h0 gibbs_h0 metro_h002 gibbs_h002
    do

        # Costruisco il nome completo della run.
        RUN_NAME=${RUN}_T${T}

        # Stampo a schermo quale simulazione sta partendo.
        echo "Running $RUN_NAME"

        # Rimuovo eventuali vecchie cartelle di input/output per questa run.
        rm -rf INPUT/runs/$RUN_NAME
        rm -rf OUTPUT/$RUN_NAME

        # Creo la cartella di input specifica per questa run.
        mkdir -p INPUT/runs/$RUN_NAME

        # Creo la cartella di output specifica per questa run.
        mkdir -p OUTPUT/$RUN_NAME

        # Copio i file comuni del generatore di numeri casuali.
        cp INPUT/common/Primes INPUT/runs/$RUN_NAME/Primes
        cp INPUT/common/seed.in INPUT/runs/$RUN_NAME/seed.in

        # Ora scelgo quale template di input copiare.
        if [ "$RUN" = "metro_h0" ]; then
            cp INPUT/templates/input_metro_h0.dat INPUT/runs/$RUN_NAME/input.dat
            cp INPUT/templates/properties_h0.dat INPUT/runs/$RUN_NAME/properties.dat
        fi

        if [ "$RUN" = "gibbs_h0" ]; then
            cp INPUT/templates/input_gibbs_h0.dat INPUT/runs/$RUN_NAME/input.dat
            cp INPUT/templates/properties_h0.dat INPUT/runs/$RUN_NAME/properties.dat
        fi

        if [ "$RUN" = "metro_h002" ]; then
            cp INPUT/templates/input_metro_h002.dat INPUT/runs/$RUN_NAME/input.dat
            cp INPUT/templates/properties_h002.dat INPUT/runs/$RUN_NAME/properties.dat
        fi

        if [ "$RUN" = "gibbs_h002" ]; then
            cp INPUT/templates/input_gibbs_h002.dat INPUT/runs/$RUN_NAME/input.dat
            cp INPUT/templates/properties_h002.dat INPUT/runs/$RUN_NAME/properties.dat
        fi

        # sed -i modifica il file direttamente: sostituisce l'intera riga che inizia con TEMP
        # con una nuova riga contenente la temperatura corrente $T.
        sed -i "s/^TEMP.*/TEMP                   $T/" INPUT/runs/$RUN_NAME/input.dat

        # Lancio il simulatore.
        cd SOURCE
        ./simulator.exe $RUN_NAME

        cd ..
    done
done

echo "Temperature scan completato."