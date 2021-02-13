#ifndef COLORRECOGNITION_HPP
#define COLORRECOGNITION_HPP

#include "CoSpaceFunctions.hpp"
#include "CommonFunctions.hpp"

struct RGBColor {
    float r = 0;
    float g = 0;
    float b = 0;
};

struct HSLColor {
    float h = 0;
    float s = 0;
    float l = 0;
};

bool isRed(HSLColor c);
bool isCyan(HSLColor c);
bool isBlack(HSLColor c);
bool isYellow(HSLColor c);
bool isOrange(HSLColor c);
bool isSwamp(HSLColor c);
bool isColor(HSLColor c);
bool isSuperObj(HSLColor c);

float rgb2h(HSLColor c);
float rgb2s(HSLColor c);
float rgb2l(HSLColor c);


HSLColor rgb2hsl(RGBColor color);

#endif //COLORRECOGNITION_HPP