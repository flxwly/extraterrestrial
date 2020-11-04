#include "Pathfinder.hpp"

#include <utility>

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
double Node::calculateCost(Node &node) {

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

	// The cost that is returned at the end
	double cost = 0;
	for (int i = 0; i < intersections.size() - 1; i++) {

		// add cost (modifier * distance)
		cost += modifier * (intersections[i + 1].second - intersections[i].second);

		// Toggle the modifier
		modifier = (modifier == SWAMP_SPEED_PENALITY) ? 1 : SWAMP_SPEED_PENALITY;
	}

	return cost;
}

bool Node::canSee(Node &node, const std::vector<Area> &Obstacles) {

	Line l1(pos_, node.getPos());
	for (const Area &Obstacle : Obstacles) {
		for (auto edge : Obstacle.getEdges()) {
			PVector intersection = geometry::isIntersecting(l1, edge);
			if (!intersection)
				return false;
		}
	}

	return true;
}

int Node::findNeighbours(const std::vector<Node> &Nodes, const std::vector<Area> &Obstacles) {

	for (auto node : Nodes) {

		if (canSee(node, Obstacles)) {

			bool already_added = false;
			for (auto neighbour : neighbours_) {
				if (neighbour.first == &node) {
					already_added = true;
					break;
				}
			}

			if (!already_added)
				neighbours_.emplace_back(&node, calculateCost(node));
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

const std::vector<std::pair<Node *, double>> &Node::getNeighbours() const {
	return neighbours_;
}

/**     -----------     **/
/**                     **/
/**     Pathfinder      **/
/**                     **/
/**     -----------     **/

// Pathfinder::Pathfinder(): Constructor for Pathfinder Class
Pathfinder::Pathfinder(Field &MAP, bool trap_sensitive) {

	trap_sensitive_ = trap_sensitive;
	Field_ = &MAP;

	if (trap_sensitive) {
		for (auto node : MAP.getMapNodes({0, 1})) {
			// create & store Node
			map.emplace_back(node, Field_);
		}
		// get neighbour Nodes
		for (auto node : Pathfinder::map) {
			node.findNeighbours(Pathfinder::map, Field_->getMapObjects({0, 1}));
		}

	} else {
		for (auto node : MAP.getMapNodes({0})) {
			// create & store Node
			map.emplace_back(node, Field_);
		}
		// get neighbour Nodes
		for (auto node : Pathfinder::map) {
			node.findNeighbours(Pathfinder::map, Field_->getMapObjects({0}));
		}
	}

}

// Distance between two nodes
double Pathfinder::heuristic(const PVector &cur, const PVector &end) {
	return geometry::dist(cur, end);
}

Path Pathfinder::AStar(PVector &begin, PVector &goal) {

	// The most cost intensive part of this algorithm
	//TODO: STOPPED HERE LAST TIME

	Node start = Node(begin, Field_);
	Node end = Node(goal, Field_);

	if (trap_sensitive_) {
		start.findNeighbours(Pathfinder::map, Field_->getMapObjects({0, 1}));
		end.findNeighbours(Pathfinder::map, Field_->getMapObjects({0, 1}));
	} else {
		start.findNeighbours(Pathfinder::map, Field_->getMapObjects({0}));
		end.findNeighbours(Pathfinder::map, Field_->getMapObjects({0}));
	}

	// start = end ==> no real path
	if (begin == goal) {
		return Path({begin}, PATH_RADIUS);
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
	start.f = (start.g + heuristic(start.getPos(), end.getPos()));

	// loop until soultion is found or no solution possible
	while (!openList.empty()) {
		// choose node with lowest f
		Node *cur = openList.top();

		// if cur and end are the same, the path is found
		if (cur == &end) {

			Path path = Pathfinder::traverse(&end);

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
			for (auto neighbour : cur->getNeighbours()) {
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
						//cout << " - updated " << neighbour << " - ";
					}
				} else {
					// add
					neighbour.first->g = temp_g;
					neighbour.first->f = temp_g + heuristic(neighbour.first->getPos(), end.getPos());
					neighbour.first->previous = cur;
					openList.push(neighbour.first);
					neighbour.first->isOpen = true;
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
		//cout << endl << end.x << " | " << end.y;
	}

	return Path(to_point(t_path), PATH_RADIUS);
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

PVector Path::getClosestNormalPoint(PVector p, double aheadness) {
	double dist = 1000;
	PVector finalNormal = PVector(0, 0);
	PVector dir = PVector(0, 0);

	for (int i = 0; i < points_.size() - 1; ++i) {
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
	dir = dir * aheadness;

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
