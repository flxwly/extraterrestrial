//
// Created by flxwl on 19.09.2021.
//

#ifndef EXTRATERRESTRIAL_PATHFINDING_HPP
#define EXTRATERRESTRIAL_PATHFINDING_HPP

#include <list>
#include "MapData.hpp"



/*! A 2D point representation for A*Star pathfinding and constructing graphs
 *
 * <p> A Node has variables for storing it's position, a pointer to a Node and
 * a list containing all it's visible Nodes with their cost of moving there.
 * Multiple Nodes together can represent an undirected graph where two Nodes will
 * represent one edge.
 */
class Node{
public:
    explicit Node(PVector pos);
    Node(double _x, double _y);

    const double x, y;

    Node *previous = nullptr;
    std::list<std::pair<Node *, double>> edges = {};

    friend std::ostream& operator<<(std::ostream& os, const Node& node);
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
    Graph(const std::vector<Area>& impassableAreas, const std::vector<PVector>& nodes,
          const std::vector<Area>& swampAreas, const std::vector<PVector>& swampNodes);

    double calculateCost(const Node& n1, const Node& n2, const std::vector<Area>& swampAreas);
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
