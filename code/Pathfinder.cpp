#include "Pathfinder.hpp"


/**     -----------     **/
/**                     **/
/**         Node        **/
/**                     **/
/**     -----------     **/

// Node::Node(): Constructor for Node Class
Node::Node(PVector &pos, Field *field) {
    Node::pos_ = pos;
    Node::Field_ = field;

    Node::isClosed = false, Node::isOpen = false;
    Node::g = 0, Node::f = 0;
    Node::previous = nullptr;

    Node::neighbours_ = {};
}

// Node::getCost():  cost calculation from Node:: to node
//      Input:  Node node
//      Return: -1 <=> impossible; >=0 <=> cost
double Node::calculateCost(const Node &node) {

    // TODO: fix calculation including swamps
    return geometry::dist(pos_, node.pos_);

    // Line from this.pos to node.pos
    Line line(pos_, node.getPos());

    // Get all swamp intersections.
    std::vector<std::pair<PVector, double>> intersections;
    for (auto &swamp : Field_->getMapObjects({2})) {
        for (auto bound : swamp.getEdges()) {
            PVector intersection = geometry::isIntersecting(line, bound);

            if (intersection) {
                intersections.emplace_back(intersection, geometry::dist(intersection, pos_));
            }
        }
    }

    // interrupt everything if no intersections were found
    if (intersections.empty())
        return geometry::dist(pos_, node.getPos());


    // sort for distance
    std::sort(intersections.begin(), intersections.end(), helper::compare);

    // A Line either enters or exits a swamp. So the Swamp_speed_penality is toggled.
    int modifier = 1;
    for (auto &swamp : Field_->getMapObjects({2})) {
        if (geometry::isInside(pos_, swamp)) {
            modifier = SWAMP_SPEED_PENALITY;
            break;
        }
    }

    intersections.insert(intersections.begin(), {pos_, 0});

    ERROR_MESSAGE("Cost from " + PVector::str(pos_) + " to " + PVector::str(node.pos_) + " is:");

    // The cost that is returned at the end
    double cost = 0;
    for (unsigned int i = 0; i < intersections.size() - 1; i++) {

        double temp = cost;
        // add cost (modifier * distance)
        cost += modifier * (intersections[i + 1].second - intersections[i].second);

        std::cout << "\tpart cost is: " << cost - temp << std::endl;

        // Toggle the modifier
        modifier = (modifier == SWAMP_SPEED_PENALITY) ? 1 : SWAMP_SPEED_PENALITY;
    }


    std::cout << intersections.data() << std::endl;

    return cost;
}

bool Node::canSee(const Node &node, const std::vector<Area> &Obstacles) {

    Line l1(pos_, node.getPos());
    for (const Area &Obstacle : Obstacles) {
        for (auto edge : Obstacle.getEdges()) {
            PVector intersection = geometry::isIntersecting(l1, edge);
            if (intersection == PVector(-1, -1)) {
                return false;
            }
        }
    }

    return true;
}

int Node::findNeighbours(std::vector<Node> &Nodes, const std::vector<Area> &Obstacles) {

    for (auto & node : Nodes) {

        if (canSee(node, Obstacles)) {

            //ERROR_MESSAGE("Node: " + PVector::str(pos_) + " can see: " + PVector::str(node.pos_))


            if (std::find_if(neighbours_.begin(), neighbours_.end(),
                             [&](std::pair<Node *, double> n) { return n.first == &node; })
                             == neighbours_.end()) {

                neighbours_.emplace_back(&node, calculateCost(node));
            }
        }
    }
    return neighbours_.size();

}

const PVector &Node::getPos() const {
    return pos_;
}

Field *Node::getField() const {
    return Field_;
}

const std::vector<std::pair<Node *, double>> &Node::getNeighbours() {
    return neighbours_;
}

void Node::addNeighbour(const std::pair<Node *, double> &neighbourNode) {

    neighbours_.push_back(neighbourNode);

}

