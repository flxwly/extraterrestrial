//
// Created by flxwly on 29.05.2020.
//
#include "DebugTools.hpp"

HANDLE wHnd; /* write (output) handle */

DebugTool::DebugTool(int initX, int initY, int delay) {
    DebugTool::x = initX, DebugTool::y = initY;
    DebugTool::minDelay = delay, DebugTool::lastRefresh = 0;
    DebugTool::changedMap = false, DebugTool::changedPaths = false, DebugTool::changedMarkers = false, DebugTool::changedPos = false, DebugTool::changedRobotPos = false;
    DebugTool::mapBeginX = 0, DebugTool::mapBeginY = 0, DebugTool::mapEndX = 0, DebugTool::mapEndY = 0;
    DebugTool::posBeginX = 0, DebugTool::posBeginY = 0, DebugTool::posEndX = 0, DebugTool::posEndY = 0;
    DebugTool::pathBeginX = 0, DebugTool::pathBeginY = 0, DebugTool::pathEndX = 0, DebugTool::pathEndY = 0;
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

    DebugTool::changedMap = true;

}

void DebugTool::addRobotPos(const std::string &name, int _x, int _y) {
    int i = 0;
    for (const std::string &label : DebugTool::robotLabels) {
        if (label == name) {
            if (DebugTool::robotPositions[i].first != _x || DebugTool::robotPositions[i].second != _y) {

                DebugTool::robotPositions[i] = {_x, _y};

                DebugTool::changedRobotPos = true;
            }
            return;
        }
        i++;
    }

    DebugTool::robotLabels.push_back(name);
    std::pair<int, int> p = {_x, _y};
    DebugTool::robotPositions.push_back(p);

    DebugTool::changedRobotPos = true;
}

