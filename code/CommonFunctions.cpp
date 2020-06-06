#include "CommonFunctions.hpp"
#include <cmath>

// 0 std; 1 Movement; 2 Pathfinding; 3 Sensor Values;

#define DEBUGLEVEL -99

#define _USE_MATH_DEFINES

void DEBUG_MESSAGE(const std::string &message, double level) {
    if (DEBUGLEVEL == level) {
        std::cout << message;
    }
}

int dround(double x) {
    return static_cast<int> (x + 0.5);
}

double toRadians(int n) {
    return n * M_PI / 180;
}

int toDegrees(double n) {
    return dround(n * 180 / M_PI);
}
int reverseAngle(int angle) {
    angle += (angle > 0) ? -360 : 360;
    return angle % 360;
}


bool isInRange(double value, double min, double max) {
    return value >= min && value <= max;
}

int max(int n, int m) {
    return (n > m) ? n : m;
}

double max(double n, double m) {
    return (n > m) ? n : m;
}

int min(int n, int m) {
    return (n < m) ? n : m;
}

double min(double n, double m) {
    return (n < m) ? n : m;
}

double dist(int x1, int x2, int y1, int y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
double dist(double x1, double x2, double y1, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

int vector2Angle(int x, int y) {

    if (x == 0)
        return (y >= 0)? 0 : 180;
    else if (y == 0)
        return (x > 0)? 90 : 270;

    int angle = toDegrees(atan(static_cast<double>(y)/x));

    // bottom left (90 - 180)
    if (x < 0 && y < 0)
        // angle is positive (180 location)
        angle = 0 + 90;
        // top left (0 - 90)
    else if (x < 0)
        // angle is negative (90 positive) + (0 location)
        angle += 90 + 0;
        // bottom right (180 - 270)
    else if (y < 0)
        // angle is negative (90 positive) + (180 location)
        angle += 90 + 180 ;
        // top right (270 - 360)
    else {
        angle += 270;
        // angle is positive
    }
    return angle;
}

int vector2Angle(std::pair<int, int> p) {
    return vector2Angle(p.first, p.second);
}

int vector2Angle(double x, double y) {

    if (x == 0)
        return (y >= 0)? 0 : 180;
    else if (y == 0)
        return (x > 0)? 90 : 270;

    int angle = toDegrees(atan(y/x));

    // bottom left (90 - 180)
    if (x < 0 && y < 0)
        // angle is positive (180 location)
        angle = 0 + 90;
    // top left (0 - 90)
    else if (x < 0)
        // angle is negative (90 positive) + (0 location)
        angle += 90 + 0;
    // bottom right (180 - 270)
    else if (y < 0)
        // angle is negative (90 positive) + (180 location)
        angle += 90 + 180 ;
    // top right (270 - 360)
    else {
        angle += 270;
        // angle is positive
    }
    return angle;
}

int vector2Angle(std::pair<double, double> p) {
    return vector2Angle(p.first, p.second);
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
