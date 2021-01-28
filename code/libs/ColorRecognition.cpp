#include "ColorRecognition.hpp"

bool isRedLeft() { return satL >= 75 && lumL >= 30 && isInRange(hueL, 340, 361); } // check
bool isRedRight() { return satR >= 75 && lumR >= 30 && isInRange(hueR, 340, 361); }

bool isCyanLeft() { return satL >= 75 && lumL >= 30 && isInRange(hueL, 175, 185); }     // check
bool isCyanRight() { return satR >= 75 && lumR >= 30 && isInRange(hueR, 175, 185); }

bool isBlackLeft() { return lumL <= 20; }   // check
bool isBlackRight() { return lumR <= 20; }

bool isYellowLeft() { return satL >= 75 && lumL >= 30 && isInRange(hueL, 58, 80); } // check
bool isYellowRight() { return satR >= 75 && lumR >= 30 && isInRange(hueR, 58, 80); }    // check

bool isOrangeLeft() { return satL >= 75 && lumL >= 30 && isInRange(hueL, 35, 50); } // check
bool isOrangeRight() { return satR >= 75 && lumR >= 30 && isInRange(hueR, 35, 50); }

bool isSuperObjLeft() { return satL >= 80 && lumL >= 30 && isInRange(hueL, 290, 310); }
bool isSuperObjRight() { return satR >= 80 && lumR >= 30 && isInRange(hueR, 290, 310); }

bool isWaterLeft() { return satL >= 75 && lumL >= 30 && isInRange(hueL, 175, 185); }
bool isWaterRight() { return satR >= 75 && lumR >= 30 && isInRange(hueR, 175, 185); }

bool isSwampLeft() { return satL <= 40 && isInRange(lumL, 40, 60) && isInRange(hueL, 200, 240); }
bool isSwampRight() { return satR <= 40 && isInRange(lumR, 40, 60) && isInRange(hueR, 200, 240); }

bool isRed() { return isRedLeft() || isRedRight(); }
bool isCyan() { return isCyanLeft() || isCyanRight(); }
bool isBlack() { return isBlackLeft() || isBlackRight(); }
bool isYellow() { return isYellowLeft() || isYellowRight(); }
bool isOrange() { return isOrangeLeft() && isOrangeRight(); }
bool isSwamp() { return isSwampLeft() || isSwampRight(); }
bool isSuperObj() { return isSuperObjLeft() || isSuperObjRight(); }
bool isColor() { return isBlack() || isRed(), isCyan(), isSuperObj(); }


int rgb2h(int R, int G, int B) {

    // map the r, g, b values to a minimum of 0 and a max of 1
    double r = static_cast<double>(R) / 255;
    double g = static_cast<double>(G) / 255;
    double b = static_cast<double>(B) / 255;

    double cmax = max(r, max(g, b));
    double cmin = min(r, min(g, b));

    double hue = 0;
    double c = cmax - cmin; // chroma
    if (c == 0) return 0;
    if (cmax == r) {
        hue = (g - b) / c;

    } else if (cmax == g) {
        hue = (b - r) / c + 2;

    } else if (cmax == b) {
        hue = (r - g) / c + 4;

    }
    return static_cast<int>(round((hue * 60 > 0) ? hue * 60 : hue * 60 + 360));
}

int rgb2s(int R, int G, int B) {

    // map the r, g, b values to a minimum of 0 and a max of 1
    double r = static_cast<double>(R) / 255;
    double g = static_cast<double>(G) / 255;
    double b = static_cast<double>(B) / 255;

    // get the min and max of r, g, b
    double cmax = max(r, max(g, b));
    double cmin = min(r, min(g, b));

    // chroma is the difference between max and min.
    double chroma = cmax - cmin;

    return (cmax != 0) ? static_cast<int>(round(chroma / cmax * 100)) : 0;
}

int rgb2l(int R, int G, int B) {

    // mapping rgb values to 0-1
    double r = static_cast<double>(R) / 255;
    double g = static_cast<double>(G) / 255;
    double b = static_cast<double>(B) / 255;

    // get the min and max of r, g, b
    double cmax = max(r, max(g, b));
    double cmin = min(r, min(g, b));

    // lightness is the average of the largest and smallest color components
    return static_cast<int>(round((cmax + cmin)) / 0.02);
}