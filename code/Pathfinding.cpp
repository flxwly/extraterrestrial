//
// Created by flxwl on 19.09.2021.
//

#include "Pathfinding.hpp"

Node::Node(double _x, double _y) : x(_x), y(_y) {
}

Node::Node(PVector pos) : x(pos.x), y(pos.y) {

}

std::ostream &operator<<(std::ostream &os, const Node &node) {
    os << std::to_string(node.x) + " | " + std::to_string(node.y);
    return os;
}

Graph::Graph(const std::vector<Area> &impassableAreas, const std::vector<PVector> &_nodes,
             const std::vector<Area> &swampAreas, const std::vector<PVector> &swampNodes) {

    for (auto node : _nodes) {
        nodes.emplace_back(node.x, node.y);
    }
    for (auto node : swampNodes) {
        nodes.emplace_back(node.x, node.y);
    }

    Node *n1 = nullptr;
    Node *n2 = nullptr;

    bool canSee;

    for (int i = 0; i < nodes.size(); i++) {
        n1 = &nodes[i];
        for (int j = i + 1; j < nodes.size(); j++) {
            n2 = &nodes[j];
            Edge edge(n1, n2);
            canSee = true;
            if (std::find(edges.begin(), edges.end(), edge) == edges.end()) {
                for (const auto &area : impassableAreas) {
                    if (geometry::isIntersecting(Line(PVector(n1->x, n1->y), PVector(n2->x, n2->y)), area)) {
                        canSee = false;
                        break;
                    }
                }
                if (canSee) {
                    double dist = calculateCost(*n1, *n2, swampAreas);

                    edges.push_back(edge);

                    n1->edges.emplace_back(n2, dist);
                    n2->edges.emplace_back(n1, dist);
                }
            }
        }
    }
}

double Graph::calculateCost(const Node &n1, const Node &n2, const std::vector<Area> &swampAreas) {

    double dist = 0;
    double multiplier = 1;

    std::vector<std::pair<PVector, double>> intersections = {};
    Line line1 = Line(PVector(n1.x, n1.y), PVector(n2.x, n2.y));
    for (const auto &area: swampAreas) {
        if (geometry::isInside(PVector(n1.x, n1.y), area))
            multiplier = SWAMP_SPEED_PENALTY;
        for (auto line : area.getEdges()) {
            auto intersection = geometry::intersection(line, line1);
            if (intersection) {
                intersections.emplace_back(intersection, geometry::dist(n1.x, n1.y, intersection.x, intersection.y));
            }
        }
    }

    if (intersections.empty()) {
        return geometry::dist(n1.x, n1.y, n2.x, n2.y);
    }

    intersections.emplace_back(PVector(n1.x, n1.y), 0);
    std::sort(intersections.begin(), intersections.end(),
              [](const std::pair<PVector, double> &d1, const std::pair<PVector, double> &d2) {
                  return d1.second < d2.second;
              });

    for (int i = 1; i < intersections.size(); ++i) {
        dist += multiplier * (intersections[i].second - intersections[i - 1].second);
        multiplier = (multiplier == 1) ? SWAMP_SPEED_PENALTY : 1;
    }

    return dist;
}

Path::Path(std::vector<PVector> _path) : path(std::move(_path)) {}

Path AStar::Pathfind(PVector start, PVector end) {


    return Path({});
}

AStar::AStar(const std::vector<Area> &strictImpassableAreas, const std::vector<PVector> &strictNodes,
             const std::vector<Area> &softImpassableAreas, const std::vector<PVector> &softNodes,
             const std::vector<Area> &slowAreas, const std::vector<PVector> &slowNodes) :
        strictGraph(strictImpassableAreas, strictNodes, slowAreas, slowNodes),
        softGraph(softImpassableAreas, softNodes, slowAreas, slowNodes) {

    for (auto edge : strictGraph.edges) {

        // TODO: Swamps
    }
    for (auto edge : softGraph.edges) {
        // TODO: Swamps
    }


}

Edge::Edge(Node *_n1, Node *_n2) : n1(_n1), n2(_n2) {
}

bool Edge::operator==(const Edge &lhs) const {
    return (n1 == lhs.n1 && n2 == lhs.n2) || (n1 == lhs.n2 && n2 == lhs.n1);
}
