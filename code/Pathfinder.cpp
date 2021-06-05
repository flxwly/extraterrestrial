#include "Pathfinder.hpp"

/**     -----------     **/
/**                     **/
/**         Node        **/
/**                     **/
/**     -----------     **/

Node::Node(PVector _pos, Field *_field) : pos(_pos), field(_field),
                                          isClosed(false), isOpen(false),
                                          g(0), f(0), previous(nullptr), neighbours() {
}


double Node::calculateCost(const Node &node) {

    // Line from this.pos to node.pos
    Line line(pos, node.pos);

    // A Line either enters or exits a swamp. So the Swamp_speed_penality is toggled.
    int modifier = 1;

    auto comp = [](std::pair<PVector, double> a, std::pair<PVector, double> b) { return a.second > b.second; };
    std::priority_queue<std::pair<PVector, double>, std::vector<std::pair<PVector, double>>, decltype(comp)>
            intersections(comp);

    // Get all swamp intersections.
    for (auto &swamp : field->getMapObjects({2})) {
        for (auto bound : swamp.getEdges()) {
            std::pair<PVector, double> intersection = {geometry::intersection(line, bound), 0};
            if (!intersection.first)
                continue;

            intersection.second = geometry::dist(intersection.first, pos);

            intersections.push(intersection);
        }

        if (geometry::isInside(pos, swamp)) {
            modifier = SWAMP_SPEED_PENALITY;
        }
    }

    // interrupt everything if no intersections were found
    if (intersections.empty())
        return geometry::dist(pos, node.pos);


    // The cost that is returned at the end
    double cost = 0;
    intersections.push({pos, 0});
    for (unsigned int i = 0; i < intersections.size(); i++) {

        double d1 = intersections.top().second;

        intersections.pop();

        // add cost (modifier * distance)
        cost += modifier * (intersections.top().second - d1);

        // Toggle the modifier
        modifier = (modifier == SWAMP_SPEED_PENALITY) ? 1 : SWAMP_SPEED_PENALITY;
    }

    return cost;
}

bool Node::canSee(const Node &node, const std::vector<Area> &Obstacles) const {
    if (pos == node.pos)
        return false;
    return !geometry::isIntersecting({pos, node.pos}, Obstacles);
}

int Node::findNeighbours(std::vector<Node> &Nodes, const std::vector<Area> &Obstacles) {
    for (auto &node : Nodes) {
        if (canSee(node, Obstacles)) {
            if (std::find_if(neighbours.begin(), neighbours.end(),
                             [&](std::pair<Node *, double> n) { return n.first == &node; })
                == neighbours.end()) {

                neighbours.emplace_back(&node, calculateCost(node));
            }
        }
    }
    return neighbours.size();

}

void Node::addNeighbour(Node *neighbour, const double &cost) {
    neighbours.emplace_back(neighbour, cost);
}

bool Node::removeNeighbour(Node *neighbour) {

    for (unsigned int i = 0; i < neighbours.size(); ++i) {
        if (neighbours[i].first == neighbour) {
            neighbours.erase(neighbours.begin() + i);
            return true;
        }
    }

    return false;
}


/**     -----------     **/
/**                     **/
/**        Path         **/
/**                     **/
/**     -----------     **/


Path::Path(std::vector<PVector> points, double _r) : points(std::move(points)), r(_r) {

}

PVector Path::getClosestNormalPoint(PVector p, double d) {
    double dist = INFINITY;
    PVector finalNormal = points.back();

    if (points.size() == 1) {
        finalNormal = points.back();
    }

    for (unsigned int i = 0; i < points.size() - 1; ++i) {

        PVector normalPoint = geometry::getNormalPoint(Line(points[i], points[i + 1]), p);

        // Test if this is the closest yet seen normalpoint
        if (geometry::dist(normalPoint, p) < dist) {

            PVector line = points[i + 1] - points[i];
            line.rotate(M_PI / 2);
            if (geometry::isLeft(points[i + 1], points[i + 1] + line, normalPoint)) {
                dist = geometry::dist(normalPoint, p);
                finalNormal = normalPoint + (points[i + 1] - points[i]).setMag(d);
            }
        }
    }

    return finalNormal;
}

void Path::addPoint(PVector p) {
    points.push_back(p);
}

void Path::removeLast() {
    points.pop_back();
}

PVector Path::getLast() {
    return points.back();
}

bool Path::isOnPath(PVector p) {

    for (unsigned int i = 0; i < points.size() - 1; i++) {
        PVector p1 = points[i];
        PVector p2 = points[i + 1];

        // see https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
        double dist = fabs((p2.y - p1.y) * p.x - (p2.x - p1.x) * p.y + p2.x * p1.y - p2.y * p1.x) /
                      sqrt(pow(p2.y - p1.y, 2) + pow(p2.x - p1.x, 2));

        if (dist <= r) {
            return true;
        }
    }

    return false;
}

double Path::length() {
    double length = 0;
    for (int i = 0; i < points.size() - 1; i++)
        length += geometry::dist(points[i], points[i + 1]);

    return length;
}



/**     -----------     **/
/**                     **/
/**     Pathfinder      **/
/**                     **/
/**     -----------     **/

