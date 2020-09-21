#include "Pathfinder.hpp"

/**     -----------     **/
/**                     **/
/**         Node        **/
/**                     **/
/**     -----------     **/

// Node::Node(): Constructor for Node Class
Node::Node(Point &pos, Field *field) {
    Node::isClosed = false, Node::isOpen = false;
    Node::pos_ = pos;
    Node::Field_ = field;
    Node::g = 0, Node::f = 0;
    Node::previous = nullptr;
    Node::neighbors_ = {};
}

// Node::neighbors(): Getter for Node::neighbors_
std::vector<std::pair<Node *, double>> Node::neighbors() {
    return Node::neighbors_;
}

// Node::pos(): Getter for Node::pos_
Point Node::pos() {
    return Node::pos_;
}

// Node::getCost():  cost calculation from Node:: to node
//      Input:  Node node
//      Return: -1 <=> impossible; >=0 <=> cost
double Node::getCost(Node &node) {

    // Check if a Node is visible
    if (!Node::canSee(node, {Node::Field_->MapObjects({1})}))
        return -1;

    // Line
    Line line(Node::pos_, node.pos());

    // Get all swamp intersections.
    std::vector<Point> intersections;
    for (auto &swamp : Node::Field_->MapObjects({3})) {
        for (auto bound : swamp.Edges()) {
            Point intersection = line.intersects(bound);
            if (intersection) {
                intersections.push_back(intersection);
            }
        }
    }

    // A Line either enters or exits a swamp. So the Swamp_speed_penality is toggled.
    int modifier = 1;
    if (!intersections.empty()) {
        for (auto &swamp : Node::Field_->MapObjects({3})) {
            if (swamp.isInside(Node::pos_)) {
                modifier = SWAMP_SPEED_PENALITY;
                break;
            }
        }
    }
    intersections.push_back(node.pos());

    // The cost that is returned at the end
    double cost = 0;

    Point last_intersection = Node::pos_;
    for (auto intersection : intersections) {

        // add cost (modifier * distance)
        cost += modifier *
                sqrt(pow(last_intersection.x - intersection.x, 2) + pow(last_intersection.y - intersection.y, 2));

        // Toggle the modifier
        modifier = (modifier == SWAMP_SPEED_PENALITY) ? 1 : SWAMP_SPEED_PENALITY;

        // set last_intersection
        last_intersection = intersection;
    }

    return cost;
}


bool Node::canSee(Node &node, const std::vector<Area> &Obstacles) {

    Line l1(Node::pos_, node.pos());
    for (Area Obstacle : Obstacles) {
        for (auto edge : Obstacle.Edges()) {
            Point intersection = l1.intersects(edge);
            if (!intersection)
                return false;
        }
    }

    return true;
}

int Node::getNeighbors(const std::vector<Node> &Nodes, const std::vector<Area> &Obstacles) {

    for (auto node : Nodes) {

        if (Node::canSee(node, Obstacles)) {

            bool already_added = false;
            for (auto neighbor : Node::neighbors_) {
                if (neighbor.first == &node) {
                    already_added = true;
                    break;
                }
            }

            if (!already_added)
                Node::neighbors_.emplace_back(&node, Node::getCost(node));
        }
    }

    return Node::neighbors_.size();
}

/**     -----------     **/
/**                     **/
/**     Pathfinder      **/
/**                     **/
/**     -----------     **/

// Pathfinder::Pathfinder(): Constructor for Pathfinder Class
Pathfinder::Pathfinder(Field &MAP, bool trap_sensitive) {

    Pathfinder::trap_sensitive_ = trap_sensitive;
    Pathfinder::field_ptr_ = &MAP;

    std::vector<Node *> node_ptrs;

    if (trap_sensitive) {
        for (auto node_pt : MAP.Nodes({1, 2})) {
            // create & store Node
            Pathfinder::map.emplace_back(node_pt, &MAP);
            // store Node ptr
            node_ptrs.push_back(&Pathfinder::map.back());
        }
        // get neighbor Nodes
        for (auto node : Pathfinder::map) {
            //TODO: If this works; If putting Pathfinder::map as arguments lets getNeigbors work on that vector
            // and adds correct ptrs.
            node.getNeighbors(Pathfinder::map, MAP.MapObjects({1, 2}));
        }
    } else {
        for (auto node_pt : MAP.Nodes({1})) {
            // create & store Node
            Pathfinder::map.emplace_back(node_pt, &MAP);
        }
        // get neighbor Nodes
        for (auto node : Pathfinder::map) {
            //TODO: If this works; If putting Pathfinder::map as arguments lets getNeigbors work on that vector
            // and adds correct ptrs.
            node.getNeighbors(Pathfinder::map, MAP.MapObjects({1}));
        }
    }





}


// Distance between two nodes
double Pathfinder::heuristic(const Point &cur, const Point &end) {
    int xDiff = abs(cur.x - end.x);
    int yDiff = abs(cur.y - end.y);

    return sqrt(pow(xDiff, 2) + pow(yDiff, 2));
}

