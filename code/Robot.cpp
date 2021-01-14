#include "Robot.hpp"

//====================================
//          Constructor
//====================================
Robot::Robot(int *_posX, int *_posY, int *_compass, int *_superObjectNum, int *_superObjectX, int *_superObjectY,
             int *_rightColorSensorRed, int *_rightColorSensorGreen, int *_rightColorSensorBlue,
             int *_leftColorSensorRed, int *_leftColorSensorGreen, int *_leftColorSensorBlue,
             int *_ultraSonicSensorLeft, int *_ultraSonicSensorFront, int *_ultraSonicSensorRight,
             int *_wheelLeft, int *_wheelRight, int *_led, int *_tp, int *_gameTime, Field *_map0, Field *_map1) :

        posX{_posX}, posY{_posY}, compass{_compass}, superObjectNum{_superObjectNum},
        superObjectX{_superObjectX}, superObjectY{_superObjectY},
        rightColorSensors{_rightColorSensorRed, _rightColorSensorGreen, _rightColorSensorBlue},
        leftColorSensors{_leftColorSensorRed, _leftColorSensorGreen, _leftColorSensorBlue},
        ultraSonicSensors{_ultraSonicSensorLeft, _ultraSonicSensorFront, _ultraSonicSensorRight},
        wheelLeft{_wheelLeft}, wheelRight{_wheelRight}, led{_led}, tp{_tp}, gameTime{_gameTime},

        loadedObjects_{0, 0, 0}, loadedObjectsNum_{0},
        collectingSince_{timer::now()}, depositingSince_{timer::now()}, aPos_{0, 0},
        lPos_{-1, -1}, lastPositionUpdate_{timer::now()}, map0_{_map0}, map1_{_map1},
        nTarget_{-1, -1}, nTargetIsLast_{false}, chasingSuperObjNum_{0}, currentlyFollowingPath_{{}, 10},
        pathfinder0_{*map0_, false}, pathfinder0T_{*map0_, true},
        pathfinder1_{*map1_, false}, pathfinder1T_{*map1_, true} {

    ERROR_MESSAGE("constructed Bot-Object")
}

//====================================
//          Private Functions
//====================================

PVector Robot::updatePos() {

    // check if robot is in signal lost zone
    if (*posX == 0 && *posY == 0) {

        // set normal coords to last coords and update with function
        *posX = static_cast<int>(round(aPos_.x)), *posY = static_cast<int>(round(aPos_.y));

    } else {
        if (geometry::dist(aPos_, PVector(*posX, *posY)) > 5) {
            aPos_.set(*posX, *posY);
        }
    }

    aPos_ += getVelocity(std::chrono::duration_cast<std::chrono::milliseconds>(
            timer::now() - lastPositionUpdate_).count());

    // return change
    return aPos_ - lPos_;
}

//TODO: logic to should collect function
bool Robot::shouldCollect() {

    // if the difference is less or equal to 3.5 seconds the robot is still collecting;
    if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - collectingSince_).count() <= 3500)
        return true;

    // The robot is full; the robot cant collect items anyway
    if (loadedObjectsNum_ >= 6)
        return false;

    // The objects color is Red
    if (isRed()) {
        // Since super objects count as red objects.
        // Only collect red objects if there's space including the chasing super objects
        return chasingSuperObjNum_ + loadedObjects_[0] < 2;


    } else if (isCyan()) {
        // nothin' special here
        return loadedObjects_[1] < 2;
    } else if (isBlack()) {
        // nothin' special here
        return loadedObjects_[2] < 2;
    }
    // if there's no object beneath the robot, don't try to collect anything
    return false;
}

int Robot::collect() {

    // the robot is already collecting
    if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - collectingSince_).count() <= 4000) {
        // This is to prevent the robot from moving
        wheels(0, 0);
        *led = 1;
        return -1;
    }
        // the robot begins to collect
    else {
        // set collectingSince to now
        collectingSince_ = Robot::timer::now();
        wheels(0, 0);
        *led = 1;

        // update the loadedObjects vars
        Robot::loadedObjectsNum_++;

        if (isRed() || isSuperObj()) {
            loadedObjects_[0]++;
            return 0;
        } else if (isCyan()) {
            loadedObjects_[1]++;
            return 1;
        } else if (isBlack()) {
            loadedObjects_[2]++;
            return 2;
        }
    }
    return -1;
}

