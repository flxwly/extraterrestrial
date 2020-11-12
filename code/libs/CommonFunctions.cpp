#include "CommonFunctions.hpp"

#define _USE_MATH_DEFINES

double toRadians(double d) {
    return d * M_PI / 180;
}

double toDegrees(double r) {
    return r * 180 / M_PI;
}

double reverseAngle(double angle) {
    return fmod((angle + ((angle > 0) ? -360 : 360)), 360);
}


bool isInRange(double value, double min, double max) {
    return value >= min && value <= max;
}

double max(double n, double m) {
    return (n > m) ? n : m;
}

double min(double n, double m) {
    return (n < m) ? n : m;
}

double map(double Mmin, double Mmax, double Imin, double Imax, double input) {
    if (Mmin > Mmax) {
        double temp = Mmin;
        Mmin = Mmax;
        Mmax = temp;
    }
    if (Imin > Imax) {
        double temp = Imin;
        Imin = Imax;
        Imax = temp;
    }
    double output = Mmin + ((Mmax - Mmin) / (Imax - Imin)) * (input - Imin);
    return output;
}


// Anything to String

template<class T1, class T2>
std::string str(T1 x, T2 y) {
	return std::to_string(x) + " | " + std::to_string(y);
}

std::string str(PVector p) {
    return str(p.x, p.y);
}