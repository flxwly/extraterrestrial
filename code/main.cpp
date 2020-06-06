#define CsBot_AI_H //DO NOT delete this line
#define CsBot_AI_C  //DO NOT delete this line
#ifndef CSBOT_REAL

#define ROBOT_RAD 10

#include <iostream>
#include <windows.h>
#include <cstdlib>

using namespace std;

#include "CoSpaceFunctions.hpp"
#include "CommonFunctions.hpp"
#include "Navigation.hpp"
#include "ColorRecognition.hpp"
#include "Pathfinding.hpp"

#include "MapData.hpp"
#include "DebugTools.hpp"
#include "Robot.hpp"

#endif

void updateHSL() {
    DEBUG_MESSAGE("Updating HSL...\n", 3);

    hueR = rgb2h(CSRight_R, CSRight_G, CSRight_B);
    hueL = rgb2h(CSLeft_R, CSLeft_G, CSLeft_B);
    DEBUG_MESSAGE("\t Hue Right: " + to_string(hueR) + " | Hue Left: " + to_string(hueL) + "\n", 3);

    satR = rgb2s(CSRight_R, CSRight_G, CSRight_B);
    satL = rgb2s(CSLeft_R, CSLeft_G, CSLeft_B);
    DEBUG_MESSAGE("\t Sat Right: " + to_string(satR) + " | Sat Left: " + to_string(satL) + "\n", 3);

    lumR = rgb2l(CSRight_R, CSRight_G, CSRight_B);
    lumL = rgb2l(CSLeft_R, CSLeft_G, CSLeft_B);
    DEBUG_MESSAGE("\t Lum Right: " + to_string(lumR) + " | Lum Left: " + to_string(lumL) + "\n", 3);

    DEBUG_MESSAGE("finished updating HSL \n", 3);
}

MapData GAME0(240, 180, &GAME0REDPOINTS, &GAME0GREENPOINTS, &GAME0BLACKPOINTS, &GAME0DEPOSITAREAS, &GAME0WALLS,
              &GAME0TRAPS, &GAME0SWAMPS);
MapData GAME1(360, 270, &GAME1REDPOINTS, &GAME1GREENPOINTS, &GAME1BLACKPOINTS, &GAME1DEPOSITAREAS, &GAME1WALLS,
              &GAME1TRAPS, &GAME1SWAMPS);

AStar PathfinderGame0(GAME0.Map, ROBOT_RAD);
AStar PathfinderGame1(GAME1.Map, ROBOT_RAD);

DebugTool Debug(0, 0, 10);

Robot Bot(&PositionX, &PositionY, &Compass, &SuperObj_Num, &SuperObj_X, &SuperObj_Y,
          &CSRight_R, &CSRight_G, &CSRight_B, &CSLeft_R, &CSLeft_G, &CSLeft_B,
          &US_Left, &US_Front, &US_Right,
          &WheelLeft, &WheelRight, &LED_1, &Teleport, &Time,
          &GAME0, &GAME1, &PathfinderGame0, &PathfinderGame1);

void Setup() {
    system("cls");

    DEBUG_MESSAGE("Init... \n", 0);

    updateHSL();

    DEBUG_MESSAGE("\tGame0...", 0);
    DEBUG_MESSAGE("\t finished\n", 0);

    DEBUG_MESSAGE("\tGame1...", 0);

    Debug.addMap(GAME1.Map);
    Debug.addMarker(10, 10, 10);

    DEBUG_MESSAGE("\t finished\n", 0);

    DEBUG_MESSAGE("finished initialization\n", 0);
}

/*
 * ///_________________________________GAME0________________________________________///
 *
 * ///__________________________________________________________________________________///
*/

void Game0() {
    updateHSL();

    Bot.game_0_loop();
}