// TODO: logic to should deposit function
bool Robot::shouldDeposit() {

    // while timer - depositingSince < 6the robot is still depositting
    if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - depositingSince_).count() <= 4000)
        return true;

    // the robot uses a treshhold to determine if it has enough points so that it is worth it to deposit
    // This treshhold should actually not really matter since the robot is supposed to
    // only drive to the deposit area if it is fully loaded

    // basic points;
    int threshold = loadedObjects_[0] * 10 + loadedObjects_[1] * 15 + loadedObjects_[2] * 20;

    // one rgb-bonus
    if (loadedObjects_[0] > 0 && loadedObjects_[1] > 0 && loadedObjects_[2] > 0) {
        threshold += 90;
        // second rgb-bonus
        if (loadedObjects_[0] > 1 && loadedObjects_[1] > 1 && loadedObjects_[2] > 1) {
            threshold += 90;

        }
    }

    // 145 = 2 red + 1 cyan + 1 black | 20 + 15 + 20 + 90
    return threshold >= 145;
}

void Robot::deposit() {
    // the robot is already depositing
    if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - depositingSince_).count() <= 5000) {
        // This is to prevent the robot from moving
        wheels(0, 0);
        *led = 2;

    }
        // the robot begins to deposit
    else {
        // set deposit_since to now
        depositingSince_ = timer::now();

        // update the loadedObjects vars
        loadedObjects_ = {0, 0, 0};
        loadedObjectsNum_ = 0;

        wheels(0, 0);
        *led = 2;
    }
}

// teleport functions
bool Robot::shouldTeleport() {
    // for ICool Challange because there's only one world
    // while in deposit_area don't teleport
    if (isOrange()) {
        return false;
    }

    // earliest possible teleport after 3min (180sec)
    //      only teleport if it has nothing to lose -> no objects loaded
    if (*gameTime > 180) {
        if (loadedObjectsNum_ == 0) {
            return true;
        }
    }
    // next possible teleport after 3min and 20sec (200sec)
    //      only teleport if points are more valuable then 145 points
    if (*gameTime > 200) {
        if (!shouldDeposit()) {
            return true;
        }
    }

    // last possible teleport after 4min (240sec)
    //      teleport in any case
    return *gameTime > 240;
}

void Robot::teleport() {
    loadedObjectsNum_ = 0;
    loadedObjects_ = {0, 0, 0};

    *tp = 1;
}


int Robot::avoidVoid() const {

    //Right END
    if (aPos_.x >= 350) {
        if (*compass > 270 && *compass <= 360)
            return -1;
        else if (*compass > 180 && *compass <= 270)
            return 1;
    }

    // LEFT END
    if (aPos_.x <= 10) {
        if (*compass > 0 && *compass <= 90)
            return 1;
        else if (*compass > 90 && *compass <= 180)
            return -1;
    }

    //TOP END
    if (aPos_.y >= 260) {
        if (*compass > 270 && *compass <= 360)
            return 1;
        else if (*compass >= 0 && *compass <= 90)
            return -1;
    }

    //BOTTOM END
    if (aPos_.y <= 10) {
        if (*compass > 180 && *compass <= 270)
            return -1;
        else if (*compass < 180 && *compass >= 90)
            return 1;
    }
    return 0;
}

//====================================
//          Public Functions
//====================================
void Robot::wheels(int l, int r) const {
    *wheelLeft = l, *wheelRight = r;
}

