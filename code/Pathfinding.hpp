//
// Created by flxwl on 19.09.2021.
//

#ifndef EXTRATERRESTRIAL_PATHFINDING_HPP
#define EXTRATERRESTRIAL_PATHFINDING_HPP

#include <list>
#include "MapData.hpp"

class Node{
public:
    const double x, y;

    Node(double _x, double _y);

    explicit Node(PVector pos);


    Node *previous = nullptr;
    std::list<std::pair<Node *, double>> edges = {};
};

class Edge {
public:
    const Node *n1, *n2;

    Edge(Node *_n1, Node *_n2);

    /** binary operator to compare two Edges with each other
     *
     * @param lhs the other Edge that should be compared to this
     *
     * @returns true if both pointers match
     *
    */
    bool operator==(const Edge &lhs) const;
};

class Graph {
public:
    std::vector<Node> nodes;
    std::vector<Edge> edges;

    Graph() = default;
    Graph(const std::vector<Area>& impassableAreas, const std::vector<PVector>& nodes);
};

class Path {
    std::vector<PVector> path;

public:
    explicit Path(std::vector<PVector> _path);
};

class AStar {
    // A graph ignoring soft areas
    Graph softGraph;

    // A graph taking everything into account
    Graph strictGraph;

public:

    /*!
     *
     * @param strictImpassableAreas
     * @param strictNodes
     * @param softImpassableAreas
     * @param softNodes
     * @param slowAreas
     * @param slowNodes
     */
    AStar(const std::vector<Area>& strictImpassableAreas, const std::vector<PVector>& strictNodes,
          const std::vector<Area>& softImpassableAreas = {}, const std::vector<PVector>& softNodes = {},
          const std::vector<Area>& slowAreas = {}, const std::vector<PVector>& slowNodes = {});



    Path Pathfind(PVector start, PVector end);


};


#endif //EXTRATERRESTRIAL_PATHFINDING_HPP