std::vector<Point> Pathfinder::AStar(Point &begin, Point &goal) {

    // The most cost intensive part of this algorithm
    //TODO: STOPPED HERE LAST TIME

    Node start = Node(begin, Pathfinder::field_ptr_);
    Node end = Node(goal, Pathfinder::field_ptr_);

    if (Pathfinder::trap_sensitive_) {
        start.getNeighbors(Pathfinder::map, Pathfinder::field_ptr_->MapObjects({1, 2}));
        end.getNeighbors(Pathfinder::map, Pathfinder::field_ptr_->MapObjects({1, 2}));
    } else {
        start.getNeighbors(Pathfinder::map, Pathfinder::field_ptr_->MapObjects({1}));
        end.getNeighbors(Pathfinder::map, Pathfinder::field_ptr_->MapObjects({1}));
    }

    // start = end ==> no real path
    if (begin == goal) {
        return {begin};
    }

    // init open- & closedList
    std::priority_queue<Node *, std::vector<Node *>, Pathfinder::PRIORITY> openList;
    std::vector<Node *> closedList;

    // add start to openList
    openList.push(&start);
    start.isOpen = true;
    double temp_g;

    // update start.g & start.f
    start.g = 0;
    start.f = (start.g + heuristic(start.pos(), end.pos()));

    // loop until soultion is found or no solution possible
    while (!openList.empty()) {
        // choose node with lowest f
        Node *cur = openList.top();

        // if cur and end are the same, the path is found
        if (cur == &end) {

            std::vector<Point> p_path = Pathfinder::to_point(Pathfinder::traverse(&end));

            for (Node *element : closedList) {
                element->isClosed = false;
            }
            while (!openList.empty()) {
                openList.top()->isOpen = false;
                openList.pop();
            }
            return p_path;
        }
            // continue loop
        else {
            // remove cur from openList & add cur to closedList
            openList.pop();
            cur->isOpen = false;

            // for every neighbour from cur:
            for (auto neighbor : cur->neighbors()) {
                if (neighbor.first->isClosed) {
                    continue;
                }
                // temp_g = g cost over cur
                temp_g =
                        cur->g + neighbor.second;
                // if neighbour is in openList just update | otherwise add and update
                if (neighbor.first->isOpen) {
                    // only if path over cur is better
                    if (neighbor.first->g > temp_g) {
                        //if(!pqContainsNode(openList, neighbour) || temp_g < neighbour.g)
                        // update
                        neighbor.first->g = temp_g;
                        neighbor.first->f = temp_g + heuristic(neighbor.first->pos(), end.pos());
                        neighbor.first->previous = cur;
                        //cout << " - updated " << neighbour << " - ";
                    }
                } else {
                    // add
                    neighbor.first->g = temp_g;
                    neighbor.first->f = temp_g + heuristic(neighbor.first->pos(), end.pos());
                    neighbor.first->previous = cur;
                    openList.push(neighbor.first);
                    neighbor.first->isOpen = true;
                    //std::cout << " - added " << neighbour << " - " << std::endl;
                }
            }

            closedList.push_back(cur);
            cur->isClosed = true;
        }
    }
    // resetting everything
    for (Node *element : closedList) {
        element->isClosed = false;
    }
    while (!openList.empty()) {
        openList.top()->isOpen = false;
        openList.pop();
    }

    return {};
}

std::vector<Node> Pathfinder::traverse(Node *end) {
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
        //cout << endl << end.x << " | " << end.y;
    }

    return t_path;
}
// unnescessary doing
/*
std::vector<node> Pathfinder::shorten(std::vector<node> t_path) {

    std::vector<node> f_path;

    if (t_path.empty()) {
        return {};
    }

    // begin = front node -> while all points from begin to curNode are on line, set next Node in path to cur;
    // repeat process until one point is not on line. Then add node before curNode to path and choose it as new beginn;
    // repeat process until checking reaches lastNode

    unsigned int end = t_path.size() - 1;
    unsigned int curStart = 0, curEnd = 2;

    node *curFirst;
    node *curLast;

    // if start < end there are still nodes to check
    while (curEnd < end) {


        curFirst = &t_path[curStart];
        // new added Node (Node until to check to)
        curLast = &t_path[curEnd];

        // loop trough every point between cur and last
        for (unsigned int i = curStart + 1; i < curEnd - 1; ++i) {
            // point temppath[i] is on line. Continue to next
            double dist;
            if (curLast->x - curFirst->x == 0 && curLast->y - curFirst->y == 0) {
                dist = sqrt(pow(curLast->y - t_path[i].y, 2) + pow(curLast->x - t_path[i].x, 2));
            } else {
                const double den = abs(
                        (curLast->y - curFirst->y) * t_path[i].x - (curLast->x - curFirst->x) * t_path[i].y +
                        curLast->x * curFirst->y - curLast->y * curFirst->x);
                const double num = sqrt(pow(curLast->y - curFirst->y, 2) + pow(curLast->x - curFirst->x, 2));
                dist = den / num;
            }
            if (dist < sqrt(2))
                continue;
            // point is not on line. set new start, add to path and break the loop
            curStart = curEnd - 1;
            f_path.push_back(t_path[curEnd - 1]);
            break;
        }
        curEnd++;
    }

    return f_path;
}
*/

std::vector<Point> Pathfinder::to_point(const std::vector<Node> &p) {
    std::vector<Point> p_path;
    p_path.reserve(p.size());
    for (Node n: p) {
        p_path.emplace_back(n.pos());
    }

    return p_path;
}




