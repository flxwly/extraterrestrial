#ifndef ERROR_MESSAGE
#define ERROR_MESSAGE(MESSAGE) {std::cerr << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#endif


#ifndef CODE_MAPDATA_HPP
#define CODE_MAPDATA_HPP

#include <vector>
#include <iostream>
#include <array>
#include <cmath>

/**
   *  @brief A point in a 2D word
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
    Point()= default;;
    Point(int x, int y);
    int x, y;

    friend bool operator== (const Point &p1, const Point &p2);
    friend bool operator!= (const Point &p1, const Point &p2);
    explicit operator bool() const;
};

/**
   *  @brief A Collectible is a type of Point that can be collected by a robot.
   *
   *  @ingroup TODO
   *
   *  @tparam p Position of the Collectible.
   *  @tparam c Color of the Collectible.
   *
   *  A %Collectible can be described as a Point in a 2D grid that
   *  can be picked up by a robot in the CoSpace-Simulator environment.
   *  It can have 3 different states that each represent a different knowledge
   *  of the Collectible: -1 doesn't exist, 0 existence unknown, 1 exists
   *  It offers functions for changing it's own state based on position and
   *  rotation of robots.
   *
   *  @note Generally Collectibles are only created upon initializing Field-Objectrs
   *
   *  TODO: Collectible::setState(Point pos, int rotation)
  */
class Collectible {
public:

    Collectible(const Point &p, const int &c);

    /**
     * @brief Getter method for Collectible::pos_
     * @return Collectible::pos_
    */
    Point pos();

    /**
     * @brief Getter method for Collectible::color_
     * @return Collectible::color_
    */
    int color();

    /**
     * @brief Getter method for Collectible::state_
     * @return Collectible::state_
    */
    int state();

    /**
     * @brief Setter method for Collectible::state_
     * @param int s
    */
    void setState(int s);


private:
    int state_;
    Point pos_{};
    int color_;
};


/**
   *  @brief A Line connects two Points.
   *
   *  @ingroup TODO
   *
   *  @tparam p1 Position of the 1st Point.
   *  @tparam p2 Position of the 2nd Point.
   *
   *  A %Line can be described as 2 Points in a 2D grid, which are connected
   *  by a straight line. It offers a function to find out if another %Line
   *  intersects it and where exactly it does so.
  */
class Line {

public:

    Line(const Point &p1, const Point &p2);

    /**
     * @brief check if this line intersects a line l1.
     * @param l1 A Line
     * @return The intersection point if existing. Otherwise P{-1, -1}
    */
    Point intersects(Line &l1);

    /**
     * @brief Getter method for Line::p1_
     * @return Point p1_
    */
    Point p1();

    /**
     * @brief Getter method for Line::p2_
     * @return Point p2_
    */
    Point p2();

private:
    Point p1_{};
    Point p2_{};
};


/**
   *  @brief An Area represents a Polygon in a 2D grid.
   *
   *  @ingroup TODO
   *
   *  @tparam p_s The Corners of the Area
   *
   *  An %Area can be described as a collection of Points in a 2D grid,
   *  which are connected the way they are given by it's params
   *                                                    (p_s[0] -> p_s[1]).
   *  It offers a function to check wether a Point lies inside it or not
   *
   *  @note It is possible to create complex Areas (complex Polygons)
   *  and the methods still work. However it is not recommended
   *  since this can lead to logical problems in other classes/functions.
   *  @note p_s must be an ordered list of Points otherwise the edges
   *  are calculated wrong and the Area might give unexpected results
  */
class Area {
public:

    explicit Area(const std::vector<Point> &p_s);

    /**
     * @brief calculates if a point lies inside the Area
     * @param p A Point
     * @return true if point lies inside of the Area. Otherwise false
    */
    bool isInside(const Point &p);

    /**
     * @brief Getter method for Area::Corners_
     * @return std::vector<Point> Corners_
    */
    std::vector<Point> Corners();

    /**
     * @brief Getter method for Area::Edges_
     * @return std::vector<Point> Edges_
    */
    std::vector<Line> Edges();


private:

    /**
     * @brief private vector that holds all Corners of the Area-polygon as ordered Points.
     * @note Since this vector is exactly as the input
     * in the constructor it is important that the given points are already
     * ordered. There's no further check for correctness and the functionality
     * might suffer from a wrong order
    */
    std::vector<Point> Corners_;

    /**
     * @brief private vector that holds all Edges of the Area-polygon as ordered Lines.
     * @note This vector is generated from the ordered points upon initialization.
     * Note that the Edges are also ordered although at this point there's no function or
     * method that makes use of this.
    */
    std::vector<Line> Edges_;

    /**
     * @brief private Points that represent either the upper right or lower left corner
     * of a rectangle which contains all Edges of the Object.
     * @note This is used as boundary box for pre checks if a point lies inside this box.
    */
    Point min_;

