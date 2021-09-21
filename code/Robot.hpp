#ifndef EXTRATERRESTRIAL_ROBOT_HPP
#define EXTRATERRESTRIAL_ROBOT_HPP

#include <list>
#include <chrono>

#include "libs/ColorRecognition.hpp"
#include "libs/PVector.hpp"
#include "libs/PPSettings.hpp"
#include "MapData.hpp"
#include "Pathfinding.hpp"

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
    PVector desiredVelocity = {1, 0};

    Field map0 = Field(270, 180, GAME0WALLS, GAME0TRAPS, GAME0SWAMPS, GAME0WATERS, GAME0DEPOSITS,
                       GAME0WALLNODES, GAME0TRAPNODES, GAME0SWAMPNODES, GAME0COLLECTIBLES),
            map1 = Field(360, 270, GAME1WALLS, GAME1TRAPS, GAME1SWAMPS, GAME1WATERS, GAME1DEPOSITS,
                         GAME1WALLNODES, GAME1TRAPNODES, GAME1SWAMPNODES, GAME1COLLECTIBLES);

    AStar pathfinderMap0 = AStar(map0.getMapObjects({0}), map0.getMapNodes({0}),
                                 map0.getMapObjects({1}), map0.getMapNodes({1}),
                                 map0.getMapObjects({2}), map0.getMapNodes({2})),
            PathfinderMap1 = AStar(map1.getMapObjects({0}), map1.getMapNodes({0}),
                                   map1.getMapObjects({1}), map1.getMapNodes({1}),
                                   map1.getMapObjects({2}), map1.getMapNodes({2}));


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
    std::vector<Collectible *> getCollectiblePath(const CollectibleLoad &desiredLoad, bool finishOnDeposit);


    /*! Let's the robot move towards a position
     *
     * @param position the position the robot should move to
     */
    void moveTo(PVector position);

};


#endif //EXTRATERRESTRIAL_ROBOT_HPP
