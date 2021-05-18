#include "ColorRecognition.hpp"

bool isRed(HSLColor c) { return c.s >= 75 && c.l >= 30 && isInRange(c.h, 340, 361); }
bool isCyan(HSLColor c) { return c.s >= 75 && c.l >= 30 && isInRange(c.h, 175, 185); }
bool isBlack(HSLColor c) { return c.l <= 20; }
bool isYellow(HSLColor c) { return c.s >= 75 && c.l >= 30 && isInRange(c.h, 58, 80); }
bool isOrange(HSLColor c) { return c.s >= 75 && c.l >= 30 && isInRange(c.h, 35, 50); }
bool isSwamp(HSLColor c) { return c.s <= 60 && isInRange(c.l, 40, 80) && isInRange(c.h, 200, 240); }
bool isSuperObj(HSLColor c) { return c.s >= 80 && c.l >= 30 && isInRange(c.h, 290, 310); }
bool isColor(HSLColor c) { return isBlack(c) || isRed(c), isCyan(c), isSuperObj(c); }

float rgb2h(RGBColor c) {

    // map the r, g, b values to a minimum of 0 and a max of 1
    c.r /= 255;
    c.g /= 255;
    c.b /= 255;

    float cmax = std::max(c.r, std::max(c.g, c.b));
    float cmin = std::min(c.r, std::min(c.g, c.b));

    float hue = 0;
    float chroma = cmax - cmin; // chroma
    if (chroma == 0) {
        return 0;
    } else if (cmax == c.r) {
        hue = (c.g - c.b) / chroma * 60;
    } else if (cmax == c.g) {
        hue = ((c.b - c.r) / chroma + 2) * 60;
    } else if (cmax == c.b) {
        hue = ((c.r - c.g) / chroma + 4) * 60;
    }
    return (hue > 0) ? hue : hue + 360;
}

float rgb2s(RGBColor c) {

    // map the r, g, b values to a minimum of 0 and a max of 1
    c.r /= 255;
    c.g /= 255;
    c.b /= 255;

    float cmax = std::max(c.r, std::max(c.g, c.b));
    float cmin = std::min(c.r, std::min(c.g, c.b));

    // chroma is the difference between max and min.
    float chroma = cmax - cmin;

    return (cmax != 0) ? (chroma / cmax * 100) : 0;
}

float rgb2l(RGBColor c) {

    // map the r, g, b values to a minimum of 0 and a max of 1
    c.r /= 255;
    c.g /= 255;
    c.b /= 255;

    float cmax = std::max(c.r, std::max(c.g, c.b));
    float cmin = std::min(c.r, std::min(c.g, c.b));

    // lightness is the average of the largest and smallest color components
    return (cmax + cmin) / 0.02f;
}

HSLColor rgb2hsl(RGBColor rgb) {

    HSLColor color;

    // map the r, g, b values to a minimum of 0 and a max of 1
    rgb.r /= 255;
    rgb.g /= 255;
    rgb.b /= 255;

    float cmax = std::max(rgb.r, std::max(rgb.g, rgb.b));
    float cmin = std::min(rgb.r, std::min(rgb.g, rgb.b));

    float c = cmax - cmin; // chroma

    color.s = (cmax != 0) ? (c / cmax * 100) : 0;
    color.l = (cmax + cmin) / 0.02f;

    if (c == 0) {
        return color;
    } else if (cmax == rgb.r) {
        color.h = (rgb.g - rgb.b) / c * 60;
    } else if (cmax == rgb.g) {
        color.h = ((rgb.b - rgb.r) / c + 2) * 60;
    } else if (cmax == rgb.b) {
        color.h = ((rgb.r - rgb.g) / c + 4) * 60;
    }

    color.h += (color.h > 0) ? 0 : 360;

    return color;
}
