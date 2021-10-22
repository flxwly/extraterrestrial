#include "ConsolePainter.hpp"


ConsolePainter::ConsolePainter(const std::string &title) : coordBufSize{initialBufferWidth, initialBufferHeight}, coordBufCoord{0, 0},
                                                           srcWriteRect{0, 0, initialBufferWidth - 1, initialBufferHeight - 1} {

    bufWidth = coordBufSize.X;
    bufHeight = coordBufSize.Y;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    hScreenBuffer = CreateConsoleScreenBuffer(
            GENERIC_WRITE,
            0,
            NULL,                    // default security attributes
            CONSOLE_TEXTMODE_BUFFER,                // must be TEXTMODE
            NULL);                   // reserved; must be NULL


    if (hScreenBuffer == INVALID_HANDLE_VALUE) {
        printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
        return;
    }

    for (auto &i : chiBuffer) {
        i.Char.UnicodeChar = NULL_CHAR;
    }

    SetConsoleTitle(title.c_str());
    SetConsoleScreenBufferSize(hScreenBuffer, coordBufSize);
    SetConsoleWindowInfo(hScreenBuffer, TRUE, &srcWriteRect);
}


void ConsolePainter::paintBuffer(CHAR_INFO data[], COORD bufSize, int x, int y) {
    for (int i = 0; i < bufSize.X; ++i) {
        for (int j = 0; j < bufSize.Y; ++j) {
            if (data[i + bufSize.X * j].Char.UnicodeChar != NULL_CHAR and x + i < coordBufSize.X and
                j + y < coordBufSize.X) {
                chiBuffer[x + i + (y + j) * coordBufSize.X] = data[i + j * bufSize.X];
            }
        }
    }

}

void ConsolePainter::printToConsole() {
    if ((std::chrono::steady_clock::now() - lastPrint).count() < maxPrintInterval) {
        return;
    }


    WriteConsoleOutput(
            hScreenBuffer, // screen buffer to write to
            chiBuffer,        // buffer to copy from
            coordBufSize,     // col-row size of chiBuffer
            coordBufCoord,    // top left src cell in chiBuffer
            &srcWriteRect);

    if (!SetConsoleActiveScreenBuffer(hScreenBuffer)) {
        printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
        return;
    }
}

void ConsolePainter::clear() {
    for (int i = 0; i < bufWidth; ++i) {
        for (int j = 0; j < bufHeight; ++j) {
            chiBuffer[i + j * bufWidth].Char.UnicodeChar = ' ';
            chiBuffer[i + j * bufWidth].Attributes = 0;
        }
    }
}


void ConsolePainter::paintPixel(int x, int y, CHAR_INFO charInfo) {
    if (x >= bufWidth || y >= bufHeight || x < 0 || y < 0) {
        throw out_of_range("(" + std::to_string(x) + ", " + std::to_string(y) + ") "
                                                                                "is not in the dimensions of (" +
                           std::to_string(bufWidth) + ", " + std::to_string(bufHeight) + ")");
    }

    chiBuffer[max((x, bufWidth), 0) + max(min(y, bufHeight), 0) * bufWidth] = charInfo;
}

void ConsolePainter::paintRectangle(int topLeftX, int topLeftY, int width, int height, CHAR_INFO charInfo) {
    topLeftX = max(topLeftX, 0);
    topLeftY = max(topLeftY, 0);
    width = min(topLeftX + width, bufWidth);
    height = min(topLeftY + height, bufHeight);

    for (int i = topLeftX; i < width; ++i) {
        for (int j = topLeftY; j < height; ++j) {
            chiBuffer[i + j * bufWidth] = charInfo;
        }
    }
}

void ConsolePainter::paintCircle(int centerX, int centerY, int radius, CHAR_INFO charInfo) {
    int width = min(centerX + radius, bufWidth);
    int height = min(centerY + radius, bufHeight);

    for (int i = max(centerX - radius, 0); i < width; ++i) {
        for (int j = max(centerY - radius, 0); j < height; ++j) {
            if ((i - centerX) * (i - centerX) + (j - centerY) * (j - centerY) < radius * radius) {
                chiBuffer[i + j * bufWidth] = charInfo;
            }
        }
    }
}

void ConsolePainter::paintConvexPolygon(std::vector<std::pair<int, int>> vertices, CHAR_INFO charInfo) {
    int minY = bufHeight;
    int maxY = 0;

    for (auto vertex: vertices) {
        if (maxY < vertex.second) {
            maxY = vertex.second;
        } else if (minY > vertex.second) {
            minY = vertex.second;
        }
    }
    minY = max(minY - 1, 0);
    maxY = min(maxY + 1, bufHeight);
    vertices.push_back(vertices.front());

    std::pair<int, int> p1(0, minY);
    std::pair<int, int> p2(bufWidth - 1, minY);
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
                chiBuffer[intersections[0].first + j * bufWidth] = charInfo;
                break;
            case 2:
                for (int i = min(intersections[0].first, intersections[1].first);
                     i < max(intersections[0].first, intersections[1].first); ++i) {
                    chiBuffer[i + j * bufWidth] = charInfo;
                }
                break;
            default:
                break;
        }
        index = 0;
    }
}

std::pair<int, int>
ConsolePainter::lineLineIntersection(std::pair<int, int> p1, std::pair<int, int> p2, std::pair<int, int> p3,
                                     std::pair<int, int> p4) {
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