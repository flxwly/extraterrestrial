#include "Robot.hpp"

//====================================
//          Constructor
//====================================
Robot::Robot(int *_posX, int *_posY, int *_compass, int *_superObjectNum, int *_superObjectX, int *_superObjectY,
             int *_ultraSonicSensorLeft, int *_ultraSonicSensorFront, int *_ultraSonicSensorRight,
             int *_wheelLeft, int *_wheelRight, int *_led, int *_tp, int *_gameTime, Field *_map0, Field *_map1) :

        posX{_posX}, posY{_posY}, compass{_compass}, superObjectNum{_superObjectNum},
        superObjectX{_superObjectX}, superObjectY{_superObjectY},
        ultraSonicSensors{_ultraSonicSensorLeft, _ultraSonicSensorFront, _ultraSonicSensorRight},
        wheelLeft{_wheelLeft}, wheelRight{_wheelRight}, led{_led}, tp{_tp}, level{1}, gameTime{_gameTime},

        loadedObjects{0, 0, 0}, loadedObjectsNum{0},
        collectingSince{timer::now()}, depositingSince{timer::now()},
        pos{static_cast<double>(*_posX), static_cast<double> (*_posY)}, lastPos{-1, -1},
        lastPositionUpdate{timer::now()}, map0{_map0}, map1{_map1},
        nextTarget{-1, -1}, chasingSuperObjNum{0},
        pathfinder0{*map0, false}, pathfinder0T{*map0, true},
        pathfinder1{*map1, false}, pathfinder1T{*map1, true} {

    ERROR_MESSAGE("constructed Bot-Object")
}

//====================================
//     position/update Methods
//====================================
PVector Robot::getVelocity(long long int dt) const {

    // For clarification on how this works see
    // https://math.stackexchange.com/questions/3962859/calculate-path-of-vehicle-with-two-wheels-parallel-to-each-other

    float penalty = (isSwamp(leftColor) || isSwamp(rightColor)) ? SWAMP_SPEED_PENALITY : 1;

    float v1 = static_cast<float>(*wheelLeft) * ROBOT_SPEED / penalty;
    float v2 = static_cast<float>(*wheelRight) * ROBOT_SPEED / penalty;

    if (v1 == v2) {
        return geometry::angle2Vector(toRadians(*compass)) * v1 * static_cast<double>(dt);
    }

    double s = (ROBOT_AXLE_LENGTH * (v1 + v2)) / (2 * (v1 - v2));

    PVector vel(s * cos(v1 * static_cast<double> (dt) / (ROBOT_AXLE_LENGTH / 2 + s)) - s,
                s * sin(v1 * static_cast<double> (dt) / (ROBOT_AXLE_LENGTH / 2 + s)));
    vel.rotate(toRadians(*compass));

    return vel;
}

PVector Robot::updatePos() {

    // check if robot is in signal lost zone
    if (*posX == 0 && *posY == 0) {

        pos += getVelocity(std::chrono::duration_cast<std::chrono::milliseconds>(
                timer::now() - lastPositionUpdate).count());

        *posX = static_cast<int>(round(pos.x)), *posY = static_cast<int>(round(pos.y));

    } else {

        if (geometry::dist(pos, PVector(*posX, *posY)) > RPOS_ERROR_MARGIN) {
            pos.set(*posX, *posY);
        } else {
            pos = pos + getVelocity(std::chrono::duration_cast<std::chrono::milliseconds>(
                    timer::now() - lastPositionUpdate).count());
        }
    }

    lastPositionUpdate = timer::now();

    // return change
    return pos - lastPos;
}

