#ifndef MAPDATA_HPP
#define MAPDATA_HPP

#include "libs/PPSettings.hpp"
#include "libs/PVector.hpp"

#include <vector>
#include <iostream>
#include <array>
#include <cmath>
#include <string>
#include <algorithm>
#include <queue>
#include <tuple>
#include <cstdlib>
#include <list>
#include <chrono>

//   ______            __
//  |      |.-----..--|  |.-----.
//  |   ---||  _  ||  _  ||  -__|
//  |______||_____||_____||_____|
//

/** An Object that can be collected by a robot.
 *
 * <p>A Collectible has two variables to determine it's value,
 * one variable that stores it's position and another one that
 * represents the state of the Collectible. A collectible doesn't know
 * which field it is operating on.
*/
class Collectible {
public:

    Collectible() = default;

    /*! Constructs a Collectible object
     * @tparam p Position of the Collectible.
     * @tparam c Color of the Collectible.
     * @tparam worthDouble If the Collectible is worth double the points
     */
    Collectible(const PVector &p, const unsigned int &c, bool worthDouble);

    /** Checks if this collectible could be the seen one (position wise)
     *
     * <p>The color sensors of the robots aren't the same coords as the robot's
     * ones. Therefore it is mandatory to check whether a collectible is actually
     * the right/seen one before changing it's state.
     *
     * @param robotPos the actual position of the robot given by the simulator
     * @param angle the actual angle of the robot given by the simulator
     * @param uncertainty how imprecise the position and the angle is
     * @param possibleStates which states the collectible can have. An empty vector means any state
     */
    [[nodiscard]] bool
    isCorrectCollectible(PVector robotPos, double angle, double uncertainty, std::vector<int> possibleStates) const;

    /** Represents the state
     *
     * <p>Depending on whether a robot has seen, not seen or collected
     * this collectible state is changed. Since not all collectibles
     * on the map will be available from the start it's important to
     * mark missing ones to prevent going after non existing / not
     * collectible collectibles
     *
     * @note state = 0 => never seen; state = 1 => seen; state = 2 => collected
    */
    unsigned int state = 0;

    /** Represents the number a collectible has been visited
     *
     * <p>Everytime the robot passes by this collectible, this variable
     * should be counted upwards. This allows to only mark collectibles as missing
     * if the robot has visited them a certain number of times.
    */
    unsigned int visited = 0;

    /** Represents the color
     *
     * <p>Normally a collectible can have either of the 3 colors (red, cyan, black,
     * [special case for super-objects: pink]).
     * Collectibles with different colors give different amounts of score points.
     * In addition certain combinations of colors create bonus score. To maximize
     * the score the robot needs to now which color each collectible has.
     */
    unsigned int color = 0;

    /** Tells if a collectible is worth double
     *
     * <p> Collectibles that are found in the cyan zones, also known as water,
     * are worth double the points. This counts only for the 3 common types:
     * red, cyan and black collectibles. Super-objects are worth double
     * if they are spawned by a double RGB-Bonus. This variable is used to
     * calculate what the value of a collection of collectibles is.
    */
    bool isWorthDouble = false;

    bool operator==(const Collectible &lhs) const;

    /// Represents the position
    PVector pos = {0, 0};
};

/** Contains all important map data about the map
 *
 *  It contains all information given by the map_interpreter.py script.
 *  This information is enough to fully reconstruct the map from one field-object
 *
 *  @tparam width width of the map
 *  @tparam height height of the map
 *  @tparam walls All walls
 *  @tparam traps All traps
 *  @tparam swamps All swamps
 *  @tparam waters All waters/bonus areas
 *  @tparam deposits All deposits
 *  @tparam collectibles All collectibles
 *
*/
class Field {
public:

    Field(const int &width, const int &height,
          const double &scaleX, const double &scaleY, const std::string &map,
          const std::vector<PVector> &deposits,
          const std::vector<Collectible> &collectibles);

    std::vector<PVector> AStarFindPath(PVector start, PVector end);

    Collectible *addCollectible(Collectible collectible);
    bool removeCollectible(Collectible collectible);

    void spawnTempWall(PVector pos, int r);
    void clearTempWall(double lifetime);

    Collectible *
    getCollectible(PVector pos, double angle, double uncertainty, int color, std::vector<int> possibleStates = {0, 1, 2});

    /// Getter method for collectibles
    std::vector<Collectible *> getCollectibles(const std::vector<unsigned int> &colors = {0, 1, 2, 3});


    /// Getter for the size
    [[nodiscard]] PVector getScale() const;

    int getWidth();
    int getHeight() const;

    std::string getMap();
    std::string getFlippedMap();

    /// Getter method for deposits
    std::vector<PVector> getDeposits();

    /// Returns the value as char of a point in the map, no boundary check, real world coordinates;
    char getCharAtRealPos(PVector pos);


    [[nodiscard]] int idx(int x, int y) const;
    [[nodiscard]] PVector coord(int idx) const;
    [[nodiscard]] double heuristic(int idx1, int idx2) const;

private:

    /// A list containing each temporal wall
    std::list<std::pair<std::chrono::time_point<std::chrono::steady_clock>, std::list<int>>> tempWallTiles_;

    ///
    std::string Map_;
    /// represents the dimensions of the char array
    const int width_, height_;

    /// The deposits saved as points in their respective center
    std::vector<PVector> Deposits_;

    /** contains collectibles of all colors
     * They're ordered the following: index = 0 <=> Red; 1 <=> Cyan/Green; 2 <=> Black; 3 <=> SuperObject
    */
    std::array<std::vector<Collectible>, 4> Collectibles_;


    /// the x and y scale to convert coordinates from objects map to the real world coordinates
    PVector scale_;

    /// Returns the char at map width / height, map coordinates;
    char getCharAt(int x, int y);
};


/// namespace for geometric functions
namespace geometry {

    bool isLeft(PVector p0, PVector p1, PVector p2);

    double sqDist(double x1, double y1, double x2, double y2);
    double sqDist(const PVector &p1, const PVector &p2);

    double dist(double x1, double y1, double x2, double y2);
    double dist(const PVector &p1, const PVector &p2);

    double distToLine(double p1_x, double p1_y, double p2_x, double p2_y, double p0_x, double p0_y);
    double distToLine(PVector p1, PVector p2, PVector p0);

    double dot(PVector p1, PVector p2);

    PVector angle2Vector(double a);

    double vector2Angle(PVector v);

    double vector2Angle(double x, double y);
}


///   _______                _____          __
///  |   |   |.---.-..-----.|     \ .---.-.|  |_ .---.-.
///  |       ||  _  ||  _  ||  --  ||  _  ||   _||  _  |
///  |__|_|__||___._||   __||_____/ |___._||____||___._|
///                  |__|

#define MAP_EMPTY_TILE 'E'
#define MAP_WALL_TILE 'W'
#define MAP_TRAP_TILE 'T'
#define MAP_SWAMP_TILE 'S'
#define MAP_TEMP_WALL_TILE 'w'
#define MAP_UNKNOWN_TILE 'U'

//------------- World1_Map --------------//

extern const std::vector<PVector> World1DEPOSITS;
extern const int World1MAP_WIDTH;
extern const int World1MAP_HEIGHT;
extern const std::string World1MAP;
extern const std::vector<Collectible> World1COLLECTIBLES;

//------------- World2_Map --------------//

extern const std::vector<PVector> World2DEPOSITS;
extern const int World2MAP_WIDTH;
extern const int World2MAP_HEIGHT;
extern const std::string World2MAP;
extern const std::vector<Collectible> World2COLLECTIBLES;


#endif // !MAPDATA_HPP