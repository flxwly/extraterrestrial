#ifndef EXTRATERRESTRIAL_CONSOLEPAINTER_HPP
#define EXTRATERRESTRIAL_CONSOLEPAINTER_HPP

#include <windows.h>
#include <vector>
#include <list>
#include <thread>
#include <chrono>
#include "PPSettings.hpp"
#include "PVector.hpp"


#define NULL_CHAR ' '
#define initialBufferWidth 200
#define initialBufferHeight 150
#define maxPrintInterval 100

#define UnicodeChar(_CHAR_, _ATTRIBUTE_) CHAR_INFO{static_cast<WCHAR>(_CHAR_), (_ATTRIBUTE_)}

class ConsolePainter {

    HANDLE hStdout, hScreenBuffer;
    CHAR_INFO chiBuffer[initialBufferWidth * initialBufferHeight];
    SMALL_RECT srcWriteRect;
    COORD coordBufSize;
    int bufWidth, bufHeight;
    COORD coordBufCoord;

    std::chrono::time_point<std::chrono::steady_clock> lastPrint;

public:
    explicit ConsolePainter(const std::string& title = "Console");

    // TODO: Might implement it later
    void resize(int width, int height);


    void clear();
    void paintPixel(int x, int y, CHAR_INFO charInfo);
    void paintRectangle(int topLeftX, int topLeftY, int width, int height, CHAR_INFO charInfo);
    void paintCircle(int centerX, int centerY, int radius, CHAR_INFO charInfo);
    void paintConvexPolygon(std::vector<std::pair<int, int>> vertices, CHAR_INFO charInfo);
    void paintBuffer(CHAR_INFO data[], COORD bufSize, int x, int y);


    void printToConsole();

private:
    static std::pair<int, int> lineLineIntersection(std::pair<int, int> p1, std::pair<int, int> p2, std::pair<int, int> p3, std::pair<int, int> p4);
};

using namespace std;

#endif //EXTRATERRESTRIAL_CONSOLEPAINTER_HPP
