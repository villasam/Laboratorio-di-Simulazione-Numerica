#include <iostream>
#include <algorithm>
#include <cmath>

#include "individual.h"

using namespace std;

// COSTRUTTORE
Individual::Individual() {
    _length = 0.0;
}

// COSTRUTTORE ORDINATO
Individual::Individual(int n_cities) {

    _path.resize(n_cities);

    for(int i = 0; i < n_cities; i++) {
        _path[i] = i;
    }

    _length = 0.0;
}

// INIZIALIZZAZIONE CASUALE
void Individual::RandomInitialize(Random& rnd) {

    int n_cities = _path.size();

    // La città 0 resta fissa in prima posizione.
    // Mescoliamo solo le città da 1 a n_cities - 1.
    for(int i = 1; i < n_cities; i++) {

        int j = int(rnd.Rannyu(i, n_cities));

        swap(_path[i], _path[j]);
    }
}

// CALCOLO LUNGHEZZA DEL PERCORSO
void Individual::ComputeLength(const Map& map) {

    _length = 0.0;

    int n_cities = _path.size();

    for(int i = 0; i < n_cities - 1; i++) {
        _length += map.Distance(_path[i], _path[i + 1]);
    }

    // chiusura del percorso: ultima città -> prima città
    _length += map.Distance(_path[n_cities - 1], _path[0]);
}


// CONTROLLO VALIDITA'
bool Individual::Check() const {

    int n_cities = _path.size();

    if(n_cities == 0) {
        return false;
    }

    if(_path[0] != 0) {
        return false;
    }

    vector<int> counter(n_cities, 0);

    for(int i = 0; i < n_cities; i++) {

        if(_path[i] < 0 || _path[i] >= n_cities) {
            return false;
        }

        counter[_path[i]]++;
    }

    for(int i = 0; i < n_cities; i++) {
        if(counter[i] != 1) {
            return false;
        }
    }

    return true;
}

// MUTAZIONE: SCAMBIO DI DUE CITTA'
void Individual::SwapMutation(Random& rnd) {

    int n_cities = _path.size();

    int i = int(rnd.Rannyu(1, n_cities));
    int j = int(rnd.Rannyu(1, n_cities));

    while(j == i) {
        j = int(rnd.Rannyu(1, n_cities));
    }

    swap(_path[i], _path[j]);
}

// MUTAZIONE: INVERSIONE DI UN TRATTO
void Individual::InversionMutation(Random& rnd) {

    int n_cities = _path.size();

    int i = int(rnd.Rannyu(1, n_cities));
    int j = int(rnd.Rannyu(1, n_cities));

    if(i > j) {
        swap(i, j);
    }

    reverse(_path.begin() + i, _path.begin() + j + 1);
}

// MUTAZIONE : shifta un blocco di città
void Individual::ShiftMutation(Random& rnd) {

    int n_cities = _path.size();

    // La città 0 resta fissa, quindi servono abbastanza città mobili
    if(n_cities < 4) {
        return;
    }

    // Lunghezza massima ragionevole del blocco
    int max_block_size = (n_cities - 1) / 3;

    if(max_block_size < 1) {
        return;
    }

    // Scelgo casualmente la lunghezza del blocco
    int block_size = int(rnd.Rannyu(1, max_block_size + 1));

    // Scelgo la posizione iniziale del blocco, escludendo la città 0
    int start = int(rnd.Rannyu(1, n_cities - block_size + 1));

    // Copio il blocco da spostare
    vector<int> block(_path.begin() + start,
                      _path.begin() + start + block_size);

    // Rimuovo il blocco dalla posizione originale
    _path.erase(_path.begin() + start,
                _path.begin() + start + block_size);

    // Dimensione del cammino dopo la rimozione
    int reduced_size = _path.size();

    // Scelgo la nuova posizione di inserimento, sempre dopo la città 0
    int new_start = int(rnd.Rannyu(1, reduced_size + 1));

    // Evito di reinserire il blocco nella stessa posizione
    while(new_start == start) {
        new_start = int(rnd.Rannyu(1, reduced_size + 1));
    }

    // Reinserisco il blocco nella nuova posizione
    _path.insert(_path.begin() + new_start,
                 block.begin(),
                 block.end());
}

void Individual::BlockSwapMutation(Random& rnd) {

    int n_cities = _path.size();

    // lunghezza massima ragionevole del blocco
    int max_block_size = n_cities / 4;

    if(max_block_size < 1) {
        return;
    }

    // lunghezza del blocco
    int block_size = int(rnd.Rannyu(1, max_block_size + 1));

    // posizione iniziale del primo blocco
    int start1 = int(rnd.Rannyu(1, n_cities - block_size));

    // posizione iniziale del secondo blocco
    int start2 = int(rnd.Rannyu(1, n_cities - block_size));

    // evita sovrapposizione dei blocchi
    while(abs(start2 - start1) < block_size) {

        start2 = int(rnd.Rannyu(1, n_cities - block_size));
    }

    // scambia i due blocchi
    for(int i = 0; i < block_size; i++) {

        swap(_path[start1 + i], _path[start2 + i]);
    }
}

// GETTER
int Individual::GetCityIndex(int i) const {
    return _path[i];
}

int Individual::GetNumberCities() const {
    return _path.size();
}

double Individual::GetLength() const {
    return _length;
}

vector<int> Individual::GetPath() const {
    return _path;
}

// SETTER
void Individual::SetPath(const vector<int>& path) {
    _path = path;
}

// DEBUG
void Individual::Print() const {

    for(int i = 0; i < int(_path.size()); i++) {
        cout << _path[i] << " ";
    }

    cout << "   length = " << _length << endl;
}