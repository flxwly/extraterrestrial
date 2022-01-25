#include "Robot.hpp"

const int Wheels::minVel;
const int Wheels::maxVel;

Robot::Robot(volatile int *pIn, volatile int *pOut) {
    In = pIn;
    Out = pOut;
}

void Robot::Update() {

    ROBOT_LOG("Robot is updating the variables")

    // Update in information
    ultraSonicSensors.set(In[1], In[0], In[2]);
    colorSensors.set(
            rgb2hsl({static_cast<float>(In[3]), static_cast<float>(In[4]), static_cast<float>(In[5])}),
            rgb2hsl({static_cast<float>(In[6]), static_cast<float>(In[7]), static_cast<float>(In[8])}));
    int tm_state = In[11];
    compass = In[12];
    time = In[13];

    // Update position
    if (In[9] != 0 or In[10] != 0) {
        simPos.set(In[9], In[10]);
    } else {
        simPos += getVelocity() *
                  std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - lastCycle).count();
    }

    // Update out information
    Out[0] = wheels.l;
    Out[1] = wheels.r;
    Out[2] = led;

#ifdef ROBOT_UPDATE_DUMB
    ROBOT_LOG("Ultra sonic sensors: " << ultraSonicSensors.l << " | " << ultraSonicSensors.f << " | "
                                      << ultraSonicSensors.r)
    ROBOT_LOG(
            "Color sensors: " << "\n\t\thue: " << colorSensors.r.h << " | " << colorSensors.l.h << "\n\t\tsaturation: "
                              << colorSensors.r.s << " | " << colorSensors.l.s << "\n\t\tluminance: "
                              << colorSensors.r.l << " | " << colorSensors.l.l)
    ROBOT_LOG("Pos: " << simPos.x << " | " << simPos.y)
    ROBOT_LOG("TM_STATE: " << tm_state)
    ROBOT_LOG("Compass: " << compass)
    ROBOT_LOG("Time: " << time)

#endif

    // Update remaining Map time

    remainingMapTime = max((level == 0) ? GAME0_TIME - time : GAME1_TIME - time, 0);

    // Update cycle time
    lastCycle = std::chrono::steady_clock::now();
}

void Robot::Game0() {

    wheels.set(-10, 10);

    //Teleport();
    ROBOT_LOG("Robot is running game0")

    // Add moving objects / things that are not in the map
    for (auto collision: ultraSonicContactPosition()) {
        if (collision) {
            if (map0.getMapAtRealPos(collision) == MAP_EMPTY_TILE) {
                map0.spawnTempWall(collision, 1);
            }
        }
    }
    // Remove temp walls that have been around for some time
    map0.clearTempWall(4000);
}

void Robot::Game1() {

    ROBOT_LOG("Robot is running game1")
    wheels.set(0, 0);
    led = 0;


    ROBOT_LOG("Temp walls")
    // Add moving objects / things that are not in the map
    for (auto collision: ultraSonicContactPosition()) {
        if (collision) {

            if (0 <= collision.x && collision.x < map1.getWidth() * map1.getScale().x
                && 0 <= collision.y && collision.y < map1.getHeight() * map1.getScale().y) {
                if (map1.getMapAtRealPos(collision) == MAP_EMPTY_TILE) {
                    map1.spawnTempWall(collision, 1);
                }
            } else {
                ROBOT_WARNING("Collision is out of bounds")
            }

        }
    }

    // Remove temp walls that have been around for some time
    map1.clearTempWall(2000);


    // Pathfinding
    if (path.empty() || geometry::dist(path.back(), simPos) < 5) {
        ROBOT_LOG("Running path finding")

        std::vector<Collectible *> collectibles = getCollectiblePath(getDesiredLoad(), map1.getCollectibles(), false);
        if (collectibles.empty()) {
            ROBOT_ERROR("No Collectible Path found!")
        } else {
            path = map1.AStarFindPath(simPos, collectibles.front()->pos);
        }


        //path = map1.AStarFindPath(simPos, collectibles.back()->pos);
        ROBOT_LOG("PATH: ")
        for (auto p: path) {
            ROBOT_LOG("\t" + p.str())
        }
    }
    if (path.empty()) {
        ROBOT_WARNING("No path")
        wheels.set(0, 0);
        return;
    }

    followPath(path);


//    moveTo(path.back());
//    if (geometry::dist(path.back(), simPos) < 5) {
//        path.pop_back();
//    }


//    debugger.clear();
//    debugger.paintBuffer(map1.getFlippedMap(), map1.getWidth(), 10, 10);
//    for (auto p : path) {
//        debugger.paintCircle(p.x, p.y, 2, UnicodeChar('A', FOREGROUND_BLUE));
//    }
//    debugger.printToConsole();

}

