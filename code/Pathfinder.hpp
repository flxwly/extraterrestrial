#ifndef PATHFINDING_HPP
#define PATHFINDING_HPP

#include "MapData.hpp"
#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
#include <queue>
#include <cmath>


// By what factor the speed is reduced in swamps.
#define SWAMP_SPEED_PENALITY 10

/**
   *  @brief A point in a 2D word used by Pathfinders
   *
   *  @ingroup TODO
   *
   *  @tparam pos   Position of Node in a 2D world.
   *  @tparam field Field-Object a Node is working on.
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
    Node(Point &pos, Field *field);

    // bools for list indication
    bool isClosed, isOpen;

    // doubles for costs
    double g, f;

    // Node visited previous
    Node *previous;

    // getter for Node::pos_
    Point pos();

    // getter for Node::neighbors_
    std::vector<std::pair<Node *, double>> neighbors();

private:
    Point pos_{};
    Field *Field_;
    std::vector<std::pair<Node *, double>> neighbors_; // Node / cost

    double getCost(Node &node);
    bool canSee(Node &node, std::vector<Area> Obstacles);


    int getNeighbors(std::vector<std::vector<Area>> ObstacleStructs);

};


// distance between two nodes



class Pathfinder {
public:
    explicit Pathfinder(const Field &MAP);

    std::vector<Node> map;

    // pathfinding algorithm
    std::vector<std::pair<int, int>> AStar(const Point &start, const Point &end, bool watch_for_traps);

private:
    struct PRIORITY {
        bool operator()(node *child, node *parent) const {
            return parent->f < child->f;
        }
    };

    static double heuristic(const node &cur, const node &end);

    // convert previous pointers of nodes to path
    static std::vector<node> traverse(node *end);

    // convert nodepath to pair
    static std::vector<Point> to_pair(const std::vector<node> &p);
};



#endif //PATHFINDING_HPP
