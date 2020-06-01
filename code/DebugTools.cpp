//
// Created by flxwly on 29.05.2020.
//
#include "DebugTools.hpp"

HANDLE wHnd; /* write (output) handle */

DebugTool::DebugTool(int initX, int initY, int delay, int time) {
    DebugTool::x = initX;
    DebugTool::y = initY;
    DebugTool::minDelay = delay;
    DebugTool::lastPrint = time;
    DebugTool::changedPaths = false;
    DebugTool::changedMarkers = false;
    DebugTool::changedPos = false;
    DebugTool::changesRobotPos = false;
    DebugTool::posH = 0;
    DebugTool::pathH = 0;
    DebugTool::mapH = 0, DebugTool::mapW = 0;
}

void DebugTool::move(int _x, int _y) {
    if (_x != DebugTool::x || _y != DebugTool::y) {
        DebugTool::x = _x;
        DebugTool::y = _y;
        DebugTool::changedPos = true;
    }
}

void DebugTool::addMap(const std::vector<std::vector<int>> &_map) {
    DebugTool::map.clear();

    int i = 0;
    for (const std::vector<int> &col : _map) {
        std::vector<int> v;
        DebugTool::map.push_back(v);
        for (int row : col) {
            DebugTool::map[i].push_back(row);
        }
        i++;
    }
    DebugTool::mapW = DebugTool::map.size();
    DebugTool::mapH = DebugTool::map[0].size();

    std::cout << DebugTool::map[195][47] << std::endl;
    DebugTool::changedMap = true;

}

void DebugTool::addRobotPos(const std::string &name, int _x, int _y) {
    int i = 0;
    for (const std::string &label : DebugTool::robotLabels) {
        if (label == name) {
            if (DebugTool::robotPositions[i].first != _x || DebugTool::robotPositions[i].second != _y) {
                DebugTool::robotPositions[i].first = _x;
                DebugTool::robotPositions[i].second = _y;
                DebugTool::changesRobotPos = true;
            }
            return;
        }
        i++;
    }
    std::pair<int, int> p = {_x, _y};
    DebugTool::robotLabels.push_back(name);
    DebugTool::robotPositions.push_back(p);
    DebugTool::changesRobotPos = true;
}

void DebugTool::removeRobotPos(const std::string &name) {
    int i = 0;
    for (const std::string &label : DebugTool::robotLabels) {
        if (label == name) {
            DebugTool::robotLabels.erase(DebugTool::robotLabels.begin() + i);
            DebugTool::robotPositions.erase(DebugTool::robotPositions.begin() + i);
            DebugTool::changesRobotPos = true;
            return;
        }
        i++;
    }
}

void DebugTool::addMarker(int _x, int _y, int color) {
    int i = 0;
    for (std::array<int, 3> marker : DebugTool::markers) {
        if (marker[0] == _x && marker[1] == _y) {
            if (marker[2] != color) {
                DebugTool::markers[i][2] = color;
                DebugTool::changedMarkers = true;
            }
            return;
        }
        i++;
    }
    DebugTool::markers.push_back({_x, _y, color});
    DebugTool::changedMarkers = true;
}

void DebugTool::removeMarker(int _x, int _y) {
    int i = 0;
    for (std::array<int, 3> marker : DebugTool::markers) {
        if (marker[0] == _x && marker[1] == _y) {
            DebugTool::markers.erase(DebugTool::markers.begin() + i);
            DebugTool::changedMarkers = true;
            return;
        }
        i++;
    }
}

void DebugTool::addPath(const std::string &name, const std::vector<std::pair<int, int>> &_path) {
    int i = 0;
    for (const std::string &label : DebugTool::pathLabels) {
        if (label == name) {
            std::iter_swap(DebugTool::pathLabels.begin() + i, DebugTool::pathLabels.end());
            std::iter_swap(DebugTool::paths.begin() + i, DebugTool::paths.end());
            DebugTool::paths[DebugTool::paths.size() - 1] = _path;
            DebugTool::changedPaths = true;
            return;
        }
        i++;
    }
    DebugTool::pathLabels.push_back(name);
    DebugTool::paths.push_back(_path);
    DebugTool::changedPaths = true;
}

