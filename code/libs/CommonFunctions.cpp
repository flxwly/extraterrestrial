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


// Anything to String

template<class T1, class T2>
std::string str(T1 x, T2 y) {
	return std::to_string(x) + " | " + std::to_string(y);
}

double angularToLinear(double r, double revPerS) {
    return r * 2 * M_PI * revPerS;
}