PVector Robot::collisionAvoidance(double maxForce) {

    PVector force = {0, 0};

    //TODO: Tweak constants
    const double G = 1;
    const double C = 1;

    force += geometry::angle2Vector(compass - ULTRASONIC_SENSOR_ANGLE_OFFSET - 180).setMag(
            G * C / (ultraSonicSensors.l * ultraSonicSensors.l));
    force += geometry::angle2Vector(compass - 180).setMag(G * C / (ultraSonicSensors.f * ultraSonicSensors.f));
    force += geometry::angle2Vector(compass + ULTRASONIC_SENSOR_ANGLE_OFFSET - 180).setMag(
            G * C / (ultraSonicSensors.r * ultraSonicSensors.r));

    if (force.getMag() > maxForce) {
        force.setMag(maxForce);
    }

    return force;
}

PVector Robot::getVelocity() {

    // For clarification on how this works see
    // https://math.stackexchange.com/questions/3962859/calculate-path-of-vehicle-with-two-wheels-parallel-to-each-other

    double penalty = (isSwamp(colorSensors.l) || isSwamp(colorSensors.r)) ? SWAMP_SPEED_PENALTY : 1;

    if (wheels.l == wheels.r) {
        return geometry::angle2Vector(static_cast<double>(compass + 90) / 180 * M_PI) *
               (static_cast<double>(wheels.l) * ROBOT_SPEED / penalty);
    }

    // v1 and v2 are the linear velocities at the wheels
    double v1 = LWHEEL_RADIUS * REVPERMS * static_cast<double>(wheels.l) / penalty;
    double v2 = RWHEEL_RADIUS * REVPERMS * static_cast<double>(wheels.r) / penalty;

    double s = (ROBOT_AXLE_LENGTH * (v1 + v2)) / (2 * (v1 - v2));

    PVector vel = (v1 != 0) ?
                  PVector(-s * cos(v1 / (ROBOT_AXLE_LENGTH / 2 + s)) + s,
                          s * sin(v1 / (ROBOT_AXLE_LENGTH / 2 + s))) :

                  PVector(-s * cos(v2 / (ROBOT_AXLE_LENGTH / 2 - s)) + s,
                          s * sin(v2 / (ROBOT_AXLE_LENGTH / 2 - s)));

    ROBOT_LOG("WheelLeft: " + std::to_string(v1))
    ROBOT_LOG("WheelLeft: " + std::to_string(v2))

    vel.rotate(compass * M_PI / 180);

    return vel;
}

