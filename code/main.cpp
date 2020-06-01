#define CsBot_AI_H //DO NOT delete this line
#define CsBot_AI_C  //DO NOT delete this line
#ifndef CSBOT_REAL

#define DEPOSITING 0
#define COLLECTING 1
#define TURN_RIGHT 2
#define TURN_LEFT 3
#define PATHFINDING_ANFUEHRUNGSZEICHEN 4
#define PATHFINDING 4
#define NORMALSPEED 5
#define GOBACKWARDS 7
#define OUTOFAREA 8
#define CORNER 9

#define ROBOT_SIZE 10

#define ENABLE_PATHFINDING true

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

#endif


int depositTime = 0;
bool isDepositting = false;

//TODO more logic to should Deposit Function -> Time runs out etc...
bool shouldDeposit() {
    /*if (!isOrange())
        return depositTime > Time;
    */
    int treshhold = 0;
    // maxpoints: 6 blacks = 120 || (135) RGB + 3 points || full RGB: 270
    treshhold += 10 * collectedItems[0] + 15 * collectedItems[1] + 20 * collectedItems[2];

    if (collectedItems[0] > 0 && collectedItems[1] > 0 && collectedItems[2] > 0) {
        treshhold += 90;
    }
    DEBUG_MESSAGE("Treshhold for Depositting is at: " + to_string(treshhold) + "\n", 0);
    return treshhold >= 130 || depositTime > Time;
}


int collectTime = 0;
bool isCollecting = false;

//TODO more logic to should Collect Function -> Time runs out etc...
bool shouldCollect() {
    if (LoadedObjects >= 6)
        return collectTime > Time;

    return (isRed() && collectedItems[0] < 2)
           || (isCyan() && collectedItems[1] < 2)
           || (isBlack() && collectedItems[2] < 3)
           || collectTime > Time;
}

int unableToInteractUpto = 0;
bool lostPosition = false;

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

AStar PathfinderGame0(GAME0.Map);
AStar PathfinderGame1(GAME1.Map);

DebugTool Debug(0, 0, 0, 0);

void Setup() {
    system("cls");

    DEBUG_MESSAGE("Init... \n", 0);

    updateHSL();

    DEBUG_MESSAGE("\tGame0...", 0);
    DEBUG_MESSAGE("\t finished\n", 0);

    DEBUG_MESSAGE("\tGame1...", 0);

    Debug.addMap(GAME1.Map);
    //Debug.addRobotPos("Blue", PositionX, PositionY);
    Debug.redraw(2);

    DEBUG_MESSAGE("\t finished\n", 0);

    DEBUG_MESSAGE("finished initialization\n", 0);
}

bool areaLeave = false;

/*
 * ///_________________________________GAME0________________________________________///
 *
 * ///__________________________________________________________________________________///
*/

void collect() {
    // CollectTime is until when we want to collect; Only needs to be set when it's lower than Time -> otherwise it's collecting
    if (collectTime < Time) {
        collectTime = Time + 5;
        LoadedObjects += 1;
        if (isRed()) {
            collectedItems[0]++;
        } else if (isCyan()) {
            collectedItems[1]++;
        } else if (isBlack()) {
            collectedItems[2]++;
        }

    } else {
        DEBUG_MESSAGE("Collecting... (" + to_string(collectTime - Time) + ") \n", 0);
        // Robot vars to collect
        DEBUG_MESSAGE("Collected: " + to_string(ObjPositionX) + " | " + to_string(ObjPositionX) + "\n", 10);
        if (ObjState == 0 && dist(ObjPositionX, PositionX, ObjPositionY, PositionY) < 10) {
            collectTime = Time;
        }
        wheels(0, 0);
        LED_1 = 1;
    }
}

void deposit() {
    if (depositTime < Time) {
        depositTime = Time + 6;
        LoadedObjects = 0;
        collectedItems[0] = 0, collectedItems[1] = 0, collectedItems[2] = 0;
    } else {
        wheels(0, 0);
        LED_1 = 2;
        DEBUG_MESSAGE("Depositting... (" + to_string(depositTime - Time) + ") \n", 0);
    }
}