//====================================
//        collect Methods
//====================================
bool Robot::shouldCollect() {

    // if the difference is less or equal to 3.5 seconds the robot is still collecting;
    if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - collectingSince).count() <= 3500)
        return true;

    // The robot is full; the robot cant collect items anyway
    if (loadedObjectsNum >= 6)
        return false;

    // The objects color is Red
    if (isRed(leftColor) || isRed(rightColor)) {
        // Since super objects count as red objects.
        // Only collect red objects if there's space including the chasing super objects
        return chasingSuperObjNum + loadedObjects[0] < 2;


    } else if (isCyan(leftColor) || isCyan(rightColor)) {
        // nothin' special here
        return loadedObjects[1] < 2;
    } else if (isBlack(leftColor) || isBlack(rightColor)) {
        // nothin' special here
        return loadedObjects[2] < 2;
    }
    // if there's no object beneath the robot, don't try to collect anything
    return false;
}

int Robot::collect() {

    // the robot is already collecting
    if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - collectingSince).count() <= 4000) {
        // This is to prevent the robot from moving
        wheels(0, 0);
        *led = 1;
        return -1;
    }
        // the robot begins to collect
    else {
        // set collectingSince to now
        collectingSince = Robot::timer::now();

        if (isRed(leftColor) || isRed(rightColor) || isSuperObj(leftColor) || isSuperObj(rightColor)) {
            loadedObjectsNum++;
            loadedObjects[0]++;
            return 0;
        } else if (isCyan(leftColor) || isCyan(rightColor)) {
            loadedObjectsNum++;
            loadedObjects[1]++;
            return 1;
        } else if (isBlack(leftColor) || isBlack(rightColor)) {
            loadedObjectsNum++;
            loadedObjects[2]++;
            return 2;
        }
    }
    return -1;
}

//====================================
//        deposit Methods
//====================================
bool Robot::shouldDeposit() {

    // while timer - depositingSince < 6the robot is still depositting
    if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - depositingSince).count() <= 4000)
        return true;

    // the robot uses a treshhold to determine if it has enough points so that it is worth it to deposit
    // This treshhold should actually not really matter since the robot is supposed to
    // only drive to the deposit area if it is fully loaded

    // basic points;
    int threshold = loadedObjects[0] * 10 + loadedObjects[1] * 15 + loadedObjects[2] * 20;

    // one rgb-bonus
    if (loadedObjects[0] > 0 && loadedObjects[1] > 0 && loadedObjects[2] > 0) {
        threshold += 90;
        // second rgb-bonus
        if (loadedObjects[0] > 1 && loadedObjects[1] > 1 && loadedObjects[2] > 1) {
            threshold += 90;

        }
    }

    // 145 = 2 red + 1 cyan + 1 black | 20 + 15 + 20 + 90
    return threshold >= 145;
}

void Robot::deposit() {
    // the robot is already depositing
    if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - depositingSince).count() <= 5000) {
        // This is to prevent the robot from moving
        wheels(0, 0);
        *led = 2;

    }
        // the robot begins to deposit
    else {
        // set deposit_since to now
        depositingSince = timer::now();

        // update the loadedObjects vars
        loadedObjects = {0, 0, 0};
        loadedObjectsNum = 0;

        wheels(0, 0);
        *led = 2;
    }
}

//====================================
//       teleport Methods
//====================================
bool Robot::shouldTeleport() {
    // for ICool Challange because there's only one world
    // while in deposit_area don't teleport
    if (isOrange(leftColor) && isOrange(rightColor)) {
        return false;
    }

    // earliest possible teleport after 3min (180sec)
    //      only teleport if it has nothing to lose -> no objects loaded
    if (remainingMapTime < 0) {
        if (loadedObjectsNum == 0) {
            return true;
        }
    }
    // next possible teleport after 3min and 20sec (200sec)
    //      only teleport if points are more valuable then 145 points
    if (remainingMapTime < -20) {
        if (!shouldDeposit()) {
            return true;
        }
    }

    // last possible teleport after 4min (240sec)
    //      teleport in any case
    return remainingMapTime < -60;
}

void Robot::teleport() {
    loadedObjectsNum = 0;
    loadedObjects = {0, 0, 0};

    *tp = 1;
    level = 1;
}