void DebugTool::removeRobotPos(const std::string &name) {
    int i = 0;
    for (const std::string &label : DebugTool::robotLabels) {
        if (label == name) {
            DebugTool::robotLabels.erase(DebugTool::robotLabels.begin() + i);
            DebugTool::robotPositions.erase(DebugTool::robotPositions.begin() + i);

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
    // only update the buffer and print it after a certain amount of time
    if (DebugTool::lastRefresh < _time - DebugTool::minDelay) {

        // a map has been added or changed -> resize the buffer and add the map to the buffer
        if (DebugTool::changedMap) {

            std::vector<CHAR_INFO> v;

            // resize bufferspace for map in X direction if necessary
            while (DebugTool::mapEndX < static_cast<int>(DebugTool::map.size())) {
                DebugTool::buffer.insert(DebugTool::buffer.begin() + DebugTool::mapEndX, v);
                DebugTool::mapEndX++;
            }
            while (DebugTool::mapEndX > static_cast<int>(DebugTool::map.size())) {
                DebugTool::buffer.erase(DebugTool::buffer.begin() + DebugTool::mapEndX);
                DebugTool::mapEndX--;
            }

            CHAR_INFO c = {219, 0};

            // resize bufferspace for map in Y direction if necessary
            while (DebugTool::mapEndY < static_cast<int>(DebugTool::map[0].size())) {
                // go through every row and add an element
                for (int i = 0; i < static_cast<int>(DebugTool::map.size()); i++) {
                    DebugTool::buffer[i].insert(DebugTool::buffer[i].begin() + DebugTool::mapEndY, c);
                }
                DebugTool::mapEndY++;
            }
            while (DebugTool::mapEndY > static_cast<int>(DebugTool::map[0].size())) {
                // go through every row and add an element
                for (int i = 0; i < static_cast<int>(DebugTool::map.size()); i++) {
                    DebugTool::buffer[i].erase(DebugTool::buffer[i].begin() + DebugTool::mapEndY);
                }
                DebugTool::mapEndY--;
            }

            // put map into the buffer
            for (int i = 0; i < static_cast<int>(DebugTool::map.size()); ++i) {
                for (int j = 0; j < static_cast<int>(DebugTool::map[i].size()); ++j) {
                    DebugTool::buffer[i][j].Attributes = static_cast<short unsigned int>(DebugTool::map[i][j] + 16 * DebugTool::map[i][j]);
                }
            }
            // set other begin and end vars;
            DebugTool::posEndY -= DebugTool::posBeginY - DebugTool::mapEndY;
            DebugTool::posBeginY = DebugTool::mapEndY;

            DebugTool::pathEndY -= DebugTool::pathBeginY - DebugTool::mapEndY;
            DebugTool::pathBeginY = DebugTool::mapEndY;
        }
            // reset the map if something has been changed TODO: maybe there's a cleaner way to do this
        else if (DebugTool::changedPos || DebugTool::changedMarkers || DebugTool::changedPaths) {
            for (int i = 0; i < static_cast<int>(DebugTool::map.size()); ++i) {
                for (int j = 0; j < static_cast<int>(DebugTool::map[i].size()); ++j) {
                    DebugTool::buffer[i][j].Attributes = static_cast<short unsigned int>(DebugTool::map[i][j] + 16 * DebugTool::map[i][j]);
                }
            }
            DebugTool::changedMap = true;
        }

        // refresh the markers on the map
        if (DebugTool::changedMarkers || DebugTool::changedMap) {
            for (auto marker : DebugTool::markers) {
                if (marker[0] >= 0 && marker[0] < static_cast<int>(DebugTool::buffer.size()) && marker[1] >= 0 &&
                    marker[1] < static_cast<int>(DebugTool::buffer[marker[0]].size())) {
                    DebugTool::buffer[marker[0]][marker[1]].Attributes = static_cast<short unsigned int>(marker[2]);
                }
            }
        }

        // refresh the robots Position on the map
        if (DebugTool::changedRobotPos || DebugTool::changedMap) {

            // allocate space in the buffer
            //      update X size:
            std::vector<CHAR_INFO> v;
            int maxX = 0;
            for (int i = 0; i < DebugTool::robotLabels.size(); i++) {
                std::string l = DebugTool::robotLabels[i] + ":";
                std::string p = "   " + std::to_string(DebugTool::robotPositions[i].first) + " | " +
                                std::to_string(DebugTool::robotPositions[i].second);

                maxX = std::max(std::max(l.length(), p.length()), static_cast<unsigned int>(maxX));
            }
            while (maxX > DebugTool::buffer.size()) {
                DebugTool::buffer.push_back(v);
            }

            //      update Y size:
            CHAR_INFO c = {0, 0};
            int maxY = DebugTool::robotLabels.size() * 2;
            for (int i = 0; i < maxY; i++) {
                DebugTool::buffer[i].push_back(c);
            }

            // write to buffer
            for (int i = 0; i < DebugTool::robotLabels.size(); i++) {
                std::string l = DebugTool::robotLabels[i] + ":";
                std::string p = "   " + std::to_string(DebugTool::robotPositions[i].first) + " | " +
                                std::to_string(DebugTool::robotPositions[i].second);


                for (int j = 0; j < l.size(); ++j) {
                    DebugTool::buffer[j][DebugTool::posBeginY + i * 2].Char.UnicodeChar = l[j];
                    DebugTool::buffer[j][DebugTool::posBeginY + i * 2].Attributes = 7;
                }
                for (int j = 0; j < p.size(); ++j) {
                    DebugTool::buffer[j][DebugTool::posBeginY + i * 2 + 1].Char.UnicodeChar = p[j];
                    DebugTool::buffer[j][DebugTool::posBeginY + i * 2 + 1].Attributes = 7;
                }
            }
            DebugTool::posEndY = DebugTool::posBeginY + maxY;
            DebugTool::posEndX = maxX;

            for (auto rPos : DebugTool::robotPositions) {
                DebugTool::buffer[rPos.first][rPos.second].Attributes = 10;
            }
        }

        // a path has been added, removed or changed -> resize the buffer based on that.
        // if the map has been updated the path was overwritten by the map chars
        if (DebugTool::changedPaths || DebugTool::changedMap) {
            // only write paths to map; The map was refreshed
            if (DebugTool::changedMap && !DebugTool::changedPaths) {
                int i = 0;
                for (const auto &path : DebugTool::paths) {
                    for (auto point : path) {
                        DebugTool::buffer[point.first][point.second].Attributes = 3 + i;
                    }
                    i++;
                }
            }
                // write to map and refresh paths hud; The paths have been updated;
            else {

                /*// allocate space in the buffer
                //      update X size:
                std::vector<CHAR_INFO> v;
                int maxX = 0;
                for (auto &pathLabel : DebugTool::pathLabels) {
                    std::string l = pathLabel + ":";
                    std::string p = "   " + std::to_string(000000) + " | " +
                                    std::to_string(000000);

                    maxX = std::max(std::max(l.length(), p.length()), static_cast<unsigned int>(maxX));
                }
                while (maxX + DebugTool::posEndX > DebugTool::buffer.size()) {
                    DebugTool::buffer.push_back(v);
                }

                //      update Y size:
                CHAR_INFO c = {0, 0};
                int maxY = DebugTool::pathLabels.size();
                for (const auto &path : DebugTool::paths) {
                    maxY += path.size();
                }
                for (auto &i : DebugTool::buffer) {
                    while (i.size() < maxY) {
                        i.push_back(c);
                    }
                }

                // write to buffer
                for (int i = 0; i < DebugTool::pathLabels.size(); i++) {
                    std::string l = DebugTool::pathLabels[i] + ":";

                    int coord_count = 0;
                    if (i > 0)
                        coord_count = DebugTool::paths[i - 1].size();

                    for (int j = 0; j < l.size(); ++j) {
                        DebugTool::buffer[j][DebugTool::posBeginY +
                                             i * coord_count].Char.UnicodeChar = l[j];
                        DebugTool::buffer[j][DebugTool::posBeginY +
                                             i * coord_count].Attributes = 7;
                    }
                    for (auto point : DebugTool::paths[i]){
                        std::string p = "   " + std::to_string(point.first) + " | " +
                                        std::to_string(point.second);
                        for (int j = 0; j < p.size(); ++j) {
                            DebugTool::buffer[j][DebugTool::posBeginY + i * coord_count + 1].Char.UnicodeChar = p[j];
                            DebugTool::buffer[j][DebugTool::posBeginY + i * coord_count + 1].Attributes = 7;
                        }
                    }
                }*/
            }
        }
        DebugTool::lastRefresh = _time;
        DebugTool::print();
    }
}

void DebugTool::print(SHORT beginX, SHORT beginY, SHORT endX, SHORT endY) {
    int _x, _y;

    auto WIDTH = static_cast<SHORT>(abs(endX - beginX));
    auto HEIGHT = static_cast<SHORT>(abs(endY - beginY));

    /* Window size coordinates, be sure to start index at zero! */
    SMALL_RECT windowSize = {beginX, beginY, endX, endY};

    /* A COORD struct for specificying the console's screen buffer dimensions */
    COORD bufferSize = {WIDTH, HEIGHT};

    /* Setting up different variables for passing to WriteConsoleOutput */
    COORD characterBufferSize = {WIDTH, HEIGHT};
    COORD characterPosition = {0, 0};
    SMALL_RECT consoleWriteArea = {beginX, beginY, static_cast<SHORT>(endX - 1), static_cast<SHORT>(endY - 1)};

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
            consoleBuffer[_x + WIDTH * _y].Char.UnicodeChar = DebugTool::buffer[_x][_y].Char.UnicodeChar;
            consoleBuffer[_x + WIDTH * _y].Attributes = DebugTool::buffer[_x][_y].Attributes;
        }
    }


    /* Write our character buffer (a single character currently) to the console buffer */
    WriteConsoleOutputA(wHnd, consoleBuffer, characterBufferSize, characterPosition, &consoleWriteArea);
    delete[] consoleBuffer;
}

void DebugTool::print() {
    auto WIDTH = static_cast<SHORT>(DebugTool::buffer.size());
    SHORT HEIGHT = 0;
    for (int i = 0; i < WIDTH; ++i) {
        HEIGHT = std::max(static_cast<SHORT>(DebugTool::buffer[i].size()), HEIGHT);
    }
    DebugTool::print(0, 0, WIDTH, HEIGHT);
}