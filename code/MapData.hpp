
#ifndef ERROR_MESSAGE
#define ERROR_MESSAGE(MESSAGE) {std::cerr << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#endif

#ifndef CODE_MAPDATA_HPP
#define CODE_MAPDATA_HPP

#include <vector>
#include <iostream>
#include <array>
#include <cmath>

/** A point in a 2D word
 *
 *  @ingroup TODO
 *
 *  @tparam x x-Position of the Point.
 *  @tparam y y-Position of the Point.
 *
 *  There's no much functionality to a %Point.
 *  It only offers simple access and storage of
 *  an integer 2D coord. For more functionality
 *  use std::pair<int, int>
*/
class Point {
public:
	Point() = default;

	Point(int _x, int _y);

	int x, y;

	friend bool operator==(const Point &p1, const Point &p2);

	friend bool operator!=(const Point &p1, const Point &p2);

	friend Point operator+(const Point& a, const Point& b);

	friend Point operator-(const Point& a, const Point& b);

	explicit operator bool() const;
};

/** A type of Point that can be collected by a robot.
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

	Collectible(const Point &p, const int &c);

	/// Getter method for Collectible::pos_
	const Point &getPos() const;

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
	bool isCorrectCollectible(Point robot_pos, double angle, double uncertainty);

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
	Point pos_{};
};

/** A Line in a 2D grid.
 *
 * <p>A line is defined by to points p1 and p2. It does not provide much
 * functionality by itself and is simply a container for two points
 * under a namespace.
 *
 * @tparam p1 Position of the 1st Point.
 * @tparam p2 Position of the 2nd Point.
*/
class Line {
public:
	Line(const Point &p1, const Point &p2);

	/// Getter method for Line::p1_
	const Point &getP1() const;

	/// Getter method for Line::p2_
	const Point &getP2() const;

	/// Setter method for Line::p1_
	void setP1(const Point &p_1);

	/// Setter method for Line::p2_
	void setP2(const Point &p_2);

private:

	/// One end of the line
	Point p1_{};

	/// The other end of the line
	Point p2_{};
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

	explicit Area(const std::vector<Point> &p_s);

	/// Getter method for Area::Corners_
	const std::vector<Point> &getCorners() const;

	/// Getter method for Area::Edges_
	const std::vector<Line> &getEdges() const;

	/// Getter method for Area::min_
	const Point &getMin() const;

	/// Getter method for Area::max_
	const Point &getMax() const;

private:

	/// Holds all corners of the Area as ordered points.
	std::vector<Point> Corners_;

	/// Holds all edges of the Area as ordered lines.
	std::vector<Line> Edges_;

	/// Lowest x and y coordinates
	Point min_;

	/// Highest x and y coordinates
	Point max_;
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
	      const std::vector<Point> &deposits,
	      const std::vector<Point> &wallNodes,
	      const std::vector<Point> &trapNodes,
	      const std::array<std::vector<Collectible>, 3> &collectibles);

	/// Getter for the size
	Point getSize();

	/**  Getter for map objects
	 *
	 * @param indices 0 -> walls; 1 -> traps; 2 -> swamps; 3 -> waters / bonus areas;
	 */
	std::vector<Area> getMapObjects(std::vector<unsigned int> indices);

	/**  Getter for map nodes
	 *
	 * @param indices 0 -> walls; 1 -> traps;
	 */
	std::vector<Point> getMapNodes(std::vector<unsigned int> indices);

	/// Getter method for deposits
	std::vector<Point> getDeposits();

	/// Getter method for collectibles
	std::vector<Collectible> getCollectibles(std::vector<unsigned int> colors);

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
	std::vector<Point> Deposits_;

	/// Contains all wall nodes
	std::vector<Point> WallNodes_;
	/// Contains all trap nodes
	std::vector<Point> TrapNodes_;

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
	Point size_;

};


/// namespace for geometric functions
namespace geometry {

	bool isLeft(Point p0, Point p1, Point p2);

	/** checks if a point lies inside the Area
	 * @param p A point
	 * @return true if point lies inside of the Area. Otherwise false
	*/
	bool isInside(const Point &p, Area &area);

	/** calculates an intersection point between two lines
	 * @param l1 a line
	 * @param l2 another line
	 * @return intersection point if existing. Otherwise (-1, -1)
	*/
	Point isIntersecting(Line &l1, Line &l2);

	double sqDist(const Point &p1, const Point &p2);

	double dist(const Point &p1, const Point &p2);
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
extern const std::vector<Point> GAME0DEPOSITS;

extern const std::vector<Point> GAME0WALLNODES;
extern const std::vector<Point> GAME0TRAPNODES;

extern const std::array<std::vector<Collectible>, 3> GAME0COLLECTIBLES;


extern const std::vector<Area> GAME1WALLS;
extern const std::vector<Area> GAME1TRAPS;
extern const std::vector<Area> GAME1SWAMPS;
extern const std::vector<Area> GAME1WATERS;
extern const std::vector<Point> GAME1DEPOSITS;

extern const std::vector<Point> GAME1WALLNODES;
extern const std::vector<Point> GAME1TRAPNODES;


extern const std::array<std::vector<Collectible>, 3> GAME1COLLECTIBLES;









#endif // !CODE_MAPDATA_HPP