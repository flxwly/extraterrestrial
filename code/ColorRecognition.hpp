#ifndef COLORRECOGNITION_HPP
#define COLORRECOGNITION_HPP
bool isRedLeft();
bool isRedRight();

bool isCyanLeft();
bool isCyanRight();

bool isBlackLeft();
bool isBlackRight();

bool isYellowLeft();
bool isYellowRight();

bool isOrangeLeft();
bool isOrangeRight();

bool isSuperObjLeft();
bool isSuperObjRight();

bool isWaterLeft();
bool isWaterRight();

bool isSwampRight();
bool isSwampLeft();

bool isRed();
bool isCyan();
bool isBlack();
bool isYellow();
bool isOrange();
bool isSwamp();
bool isColor();
bool isSuperObj();

int rgb2h(int R, int G, int B);
int rgb2s(int R, int G, int B);
int rgb2l(int R, int G, int B);

#endif //COLORRECOGNITION_HPP