void DebugTool::removePath(const std::string &name) {
    int i = 0;
    for (const std::string &label : DebugTool::pathLabels) {
        if (label == name) {
            std::iter_swap(DebugTool::pathLabels.begin() + i, DebugTool::pathLabels.end());
            std::iter_swap(DebugTool::paths.begin() + i, DebugTool::paths.end());
            DebugTool::paths.pop_back();
            DebugTool::pathLabels.pop_back();
            DebugTool::changedPaths = true;
            return;
        }
        i++;
    }
}

void DebugTool::redraw(int _time) {
    if (DebugTool::lastPrint < _time - DebugTool::minDelay) {
        if (DebugTool::changedMap) {
            if (mapW != DebugTool::map.size()) {
                DebugTool::changedPaths = true;
                DebugTool::changesRobotPos = true;
            }

            for (int i = 0; i < DebugTool::mapW; i++) {

                if (DebugTool::buffer.size() <= i) {
                    std::vector<CHAR_INFO> v;
                    DebugTool::buffer.push_back(v);
                }

                for (int j = 0; j < DebugTool::mapH; j++) {

                    CHAR_INFO c = {0, 0};
                    c.Char.AsciiChar = static_cast<unsigned char>(219);
                    c.Attributes = (DebugTool::map[i][j] + 10 * DebugTool::map[i][j] + 1);

                    if (DebugTool::buffer[i].size() <= j) {
                        DebugTool::buffer[i].push_back(c);
                    } else {
                        DebugTool::buffer[i][j] = c;
                    }
                }
            }
            DebugTool::print(_time);
        }
    }
}

void DebugTool::print(int time) {
    int _x, _y;
    auto WIDTH = static_cast<SHORT>(DebugTool::buffer.size());
    auto HEIGHT = static_cast<SHORT>(DebugTool::buffer[0].size());

    auto WIDTH1 = static_cast<SHORT>(DebugTool::buffer.size() - 1);
    auto HEIGHT1 = static_cast<SHORT>(DebugTool::buffer[0].size() - 1);


    /* Window size coordinates, be sure to start index at zero! */
    SMALL_RECT windowSize = {0, 0, WIDTH1, HEIGHT1};

    /* A COORD struct for specificying the console's screen buffer dimensions */
    COORD bufferSize = {WIDTH, HEIGHT};

    /* Setting up different variables for passing to WriteConsoleOutput */
    COORD characterBufferSize = {WIDTH, HEIGHT};
    COORD characterPosition = {0, 0};
    SMALL_RECT consoleWriteArea = {0, 0, WIDTH1, HEIGHT1};

    /* A CHAR_INFO structure containing data about a single character */
    auto *consoleBuffer = new CHAR_INFO[WIDTH * HEIGHT];

    /* initialize handles */
    wHnd = GetStdHandle(STD_OUTPUT_HANDLE);

    /* Set the console's title */
    SetConsoleTitle("Easter Egg");

    /* Set the window size */
    SetConsoleWindowInfo(wHnd, TRUE, &windowSize);

    /* Set the screen's buffer size */
    SetConsoleScreenBufferSize(wHnd, bufferSize);

    for (_y = 0; _y < HEIGHT; ++_y) {
        for (_x = 0; _x < WIDTH; ++_x) {
            consoleBuffer[_x + WIDTH * _y].Char.AsciiChar = DebugTool::buffer[_x][_y].Char.AsciiChar;
            consoleBuffer[_x + WIDTH * _y].Attributes = DebugTool::buffer[_x][_y].Attributes * 10;  //
        }
    }

    /* Write our character buffer (a single character currently) to the console buffer */
    WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);
    delete[] consoleBuffer;
}