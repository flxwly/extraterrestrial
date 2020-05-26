#ifndef PATHFINDING_HPP
#define PATHFINDING_HPP

#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
#include <queue>
#include <cmath>


class node {
public:
    bool isClosed, isOpen, isTrap;
    int x, y;
    double g, f;
    std::vector<node *> neighbours;
    node *previous;
};

// Distanz zweier nodes
double heuristic(const node &cur, const node &end);

class AStar {
public:
    std::vector<node> path;
    std::vector<std::vector<node>> map;

    explicit AStar(const std::vector<std::vector<int>>& MAP);

    // TODO: Fix Weird Bug, where sometimes diagonals are chosen although a straight part is faster
    void findPath(node *start, node *end, bool watchForTraps);

private:
    struct PRIORITY;
    void traversePath(node *end);
    void cleanUpPath(std::vector<node> temppath);
};



#endif //PATHFINDING_HPP
