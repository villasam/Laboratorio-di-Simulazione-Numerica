#ifndef __CITY_H__
#define __CITY_H__

class City {

private:

    double _x;
    double _y;

public:

    City();
    City(double x, double y);

    double GetX() const;
    double GetY() const;

    void SetX(double x);
    void SetY(double y);
};

#endif