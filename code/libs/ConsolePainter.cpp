#include "ConsolePainter.hpp"


ConsolePainter::ConsolePainter(const std::string &title) : coordBufSize{bufferWidth, bufferHeight}, coordBufCoord{0, 0},
                                                      srcWriteRect{0, 0, bufferWidth - 1, bufferHeight - 1} {

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    hNewScreenBuffer = CreateConsoleScreenBuffer(
            GENERIC_WRITE,
            0,
            NULL,                    // default security attributes
            CONSOLE_TEXTMODE_BUFFER,                // must be TEXTMODE
            NULL);                   // reserved; must be NULL

    if (hNewScreenBuffer == INVALID_HANDLE_VALUE) {
        printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
        return;
    }

    for (auto &i : chiBuffer) {
        i.Char.UnicodeChar = NULL_CHAR;
        i.Attributes = FOREGROUND_BLUE;
    }

    SetConsoleTitle(title.c_str());
    SetConsoleScreenBufferSize(hNewScreenBuffer, coordBufSize);
    SetConsoleWindowInfo(hNewScreenBuffer, TRUE, &srcWriteRect);
}

void ConsolePainter::paintBuffer(std::string data, int width, int x, int y) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < data.length() / width; ++j) {
            if (data.at(i + width * j) != NULL_CHAR and x + i < bufferWidth and j + y < bufferHeight) {
                chiBuffer[x + i + (y + j) * bufferWidth].Char.UnicodeChar = data[i + j * width];
                chiBuffer[x + i + (y + j) * bufferWidth].Attributes = BACKGROUND_RED;
            }
        }
    }
}

void ConsolePainter::printToConsole() {
    if ((std::chrono::steady_clock::now() - lastPrint).count() < maxPrintInterval) {
        return;
    }


    WriteConsoleOutput(
            hNewScreenBuffer, // screen buffer to write to
            chiBuffer,        // buffer to copy from
            coordBufSize,     // col-row size of chiBuffer
            coordBufCoord,    // top left src cell in chiBuffer
            &srcWriteRect);

    if (!SetConsoleActiveScreenBuffer(hNewScreenBuffer)) {
        printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
        return;
    }
}

void ConsolePainter::clear(int x, int y) {
    x = min(bufferWidth, x);
    y = min(bufferHeight, y);
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            chiBuffer[i + j * bufferWidth].Char.UnicodeChar = '0';
            chiBuffer[i + j * bufferWidth].Attributes = FOREGROUND_GREEN;
        }
    }
}

void ConsolePainter::paintRectangle(int topLeftX, int topLeftY, int width, int height, CHAR_INFO charInfo) {
    topLeftX = max(topLeftX, 0);
    topLeftY = max(topLeftY, 0);
    width = min(topLeftX + width, bufferWidth);
    height = min(topLeftY + height, bufferHeight);

    for (int i = topLeftX; i < width; ++i) {
        for (int j = topLeftY; j < height; ++j) {
            chiBuffer[i + j * bufferWidth] = charInfo;
        }
    }
}

void ConsolePainter::paintCircle(int centerX, int centerY, int radius, CHAR_INFO charInfo) {
    int width = min(centerX + radius, bufferWidth);
    int height = min(centerY + radius, bufferHeight);

    for (int i = max(centerX - radius, 0); i < width; ++i) {
        for (int j = max(centerY - radius, 0); j < height; ++j) {
            if ((i - centerX) * (i - centerX) + (j - centerY) * (j - centerY) < radius * radius) {
                chiBuffer[i + j * bufferWidth] = charInfo;
            }
        }
    }
}

void ConsolePainter::paintConvexPolygon(std::vector<std::pair<int, int>> vertices, CHAR_INFO charInfo) {
    int minY = bufferHeight;
    int maxY = 0;

    for (auto vertex: vertices) {
        if (maxY < vertex.second) {
            maxY = vertex.second;
        } else if (minY > vertex.second) {
            minY = vertex.second;
        }
    }
    minY = max(minY - 1, 0);
    maxY = min(maxY + 1, bufferHeight);
    vertices.push_back(vertices.front());

    std::pair<int, int> p1(0, minY);
    std::pair<int, int> p2(bufferWidth - 1, minY);
    std::pair<int, int> intersection(-1, -1);
    std::array<std::pair<int, int>, 2> intersections;

    int index = 0;
    for (int j = minY; j < maxY; ++j) {
        p1.second = j;
        p2.second = j;
        for (int k = 0; k < vertices.size() - 1 && index < 2; ++k) {
            intersection = lineLineIntersection(p1, p2, vertices.at(k), vertices.at(k + 1));
            if (intersection.first != -1 && intersection.second != -1) {
                intersections[index] = intersection;
                index++;
            }
        }

        switch (index) {
            case 1:
                chiBuffer[intersections[0].first + j * bufferWidth] = charInfo;
                break;
            case 2:
                for (int i = min(intersections[0].first, intersections[1].first);
                     i < max(intersections[0].first, intersections[1].first); ++i) {
                    chiBuffer[i + j * bufferWidth] = charInfo;
                }
                break;
            default:
                break;
        }
        index = 0;
    }
}

std::pair<int, int>
ConsolePainter::lineLineIntersection(std::pair<int, int> p1, std::pair<int, int> p2, std::pair<int, int> p3, std::pair<int, int> p4) {
    // Store the values for fast access and easy
// equations-to-code conversion
    float x1 = p1.first, x2 = p2.first, x3 = p3.first, x4 = p4.first;
    float y1 = p1.second, y2 = p2.second, y3 = p3.second, y4 = p4.second;

    float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
// If d is zero, there is no intersection
    if (d == 0) return {-1, -1};

// Get the x and y
    float pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
    float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
    float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

// Check if the x and y coordinates are within both lines
    if (x < min(x1, x2) || x > max(x1, x2) ||
        x < min(x3, x4) || x > max(x3, x4))
        return {-1, -1};
    if (y < min(y1, y2) || y > max(y1, y2) ||
        y < min(y3, y4) || y > max(y3, y4))
        return {-1, -1};

// Return the point of intersection
    return {round(x), round(y)};
}