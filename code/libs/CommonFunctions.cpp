#include "CommonFunctions.hpp"

#define _USE_MATH_DEFINES

double toRadians(double d) {
    return d * M_PI / 180;
}

double toDegrees(double r) {
    return r * 180 / M_PI;
}

bool isInRange(double value, double min, double max) {
	return min <= value && value <= max;
}

double angularToLinear(double r, double revPerS) {
    return r * 2 * M_PI * revPerS;
}