int Robot::moveToPosition(PVector p, bool safety) {

    double dist = geometry::dist(aPos_, p);

    // an angle should be created that represent the difference between the point to 0;
    // It should range from -180 to 180 instead of 0 tp 360;
    double angle = toDegrees(geometry::vector2Angle(p - aPos_)) - 90;

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


void Robot::game0Loop() {

    // ====== Just for speed measure only works in second world ====== //     (leave it in for later)
    if (false) {

        double time_dif = std::chrono::duration_cast<std::chrono::milliseconds>(
                timer::now() - lastPositionUpdate_).count();

        double distance = geometry::dist(PVector(*posX, *posY), lPos_);

        double calculated = Robot::getVelocity(time_dif).getMag();

        if (measures.size() > 100) {
            measures.erase(measures.begin());
        }

        measures.push_back(calculated - distance / time_dif);

        std::cout << "time dif between measures: " << time_dif << std::endl;
        std::cout << "expected vel: " << distance / time_dif << std::endl;
        std::cout << "calculated vel: " << calculated << std::endl;

        double error = 0;
        for (double err : measures) {
            error += err;
        }
        error = error / measures.size();

        std::cout << "error: " << error << std::endl;


        // set last coords to normal coords (last coords wont get overwritten by the sim)
        lPos_.set(*posX, *posY);
        lastPositionUpdate_ = timer::now();
    }


    if (Robot::shouldDeposit() && (isOrangeLeft() || isOrangeRight())) {
        if (isOrange()) {
            Robot::deposit();
        } else if (isOrangeRight()) {
            Robot::wheels(3, 0);
        } else {
            Robot::wheels(0, 3);
        }

    } else if (Robot::shouldCollect()) {
        //Robot::collect();
    } else {
        // avoid trap on the right if objects are loaded
        if (isYellowRight() && Robot::loadedObjectsNum_ > 0) {
            wheels(0, 5);
        }
            // avoid trap on the left
        else if (isYellowLeft() && Robot::loadedObjectsNum_ > 0) {
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
            timer::now() - lastCycle_).count()));

    // set last coords to normal coords (last coords wont get overwritten by the sim)
    updatePos();
    lPos_.set(*posX, *posY);
    lastPositionUpdate_ = timer::now();

    //#####################
    //  TODO -- PATHFINDING --
    //#####################

    // There's no path to follow
    if (completePath.empty()) {

        // get a path of points
        std::vector<PVector> pathOfCollectibles = map1_->getPointPath();

        // the first start point should be the current position of the robot
        PVector start = aPos_;

        pathOfCollectibles = {{150, 125}};

        // calculate a path from one point to the next
        for (int i = 0; i < pathOfCollectibles.size(); i++) {

            auto end = pathOfCollectibles[i];

            // depending on the current number of objects traps should be avoided or ignored
            Path path = (loadedObjectsNum_ > 0 || i > 0) ? pathfinder1T_.AStar(start, end)
                    : pathfinder1_.AStar(start, end);


            if (!path.isEmpty()) {
                // add the path to the complete path
                // the first path is at the front of the vector
                completePath.push_back(path);

            } else {
                ERROR_MESSAGE("No Path found");
            }

            //std::cout << "Path from: " << str(start) << " to " << str(end) << std::endl;
            start = end;
        }
    }

    // remove path if reached
    if (geometry::dist(completePath.front().getLast(), aPos_) < 5) {
        ERROR_MESSAGE("--- Reached path end! ---")
        completePath.erase(completePath.begin());
    }


    /*--------------------
     * Priority Structure
     * -------------------
     * Deposit
     * Collect
     * Dodge Traps and Void
     * Pathfinding
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

    } else if (shouldCollect() && false) {
        int color = collect();

        // if the robot is collecting take some time
        if (color != -1) {
            // set state of collected collectible to 0

            Collectible *collectible = map1_->getCollectible(aPos_, *compass, 2, color);
            if (collectible) {
                collectible->state = 0;
            }
        }

    } else {

        *led = 0;
        if (!completePath.empty()) {
            moveAlongPath(completePath.front());
        }

        // avoid the void by driving left || avoid trap on the right if objects are loaded
        if (avoidVoid() == -1 || (isYellowRight() && loadedObjectsNum_ > 0)) {
            wheels(0, 5);
        }
            // avoid the void by driving right || avoid trap on the left
        else if (avoidVoid() == 1 || (isYellowLeft() && loadedObjectsNum_ > 0)) {
            wheels(5, 0);
        }
    }


    lastCycle_ = timer::now();
}

double Robot::getBrakingDistance(double friction) {
    return static_cast<double>(*wheelLeft + *wheelRight) / 2;
}

PVector Robot::getVelocity(long long int dt) const {

    // For clarification on how this works see
    // https://math.stackexchange.com/questions/3962859/calculate-path-of-vehicle-with-two-wheels-parallel-to-each-other

    double v1 = *wheelLeft * ROBOT_SPEED;
    double v2 = *wheelRight * ROBOT_SPEED;

    if (v1 == v2) {
        return (geometry::angle2Vector(*compass) * v1) * static_cast<double>(dt);
    }

    double s = (ROBOT_AXLE_LENGTH * (v1 + v2)) / (2 * (v1 - v2));

    PVector vel(s * cos(v1 * static_cast<double> (dt) / (ROBOT_AXLE_LENGTH / 2 + s)) - s,
                s * sin(v1 * static_cast<double> (dt) / (ROBOT_AXLE_LENGTH / 2 + s)));
    vel.rotate(toRadians(*compass));

    return vel;
}

void Robot::moveAlongPath(Path &path) {
    PVector target = path.getClosestNormalPoint(aPos_, 10);

    nTarget_ = target;
    moveToPosition(target, geometry::dist(path.getLast(), aPos_) >= 10);
    // ERROR_MESSAGE("Moving to: " + PVector::str(target))
    if (!path.isOnPath(aPos_)) {
        ERROR_MESSAGE("Robot is not on Path!")
    }
}
