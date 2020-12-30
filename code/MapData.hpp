#ifndef ERROR_MESSAGE
#define ERROR_MESSAGE(MESSAGE) {std::cerr << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#endif

#ifndef CSBOT_MAPDATA_HPP
#define CSBOT_MAPDATA_HPP


#include <vector>
#include <iostream>
#include <array>
#include <cmath>

#define COLOR_SENSOR_ANGLE_OFFSET 3.5
#define COLOR_SENSOR_DIST_TO_CORE 5

/** A point in a 2D word
 *
 *  @tparam x posX-Position of the PVector.
 *  @tparam y posY-Position of the PVector.
 *
 *  Can also be used as a 2D vector.
*/
class PVector {
public:
    PVector() = default;
    PVector(double _x, double _y);

    double x, y;

    void set(double _x, double _y);

    void set(PVector p);

    double getMag() const;

    void setMag(double mag);

    /// counter-clockwise rotation
    void rotate(double angle);

    [[nodiscard]] PVector round() const;

    friend bool operator==(const PVector &p1, const PVector &p2);

    friend bool operator==(const PVector &p, const double &n);

    friend bool operator!=(const PVector &p1, const PVector &p2);

    friend bool operator!=(const PVector &p, const double &n);

    PVector &operator+=(const PVector &rhs) {
        x = x + rhs.x, y = y + rhs.y;
        return *this;
    };

    friend PVector operator+(PVector &lhs, const PVector &rhs) {
        return lhs += rhs;
    };

    PVector &operator-=(const PVector &rhs) {
        x = x - rhs.x, y = y - rhs.y;
        return *this;
    };

    friend PVector operator-(const PVector &lhs, const PVector &rhs) {
        return PVector(lhs.x - rhs.x, lhs.y - rhs.y);
    };

    friend PVector operator*(const PVector &p, const double &m);

    friend PVector operator/(const PVector &p, const double &m);

    explicit operator bool() const;

    static std::string str(PVector pVector);
};

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

    Collectible(const PVector &p, const int &c);

    /// Getter method for Collectible::pos_
    const PVector &getPos() const;

    /// Getter method for Collectible::color_
    unsigned int getColor() const;

    /// Getter method for Collectible::state_
    unsigned int getState() const;

    /// Setter method for Collectible::state_
    void setState(unsigned int state);

    /** Checks if this collectible could be the seen one (position wise)
     *
     * <p>The color sensors of the robots aren't the same coords as the robot's
     * ones. Therefore it is mandatory to check whether a collectible is actually
     * the right/seen one before changing it's state.
     *
     * @param robot_pos the actual position of the robot given by the simulator
     * @param angle the actual angle of the robot given by the simulator
     * @param uncertainty how imprecise the position and the angle is
     */
    bool isCorrectCollectible(PVector robot_pos, double angle, double uncertainty);

private:

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
    unsigned int state_;

    /** Represents the color
     *
     * <p>Normally a collectible can have either of the 3 colors (red, cyan, black,
     * [special case for superobjects: pink]).
     * Collectibles with different colors give different amounts of score points.
     * In addition certain combinations of colors create bonus score. To track
     * maximize the score the robot needs to now which color each object.
     */
    unsigned int color_;

    /// Represents the position
    PVector pos_{};
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
    Line(const PVector &p1, const PVector &p2);

    /// Getter method for Line::p1_
    const PVector &getP1() const;

    /// Getter method for Line::p2_
    const PVector &getP2() const;

    /// Setter method for Line::p1_
    void setP1(const PVector &p_1);

    /// Setter method for Line::p2_
    void setP2(const PVector &p_2);

private:

    /// One end of the line
    PVector p1_{};

    /// The other end of the line
    PVector p2_{};
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
          const std::vector<Collectible> &collectibles);

    /// Getter for the size
    PVector getSize();

    /**  Getter for map objects
     *
     * @param indices 0 -> walls; 1 -> traps; 2 -> swamps; 3 -> waters / bonus areas;
     */
    std::vector<Area> getMapObjects(const std::vector<unsigned int> &indices);

    /**  Getter for map nodes
     *
     * @param indices 0 -> walls; 1 -> traps;
     */
    std::vector<PVector> getMapNodes(const std::vector<unsigned int> &indices);

    /// Getter method for deposits
    std::vector<PVector> getDeposits();

    /// Getter method for collectibles
    std::vector<Collectible> getCollectibles(const std::vector<unsigned int> &colors);

    Collectible *getCollectible(PVector robot_pos, double angle, double uncertainty, int color);

    std::vector<PVector> getPointPath();

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

    /** contains collectibles of all colors
     * They're ordered the following: index = 0 <=> Red; 1 <=> Cyan/Green; 2 <=> Black
     *
     * @Note It doesn't contain super-objects
    */
    std::array<std::vector<Collectible>, 3> Collectibles_;

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
    PVector isIntersecting(Line &l1, Line &l2);

    double sqDist(const PVector &p1, const PVector &p2);

    double dist(const PVector &p1, const PVector &p2);

    double dot(PVector p1, PVector p2);

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

extern const std::vector<Collectible> GAME0COLLECTIBLES;

extern const std::vector<Area> GAME1WALLS;
extern const std::vector<Area> GAME1TRAPS;
extern const std::vector<Area> GAME1SWAMPS;
extern const std::vector<Area> GAME1WATERS;
extern const std::vector<PVector> GAME1DEPOSITS;

extern const std::vector<PVector> GAME1WALLNODES;
extern const std::vector<PVector> GAME1TRAPNODES;

extern const std::vector<Collectible> GAME1COLLECTIBLES;

#endif // !CSBOT_MAPDATA_HPP