void Robot::moveTo(PVector position, double speed) {

    double angleDif = std::fmod(geometry::vector2Angle(simPos - position) * 180 / M_PI - compass + 90, 360);

    ROBOT_LOG("Angle dif: " << angleDif << " | compass: " << compass)

    if (angleDif > 180) {
        angleDif -= 360;
    } else if (angleDif < -180) {
        angleDif += 360;
    }


    int action = -1;
    if (std::fabs(angleDif) < 7.5) {
        action = 0;
    } else if (std::fabs(angleDif) < 15) {
        action = 1;
    } else if (std::fabs(angleDif) < 30) {
        action = 2;
    } else if (std::fabs(angleDif) < 45) {
        action = 3;
    } else if (std::fabs(angleDif) < 60) {
        action = 4;
    } else if (std::fabs(angleDif) < 75) {
        action = 5;
    } else {
        action = 16;
    }

    const int div = (isSwamp(colorSensors.l) || isSwamp(colorSensors.r)) ? 1 : 16;

    double magnitude = std::pow(2, speed) / div;

    ROBOT_LOG("Magnitude: " << magnitude)

    switch (action) {
        case 0:
            wheels.set(magnitude * 100, magnitude * 100);
            break;
        case 1:
            if (angleDif > 0) {
                wheels.set(magnitude * 90, magnitude * 100);
            } else {
                wheels.set(magnitude * 100, magnitude * 90);
            }
            break;
        case 2:
            if (angleDif > 0) {
                wheels.set(magnitude * 50, magnitude * 100);
            } else {
                wheels.set(magnitude * 100, magnitude * 50);
            }
            break;
        case 3:
            if (angleDif > 0) {
                wheels.set(magnitude * -20, magnitude * 100);
            } else {
                wheels.set(magnitude * 70, magnitude * -70);
            }
            break;
        case 4:
            if (angleDif > 0) {
                wheels.set(magnitude * -40, magnitude * 70);
            } else {
                wheels.set(magnitude * 70, magnitude * -40);
            }
            break;
        case 5:
            if (angleDif > 0) {
                wheels.set(magnitude * -80, magnitude * 80);
            } else {
                wheels.set(magnitude * 80, magnitude * -80);
            }
            break;
        default:
            if (angleDif > 0) {
                wheels.set(magnitude * -100, magnitude * 100);
            } else {
                wheels.set(magnitude * 100, magnitude * -100);
            }
            break;
    }


}

void Robot::followPath(std::vector<PVector> local_path) {
    // Predict position 50 (arbitrary choice) frames ahead
    // This could be based on speed
    PVector predictpos = simPos + getVelocity().setMag(10);

    // Now we must find the normal to the local_path from the predicted position
    // We look at the normal for each line segment and pick out the closest one

    PVector target;
    double worldRecord = INFINITY;  // Start with a very high record distance that can easily be beaten

    // Loop through all points of the local_path
    for (int i = 0; i < local_path.size() - 1; i++) {

        // --------------

        // Look at a line segment
        PVector a = local_path.at(i);
        PVector b = local_path.at(i + 1);

        // Vector from a to b
        PVector ab = (b - a);
        ab.normalize();
        PVector normalPoint = a + (ab * PVector::dot(predictpos - a, ab));

        // --------------

        // Rotate the line segment by 90° to get an orthogonal line to the segment
        ab.rotate(M_PI / 2);

        if (geometry::isLeft(a, a + ab, normalPoint) || !geometry::isLeft(b, b + ab, normalPoint)) {
            // This is something of a hacky solution, but if it's not within the line segment
            // consider the normal to just be the end of the line segment (point b)
            normalPoint = b;
        }

        // How far away are we from the local_path?
        double distance = geometry::dist(predictpos, normalPoint);
        // Did we beat the record and find the closest line segment?
        if (distance < worldRecord) {
            worldRecord = distance;

            // Look at the direction of the line segment so we can seek a little bit ahead of the normal
            PVector dir = b - a;

            // This is an oversimplification
            // TODO: Should be based on distance to local_path & velocity
            dir.setMag(10);
            target = normalPoint;
            target += dir;
        }
    }

    if (target) {
        if (geometry::dist(target, local_path.back()) < 5) {
            moveTo(target, 1);
        } else {
            moveTo(target, 4);
        }
    } else {
        ROBOT_ERROR("No target for local_path following")
        wheels.set(0, 0);
    }
}

