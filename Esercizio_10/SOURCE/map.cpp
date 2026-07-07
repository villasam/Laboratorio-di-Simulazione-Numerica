#include <iostream>
#include <fstream>
#include <cmath>

#include "map.h"

using namespace std;

Map::Map() {}

// LETTURA FILE
void Map::LoadFromFile(string filename) {

    ifstream fin(filename);

    if(!fin.is_open()) {
        cerr << "ERROR: unable to open file " << filename << endl;
        return;
    }

    _cities.clear();

    double x, y;
    int index;

    while(fin >> index >> x >> y) {

        _cities.push_back(City(x, y));
    }

    fin.close();
}

// INFO
int Map::GetNumberCities() const {
    return _cities.size();
}


City Map::GetCity(int index) const {
    return _cities[index];
}

// DISTANZA L1
double Map::Distance(int i, int j) const {

    double dx =_cities[i].GetX() - _cities[j].GetX();

    double dy = _cities[i].GetY() - _cities[j].GetY();

    return sqrt(dx*dx + dy*dy);
}


// DEBUG
void Map::Print() const {

    for(int i = 0; i < this->GetNumberCities(); i++) {
        cout << i << " " << _cities[i].GetX() << " " << _cities[i].GetY() << endl;
    }
}