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
    Node(double _x, double _y);

    const double x, y;

    Node *previous = nullptr;
    std::list<std::pair<Node *, double>> edges = {};

    friend std::ostream& operator<<(std::ostream& os, const Node& node);
};


class Graph {
    std::vector<Node> nodes{};

public:
    Graph() = default;
    Graph(const std::vector<Area>& impassableAreas, const std::vector<PVector>& nodes,
          const std::vector<Area>& swampAreas, const std::vector<PVector>& swampNodes);

    static double calculateCost(const Node& n1, const Node& n2, const std::vector<Area>& swampAreas);
};




class Pathfinder {
    // A graph ignoring soft areas
    Graph softGraph;

    // A graph taking everything into account
    Graph strictGraph;

public:

    /*! Constructor for an Pathfinder Object
     *
     * @param strictAreas
     * @param strictNodes
     * @param softAreas
     * @param softNodes
     * @param slowAreas
     * @param slowNodes
     */
    Pathfinder(const std::vector<Area>& strictAreas, const std::vector<PVector>& strictNodes,
               const std::vector<Area>& softAreas = {}, const std::vector<PVector>& softNodes = {},
               const std::vector<Area>& slowAreas = {}, const std::vector<PVector>& slowNodes = {});

    /*! Rebuilds the graph from scratch
     *
     * @param strictAreas
     * @param strictNodes
     * @param softAreas
     * @param softNodes
     * @param slowAreas
     * @param slowNodes
     */
    void rebuildGraphs(const std::vector<Area>& strictAreas, const std::vector<PVector>& strictNodes,
                       const std::vector<Area>& softAreas = {}, const std::vector<PVector>& softNodes = {},
                       const std::vector<Area>& slowAreas = {}, const std::vector<PVector>& slowNodes = {});

    void addObstacles(const std::vector<Area>& strictAreas, const std::vector<Area>& softAreas, const std::vector<Area>& slowAreas);

};


#endif //EXTRATERRESTRIAL_PATHFINDING_HPP
