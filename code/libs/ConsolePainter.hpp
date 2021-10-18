#ifndef EXTRATERRESTRIAL_CONSOLEPAINTER_HPP
#define EXTRATERRESTRIAL_CONSOLEPAINTER_HPP

#include <windows.h>
#include <vector>
#include <list>
#include <thread>
#include <chrono>
#include "PPSettings.hpp"
#include "PVector.hpp"


#define NULL_CHAR '0'
#define bufferWidth 200
#define bufferHeight 200
#define maxPrintInterval 100

#define UnicodeChar(_CHAR_, _ATTRIBUTE_) {(_CHAR_), (_ATTRIBUTE_)}

class ConsolePainter {

    HANDLE hStdout, hNewScreenBuffer;
    CHAR_INFO chiBuffer[bufferHeight * bufferWidth];
    SMALL_RECT srcWriteRect;
    COORD coordBufSize;
    COORD coordBufCoord;

    std::chrono::time_point<std::chrono::steady_clock> lastPrint;

public:
    explicit ConsolePainter(const std::string& title = "Console");

    void clear(int x = bufferWidth, int y = bufferHeight);
    void paintRectangle(int topLeftX, int topLeftY, int width, int height, CHAR_INFO charInfo);
    void paintCircle(int centerX, int centerY, int radius, CHAR_INFO charInfo);
    void paintConvexPolygon(std::vector<std::pair<int, int>> vertices, CHAR_INFO charInfo);

    void paintBuffer(std::string data, int width, int x, int y);

    void printToConsole();

private:
    static std::pair<int, int> lineLineIntersection(std::pair<int, int> p1, std::pair<int, int> p2, std::pair<int, int> p3, std::pair<int, int> p4);
};

using namespace std;

#endif //EXTRATERRESTRIAL_CONSOLEPAINTER_HPP
