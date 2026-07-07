#include "city.h"

// Generatore nullo
City::City() {
    _x = 0.0;
    _y = 0.0;
}

// Inizializza con x e y
City::City(double x, double y) {
    _x = x;
    _y = y;
}

double City::GetX() const {
    return _x;
}

double City::GetY() const {
    return _y;
}

void City::SetX(double x) {
    _x = x;
}

void City::SetY(double y) {
    _y = y;
}