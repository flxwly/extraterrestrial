#define CsBot_AI_H //DO NOT delete this line
#define CsBot_AI_C  //DO NOT delete this line
#ifndef CSBOT_REAL

#define ROBOT_RAD 6

#include <iostream>
#include <windows.h>
#include <cstdlib>

using namespace std;

#include "CoSpaceFunctions.hpp"
#include "CommonFunctions.hpp"
#include "ColorRecognition.hpp"

// Classes
#include "AStar.hpp"
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

    std::vector<std::vector<int>> pMap;
    for (int i = 0; i < PathfinderGame1.map.size(); i++) {
        std::vector<int> _v;
        pMap.push_back(_v);
        for (int j = 0; j < PathfinderGame1.map[i].size(); j++) {
            auto node = PathfinderGame1.map[i][j];
            if (node.isWall) {
                pMap[i].push_back(1);
            } else if (node.isTrap) {
                pMap[i].push_back(2);
            } else if (node.isSwamp) {
                pMap[i].push_back(3);
            } else {
                pMap[i].push_back(0);
            }
        }
    }

    Debug.addMap(pMap);

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

void Game1() {
    updateHSL();


    Bot.game_1_loop();

    // === Debug ===
    // Pos
    //Debug.addRobotPos("Bot: ", PositionX, PositionY);

    //Debug.redraw(cycles);
}