#ifndef ERROR_MESSAGE
#define ERROR_MESSAGE(MESSAGE) {std::cerr << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#endif


#ifndef CODE_MAPDATA_HPP
#define CODE_MAPDATA_HPP

#include <vector>
#include <iostream>
#include <array>
#include <cmath>

/** struct Point represents a simple Point with integer coordinates
 *          it could also be a typedef for std::pair<int, int> **/
typedef struct {
    /** int x, y are the coordinates of the Point**/
    int x, y;
} Point;


class Collectible {
public:
    Collectible(const Point &p, const short unsigned int &c);

    Point pos();

    [[nodiscard]] int color() const;

private:
    Point pos_{};
    unsigned short color_;
};

class Line {
public:
    Line(const Point &p1, const Point &p2);

    Point intersects(Line &l1);

    Point p1();

    Point p2();

private:
    Point p1_{};
    Point p2_{};
};


/** class Area represents an area (lol)
 *  It contains a set of "ordered" points stored in a vector "corners". Ordered means that P[i] and P[i+1] make up one boundary.
 *                                                                              (special case for P[n] and P[0] since the polygon is closed)
 *  Additionally a vector of Lines "edges" is generated upon initializing a member of this class. This vector contains all edges of the polygon (also ordered).
 *  An Area can never be changed and is ultimately just a collection of "pseudo constant" vars and functions.
 *
 * **/
class Area {
public:

    /** Area::Area(): Constructor for Area class
     * Input:  std::vector<Point> p_s
     * Return: Area */
    explicit Area(const std::vector<Point> &p_s);


    /** Area::isInside(): calculates if a point lies inside the Area
     * Input: Point p
     * Return: true if point lies inside of the Area. Otherwise false */
    [[nodiscard]] bool isInside(const Point &p) const;


    /** Area::Corners(): Getter method for Area::Corners_
     * Input:
     * Return: std::vector<Point> Corners_ */
    std::vector<Point> Corners();


    /** Area::Edges(): Getter method for Area::Edges_
     * Input:
     * Return: std::vector<Point> Edges_ */
    std::vector<Line> Edges();


private:

    /** Area::Corners_: private vector that holds all Corners of the Area-polygon as ordered Points.
     *                      Since this vector is exactly as the input in the constructor it is important
     *                              that the given points are already ordered. There's no further check for correctness
     *                              and the functionality might suffer from a wrong order**/
    std::vector<Point> Corners_;


    /** Area::Edges_: private vector that holds all Edges of the Area-polygon as ordered Lines.
     *                      This vector is generated from the ordered points upon initialization.
     *                              Note that the Edges are also ordered although at this point there's no function or method
     *                              that makes use of this.**/
    std::vector<Line> Edges_;


    /** Area::(min/max)_: private Points that represent either the upper right or lower left corner of a
     *                      rectangle which contains all Edges of the Object. This is used as boundary box for pre checks
     *                              if a point lies inside this box.**/
    Point min_{};
    Point max_{};
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
     * Input:  short int index => walls - 1; traps - 2; swamps - 3; waters - 5;
     * Return: std::vector<Area> MapObject (of one type)*/
    std::vector<Area> MapObjects(short int index);


    /** Field::Nodes(): Getter method for Field::(type)Node_
     * Input:  short int index => wallNodes - 1; trapNodes - 2;
     * Return: std::vector<Point> Nodes (of one type)*/
    std::vector<Point> Nodes(short int index);

    /** Field::Deposits(): Getter method for Field::Deposits_
     * Input:
     * Return: std::vector<Point> Deposits*/
    std::vector<Point> Deposits();

    /** Field::Deposits(): Getter method for Field::Collectibles_
     * Input:  short int index => reds - 1; cyans/greens - 2; blacks - 3
     * Return: std::vector<Collectible> Collectibles (one type)*/
    std::vector<Collectible> Collectibles(short int index);

private:

    /** Map::(MapObjectName)_: private vector array that holds all MapObject-Polygons of one type (e.g. Walls)
     *                                                      note that deposits are Points and no MapObjects
     *                         the vectors hold members of the Area class which each represent one MapObject
     *                         A MapObject is a closed simple polygon.
     *                         There's a "general" getter for the MapObjects: Field::MapObject(short int index); 1 <= index < 6; index != 4 **/
    std::vector<Area> Walls_;
    std::vector<Area> Traps_;
    std::vector<Area> Swamps_;
    std::vector<Area> Waters_;


    /** Map::(type_t)Node_: private vector array that holds all Nodes of one type (e.g. Walls)
     *                                                      note that at this point there are only Wall and Trap Nodes imported
     *                     the vector holds members of the Point class which represent Nodes.
     *                     It is inefficient to have members of the Node class instead, because they're only needed by the Pathfinder class.
     *                     There's a general getter for the Nodes: Field::Nodes(short int index); 1 <= index < 3 **/
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
     *                      There's a getter Method: Field::Collectibles(short int index); 0 <= index < 4 **/
    std::array<std::vector<Collectible>, 3> Collectibles_;


    /** Map::size_: private Point var that represents the size in cm of the Field class
     *  Possible values: There can never be a negative sized Field.
     *                   The size is also defined by the CoSpace-Rules
     *                                          (FieldA = {270, 180}; FieldB = {360, 270}) **/
    Point size_;

};

#endif // !CODE_MAPDATA_HPP