#ifndef __INDIVIDUAL_H__
#define __INDIVIDUAL_H__

#include <vector>

#include "random.h"
#include "map.h"

using namespace std;

class Individual {

private:

    vector<int> _path;
    double _length;

public:

    Individual();
    Individual(int n_cities);

    void RandomInitialize(Random& rnd);

    void ComputeLength(const Map& map);

    bool Check() const;

    void SwapMutation(Random& rnd);
    void InversionMutation(Random& rnd);
    void ShiftMutation(Random& rnd);
    void BlockSwapMutation(Random& rnd);

    int GetCityIndex(int i) const;
    int GetNumberCities() const;

    double GetLength() const;
    vector<int> GetPath() const;

    void SetPath(const vector<int>& path);

    void Print() const;
};

#endif