//====================================
//       movement Methods
//====================================
int Robot::avoidVoid() const {

    //Right END
    if (pos.x >= 350) {
        if (*compass > 270 && *compass <= 360)
            return -1;
        else if (*compass > 180 && *compass <= 270)
            return 1;
    }

    // LEFT END
    if (pos.x <= 10) {
        if (*compass > 0 && *compass <= 90)
            return 1;
        else if (*compass > 90 && *compass <= 180)
            return -1;
    }

    //TOP END
    if (pos.y >= 260) {
        if (*compass > 270 && *compass <= 360)
            return 1;
        else if (*compass >= 0 && *compass <= 90)
            return -1;
    }

    //BOTTOM END
    if (pos.y <= 10) {
        if (*compass > 180 && *compass <= 270)
            return -1;
        else if (*compass < 180 && *compass >= 90)
            return 1;
    }
    return 0;
}

void Robot::wheels(int l, int r) const {
    *wheelLeft = l, *wheelRight = r;
}

int Robot::moveToPosition(PVector p, bool safety) {

    double dist = geometry::dist(pos, p);
    safety = true;

    // an angle should be created that represent the difference between the point to 0;
    // It should range from -180 to 180 instead of 0 tp 360;
    double angle = toDegrees(geometry::vector2Angle(p - pos)) - 90;

    // Difference between compass
    angle -= *compass;

    // If the angle is higher then 180 the point is on the other side
    if (fabs(angle) > 180) {
        //          -> get the same angle but with another prefix
        angle = fmod(angle + ((angle > 0) ? -360 : 360), 360);
    }

    switch (Robot::checkUsSensors(6, 5, 6)) {
        // case 0 means checkUsSensors has detected no near obstacles
        //      -> the robot can move freely
        case 0:
            // the angle to posX, posY is small so there's no correction of it needed
            //      -> drive straight
            if (fabs(angle) < 10) {
                if (!safety || dist < 15) {
                    wheels(1, 1);
                } else {
                    wheels(4, 4);
                }
            }
                // the angle is a bit bigger so the robot needs to make a small correction
            else if (fabs(angle) < 20) {
                if (angle < 0) {
                    if (!safety || dist < 15) {
                        wheels(2, 1);
                    } else {
                        wheels(5, 4);
                    }
                } else {
                    if (!safety || dist < 15) {
                        wheels(1, 2);
                    } else {
                        wheels(4, 5);
                    }
                }
            }
                // the angle is quite big; now a bigger correction is needed
            else if (fabs(angle) < 60) {
                if (angle < 0) {
                    if (!safety || dist < 15) {
                        wheels(2, -1);
                    } else {
                        wheels(5, 2);
                    }
                } else {
                    if (!safety || dist < 15) {
                        wheels(-1, 2);
                    } else {
                        wheels(2, 5);
                    }
                }
            }
                // the angle is very big; a huge correction is needed
                //      -> no forward momentum; spinning
            else {
                if (angle < 0) {
                    wheels(3, -3);
                } else {
                    wheels(-3, 3);
                }
            }
            return 0;

        case 1: // obstacle left
            wheels(-2, -5);
            return 1;
        case 2: // obstacle front
            wheels(-5, -5);
            return 2;
        case 3: // obstacles left & front
            wheels(-2, -5);
            return 3;
        case 4: // obstacle right
            wheels(-5, -2);
            return 4;
        case 5: // obstacles left & right; turning would be fatal; just drive forward
            wheels(3, 3);
            return 5;
        case 6: // obstacles front & right
            wheels(-5, -2);
            return 6;
        case 7: // all sensors see an obstacle -> dead end; Just spinn
            wheels(-5, 5);
            return 7;
        default:
            return -1;
    }
}

