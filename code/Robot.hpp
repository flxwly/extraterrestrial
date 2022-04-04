#ifndef EXTRATERRESTRIAL_ROBOT_HPP
#define EXTRATERRESTRIAL_ROBOT_HPP

#include <list>
#include <chrono>
#include <Windows.h>

#include "libs/ColorRecognition.hpp"
#include "libs/PVector.hpp"
#include "libs/PPSettings.hpp"
#include "libs/ConsolePainter.hpp"
#include "MapData.hpp"

struct Wheels {

    const static int minVel = -100;
    const static int maxVel = 100;

    int l = 0, r = 0;

    void set(int pL, int pR) {
        l = std::max(minVel, std::min(pL, maxVel));
        r = std::max(minVel, std::min(pR, maxVel));
    };

    void accelerate(int pL, int pR) {
        l += (pL + l < minVel) ? minVel - l : ((pL + l > maxVel) ? maxVel - l : pL);
        r += (pR + r < minVel) ? minVel - r : ((pR + r > maxVel) ? maxVel - r : pR);
    };
};


struct ColorSensors {
    HSLColor l, r = {0, 0, 0};

    void set(HSLColor pL, HSLColor pR) {
        l = pL;
        r = pR;
    }
};

struct UltraSonicSensors {
    int l = 0, f = 0, r = 0;

    void set(int pL, int pF, int pR) {
        l = pL;
        f = pF;
        r = pR;
    }
};

struct CollectibleLoad {
private:
    unsigned int _num = 0;
    unsigned int _value = 0;
    std::array<std::vector<Collectible *>, 4> _load = {};

public:
    unsigned int num() {
        return _num;
    }

    std::vector<Collectible *> getColor(unsigned int color) {
        switch (color) {
            default:
                return {};
            case 0:
                return _load[0];
            case 1:
                return _load[1];
            case 2:
                return _load[2];
            case 3:
                return _load[3];
        }
    }

    unsigned int value() {
        return _value + 90 * rgb();
    }

    unsigned int rgb() {
        return std::min({_load[0].size(), _load[1].size(), _load[2].size()});
    }

    void add(Collectible *collectible) {
        if (collectible) {
            _load[collectible->color].push_back(collectible);
            _num++;

            switch (collectible->color) {
                case 0:
                    _value += ((collectible->isWorthDouble) ? 20 : 10);
                    break;
                case 1:
                    _value += ((collectible->isWorthDouble) ? 30 : 15);
                    break;
                case 2:
                    _value += ((collectible->isWorthDouble) ? 40 : 20);
                    break;
                case 3:
                    _value += ((collectible->isWorthDouble) ? 180 : 90);
                    break;
                default:
                    break;
            }
        }
    }

    void clear() {
        for (auto &color : _load) {
            color = {};
        }
        _num = 0;
        _value = 0;
    }
};


/** Represents one Robot in the CoSpace Simulator
 *
 * <p>
 *
 * @tparam
 */

class Robot {

public:
    volatile int *In, *Out = nullptr;

    // In vars
    ColorSensors colorSensors;
    UltraSonicSensors ultraSonicSensors;
    PVector simPos;
    int compass = 0;
    int time = 0;


    // Out vars
    Wheels wheels;
    int led = 0;

    // Own vars
    int level = 0;
    CollectibleLoad loadedCollectibles;

    Field map0 = Field(World1MAP_WIDTH, World1MAP_HEIGHT, static_cast<double > (World1MAP_WIDTH) / REAL_GAME0MAP_WIDTH,
                       static_cast<double > (World1MAP_HEIGHT) / REAL_GAME0MAP_HEIGHT, World1MAP, World1DEPOSITS,
                       World1COLLECTIBLES),
            map1 = Field(World2MAP_WIDTH, World2MAP_HEIGHT, static_cast<double > (World2MAP_WIDTH) / REAL_GAME1MAP_WIDTH,
                         static_cast<double > (World2MAP_HEIGHT) / REAL_GAME1MAP_HEIGHT, World2MAP, World2DEPOSITS,
                         World2COLLECTIBLES);
    std::vector<PVector> path;

    ConsolePainter debugger = ConsolePainter("Debug");

    std::chrono::time_point<std::chrono::steady_clock> lastCycle;
    std::chrono::time_point<std::chrono::steady_clock> timeStartedCollecting;
    std::chrono::time_point<std::chrono::steady_clock> timeStartedDepositing;

    /*! Constructs a Robot object
     *
     * @param pIn
     * @param pOut
     */
    Robot(volatile int *pIn, volatile int *pOut);


    // Methods
    void Update();

    void Game0();

    void Game1();

    void Teleport();

    std::array<int, 4> getDesiredLoad();
    bool shouldCollect();
    int collect();

    /*! Calculates the current velocity of the robot
     *
     * @return A PVector representing the speed in cm/ms
     */
    PVector getVelocity();

    /*! Calculates a vector that tells the robot in what direction it should moveTo to avoid walls
     *
     * @param maxForce
     * @return a PVector
     */
    PVector collisionAvoidance(double maxForce);

    /*! Calculates a path consisting of Collectibles and one optional deposit at the end
     *
     * @param desiredLoad The collectibles the robot should have after completing the calculated route
     * @param finishOnDeposit Whether the path should end on a deposit or not
     * @return a path of pointers to collectibles
     */
    std::vector<Collectible *> getCollectiblePath(std::array<unsigned int, 4> desiredLoad, std::vector<Collectible *> collectibles, bool finishOnDeposit);


    /*! Lets the robot move towards a position
     *
     * @param position the position the robot should move to
     * @param speed
     */
    void moveTo(PVector position, double speed);

    void followPath(std::vector<PVector> local_path);

    std::array<PVector, 3> ultraSonicContactPosition();
};


#endif //EXTRATERRESTRIAL_ROBOT_HPP
