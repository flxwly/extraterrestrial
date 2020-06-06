#ifndef COMMONFUNCTIONS_HPP
#define COMMONFUNCTIONS_HPP

#include <iostream>

void DEBUG_MESSAGE(const std::string &message, double level);
int dround(double x);
double toRadians(int n);
int toDegrees(double n);
int reverseAngle(int angle);

bool isInRange(double value, double min, double max);
int max (int n, int m);
double max (double n, double m);
int min (int n, int m);
double min (double n, double m);
double dist(int x1, int x2, int y1, int y2);
double dist(double x1, double x2, double y1, double y2);
double map(double Mmin, double Mmax, double Imin, double Imax, double input);

int vector2Angle(int x, int y);
int vector2Angle(double x, double y);
int vector2Angle(std::pair<int, int> p);
int vector2Angle(std::pair<double, double> p);

#endif // !COMMONFUNCTIONS_HPP