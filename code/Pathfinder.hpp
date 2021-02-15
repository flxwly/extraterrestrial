#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include "libs/PPSettings.hpp"
#include "libs/CommonFunctions.hpp"

#include "MapData.hpp"

#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
#include <queue>
#include <cmath>


/** A point in a 2D word used by Pathfinders
 *
 *  @ingroup Points
 *
 *  @tparam pos   Position of Node in a 2D world.
 *  @tparam field field-Object a Node is working on.
 *
 *  A %Node can be described as anchor point in a 2D grid.
 *  It offers functions for quick visibility checks for other
 *  Nodes, which can even be in other 2D grids, variables to keep
 *  track of their status in the current pathfinding problem and
 *  a vector containing all visible nodes in the same grid together
 *  with respective their costs.
*/
class Node {

public:
    Node(PVector pos, Field *field);

    /** Booleans that indicate if this Node is in a special list
     *  used by the A*Pathfinding algorithm
     *
     *  @note These variables aren't updated automatically
    */
    bool isClosed, isOpen;

    /** Doubles that are used in the A*Pathfinding algorithm
     *
     *  @note These variables aren't updated automatically
    */
    double g, f;

    /** A pointer to the last visited Node. This is used
     *  by the A*Pathfinding algorithm
     *
     *  @note This variable isn't updated automatically
    */
    Node *previous;

    /** This Method calculates a cost to a Node
     *
     *  @param node
     *
     *  @details This function takes visibility into account(for that it uses Node::canSee()).
     *  If this Node can't see the other Node the cost will be -1.
     *  Other wise the cost is calculated also taking swamps into account.
    */
    double calculateCost(const Node &node);

    /** This Method checks if this Node can see a certain other Node
     *
     *  @param node
     *  @param ObstaclesStructs A vector containing all structures that are counted
     *  as Obstacles
    */
    bool canSee(const Node &node, const std::vector<Area> &ObstaclesStructs) const;

    /** This Method gets every neighbour and calculates the cost.
     *
     *  @param Nodes A vector of pointers to Nodes
     *  @param ObstaclesStructs A vector containing all structures that are counted
     *  as Obstacles
     *
     *  @return the number of existing neighbours
     *
     *  @note Every Node in %Nodes has to be initialized before executing this Method
    */
    int findNeighbours(std::vector<Node> &Nodes, const std::vector<Area> &ObstacleStructs);

    /** A PVector struct that stores the position of this node
     *
     *  @note This variable could be constant and is not meant to change.
     *  However for usability reasons it's not constant.
    */
    PVector pos;

    /** A pointer to the field Object this Node is stored in
     *
     *  @details This pointer can be used to find neighbours or obstacles.
     *  It is important that a node knows in which field it is operating in.
     *
     *  @note This variable could be constant and is not meant to change.
     *  However for usability reasons it's not constant.
    */
    Field *field;

    /** A vector that stores all visible neighbour nodes with their
     *  respective costs.
     *
     *  @details To optimise speed the A*Pathfinding
     *  works on a precalculated environment. This vector keeps track of
     *  neighbours and distances/costs.
    */
    std::vector<std::pair<Node *, double>> neighbours; // Node / cost

    /** Adds a neighbour and it's cost
     *
     * @details To increase the speed this method will not calculate the
     * cost. This has to be done via "calculateCost()" and
     * then funneled in as a parameter to this function
    */
    void addNeighbour(Node *neighbour, const double &cost);

    /** Removes a neighbour
     *
     * @details This function only removes one neighbour by iterating
     * over "neighbours" and exiting as soon as one element has been
     * found
    */
    bool removeNeighbour(Node *neighbour);
};


/** A container class for a path generated by the Pathfinder class
 *
 *  @tparam points PVector vector containing all points in the path
 *  @tparam r the radius of the path
*/
class Path {
public:
    Path(std::vector<PVector> points, double r);

    /** Calculates the closest normal point to a point that is extended by the factor d
     *
     * @param p a point the normal should be calculated to
     * @param d the amount by which the point should lie ahead on the path.
     *
     * @details This method calculates a point that lies on the path
     * or an extension of one of it's lines. This point is moved by the
     * factor d on the path. (If d = 0 than the point is just a normal point
     * to p)
    */
    PVector getClosestNormalPoint(PVector p, double d);

    /// Checks whether a point is inside the radius of the path
    bool isOnPath(PVector p);

    /// Adds a point to the Path
    void addPoint(PVector p);

    /// Removes the last point of the path
    void removeLast();

    /// Returns the last point of the path
    PVector getLast();

    /// Checks whether the Path is empty
    [[nodiscard]] bool isEmpty() const { return points.empty(); };

    /// Returns the length of the Path
    [[nodiscard]] unsigned int length() const { return points.size(); };

    /// Contains all points of the path in order so that p[0] -> p[1] -> p[2] represents the path
    std::vector<PVector> points;

    /// The radius of the Path
    double r;
};

/** A Pathfinder class that operates on a specific maps using nodes
 *
 * @tparam MAP A field-object representing a map
 * @tparam trap_sensitive A bool that determines whether the pathfinder
 * watches for traps or not
 *
 * @details At this point there's only one pathfinding algorithm (AStar)
 * This works fine but may be extended later
 */
class Pathfinder {
public:
    Pathfinder(Field &MAP, bool trap_sensitive);

    /** Finds a path between a start and an end node
     *
     * @details Here the AStar pathfinding algorithm is used because it's
     * fast and efficient. Note that this works on a prepared environment hence
     * it is even faster.
     */
    Path AStar(PVector &start, PVector &end);

    /// Contains all nodes that are important to this Pathfinder
    std::vector<Node> map;

    const bool trapSensitive;
    Field *field;

private:

    /// Priority struct for open list
    struct PRIORITY {
        bool operator()(Node *child, Node *parent) const {
            return parent->f < child->f;
        }
    };

    /// The heuristic function for approximate cost calculation
    static double heuristic(const PVector &cur, const PVector &end);

    /// Converts previous pointers of nodes to path
    static Path traverse(Node *end) ;
};

#endif // PATHFINDER_HPP