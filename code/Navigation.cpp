#include "Navigation.hpp"
#include "CoSpaceFunctions.hpp"
#include "CommonFunctions.hpp"

using namespace std;

void updatePos(double speed) {
    if (WheelRight != WheelLeft) {
        return;
    }

    speed = static_cast<double> (WheelLeft + WheelRight) / 2 * speed;

    double angle = toRadians(Compass);

    PositionX += dround(-sin(angle) * speed);
    PositionY += dround(cos(angle) * speed);
}

/*bool isInArea(int x1, int y1, int x2, int y2, int posX, int posY) {
    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = x1;
    }
    if (y1 > y2) {
        int temp = y1;
        y1 = y2;
        y2 = y1;
    }
    return x1 > posX && posX > x2 && y1 > posY && posY > y2;
}*/

int checkSensors(int dist_left, int dist_front, int dist_right) {
    int result = 0;
    result += (US_Left < dist_left) ? 1 : 0;
    result += (US_Front < dist_front) ? 2 : 0;
    result += (US_Right < dist_right) ? 4 : 0;
    return result;
}

int invertAngle(int angle) {
    angle += (angle > 0) ? -360 : 360;
    return angle % 360;
}

int oppositeAngle(int angle) {
    return -angle + 360;
}

void steerToPoint(int x, int y) { // control wheels depending on obstacles and compass
    if(dist(PositionX, x, PositionY, y) < 5)
        return;
    int angle = dvector2Angle(x - PositionX , y - PositionY);
    DEBUG_MESSAGE("Drive from: " + to_string(PositionX) + " | " + to_string(PositionY) + "\t to: "
    + to_string(x) + " | " + to_string(y) + " with angle: " + to_string(angle) + "\n", 0);

    // Difference counterclockwise
    angle -= Compass;

    //angle %= 360;
    DEBUG_MESSAGE("need to turn " + to_string(angle) + "...\t", 0);

    // if abs angle is higher than 180 get same angle with opposite vorzeichen to turn in the other direction
    // angle changes rotation direction so multiplying doesnt work
    if (abs(angle) > 180) {
        angle = invertAngle(angle);
    }

    // turn dependend on obstacles
    switch (checkSensors(10, 10, 10)) {
        // no obstacle
        case 0:
            // the angle is pretty straight
            if (abs(angle) < 10) {
                DEBUG_MESSAGE("No turning required\n", 0);
                if (dist(PositionX, x, PositionY, y) < 15) {
                    wheels(2, 2);
                } else {
                    wheels(5, 5);
                }
                // slightly to the left or right
            } else if (abs(angle) < 30) {
                DEBUG_MESSAGE("Turn normal\n", 0);
                if (angle < 0) {
                    if (dist(PositionX, x, PositionY, y) < 15) {
                        wheels(2, 1);
                    } else {
                        wheels(5, 4);
                    }
                } else {
                    if (dist(PositionX, x, PositionY, y) < 15) {
                        wheels(1, 2);
                    } else {
                        wheels(4, 5);
                    }
                }
                // little more to the left or right
            } else if (abs(angle) < 60) {
                DEBUG_MESSAGE("Turning fast\n", 0);
                if (angle < 0) {
                    if (dist(PositionX, x, PositionY, y) < 15) {
                        wheels(2, -1);
                    } else {
                        wheels(5, 2);
                    }
                } else {
                    if (dist(PositionX, x, PositionY, y) < 15) {
                        wheels(-1, 2);
                    } else {
                        wheels(2, 5);
                    }
                }
                // going nuts to the left or right
            } else {
                DEBUG_MESSAGE("Spinning\n", 0);
                if (angle < 0) {
                    wheels(3, -3);
                } else {
                    wheels(-3, 3);
                }
            }
            break;

        case 1: // obstacle left
            DEBUG_MESSAGE("Turn A LITTLE BIT RIGHT (object on the left) Case: 1\n", 0);
            wheels(-3, -5);
            break;
        case 2: // obstacle front
            DEBUG_MESSAGE("Turn EITHER RIGHT OR LEFT (object on the front) Case: 2\n", 0);
            wheels(-5, -5);
            break;
        case 3: // obstacles left & front
            DEBUG_MESSAGE("\tTurn NORMAL RIGHT (object on the front & left) Case: 3\n", 0);
            wheels(-3, -5);
            break;
        case 4: // obstacle right
            DEBUG_MESSAGE("\tTurn NORMAL LEFT (object the right) Case: 4\n", 0);
            wheels(-5, -3);
            break;
        case 5: // obstacles left & right
            DEBUG_MESSAGE("\tMaybe dont turn (object on the left & right) Case: 5\n", 0);
            wheels(3, 3);
            break;
        case 6: // obstacles front & right
            DEBUG_MESSAGE("\tTurn NORMAL LEFT (object on the front & right) Case: 6\n", 0);
            wheels(-5, -3);
            break;
        case 7: // obstacles everywhere
            DEBUG_MESSAGE("\tNO WAY (object on the left & front & right) Case: 7\n", 0);
            wheels(-5, 5);
            break;
        default:
            break;
    }
    DEBUG_MESSAGE("Moving with: " + to_string(WheelLeft) + " | " + to_string(WheelRight) + "\n", 0);
}

void steerToPoint(pair<int, int> p) {
    steerToPoint(p.first, p.second);
} 