//
// Created by flxwly on 29.05.2020.
//
#ifndef EXTRATERRESTRIAL_DEBUGTOOLS_HPP
#define EXTRATERRESTRIAL_DEBUGTOOLS_HPP

#include <windows.h>
#include <iostream>
#include <random>
#include <cstdio>


#include <algorithm>
#include <vector>
#include <array>
#include <string>


class DebugTool {
public:
    DebugTool(int initX, int initY, int delay, int time);

    void redraw(int time); // Check if something has changed -> call own print method

    void move(int x, int y); // move buffer
    void addMap(const std::vector<std::vector<int>> &_map);

    void addRobotPos(const std::string &name, int _x, int _y); // add Robot Pos to Hud;
    void removeRobotPos(const std::string &name);

    void addMarker(int _x, int _y, int color); // add Marker to Map;
    void removeMarker(int _x, int _y);

    void addPath(const std::string &name, const std::vector<std::pair<int, int>> &_path); // add Path to Hud;
    void removePath(const std::string &name);


private:
    int x, y;
    int minDelay;
    int lastPrint;

    int posH, posW;
    std::vector<std::string> robotLabels;
    std::vector<std::pair<int, int>> robotPositions;
    int pathH, pathW;
    std::vector<std::string> pathLabels;
    std::vector<std::vector<std::pair<int, int>>> paths;
    int mapW, mapH;
    std::vector<std::vector<int>> map;
    std::vector<std::array<int, 3>> markers;

    bool changedMap;
    bool changedPos;
    bool changedPaths;
    bool changedRobotPos;
    bool changedMarkers;

    std::vector<std::vector<CHAR_INFO>> buffer;

    void print(int beginX, int beginY, int endX, int endY);

    void print();
};


#endif //EXTRATERRESTRIAL_DEBUGTOOLS_HPP