Pathfinder::Pathfinder(Field &MAP, bool trap_sensitive) : trapSensitive{trap_sensitive}, field{&MAP} {

    // get map objects
    std::vector<Area> mapObjects = {};
    if (trap_sensitive) {
        mapObjects = field->getMapObjects({0, 1});
    } else {
        mapObjects = field->getMapObjects({0});
    }
    // get map nodes
    std::vector<PVector> mapNodes = {};
    if (trap_sensitive) {
        mapNodes = field->getMapNodes({0, 1, 2});
    } else {
        mapNodes = field->getMapNodes({0, 2});
    }

    // create & store Node
    for (auto node : mapNodes) {
        map.emplace_back(node, field);
    }

    // get neighbour Nodes
    for (auto &node : map) {
        node.findNeighbours(map, mapObjects);
    }

}

double Pathfinder::heuristic(const PVector &cur, const PVector &end) {
    return geometry::dist(cur, end);
}

Path Pathfinder::AStar(PVector &begin, PVector &goal) {

    PATHFINDER_LOG("Running Pathfinder from " + begin.str() + " to " + goal.str())

    // If the begin is also the goal a path is just the goal
    if (begin == goal) {
        PATHFINDER_WARNING("\tStart and end are the same.")
        return Path({goal}, PATH_RADIUS);
    }

    Node start = Node(begin, field);
    Node end = Node(goal, field);

    PATHFINDER_LOG("\tBuilding Map...")
    // If the pathfinder is trap sensitive traps have to be taken into account
    std::vector<Area> mapObjects;
    mapObjects = (trapSensitive) ? field->getMapObjects({0, 1}) : field->getMapObjects({0});

    PATHFINDER_LOG("\t\tinitialize start and end nodes...")
    // Initialize the start and end nodes
    PATHFINDER_LOG("\t\tFound " << start.findNeighbours(map, mapObjects) << " neighbours for start node")
    if (start.canSee(end, mapObjects)) {
        PATHFINDER_WARNING("\t\t\tStart and End are directly connected!!!")
        start.addNeighbour(&end, start.calculateCost(end));
    }

    PATHFINDER_LOG("\t\tFound " << end.findNeighbours(map, mapObjects) << " neighbours for end node");
    for (auto &neighbour : end.neighbours) {
        neighbour.first->addNeighbour(&end, neighbour.second);
    }

    PATHFINDER_LOG("\tCreate priority queue..")

    // init open- & closedList
    std::priority_queue<Node *, std::vector<Node *>, Pathfinder::PRIORITY> openList;
    std::vector<Node *> closedList;

    // add start to openList
    openList.push(&start);
    start.isOpen = true;
    double tempG;

    // update start.g & start.f
    start.g = 0;
    start.f = (start.g + heuristic(start.pos, end.pos));

    PATHFINDER_LOG("\tBegin pathfinding...")

    // loop until solution is found or no solution possible
    while (!openList.empty()) {

        // choose node with lowest f
        Node *cur = openList.top();

        // if cur and end are the same, the path is found
        if (cur == &end) {

            PATHFINDER_LOG("Found path")

            Path path = Pathfinder::traverse(&end);

            PATHFINDER_LOG("Cleaning up")
            // remove end node from map nodes neighbour list
            for (auto &neighbour : end.neighbours) {
                neighbour.first->removeNeighbour(&end);
            }

            // reset node booleans
            for (Node *element : closedList) {
                element->isClosed = false;
            }
            while (!openList.empty()) {
                openList.top()->isOpen = false;
                openList.pop();
            }

            return path;
        }

            // continue loop
        else {
            // remove cur from openList & add cur to closedList
            openList.pop();
            cur->isOpen = false;

            // for every neighbour from cur:
            for (auto &neighbour : cur->neighbours) {
                if (neighbour.first->isClosed) {
                    continue;
                }

                // tempG = g cost over cur
                tempG = cur->g + neighbour.second;

                // if neighbour is in openList just update | otherwise add and update
                if (neighbour.first->isOpen) {
                    // only if path over cur is better
                    if (neighbour.first->g > tempG) {
                        //if(!pqContainsNode(openList, neighbour) || tempG < neighbour.g)
                        // update
                        neighbour.first->g = tempG;
                        neighbour.first->f = tempG + heuristic(neighbour.first->pos, end.pos);
                        neighbour.first->previous = cur;
                        //ERROR_MESSAGE(" - updated " + PVector::str(neighbour.first->pos) + " - ");
                    }
                } else {
                    // add
                    neighbour.first->g = tempG;
                    neighbour.first->f = tempG + heuristic(neighbour.first->pos, end.pos);
                    neighbour.first->previous = cur;
                    openList.push(neighbour.first);
                    neighbour.first->isOpen = true;
                    //ERROR_MESSAGE(" - added " + PVector::str(neighbour.first->pos) + " - ")
                }
            }

            closedList.push_back(cur);
            cur->isClosed = true;
        }
    }

    // resetting everything
    PATHFINDER_LOG("Cleaning up")
    for (auto &neighbour : end.neighbours) {
        neighbour.first->removeNeighbour(&end);
    }

    for (Node *element : closedList) {
        element->isClosed = false;
    }
    while (!openList.empty()) {
        openList.top()->isOpen = false;
        openList.pop();
    }

    return Path({}, PATH_RADIUS);
}

Path Pathfinder::traverse(Node *end) {
    std::vector<PVector> path;

    while (end != nullptr) {
        path.push_back(end->pos);
        end = end->previous;
    }
    std::reverse(path.begin(), path.end());

    PATHFINDER_LOG("Traversed " << path.size() << " points.")

    return Path(path, PATH_RADIUS);
}


