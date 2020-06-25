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
    node(int _x, int _y);

    node(int _x, int _y, bool _is_w, bool _is_t, bool _is_s);

    bool isClosed, isOpen;
    bool isTrap, isWall, isAtWall, isSwamp;
    int x, y;
    double g, f;
    std::vector<node *> neighbours;
    node *previous;
};


// distance between two nodes
double heuristic(const node &cur, const node &end);


class AStar {
public:
    explicit AStar(const std::vector<std::vector<int>> &MAP, const std::vector<std::pair<int, int>> &NODES);

    // the map the pathfinding works on
    //      TODO: don't use 2d array. instead use more prepared 1d array
    std::vector<std::vector<node>> map;
    std::vector<node> nodes;

    // pathfinding algorithm
    std::vector<std::pair<int, int>> findPath(node *start, node *end, bool watchForTraps);

    std::vector<std::pair<int, int>> findPath(node *start, node *end);

    std::vector<std::pair<int, int>> _findPath(node *start, node *end);

    std::vector<std::pair<int, int>> findPath(std::pair<int, int> start, std::pair<int, int> end);

    std::vector<std::pair<int, int>> findPath(std::pair<int, int> start, std::pair<int, int> end, bool watch_for_traps);

private:
    struct PRIORITY {
        bool operator()(node *child, node *parent) const {
            return parent->f < child->f;
        }
    };

    static bool isPassable(node *, bool traps);

    // convert previous pointers of nodes to path
    static std::vector<node> traverse(node *end);

    // shorten path
    static std::vector<node> shorten(std::vector<node> t_path);

    // convert nodepath to pair
    static std::vector<std::pair<int, int>> to_pair(const std::vector<node> &p);
};



#endif //PATHFINDING_HPP
