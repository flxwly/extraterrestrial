#ifndef MAPDATA_HPP
#define MAPDATA_HPP

#include "libs/PPSettings.hpp"
#include "libs/CommonFunctions.hpp"
#include "libs/PVector.hpp"

#include <vector>
#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>

/** A type of PVector that can be collected by a robot.
 *
 * <p>Collectibles have a color value in addition to normal points in
 * 2D-plane. They can be picked up by the robot in the CoSpace-SE.
 * However the robot can't interact directly with this program and the map
 * the robot uses is actually created by the @b map_interpreter.py script.
 * This can result in imprecise movement and calculations. The method
 * @b Collectible::isCorrectCollectible() resolves this problem taking
 * the imprecision into account.
 *
 * @tparam p Position of the Collectible.
 * @tparam c Color of the Collectible.
 *
 * @note Generally Collectibles are only created upon initializing members
 * of the field class. The only exception is made when a super-object spawns
*/
class Collectible {
public:
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
     */
    [[nodiscard]] bool isCorrectCollectible(PVector robotPos, double angle, double uncertainty) const;

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
    unsigned int state;

    /** Represents the number a collectible has been visited
     *
     * <p>Everytime the robot passes by this collectible, this variable
     * should be counted upwards. This allows to only mark collectibles as missing
     * if the robot has visited them a certain number of times
    */
    unsigned int visited;

    /** Represents the color
     *
     * <p>Normally a collectible can have either of the 3 colors (red, cyan, black,
     * [special case for superobjects: pink]).
     * Collectibles with different colors give different amounts of score points.
     * In addition certain combinations of colors create bonus score. To track
     * maximize the score the robot needs to now which color each object.
     */
    unsigned int color;

    bool isWorthDouble;

    bool operator==(const Collectible &lhs) const;

    /// Represents the position
    PVector pos;
};

/** A Line in a 2D grid.
 *
 * <p>A line is defined by to points p1 and p2. It does not provide much
 * functionality by itself and is simply a container for two points
 * under a namespace.
 *
 * @tparam p1 Position of the 1st PVector.
 * @tparam p2 Position of the 2nd PVector.
*/
class Line {
public:
    Line(const PVector &_p1, const PVector &_p2);

    /// One end of the line
    PVector p1;

    /// The other end of the line
    PVector p2;

    static std::string str(Line &l) {
	    return "from " + PVector::str(l.p1) + "to " + PVector::str(l.p2);
    }
};

/** A polygon in a 2D grid
 *
 * <p>The area is defined by a collection of points (also known as polygons).
 * The points have to be stored in a specific order to prevent mistakes.
 * The edge a is defined by line(p_s[i], p_s[i+1]) with the only exception
 * being the last element of p that is connected with the first one
 *
 * @tparam p_s The Corners of the Area
 */
class Area {
public:

    explicit Area(const std::vector<PVector> &p_s);

    /// Getter method for Area::Corners_
    const std::vector<PVector> &getCorners() const;

    /// Getter method for Area::Edges_
    const std::vector<Line> &getEdges() const;

    /// Getter method for Area::min_
    const PVector &getMin() const;

    /// Getter method for Area::max_
    const PVector &getMax() const;

private:

    /// Holds all corners of the Area as ordered points.
    std::vector<PVector> Corners_;

    /// Holds all edges of the Area as ordered lines.
    std::vector<Line> Edges_;

    /// Lowest posX and posY coordinates
    PVector min_;

    /// Highest posX and posY coordinates
    PVector max_;
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
 *  TODO: show function for Field class. (for Debugging)
 *
*/
class Field {
public:

    Field(const int &width, const int &height,
          const std::vector<Area> &walls,
          const std::vector<Area> &traps,
          const std::vector<Area> &swamps,
          const std::vector<Area> &waters,
          const std::vector<PVector> &deposits,
          const std::vector<PVector> &wallNodes,
          const std::vector<PVector> &trapNodes,
          const std::vector<PVector> &swampNodes,
          const std::vector<Collectible> &collectibles);


    Collectible* addCollectible(Collectible collectible);
    bool removeCollectible(Collectible collectible);

    Collectible *getCollectible(PVector pos, double angle, double uncertainty, int color);


    /// Getter for the size
    PVector getSize();

