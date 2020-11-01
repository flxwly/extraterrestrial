
#ifndef COMMONFUNCTIONS_HPP
#define COMMONFUNCTIONS_HPP

#include <iostream>
#include <cmath>
#include "../MapData.hpp"

double toRadians(double n);

double toDegrees(double n);

double reverseAngle(double angle);

bool isInRange(double value, double min, double max);

double max(double n, double m);

double min(double n, double m);


double map(double Mmin, double Mmax, double Imin, double Imax, double input);

// Anything to String
template<class T1, class T2>
std::string str(T1 x, T2 y);

std::string str(Point p);


#endif // !COMMONFUNCTIONS_HPP