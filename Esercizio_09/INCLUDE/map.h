#ifndef __MAP_H__
#define __MAP_H__

#include <vector>
#include <string>
#include "city.h"
#include "random.h"

using namespace std;

class Map {

private:

    vector<City> _cities;

public:

    // costruttori
    Map();

    // file IO
    void LoadFromFile(string filename);

    // informazioni
    int GetNumberCities() const;

    City GetCity(int index) const;

    // geometria
    double Distance(int i, int j) const;

    // debug
    void Print() const;
};

#endif