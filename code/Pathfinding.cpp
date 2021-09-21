//
// Created by flxwl on 19.09.2021.
//

#include "Pathfinding.hpp"

Node::Node(double _x, double _y) : x(_y), y(_y) {
}

Node::Node(PVector pos) : x(pos.x), y(pos.y) {

}


Graph::Graph(const std::vector<Area> &impassableAreas, const std::vector<PVector> &_nodes) {

    for (auto node : _nodes) {
        nodes.emplace_back(node.x, node.y);
    }

    Node *n1 = nullptr;
    Node *n2 = nullptr;

    for (int i = 0; i < nodes.size(); i++) {
        n1 = &nodes[i];
        for (int j = i; j < nodes.size(); j++) {
            n2 = &nodes[j];
            Edge edge(n1, n2);
            if (std::find(edges.begin(), edges.end(), edge) != edges.end()) {
                for (const auto &area : impassableAreas) {
                    if (!geometry::isIntersecting(Line(PVector(n1->x, n1->y), PVector(n2->x, n2->y)), area)) {
                        double dist = geometry::dist({n1->x, n1->y}, {n2->x, n2->y});

                        edges.push_back(edge);

                        n1->edges.emplace_back(n2, dist);
                        n2->edges.emplace_back(n1, dist);
                    }
                }
            }
        }
    }
}

Path::Path(std::vector<PVector> _path) : path(std::move(_path)) {}

Path AStar::Pathfind(PVector start, PVector end) {


    return Path({});
}

AStar::AStar(const std::vector<Area> &strictImpassableAreas, const std::vector<PVector> &strictNodes,
             const std::vector<Area> &softImpassableAreas, const std::vector<PVector> &softNodes,
             const std::vector<Area> &slowAreas, const std::vector<PVector> &slowNodes) :
        strictGraph(strictImpassableAreas, strictNodes),
        softGraph(softImpassableAreas, softNodes) {

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
