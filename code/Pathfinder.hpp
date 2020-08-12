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

class Node {
public:
    Node(const Point &pos, Field *field);

    // bools for list indication
    bool isClosed, isOpen;

    // doubles for costs
    double g, f;

    // Node visited previous
    Node *previous;

    // getter for Node::pos_
    Point pos();

    // 0 = walls; 1 = walls + traps
    std::vector<std::pair<Node *, double>> neighbors(bool traps);

private:
    Point pos_{};
    Field *Field_;
    std::array<std::vector<std::pair<Node *, double>>, 2> neighbors_; // Node / cost

    double getCost(Node &node);

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