/*
 * ///_________________________________GAME1________________________________________///
 * TODO Make Pathfinding work
 *  - Get new Waypoint if old one is not needed anymore
 *  - follow route
 *      - choose best point (in  to)
 *      - remember time of visited points.
 * TODO Hunt Superobject
 * TODO
 * ///__________________________________________________________________________________///
*/

int neededColors() {
    // pointColor: 1 = red, 2 = cyan, 4 = black

    int sum = 0;
    if (collectedItems[0] < 2)
        sum += 1;
    if (collectedItems[1] < 2)
        sum += 2;
    if (collectedItems[2] < 2)
        sum += 4;

    return sum;
}
pair<int, int> getNextPoint(MapData *mData) {

    const int GET_NEXT_POINT_VALUATE_POINTS_NUM = 2;

    int need = neededColors();
    int avl = mData->availableColors();

    if (avl == 0)
        return {-1, -1};


    /* Die Überschneidungsmenge von den gebrauchten und den vorhandenen Punkten finden.
     * Das weitere Verfahren nur für diese überschneidungsmenge durchführen.
     *      Eine Linie zu jedem Depositarea ziehen.
     *      Mithilfe von Dreiecken die Distanz aller Punkte zu den Linien rausfinden.
     *      Die besten 2 von jeder benötigten Farbe merken.
     *      Den zum Roboter nächsten Punkt zurückgeben
     *
     * */

    vector<array<int, 3>> arr;
    if (need % 2 == 1 && avl % 2 == 1) {    // needs red object and red is available
        for (int i = 0; i < mData->getPointCount(0); i++) {
            arr.push_back({mData->getPoint(i, 0).first, mData->getPoint(i, 0).second, 0});
        }
    }
    if ((need == 2 || need == 3 || need == 6 || need == 7) && (avl == 2 || avl == 3 || avl == 6 || avl == 7)) {
        for (int i = 0; i < mData->getPointCount(1); i++) {
            arr.push_back({mData->getPoint(i, 1).first, mData->getPoint(i, 1).second, 1});
        }
    }
    if (need >= 4 && avl >= 4) {    // needs black objeckt and black is available
        for (int i = 0; i < mData->getPointCount(2); i++) {
            arr.push_back({mData->getPoint(i, 2).first, mData->getPoint(i, 2).second, 2});
        }
    }

    struct PRIORITY {
        bool operator()(array<int, 3> child, array<int, 3> parent) const {
            return child[2] < parent[2];
        }
    };

    priority_queue<array<int, 3>, vector<array<int, 3>>, PRIORITY> bPs[3];
    for (pair<int, int> area : mData->getDepositAreas()) {
        double g = dist(area.first, PositionX, area.second, PositionY);
        for (array<int, 3> p : arr) {
            const double den = abs(
                    (area.second - PositionY) * p[0] - (area.first - PositionX) * p[1] +
                    area.first * PositionY - area.second * PositionX);
            int distToLine = static_cast<int> (round(den / g));
            bPs[p[2]].push({p[0], p[1], distToLine});
            if (bPs[p[2]].size() > GET_NEXT_POINT_VALUATE_POINTS_NUM) {
                bPs[p[2]].pop();
            }
        }
    }

    array<int, 3> bP = bPs[0].top();
    for (auto &bP1c : bPs) {
        array<int, 3> p = bP1c.top();
        bP1c.pop();
        if (dist(p[0], PositionX, p[1], PositionY) < dist(bP[0], PositionX, bP[1], PositionY)) {
            bP = p;
        }
    }
    return pair<int, int>{bP[0], bP[1]};
}

void Game1() {
    updateHSL();

    Bot.game_1_loop();

    // === Debug ===
    // Pos
    Debug.addRobotPos("Bot: ", PositionX, PositionY);
    // Paths
    /*int i = 0;
    for (const auto& path : Bot.complete_path) {
        Debug.addPath("Path: " + to_string(i), path);
        i++;
    }
    for (int j = 7; j > i; j--) {
        Debug.removePath("Path: " + to_string(j));
    }*/

    Debug.redraw(cycles);
}