    /** gets map objects based on their index
     *
     * @param indices the index of the wanted map objects 0 -> walls; 1 -> traps; 2 -> swamps; 3 -> bonus areas;
     *
     * @returns a vector containing all map object areas with the indices
     */
    std::vector<Area> getMapObjects(const std::vector<unsigned int> &indices);

    /**  gets map nodes based on their index
     *
     * @param indices 0 -> walls; 1 -> traps; 2 -> swamps
     *
     * @returns a vector containing all map object areas with the indices
     */
    std::vector<PVector> getMapNodes(const std::vector<unsigned int> &indices);

    /// Getter method for deposits
    std::vector<PVector> getDeposits();

    /// Getter method for collectibles
    std::vector<Collectible> getCollectibles(const std::vector<unsigned int> &colors);

private:
    /// Contains all walls
    std::vector<Area> Walls_;
    /// Contains all traps
    std::vector<Area> Traps_;
    /// Contains all swamps
    std::vector<Area> Swamps_;
    /// Contains all waters / bonus areas
    std::vector<Area> Waters_;

    /// The deposits saved as points in their respective center
    std::vector<PVector> Deposits_;

    /// Contains all wall nodes
    std::vector<PVector> WallNodes_;
    /// Contains all trap nodes
    std::vector<PVector> TrapNodes_;
    /// Contains all trap nodes
    std::vector<PVector> SwampNodes_;

    /** contains collectibles of all colors
     * They're ordered the following: index = 0 <=> Red; 1 <=> Cyan/Green; 2 <=> Black; 3 <=> SuperObject
    */
    std::array<std::vector<Collectible>, 4> Collectibles_;

    /** represents the size in cm of the field object
     *
     * @note There can never be a negative sized Field. The size is
     * also defined by the CoSpace-Rules so FieldA = {270, 180}; FieldB = {360, 270})
    */
    PVector size_;

};


/// namespace for geometric functions
namespace geometry {

    bool isLeft(PVector p0, PVector p1, PVector p2);

    /** checks if a point lies inside the Area
     * @param p A point
     * @return true if point lies inside of the Area. Otherwise false
    */
    bool isInside(const PVector &p, Area &area);

    /** calculates an intersection point between two lines
     * @param l1 a line
     * @param l2 another line
     * @return intersection point if existing. Otherwise (-1, -1)
    */
    PVector intersection(Line &l1, Line &l2);

	bool isIntersecting(Line &l1, Line &l2);

	bool isIntersecting(Line l1, const std::vector<Area>& Obstacles);

    double sqDist(const PVector &p1, const PVector &p2);

    double dist(const PVector &p1, const PVector &p2);

    double dot(PVector p1, PVector p2);

    /** Calculates normal point on a line to another point
     *
     * @param line The line
     * @param point The point
     *
     * @details This function returns the intersection between the line
     * and a line which goes through the point p and is orthogonal to the line
     * This point however doesn't need to be on the line and can also lie on
     * a extension of this line
    */
    PVector getNormalPoint(Line line, PVector point);

    PVector angle2Vector(double a);

    double vector2Angle(PVector v);

    double vector2Angle(double x, double y);
}


///   _______                _____          __
///  |   |   |.---.-..-----.|     \ .---.-.|  |_ .---.-.
///  |       ||  _  ||  _  ||  --  ||  _  ||   _||  _  |
///  |__|_|__||___._||   __||_____/ |___._||____||___._|
///                  |__|

extern const std::vector<Area> GAME0WALLS;
extern const std::vector<Area> GAME0TRAPS;
extern const std::vector<Area> GAME0SWAMPS;
extern const std::vector<Area> GAME0WATERS;
extern const std::vector<PVector> GAME0DEPOSITS;

extern const std::vector<PVector> GAME0WALLNODES;
extern const std::vector<PVector> GAME0TRAPNODES;
extern const std::vector<PVector> GAME0SWAMPNODES;

extern const std::vector<Collectible> GAME0COLLECTIBLES;

extern const std::vector<Area> GAME1WALLS;
extern const std::vector<Area> GAME1TRAPS;
extern const std::vector<Area> GAME1SWAMPS;
extern const std::vector<Area> GAME1WATERS;
extern const std::vector<PVector> GAME1DEPOSITS;

extern const std::vector<PVector> GAME1WALLNODES;
extern const std::vector<PVector> GAME1TRAPNODES;
extern const std::vector<PVector> GAME1SWAMPNODES;

extern const std::vector<Collectible> GAME1COLLECTIBLES;

#endif // !MAPDATA_HPP