bool Node::removeNeighbour(Node *neighbour) {
    for (int i = static_cast<int> (neighbours_.size()) - 1; i > 0; i--) {
        if (neighbours_[i].first == neighbour) {
            neighbours_.erase(neighbours_.begin() + i);
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

// Pathfinder::Pathfinder(): Constructor for Pathfinder Class
Pathfinder::Pathfinder(Field &MAP, bool trap_sensitive) : trap_sensitive_{trap_sensitive}, Field_{&MAP} {

    ERROR_MESSAGE(" ----- Initializing Pathfinder ----- ")

    std::vector<unsigned int> indices = {0};

    if (trap_sensitive)
        indices.push_back(1);


    for (auto node : MAP.getMapNodes(indices)) {
        // create & store Node
        map.emplace_back(node, Field_);
    }

    // get neighbour Nodes
    for (auto &node : Pathfinder::map) {
        node.findNeighbours(Pathfinder::map, Field_->getMapObjects(indices));
    }

}

// Distance between two nodes
double Pathfinder::heuristic(const PVector &cur, const PVector &end) {
    return geometry::dist(cur, end);
}

Path Pathfinder::AStar(PVector &begin, PVector &goal) {

    // start = end ==> no real path
    if (begin == goal) {
        ERROR_MESSAGE("begin is end")
        return Path({begin}, PATH_RADIUS);
    }

    Node start = Node(begin, Field_);
    Node end = Node(goal, Field_);

    std::vector<unsigned int> indices = {0};

    if (trap_sensitive_) {
        indices.push_back(1);
    }

    ERROR_MESSAGE("0")
    start.findNeighbours(Pathfinder::map, Field_->getMapObjects(indices));

    end.findNeighbours(Pathfinder::map, Field_->getMapObjects(indices));
    for (auto &neighbour : end.getNeighbours()) {
        neighbour.first->addNeighbour({&end, neighbour.second});
    }
    ERROR_MESSAGE("1")

    // init open- & closedList
    std::priority_queue<Node *, std::vector<Node *>, Pathfinder::PRIORITY> openList;
    std::vector<Node *> closedList;

    // add start to openList
    openList.push(&start);
    start.isOpen = true;
    double temp_g;

    // update start.g & start.f
    start.g = 0;
    start.f = (start.g + heuristic(start.getPos(), end.getPos()));

    ERROR_MESSAGE("2")

    // loop until soultion is found or no solution possible
    while (!openList.empty()) {
        // choose node with lowest f
        Node *cur = openList.top();

        // if cur and end are the same, the path is found
        if (cur == &end) {

            ERROR_MESSAGE("3")

            Path path = Pathfinder::traverse(&end);

            // resetting everything
            for (auto &neighbour : end.getNeighbours()) {
                neighbour.first->removeNeighbour(&end);
            }

            for (auto &neighbour : start.getNeighbours()) {
                neighbour.first->removeNeighbour(&start);
            }

            for (Node *element : closedList) {
                element->isClosed = false;
            }
            while (!openList.empty()) {
                openList.top()->isOpen = false;
                openList.pop();
            }

            ERROR_MESSAGE("+3")

            return path;
        }
            // continue loop
        else {
            // remove cur from openList & add cur to closedList
            openList.pop();
            cur->isOpen = false;

            // for every neighbour from cur:
            for (auto &neighbour : cur->getNeighbours()) {
                if (neighbour.first->isClosed) {
                    continue;
                }
                // temp_g = g cost over cur
                temp_g =
                        cur->g + neighbour.second;
                // if neighbour is in openList just update | otherwise add and update
                if (neighbour.first->isOpen) {
                    // only if path over cur is better
                    if (neighbour.first->g > temp_g) {
                        //if(!pqContainsNode(openList, neighbour) || temp_g < neighbour.g)
                        // update
                        neighbour.first->g = temp_g;
                        neighbour.first->f = temp_g + heuristic(neighbour.first->getPos(), end.getPos());
                        neighbour.first->previous = cur;
                        ERROR_MESSAGE(" - updated " + PVector::str(neighbour.first->getPos()) + " - ");
                    }
                } else {
                    // add
                    neighbour.first->g = temp_g;
                    neighbour.first->f = temp_g + heuristic(neighbour.first->getPos(), end.getPos());
                    neighbour.first->previous = cur;
                    openList.push(neighbour.first);
                    neighbour.first->isOpen = true;
                    ERROR_MESSAGE(" - added " + PVector::str(neighbour.first->getPos()) + " - ")
                }
            }

            closedList.push_back(cur);
            cur->isClosed = true;
        }
    }

    ERROR_MESSAGE("4")

    // resetting everything
    for (auto &neighbour : end.getNeighbours()) {
        neighbour.first->removeNeighbour(&end);
    }

    for (Node *element : closedList) {
        element->isClosed = false;
    }
    while (!openList.empty()) {
        openList.top()->isOpen = false;
        openList.pop();
    }

    ERROR_MESSAGE("+4")

    return Path({}, PATH_RADIUS);
}

Path Pathfinder::traverse(Node *end) {
    // clear
    std::vector<Node> t_path;
    Node *t_ptr;

    while (end->previous != nullptr) {
        // add
        t_path.push_back(*end);

        // get next
        t_ptr = end->previous;
        end->previous = nullptr;
        end = t_ptr;
    }

    return Path(to_point(t_path), PATH_RADIUS);
}

std::vector<PVector> Pathfinder::to_point(const std::vector<Node> &p) {
    std::vector<PVector> p_path;
    p_path.reserve(p.size());
    for (const Node &n: p) {
        p_path.emplace_back(n.getPos());
    }

    return p_path;
}

bool helper::compare(const std::pair<PVector, double> &p, const std::pair<PVector, double> &q) {
    return p.second < q.second;
}


/**     -----------     **/
/**                     **/
/**        Path         **/
/**                     **/
/**     -----------     **/



Path::Path(std::vector<PVector> points, double r) : points_(std::move(points)), r_(r) {

}

double Path::getR() const {
    return r_;
}

void Path::setR(double r) {
    r_ = r;
}

std::vector<PVector> Path::getPoints() const {
    return points_;
}

PVector Path::getClosestNormalPoint(PVector p, double d) {
    double dist = 1000;
    PVector finalNormal = PVector(0, 0);
    PVector dir = PVector(0, 0);

    for (unsigned int i = 0; i < points_.size() - 1; ++i) {
        PVector normalPoint = geometry::getNormalPoint(Line(points_[i], points_[i + 1]), p);

        // Test if this is the closest yet seen normalpoint
        if (geometry::dist(normalPoint, p) < dist) {

            // Test if the normal Point is on the line i -> i+1
            double lineLength = geometry::dist(points_[i], points_[i + 1]);
            if (geometry::dist(points_[i], normalPoint) < lineLength
                && geometry::dist(points_[i + 1], normalPoint) < lineLength) {

                dist = geometry::dist(normalPoint, p);
                finalNormal = normalPoint;
                dir = points_[i + 1] - points_[i];
            }
        }
    }
    dir = dir * d;

    return finalNormal + dir;
}

void Path::addPoint(PVector p) {
    points_.push_back(p);
}

void Path::removeLast() {
    points_.pop_back();
}

bool Path::isOnPath(PVector p0) {

    for (unsigned int i = 0; i < points_.size() - 1; i++) {
        PVector p1 = points_[i];
        PVector p2 = points_[i + 1];
        double dist = fabs((p2.y - p1.y) * p0.x - (p2.x - p1.x) * p0.y + p2.x * p1.y - p2.y * p1.x) /
                      sqrt(pow(p2.y - p1.y, 2) + pow(p2.x - p1.x, 2));

        if (dist <= r_) {
            return true;
        }
    }

    return false;
}
