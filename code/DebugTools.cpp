//
// Created by flxwly on 29.05.2020.
//
#include "DebugTools.hpp"

HANDLE wHnd; /* write (output) handle */

DebugTool::DebugTool(int initX, int initY, int delay, int time) {
    DebugTool::x = initX, DebugTool::y = initY;
    DebugTool::minDelay = delay, DebugTool::lastPrint = time;
    DebugTool::changedMap = false, DebugTool::changedPaths = false, DebugTool::changedMarkers = false, DebugTool::changedPos = false, DebugTool::changedRobotPos = false;
    DebugTool::posH = 0, DebugTool::posW = 0;
    DebugTool::pathH = 0, DebugTool::pathW = 0;
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

    DebugTool::changedMap = true;

}

void DebugTool::addRobotPos(const std::string &name, int _x, int _y) {
    int i = 0;
    for (const std::string &label : DebugTool::robotLabels) {
        if (label == name) {
            if (DebugTool::robotPositions[i].first != _x || DebugTool::robotPositions[i].second != _y) {

                DebugTool::robotPositions[i].first = _x;
                DebugTool::robotPositions[i].second = _y;

                DebugTool::changedRobotPos = true;
            }
            return;
        }
        i++;
    }

    DebugTool::robotLabels.push_back(name);
    std::pair<int, int> p = {_x, _y};
    DebugTool::robotPositions.push_back(p);
    DebugTool::posH += 2;

    DebugTool::changedRobotPos = true;
}

void DebugTool::removeRobotPos(const std::string &name) {
    int i = 0;
    for (const std::string &label : DebugTool::robotLabels) {
        if (label == name) {
            DebugTool::robotLabels.erase(DebugTool::robotLabels.begin() + i);
            DebugTool::robotPositions.erase(DebugTool::robotPositions.begin() + i);
            DebugTool::posH -= 2;

            DebugTool::changedRobotPos = true;
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
            DebugTool::pathH -= DebugTool::paths[i].size();

            DebugTool::paths[DebugTool::paths.size() - 1] = _path;
            DebugTool::pathH += _path.size();

            DebugTool::changedPaths = true;
            return;
        }
        i++;
    }


    DebugTool::pathLabels.push_back(name);
    DebugTool::paths.push_back(_path);
    DebugTool::pathH += static_cast<int>(_path.size()) + 1;

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
            DebugTool::pathH -= static_cast<int>(DebugTool::paths[i].size()) + 1;

            DebugTool::changedPaths = true;
            return;
        }
        i++;
    }
}

void DebugTool::redraw(int _time) {
    //std::cout << "writing to first buffer" << std::endl;
    if (DebugTool::lastPrint < _time - DebugTool::minDelay) {
        if (DebugTool::changedMap) {
            if (mapW != DebugTool::map.size()) {
                DebugTool::changedMarkers = true;
                DebugTool::changedPaths = true;
                DebugTool::changedRobotPos = true;
            }

            for (int i = 0; i < DebugTool::mapW; i++) {

                if (DebugTool::buffer.size() <= i) {
                    std::vector<CHAR_INFO> v;
                    DebugTool::buffer.push_back(v);
                }

                for (int j = 0; j < DebugTool::mapH; j++) {

                    CHAR_INFO c = {0, 0};
                    c.Char.UnicodeChar = static_cast<unsigned char>(254);
                    c.Attributes = DebugTool::map[i][j] + 16 * DebugTool::map[i][j];

                    if (DebugTool::buffer[i].size() <= j) {
                        DebugTool::buffer[i].push_back(c);
                    } else {
                        DebugTool::buffer[i][j] = c;
                    }
                }
            }
            DebugTool::changedMap = false;
        }
        if (DebugTool::changedPaths) {
            int i = 0;
            for (const auto &path : DebugTool::paths) {
                for (auto point : path) {
                    DebugTool::buffer[point.first][point.second].Attributes = 3 + i;
                }
                i++;
            }
            DebugTool::changedPaths = false;
        }
        if (DebugTool::changedMarkers) {
            for (auto marker : DebugTool::markers) {
                if (marker[0] >= 0 && marker[0] < DebugTool::buffer.size() && marker[1] >= 0 &&
                    marker[1] < DebugTool::buffer[marker[0]].size())
                    DebugTool::buffer[marker[0]][marker[1]].Attributes = marker[2];
            }
            DebugTool::changedMarkers = false;
        }
        if (DebugTool::changedRobotPos) {
            int k = 0;
            int w = 0;
            for (auto rPos : DebugTool::robotPositions) {
                std::string label = DebugTool::robotLabels[k / 2] + ":";
                std::string pos = " " + std::to_string(rPos.first) + " | " + std::to_string(rPos.second);

                // allocate space in the buffer
                int i;
                while (DebugTool::buffer.size() < pos.size() ||
                       DebugTool::buffer.size() < label.size()) {
                    std::vector<CHAR_INFO> v;
                    DebugTool::buffer.push_back(v);
                }
                for (i = 0; i < pos.size() || i < label.size(); ++i) {
                    while (DebugTool::buffer[i].size() < DebugTool::mapH + k + 2) {
                        CHAR_INFO c = {0, 0};
                        DebugTool::buffer[i].push_back(c);
                    }
                }

                //std::cout << label.size() << " | " << pos.size() << std::endl;
                //std::cout << DebugTool::mapW + label.size() << " | " << DebugTool::mapW + pos.size() << std::endl;
                //std::cout << DebugTool::buffer.size() << std::endl;
                // write to the buffer

                for (i = 0; i < label.size(); ++i) {
                    //std::cout << label[i];
                    DebugTool::buffer[i][DebugTool::mapH + k].Char.UnicodeChar = label[i];
                    DebugTool::buffer[i][DebugTool::mapH + k].Attributes = 7;
                }
                //std::cout << std::endl;

                for (i = 0; i < pos.size(); ++i) {
                    //std::cout << pos[i];
                    DebugTool::buffer[i][DebugTool::mapH + k + 1].Char.UnicodeChar = pos[i];
                    DebugTool::buffer[i][DebugTool::mapH + k + 1].Attributes = 7;
                }

                w = std::max(static_cast<int>(label.size()), std::max(static_cast<int>(pos.size()), w));

                k += 2;
            }
            //DebugTool::print(0, DebugTool::mapH, w, DebugTool::mapH + DebugTool::posH);

        }
        DebugTool::lastPrint = _time;
        DebugTool::print();
    }
}

void DebugTool::print(int beginX, int beginY, int endX, int endY) {

    //std::cout << "setup final buffer" << std::endl;

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

    //std::cout << "writing to final buffer" << std::endl;
    // write everything
    for (_y = 0; _y < HEIGHT; ++_y) {
        for (_x = 0; _x < WIDTH; ++_x) {
            if ((DebugTool::buffer.size() > _x && DebugTool::buffer[_x].size() > _y)
                && (beginX <= _x && endX >= _x && beginY <= _y && endY >= _y)) {
                consoleBuffer[_x + WIDTH * _y].Char.UnicodeChar = DebugTool::buffer[_x][_y].Char.UnicodeChar;
                consoleBuffer[_x + WIDTH * _y].Attributes = DebugTool::buffer[_x][_y].Attributes;
            } else {
                consoleBuffer[_x + WIDTH * _y] = {0, 0};
            }
        }
    }


    /* Write our character buffer (a single character currently) to the console buffer */
    WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);
    delete[] consoleBuffer;
}

void DebugTool::print() {
    int h = DebugTool::mapH + DebugTool::posH + DebugTool::pathH;
    DebugTool::print(0, 0, DebugTool::buffer.size(), h);
}