void Robot::moveAlongPath(Path &path, bool trapSensitive) {
    PVector target = path.getClosestNormalPoint(pos, 10);

    nextTarget = target;
    moveToPosition(target, geometry::dist(path.getLast(), pos) >= 10);
    // ERROR_MESSAGE("Moving to: " + PVector::str(target))
    if (!path.isOnPath(pos)) {
        ERROR_MESSAGE("Robot is not on Path!")
    }
}

//====================================
//        Sensor Methods
//====================================
int Robot::checkUsSensors(int l, int f, int r) {
    int sum = 0;

    if (*ultraSonicSensors[0] < l)
        sum += 1;
    if (*ultraSonicSensors[1] < f)
        sum += 2;
    if (*ultraSonicSensors[2] < r)
        sum += 4;
    return sum;
}

std::vector<PVector> Robot::getPointPath(int max) {

    Field *field = (level == 0) ? map0 : map1;

    std::vector<PVector> points = {};
    PVector start = pos;
    PVector end = pos;
    int num = std::max(max - loadedObjects[0], 0) + std::max(max - loadedObjects[1], 0) + std::max(max - loadedObjects[2], 0);
    int added = 0;

    std::array<int, 3> _loadedObjects = loadedObjects;

    while (num > added) {
        unsigned int color = 0;
        double dist = INFINITY;
        for (unsigned int i = 0; i < 3; ++i) {
            for (auto collectible : field->getCollectibles({i})) {
                if ((geometry::dist(start, collectible.pos) < dist)
                    && collectible.pos != start && _loadedObjects[i] < max) {

                    color = collectible.color;
                    end = collectible.pos;
                    dist = geometry::dist(start, end);
                }
            }


        }

        for (auto superObj : superObjects) {
            if ((geometry::dist(start, superObj) * 1.5 < dist || remainingMapTime < 60)
                && superObj != start && _loadedObjects[0] < max) {

                color = 0;
                end = superObj;
                dist = geometry::dist(start, end);
            }
        }

        if (start != end) {
            start = end;
            points.push_back(end);
            _loadedObjects[color]++;
        } else {
            return points;
        }
        added++;
    }
    return points;
}

//====================================
//          loop Methods
//====================================
void Robot::updateLoop() {
    leftColor = rgb2hsl({static_cast<float>(CSLeft_R), static_cast<float>(CSLeft_G), static_cast<float>(CSLeft_B)});
    rightColor = rgb2hsl({static_cast<float>(CSRight_R), static_cast<float>(CSRight_G), static_cast<float>(CSRight_B)});

    remainingMapTime = GAME0_TIME - Time + ((level == 1) ? GAME1_TIME : 0);

    if (*superObjectX != 0 || *superObjectY != 0) {
        superObjects.emplace_back(*superObjectX, *superObjectY);
        *superObjectX = 0, *superObjectY = 0;
    }
}

void Robot::game0Loop() {


    if (Robot::shouldDeposit() && (isOrange(leftColor) || isOrange(rightColor))) {
        if (isOrange(leftColor) && isOrange(rightColor)) {
            Robot::deposit();
        } else if (isOrange(leftColor)) {
            Robot::wheels(0, 3);
        } else {
            Robot::wheels(3, 0);
        }

    } else if (Robot::shouldCollect()) {
        //Robot::collect();
    } else {
        // avoid trap on the right if objects are loaded
        if (isYellow(rightColor) && Robot::loadedObjectsNum > 0) {
            wheels(0, 5);
        }
            // avoid trap on the left
        else if (isYellow(leftColor) && Robot::loadedObjectsNum > 0) {
            wheels(5, 0);
        } else {
            switch (Robot::checkUsSensors(8, 12, 8)) {
                // no obstacle
                case 0:
                    // 4 | 4 is standard movement speed in w1
                    wheels(3, -2);
                    break;
                case 1: // obstacle left
                    wheels(4, 0);
                    break;
                case 2: // obstacle front
                    wheels(-3, -5);
                    break;
                case 3: // obstacles left & front
                    wheels(-1, -5);
                    break;
                case 4: // obstacle right
                    wheels(0, 4);
                    break;
                case 5: // obstacles left & right; turning would be fatal; just drive forward
                    wheels(3, 3);
                    break;
                case 6: // obstacles front & right
                    wheels(-5, -1);
                    break;
                case 7: // all sensors see an obstacle -> dead end; Just spinn
                    wheels(-5, 5);
                    break;
                default:
                    break;

            }
            *Robot::led = 0;
        }
    }
    // Teleport
    if (Robot::shouldTeleport()) {
        Robot::teleport();
    }
}