    /**
     * @brief private Points that represent either the upper right or lower left corner
     * of a rectangle which contains all Edges of the Object.
     * @note This is used as boundary box for pre checks if a point lies inside this box.
    */
    Point max_;
};

/** class Field represents one map of the CoSpaceRescue-Sim
 *  It contains all information given by the map_interpreter.py script.
 *  Once initialized nothing but the Collectibles can be changed. However Collectibles are members of the class Collectibles on
 *  their own, which means that they are changed through their functions and not directly in the Field class.
 *
 *  TODO: show function for Field class. (for Debugging)
 *
 * **/
class Field {
public:

    /** Field::Field(): Constructor for Field class
     * Input:  Dimensions, MapObjects, Nodes, Deposits, Collectibles
     * Return: Field */
    Field(const int &width, const int &height,
          const std::vector<Area> &walls,
          const std::vector<Area> &traps,
          const std::vector<Area> &swamps,
          const std::vector<Area> &waters,
          const std::vector<Point> &wnodes,
          const std::vector<Point> &tnodes,
          const std::vector<Point> &deposits,
          const std::array<std::vector<Collectible>, 3> &collectibles);


    /** Field::size(): Getter method for Field::size_
     * Input:
     * Return: Point size (width, height)*/
    Point size();


    /** Field::MapObjects(): Getter method for Field::(MapObjectName)_
     * Input:  unsigned int index => walls - 1; traps - 2; swamps - 3; waters - 5;
     * Return: std::vector<Area> MapObject (of one type)*/
    std::vector<Area> MapObjects(std::vector<unsigned int> indices);


    /** Field::Nodes(): Getter method for Field::(type)Node_
     * Input:  unsigned int index => wallNodes - 1; trapNodes - 2;
     * Return: std::vector<Point> Nodes (of one type)*/
    std::vector<Point> Nodes(std::vector<unsigned int> indices);

    /** Field::Deposits(): Getter method for Field::Deposits_
     * Input:
     * Return: std::vector<Point> Deposits*/
    std::vector<Point> Deposits();

    /** Field::Deposits(): Getter method for Field::Collectibles_
     * Input:  unsigned int index => reds - 1; cyans/greens - 2; blacks - 3
     * Return: std::vector<Collectible> Collectibles (one type)*/
    std::vector<Collectible> Collectibles(std::vector<unsigned int> indices);

private:

    /** Map::(MapObjectName)_: private vector array that holds all MapObject-Polygons of one type (e.g. Walls)
     *                                                      note that deposits are Points and no MapObjects
     *                         the vectors hold members of the Area class which each represent one MapObject
     *                         A MapObject is a closed simple polygon.
     *                         There's a "general" getter for the MapObjects: Field::MapObject(int index); 1 <= index < 6; index != 4 **/
    std::vector<Area> Walls_;
    std::vector<Area> Traps_;
    std::vector<Area> Swamps_;
    std::vector<Area> Waters_;


    /** Map::(type_t)Node_: private vector array that holds all Nodes of one type (e.g. Walls)
     *                                                      note that at this point there are only Wall and Trap Nodes imported
     *                     the vector holds members of the Point class which represent Nodes.
     *                     It is inefficient to have members of the Node class instead, because they're only needed by the Pathfinder class.
     *                     There's a general getter for the Nodes: Field::Nodes(int index); 1 <= index < 3 **/
    std::vector<Point> wNodes_;
    std::vector<Point> tNodes_;


    /** Map::Deposits_: private vector array that holds all Deposit Areas as Points.
     *                      Theses Points represent the average Point or if the Deposit Area is simple the middle Point
     *                      It can happen that such a Point lies outside of the actual Area so it is mandatory to check it. **/
    std::vector<Point> Deposits_;


    /** Map::Collectibles_: private array[3] of vectors that hold Collectibles of one Color
     *                      These Collectibles represent all *possible* Collectible Points and there Position.
     *                      TODO: Check if Collectible lies inside Wall Area which means that a Collectible is not reachable and therefore can be ignored.
     *                      They're ordered the following: index = 0 <=> Red; 1 <=> Cyan/Green; 2 <=> Black
     *                      There's a getter Method: Field::Collectibles(unsigned int index); 0 <= index < 4 **/
    std::array<std::vector<Collectible>, 3> Collectibles_;


    /** Map::size_: private Point var that represents the size in cm of the Field class
     *  Possible values: There can never be a negative sized Field.
     *                   The size is also defined by the CoSpace-Rules
     *                                          (FieldA = {270, 180}; FieldB = {360, 270}) **/
    Point size_;

};

namespace geometry {
    int onSide(Point p0, Point p1, Point p2);

    bool isInside(const Point &p, Area &area);

    Point intersects(Line &l1, Line &l2);
}


#endif // !CODE_MAPDATA_HPP