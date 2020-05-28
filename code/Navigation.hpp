#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP
#include <vector>

void updatePos(double speed);
int checkSensors(int left, int front, int right);
int invertAngle(int angle);
int oppositeAngle(int angle);

void steerToPoint(int x, int y);

void steerToPoint(std::pair<int, int> p);
//bool isInArea(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int posX, int posY);

#endif // !NAVIGATION_HPP
