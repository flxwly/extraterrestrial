#include "Pathfinder.hpp"

/**     -----------     **/
/**                     **/
/**         Node        **/
/**                     **/
/**     -----------     **/

Node::Node(PVector pos, Field *field) : pos(pos), field(field),
                                        isClosed(false), isOpen(false),
                                        g(0), f(0), previous(nullptr), neighbours() {
    ERROR_MESSAGE("Constructed node")
}


double Node::calculateCost(const Node &node) {

    // Line from this.pos to node.pos
    Line line(pos, node.pos);

    // Get all swamp intersections.
    std::vector<std::pair<PVector, double>> intersections;
    for (auto &swamp : field->getMapObjects({2})) {
        for (auto bound : swamp.getEdges()) {
            PVector intersection = geometry::intersection(line, bound);

            if (intersection) {
                intersections.emplace_back(intersection, geometry::dist(intersection, pos));
            }
        }
    }

    // interrupt everything if no intersections were found
    if (intersections.empty())
        return geometry::dist(pos, node.pos);


    // sort for distance
    std::sort(intersections.begin(), intersections.end(),
              [&](const std::pair<PVector, double> &a, const std::pair<PVector, double> &b)
                      -> bool { return a.second <= b.second; });

    // A Line either enters or exits a swamp. So the Swamp_speed_penality is toggled.
    int modifier = 1;
    for (auto &swamp : field->getMapObjects({2})) {
        if (geometry::isInside(pos, swamp)) {
            modifier = SWAMP_SPEED_PENALITY;
            break;
        }
    }

    intersections.insert(intersections.begin(), {pos, 0});

    // The cost that is returned at the end
    double cost = 0;
    for (unsigned int i = 0; i < intersections.size() - 1; i++) {

        // add cost (modifier * distance)
        cost += modifier * (intersections[i + 1].second - intersections[i].second);

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

    for (int i = 0; i < neighbours.size(); ++i) {
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


Path::Path(std::vector<PVector> points, double r) : points(std::move(points)), r(r) {

}

PVector Path::getClosestNormalPoint(PVector p, double d, bool trapSensitive) {
    double dist = -1;
    PVector finalNormal = p;
    PVector dir = PVector(0, 0);

    if (points.size() == 1) {
        finalNormal = points[0];
    }

    for (unsigned int i = 0; i < points.size() - 1; ++i) {

        PVector normalPoint = geometry::getNormalPoint(Line(points[i], points[i + 1]), p);

        // Test if this is the closest yet seen normalpoint
        if (geometry::dist(normalPoint, p) < dist || dist == -1) {

            // Test if the normal Point is within the line segment

            // TODO: Get a line that halves the angle between two line segments
            //  Check whether the normal point lies on the correct side

            bool liesOnLeftToRightSide = false;
            bool liesOnRightToLeftSide = false;

            // TODO: special cases when a line segment is only defined by one such line
            if (points.size() <= 2) {
                // there are only start and end point
                // every normal point lies in that segment

                liesOnRightToLeftSide = true, liesOnLeftToRightSide = true;

            } else {
                // start point; only check whether the point is on the left side
                // check for segment p[0] p[1] n p[1] p[2]

                if (i < points.size() - 2) {
                    PVector p0 = points[i + 1]
                                 + (points[i + 1] - points[i]).normalize()
                                 + (points[i + 1] - points[i + 2]).normalize();

                    if (geometry::isLeft(points[i + 1], p0, points[i])) {
                        if (geometry::isLeft(points[i + 1], p0, normalPoint))
                            liesOnLeftToRightSide = true;
                    } else {
                        if (geometry::isLeft(p0, points[i + 1], normalPoint))
                            liesOnLeftToRightSide = true;
                    }
                } else {
                    liesOnLeftToRightSide = true;
                }
                if (i > 0) {
                    // end point; only check whether the point is on the right side

                    PVector p0 = points[i]
                                 + (points[i] - points[i - 1]).normalize()
                                 + (points[i] - points[i + 1]).normalize();

                    if (geometry::isLeft(points[i], p0, points[i - 1])) {
                        if (geometry::isLeft(p0, points[i], normalPoint))
                            liesOnRightToLeftSide = true;
                    } else {
                        if (geometry::isLeft(points[i], p0, normalPoint))
                            liesOnRightToLeftSide = true;
                    }

                } else {
                    liesOnRightToLeftSide = true;
                }

            }


            if (liesOnRightToLeftSide && liesOnLeftToRightSide) {

                dist = geometry::dist(normalPoint, p);
                finalNormal = normalPoint;

                // Vector from p[i] to p[i + 1]
                if (i == points.size() - 2) {
                    dir = points[i + 1] - p;
                } else {
                    dir = points[i + 1] - points[i];
                }
            }
        }
    }

    dir.setMag(d);

    return finalNormal + dir;
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

    // If the begin is also the goal a path is just the goal
    if (begin == goal)
        return Path({goal}, PATH_RADIUS);

    Node start = Node(begin, field);
    Node end = Node(goal, field);

    // If the pathfinder is trap sensitive traps have to be taken into account
    std::vector<Area> mapObjects = {};
    if (trapSensitive) mapObjects = field->getMapObjects({0, 1});
    else mapObjects = field->getMapObjects({0});


    // Initialize the start and end nodes
    start.findNeighbours(map, mapObjects);
    if (start.canSee(end, mapObjects))
        start.addNeighbour(&end, start.calculateCost(end));

    end.findNeighbours(map, mapObjects);
    for (auto &neighbour : end.neighbours)
        neighbour.first->addNeighbour(&end, neighbour.second);


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

    // loop until solution is found or no solution possible
    while (!openList.empty()) {

        // choose node with lowest f
        Node *cur = openList.top();

        // if cur and end are the same, the path is found
        if (cur == &end) {

            Path path = Pathfinder::traverse(&end);

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

    while (end->previous != nullptr) {
        path.push_back(end->pos);
        end = end->previous;
    }
    std::reverse(path.begin(), path.end());

    return Path(path, PATH_RADIUS);
}


