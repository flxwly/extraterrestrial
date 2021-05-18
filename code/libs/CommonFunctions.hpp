#ifndef COMMONFUNCTIONS_HPP
#define COMMONFUNCTIONS_HPP

#include <iostream>
#include <cmath>
#include <vector>

double toRadians(double n);

double toDegrees(double n);

double angularToLinear(double r, double revPerS);

bool isInRange(double value, double min, double max);

#endif // !COMMONFUNCTIONS_HPP