void Robot::game1Loop() {


    ERROR_MESSAGE("Time for one cycle: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            timer::now() - lastCycle).count()));

    // set last coords to normal coords (last coords wont get overwritten by the sim)
    lastPos.set(*posX, *posY);
    updatePos();

    // -------------------- //
    //    Get a new Path    //
    // -------------------- //
    if (completePath.empty()) {
        if (loadedObjectsNum < 6) {
            // get a path of points
            std::vector<PVector> pathOfCollectibles = getPointPath(2);

            // the first start point should be the current position of the robot
            PVector start = pos;

            // calculate a path from one point to the next
            for (unsigned int i = 0; i < pathOfCollectibles.size(); i++) {

                auto end = pathOfCollectibles[i];

                // depending on the current number of objects traps should be avoided or ignored
                Path path = (loadedObjectsNum > 0 || i > 0) ? pathfinder1T.AStar(start, end)
                                                            : pathfinder1.AStar(start, end);

                if (!path.isEmpty()) {
                    completePath.push_back(path);
                } else {
                    ERROR_MESSAGE("No Path found");
                }

                //std::cout << "Path from: " << str(start) << " to " << str(end) << std::endl;
                start = end;

            }
        } else {

            std::vector<PVector> deposits = map1->getDeposits();

            if (deposits.empty()) {
                std::cout << "NO DEPOSITS EXISTING!!!!" << std::endl;
                return;
            }

            Path path = pathfinder1T.AStar(pos, deposits.front());

            for (unsigned int i = 1; i < deposits.size(); i++) {
                Path temp = pathfinder1T.AStar(pos, deposits[i]);
                if (path.length() < temp.length() && !temp.isEmpty()) {
                    path = temp;
                }

            }

            completePath.push_back(path);

        }
    }

    // remove path if point reached
    if (geometry::dist(completePath.front().getLast(), pos) < 5) {
        completePath.erase(completePath.begin());
    }


    /* --------------------
     * Priority Structure
     * --------------------
     * Deposit (When inventory full)
     * Collect (When matching collectible is found)
     * Pathfinding
     *
     * -------------------- */


    if (shouldDeposit() && (isOrange(leftColor) || isOrange(rightColor))) {
        if (isOrange(leftColor) && isOrange(rightColor)) {
            deposit();
        } else if (isOrange(leftColor)) {
            wheels(0, 3);
        } else {
            wheels(3, 0);
        }

    } else if (shouldCollect()) {
        // 1. color will be non -1.
        // Afterwards, until collecting has finished, it will be -1
        int color = collect();

        if (color != -1) {

            Collectible *collectible = map1->getCollectible(pos, *compass, 5, color);
            std::cout << "Collectible: " << collectible << std::endl;
            if (collectible) {
                collectible->state = 0;
            }

        }

    } else {

        *led = 0;
        if (!completePath.empty()) {
            moveAlongPath(completePath.front(), loadedObjectsNum > 0);
        }

        // avoid the void by driving left || avoid trap on the right if objects are loaded
        if (avoidVoid() == -1 || (isYellow(rightColor) && loadedObjectsNum > 0)) {
            wheels(0, 3);
        }
            // avoid the void by driving right || avoid trap on the left
        else if (avoidVoid() == 1 || (isYellow(leftColor) && loadedObjectsNum > 0)) {
            wheels(3, 0);
        }
    }

    lastCycle = timer::now();
}