std::vector<Collectible *>
Robot::getCollectiblePath(std::array<unsigned int, 4> desiredLoad, std::vector<Collectible *> collectibles,
                          bool finishOnDeposit) {

    // the point path
    std::vector<Collectible *> chosenCollectibles = {};

    // start and end position of one point path segment
    PVector start = simPos;

    // number of objects that need to be added to the point path
    int num = -static_cast<int> (loadedCollectibles.num());
    for (auto item: desiredLoad) {
        num += static_cast<int>(item);
    }

    ROBOT_LOG("Collectibles to add: " << num)

    // copy of loaded objects array that can be modified
    auto tLoadedObjects = loadedCollectibles;

    // add a certain number of objects and super objects to the point path
    for (unsigned int iter = 0; iter < num; iter++) {
        Collectible *curCollectible = nullptr;
        unsigned int color = 0;
        double dist = INFINITY;

        // ------------- objects [0 - red, 1 - cyan, 2 - black, 3 - super] --------------------
        for (unsigned int i = 0; i < 4; ++i) {

            // If there are already enough collectibles of the color added
            if (tLoadedObjects.getColor(i).size() >= desiredLoad.at(i)) {
                continue;
            }

            // loop through all collectibles
            for (Collectible *collectible: collectibles) {

                // Find the closest to the start location or the last collectible
                // that hasn't been collected already
                if (geometry::dist(start, collectible->pos) >= dist || collectible->state == 2) {
                    continue;
                }

                // It should not be chosen already
                if (std::find(chosenCollectibles.begin(), chosenCollectibles.end(), collectible) ==
                    chosenCollectibles.end()) {

                    // update the curCollectible pointer and the dist
                    curCollectible = collectible;
                    dist = geometry::dist(start, curCollectible->pos);
                }
            }

        }

        // -------------- add object / super object to point path ----------------
        if (curCollectible) {
            start = curCollectible->pos;
            chosenCollectibles.push_back(curCollectible);
            tLoadedObjects.add(curCollectible);

            ROBOT_LOG("Added Collectible at " << curCollectible->pos << " to collectible path")

        } else {
            ROBOT_WARNING("Got nullptr")
            return chosenCollectibles;
        }

    }
    return chosenCollectibles;
}

void Robot::Teleport() {
    level = 1;
    loadedCollectibles.clear();
}

// TODO: Not working as intended
std::array<PVector, 3> Robot::ultraSonicContactPosition() {

    std::array<PVector, 3> array = {PVector(NAN, NAN), PVector(NAN, NAN), PVector(NAN, NAN)};

    if (ultraSonicSensors.l < 100) {
        array[0] = simPos +
                   geometry::angle2Vector((compass + ULTRASONIC_SENSOR_ANGLE_OFFSET) * M_PI / 180) *
                   ultraSonicSensors.l;
    }

    if (ultraSonicSensors.f < 100) {
        array[1] = simPos + geometry::angle2Vector((compass) * M_PI / 180) *
                            (ultraSonicSensors.f + ULTRASONIC_SENSOR_DIST_TO_CORE);
    }

    if (ultraSonicSensors.r < 100) {
        array[2] = simPos + geometry::angle2Vector((compass - ULTRASONIC_SENSOR_ANGLE_OFFSET) * M_PI / 180) *
                            (ultraSonicSensors.r + ULTRASONIC_SENSOR_DIST_TO_CORE);
    }

    return array;
}

std::array<unsigned int, 4> Robot::getDesiredLoad() {

    Field *field = (level == 0) ? &map0 : &map1;
    std::array<unsigned int, 4> desiredLoad = {};

    const unsigned int reds = (field->getCollectibles({0}).size());
    const unsigned int cyans = (field->getCollectibles({1}).size());
    const unsigned int blacks = (field->getCollectibles({2}).size());
    const unsigned int supers = (field->getCollectibles({3}).size());

    // Max points run
    if (supers >= 3 || remainingMapTime < 40) {

        desiredLoad[3] = min(supers, static_cast<unsigned int> (6));
        desiredLoad[2] = min(blacks, 6 - desiredLoad[3]);
        desiredLoad[1] = min(cyans, 6 - desiredLoad[3] - desiredLoad[2]);
        desiredLoad[0] = min(reds, 6 - desiredLoad[3] - desiredLoad[2] - desiredLoad[1]);

        return desiredLoad;
    }


    desiredLoad[3] = 0;
    desiredLoad[0] = min(reds, static_cast<unsigned int> (2));
    desiredLoad[1] = min(cyans, static_cast<unsigned int> (2));
    desiredLoad[2] = min(blacks, static_cast<unsigned int> (2));

    return desiredLoad;
}