int tPosX = 0;
int tPosY = 0;

void Game0() {
    DEBUG_MESSAGE("---------\nGame0 Loop Nr:" + to_string(cycles) + "\n", 0);
    DEBUG_MESSAGE(
            "Reds: " + to_string(collectedItems[0]) + " | Blues: " + to_string(collectedItems[1]) + " | Blacks: " +
            to_string(collectedItems[2])
            + " | Total: " + to_string(LoadedObjects) + "\n", 0);
    updateHSL();
    /*if (deposited && Time > 180 && unableToInteractUpto < Time) {
        readyToTP = true;
    }

    int targetX = -1, targetY = -1;
    // From 1 to 5;
    int speed = 1;
    CurAction = 7;
    if (shouldCollect()) {
        CurAction = COLLECTING;
    } else if (areaLeave) {
        CurAction = OUTOFAREA;
    } else if (isYellowLeft()) {
        CurAction = TURN_RIGHT;
        speed = 1;
    } else if (isYellowRight()) {
        CurAction = TURN_LEFT;
        speed = 1;
    } else if ((isOrange() && collectedItems[0] > 0 && collectedItems[1] > 0 && collectedItems[2] > 0) ||
               (isOrange() && Time > 120 && LoadedObjects > 0)) {
        CurAction = DEPOSITING;
        deposited = false;
    } else if (targetX != -1 && targetY != -1) {
        CurAction = PATHFINDING_ANFUEHRUNGSZEICHEN;
    } else {

        switch (checkSensors(8, 12, 8)) {
            // no obstacle
            case 0:
                CurAction = NORMALSPEED;
                speed = 3;
//                printf("\nno obstacle");
                break;
            case 1: // obstacle left
                CurAction = TURN_RIGHT;
                speed = 2;
                //printf("\nobstacle left");
                break;
            case 2: // obstacle front
                CurAction = TURN_RIGHT;
                speed = 2;
                //printf("\nobstacle right");
                break;
            case 3: // obstacles left & front
                CurAction = TURN_RIGHT;
                speed = 3;
                //printf("\nobstacles left & front\n");
                break;
            case 4: // obstacle right
                CurAction = TURN_LEFT;
                speed = 2;
                //printf("obstacle right\n");
                break;
            case 5: // obstacles left & right
                CurAction = NORMALSPEED;
                speed = 1;
                //printf("obstacles left & right\n");
                break;
            case 6: // obstacles front & right
                CurAction = TURN_LEFT;
                speed = 2;
                //printf("obstacles front & right\n");
                break;
            default: // obstacles everywhere
                CurAction = GOBACKWARDS;
                speed = 2;
                //printf("obstacles everywhere\n");
                break;

        }
    }

    if (unableToInteractUpto < Time) {
        switch (CurAction) {
            case DEPOSITING:
                DEBUG_MESSAGE("DEPOSITING", 0);
                unableToInteractUpto = Time + 4;
                WheelLeft = 0;
                WheelRight = 0;
                collectedItems[0] = 0, collectedItems[1] = 0, collectedItems[2] = 0;
                LED_1 = 2;
                LoadedObjects = 0;
                if (Time > 10) {
                    areaLeave = true;
                }

                break;
            case COLLECTING:
                if (isRed()) {
                    collectedItems[0]++;
                } else if (isCyan()) {
                    collectedItems[1]++;
                } else if (isBlack()) {
                    collectedItems[2]++;
                }
                LoadedObjects++;

                WheelLeft = 0;
                WheelRight = 0;
                LED_1 = 1;
                unableToInteractUpto = Time + 4;

                break;
            case TURN_RIGHT:

                WheelRight = -1 * speed;
                WheelLeft = 1 * speed;
                LED_1 = 0;

                break;
            case TURN_LEFT:

                WheelRight = 1 * speed;
                WheelLeft = -1 * speed;
                LED_1 = 0;

                break;
            case PATHFINDING_ANFUEHRUNGSZEICHEN:
                //driveToAngle(dvector2Angle(PositionX - targetX, PositionY - targetY));
                LED_1 = 0;

                break;
            case NORMALSPEED:

                WheelRight = 1 * speed;
                WheelLeft = 1 * speed;
                LED_1 = 0;

                break;
            case GOBACKWARDS:
                WheelLeft = -1 * speed;
                WheelRight = -1 * speed;
                break;
            case OUTOFAREA:
                WheelLeft = -1;
                WheelRight = -3;
                unableToInteractUpto = Time + 1;
                DEBUG_MESSAGE("LeavingArea", 0);
                deposited = true;
                areaLeave = false;
                break;
            default:
                break;
        }
    }*/

    PositionX = tPosX;
    PositionY = tPosY;
    updatePos(0.636);
    tPosX = PositionX;
    tPosY = PositionY;

    if (shouldDeposit() && (isOrangeLeft() || isOrangeRight())) {
        if (isOrange()) {
            deposit();
        } else if (isOrangeRight()) {
            wheels(3, 0);
        } else {
            wheels(0, 3);
        }

    } else if (shouldCollect()) {
        collect();

    } else {
        DEBUG_MESSAGE("Using active Object avoidens System\n", 0);
        wheels(3, 3);
        if ((isYellowRight() && LoadedObjects > 0)) {
            DEBUG_MESSAGE("\tAvoid Trap: TURN LEFT\n", 0);
            wheels(-3, 3);
        } else if ((isYellowLeft() && LoadedObjects > 0)) {
            DEBUG_MESSAGE("\tAvoid Trap: TURN RIGHT\n", 0);
            wheels(3, -3);
        } else {
            switch (checkSensors(8, 12, 8)) {
                // no obstacle
                case 0:
                    wheels(3, 3);
                    break;
                case 1: // obstacle left
                    DEBUG_MESSAGE("\tTurn A LITTLE BIT RIGHT (object on the left) Case: 1\n", 0);
                    wheels(3, 0);
                    break;
                case 2: // obstacle front
                    DEBUG_MESSAGE("\tTurn EITHER RIGHT OR LEFT (object on the front) Case: 2\n", 0);
                    wheels(-4, -5);
                    break;
                case 3: // obstacles left & front
                    DEBUG_MESSAGE("\tTurn NORMAL RIGHT (object on the front & left) Case: 3\n", 0);
                    wheels(3, -3);
                    break;
                case 4: // obstacle right
                    DEBUG_MESSAGE("\tTurn NORMAL LEFT (object the right) Case: 4\n", 0);
                    wheels(0, 3);
                    break;
                case 5: // obstacles left & right
                    DEBUG_MESSAGE("\tMaybe dont turn (object on the left & right) Case: 5\n", 0);
                    wheels(3, 3);
                    break;
                case 6: // obstacles front & right
                    DEBUG_MESSAGE("\tTurn NORMAL LEFT (object on the front & right) Case: 6\n", 0);
                    wheels(-3, 3);
                    break;
                default: // obstacles everywhere
                    DEBUG_MESSAGE("\tNO WAY (object on the left & front & right) Case: 7\n", 0);
                    wheels(-5, 5);
                    break;
            }
            LED_1 = 0;
        }
    }
    updatePos(0.654);
    DEBUG_MESSAGE("\tMoving with: " + to_string(WheelLeft) + " | " + to_string(WheelRight) + "\n", 0);
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

int prevPosX = 0;
int prevPosY = 0;
pair<int, int> waypoint = {-1, -1};

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

int bounds(int lastAction) {
    //Left END
    if (PositionX >= 350 && Compass > 270 && Compass <= 360) {
        return TURN_LEFT;
    } else if (PositionX >= 350 && Compass > 180 && Compass <= 270) {
        return TURN_RIGHT;
        //Right END
    } else if (PositionX <= 10 && Compass > 0 && Compass <= 90) {
        return TURN_RIGHT;
    } else if (PositionX <= 10 && Compass > 90 && Compass <= 180) {
        return TURN_LEFT;
        //TOP END
    } else if (PositionY >= 260 && Compass > 270 && Compass <= 360) {
        return TURN_RIGHT;
    } else if (PositionY >= 260 && Compass >= 0 && Compass <= 90) {
        return TURN_LEFT;
        //BOTTOM END
    } else if (PositionY <= 10 && Compass > 180 && Compass <= 270) {
        return TURN_LEFT;
    } else if (PositionY <= 10 && Compass < 180 && Compass >= 90) {
        return TURN_RIGHT;
    }
    return lastAction;
}

void Game1() {

    updateHSL();

    //----------- Postion lost? -----------
    DEBUG_MESSAGE("Game1 Loop Nr: " + to_string(cycles) + "\n", 0);
    if (PositionX == 0 && PositionY == 0) {
        PositionX = prevPosX;
        PositionY = prevPosY;
        updatePos(0.6);

        //TODO update Position
        lostPosition = true;
    } else {
        lostPosition = false;
    }
    DEBUG_MESSAGE("Lost Position? :\t" + to_string(lostPosition) + "\n", 2);

    //#####################
    // -- PATHFINDING --
    //#####################

    if (ENABLE_PATHFINDING) {
        //----------- run pathfinding -----------
        if (PathfinderGame1.path.empty()) {

            DEBUG_MESSAGE("No Waypoints left\n", 2);

            DEBUG_MESSAGE("Setting up start and goal for Pathfinding...\n", 2);
            node *start = &PathfinderGame1.map[PositionX][PositionY];
            DEBUG_MESSAGE("\tStartnode: " + to_string(start->x) + " | " + to_string(start->y) + " state: " +
                          to_string(GAME1.Map[PositionX][PositionY]) + "\n", 2);

            //TODO target choosing
            pair<int, int> nextPoint = getNextPoint(&GAME1);
            node *goal = &PathfinderGame1.map[nextPoint.first][nextPoint.second];

            DEBUG_MESSAGE("\tEndnode: " + to_string(goal->x) + " | " + to_string(goal->y) + "\n", 2);
            DEBUG_MESSAGE("finished\n", 2);
            DEBUG_MESSAGE("finding Path from: " + to_string(start->x) + " | " + to_string(start->y) + " to " +
                          to_string(goal->x) + " | " + to_string(goal->y) + " ...", 2);

            if (PathfinderGame1.findPath(start, goal, ROBOT_SIZE, ROBOT_SIZE, LoadedObjects > 0)) {
                waypoint.first = PathfinderGame1.path.back().x;
                waypoint.second = PathfinderGame1.path.back().y;
                PathfinderGame1.path.pop_back();
                DEBUG_MESSAGE(
                        "\t Next Waypoint is: " + to_string(waypoint.first) + " | " + to_string(waypoint.second) +
                        "\n",
                        2);


            } else {
                DEBUG_MESSAGE("Goal is not reachable\n", 2);
            }
            /*
            DEBUG_MESSAGE("Way: ", 0);
            for (const node &point : PathfinderGame1.path) {
                DEBUG_MESSAGE(to_string(point.x) + " | " + to_string(point.y) + "\n", 0);
            }
            */

        } else {
            DEBUG_MESSAGE("Path contains nodes\n", 2);
        }

        DEBUG_MESSAGE("\tfinished\n", 2);

        //----------- next waypoint -----------
        if (dist(waypoint.first, PositionX, waypoint.second, PositionY) < 7 ||
            (dist(waypoint.first, PositionX, waypoint.second, PositionY) < 10 && lostPosition)) {
            DEBUG_MESSAGE("Reached Waypoint... Getting next waypoint... ", 2);
            if (!PathfinderGame1.path.empty()) {
                node nextNode = PathfinderGame1.path.back();
                PathfinderGame1.path.pop_back();
                waypoint.first = nextNode.x, waypoint.second = nextNode.y;
                DEBUG_MESSAGE(
                        "\t Next Waypoint is: " + to_string(waypoint.first) + " | " + to_string(waypoint.second) + "\n",
                        2);
            }
        } else {
            DEBUG_MESSAGE("Distance to Waypoint: "
                          + to_string(dist(waypoint.first, PositionX, waypoint.second, PositionY)) + "\n", 2);
        }
    }


    CurAction = -1;
    /*--------------------
     * Priority Structure
     * -------------------
     * Deposit
     * Collect
     * dodge traps and out of bounds <- only for safety.
     * Pathfind using active collision avoidense
     *
     * */


    if (shouldDeposit() && (isOrangeLeft() || isOrangeRight())) {
        if (isOrange()) {
            deposit();
        } else if (isOrangeRight()) {
            wheels(3, 0);
        } else {
            wheels(0, 3);
        }

    } else if (shouldCollect()) {
        //TODO collect item(To be tested)
        collect();

    } else {
        DEBUG_MESSAGE("Using active Object avoidens System\n", 1);
        wheels(3, 3);
        if (ENABLE_PATHFINDING) {
            steerToPoint(waypoint.first, waypoint.second);
            LED_1 = 0;
        }
        //TODO  optimal Pathfinding -> Pathfind override should never occur
        int Override = bounds(0);
        if (Override == TURN_LEFT || (isYellowRight() && LoadedObjects > 0)) {
            DEBUG_MESSAGE("Override Pathfinding for safety: TURN LEFT\n", 1);
            wheels(5, 0);
        } else if (Override == TURN_RIGHT || (isYellowLeft() && LoadedObjects > 0)) {
            DEBUG_MESSAGE("Override Pathfinding for safety: TURN RIGHT\n", 1);
            wheels(0, 5);
        } else if (!ENABLE_PATHFINDING) {
            DEBUG_MESSAGE("Should be Case: " + to_string(checkSensors(8, 12, 8)) + "\n", 1);
            switch (checkSensors(8, 12, 8)) {
                // no obstacle
                case 0:

                    wheels(3, 3);
                    break;
                case 1: // obstacle left
                    cout << "\n" << US_Left << "\n";
                    DEBUG_MESSAGE("Turn A LITTLE BIT RIGHT (object on the left) Case: 1\n", 1);
                    wheels(-3, -5);
                    break;
                case 2: // obstacle front
                    DEBUG_MESSAGE("Turn EITHER RIGHT OR LEFT (object on the front) Case: 2\n", 1);
                    wheels(-5, -5);
                    break;
                case 3: // obstacles left & front
                    DEBUG_MESSAGE("\tTurn NORMAL RIGHT (object on the front & left) Case: 3\n", 1);
                    wheels(-3, -5);
                    break;
                case 4: // obstacle right
                    DEBUG_MESSAGE("\tTurn NORMAL LEFT (object the right) Case: 4\n", 1);
                    wheels(-5, -3);
                    break;
                case 5: // obstacles left & right
                    DEBUG_MESSAGE("\tMaybe dont turn (object on the left & right) Case: 5\n", 1);
                    wheels(3, 3);
                    break;
                case 6: // obstacles front & right
                    DEBUG_MESSAGE("\tTurn NORMAL LEFT (object on the front & right) Case: 6\n", 1);
                    wheels(-5, -3);
                    break;
                default: // obstacles everywhere
                    DEBUG_MESSAGE("\tNO WAY (object on the left & front & right) Case: 7\n", 1);
                    wheels(-5, 5);
                    break;
            }
            LED_1 = 0;
        }
        //steerToPoint(20, GAME1MAXY - 20);
        // TODO: Geht nicht wenn drehung durch geringere Motorstärke erzeugt wird (schlechte Erklärung)
        if (isSwamp()) {
            DEBUG_MESSAGE("Is in Swamp\n", 3);
            if (abs(WheelLeft) == abs(WheelRight)) {
                WheelRight *= 5;
                WheelLeft *= 5;
            }
        }

    }

    prevPosX = PositionX;
    prevPosY = PositionY;


}