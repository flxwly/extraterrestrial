#ifndef PATHFINDING_HPP
#define PATHFINDING_HPP

#include "CommonFunctions.hpp"
#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
#include <queue>
#include <cmath>

class node {
public:
    node(int _x, int _y, bool _is_w, bool _is_t, bool _is_s);

    bool isClosed, isOpen;
    bool isTrap, isWall, isSwamp;
    int x, y;
    double g, f;
    std::vector<node *> neighbours;
    node *previous;
};


// distance between two nodes



class Pathfinder {
public:
    explicit Pathfinder(const std::vector<std::vector<int>> &MAP);

    // the map the pathfinding works on
    //      TODO: don't use 2d array. instead use more prepared 1d array
    std::vector<std::vector<node>> map;

    // pathfinding algorithm
    std::vector<std::pair<int, int>> AStar(node *start, node *end, bool watchForTraps);
    std::vector<std::pair<int, int>> AStar(std::pair<int, int> start, std::pair<int, int> end, bool watch_for_traps);

private:
    struct PRIORITY {
        bool operator()(node *child, node *parent) const {
            return parent->f < child->f;
        }
    };

    static double heuristic(const node &cur, const node &end);
    static bool isPassable(node *, bool traps);

    // convert previous pointers of nodes to path
    static std::vector<node> traverse(node *end);

    // shorten path
    static std::vector<node> shorten(std::vector<node> t_path);

    // convert nodepath to pair
    static std::vector<std::pair<int, int>> to_pair(const std::vector<node> &p);
};



#endif //PATHFINDING_HPP
