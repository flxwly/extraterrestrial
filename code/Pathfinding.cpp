#include "Pathfinding.hpp"

#include <utility>
#include "CommonFunctions.hpp"

// Distanz zweier nodes
double heuristic(const node &cur, const node &end) {
    int xDiff = abs(cur.x - end.x);
    int yDiff = abs(cur.y - end.y);
    /*if (xDiff > yDiff) {
        return yDiff + sqrt(2) * (xDiff - yDiff);
    } else {
        return xDiff + sqrt(2) * (yDiff - xDiff);
    }*/
    //cout << H << "\n";
    return sqrt(pow(xDiff, 2) + pow(yDiff, 2));
}

AStar::AStar(const std::vector<std::vector<int>> &MAP, const int &r) {
    DEBUG_MESSAGE("Init new Pathfinder... ", 0);
    for (unsigned int i = 0; i < MAP.size(); i++) {
        const std::vector<node> _v;
        this->map.push_back(_v);
        for (unsigned int j = 0; j < MAP[i].size(); j++) {
            node _n;
            this->map[i].push_back(_n);
            this->map[i][j].x = static_cast<int>(i), this->map[i][j].y = static_cast<int>(j);
            this->map[i][j].f = 0, this->map[i][j].g = 0;
            this->map[i][j].previous = nullptr;
            // Set Traps
            this->map[i][j].isOpen = false, this->map[i][j].isClosed = false;
            this->map[i][j].isTrap = MAP[i][j] == 2, this->map[i][j].isWall = MAP[i][j] == 1;
            // get neighbours
        }
    }
    // expand walls by r
    for (unsigned int i = 0; i < this->map.size(); i++) {
        for (unsigned int j = 0; j < this->map.size(); j++) {
            if (!this->map[i][j].isAtWall && this->map[i][j].isWall) {

                // every field around i,j
                for (int x = -r; x < r; ++x) {
                    for (int y = -r; y < r; ++y) {

                        // out of bounds check
                        if (x >= 0 && x < static_cast<int>(map.size()) && y >= 0 &&
                            y < static_cast<int>(map[i].size())) {

                            // only convert walls that aren't walls
                            if (!this->map[i][j].isWall) {
                                this->map[x][y].isAtWall = true;
                                this->map[x][y].isWall = true;
                            }
                        }
                    }
                }
            }
        }
    }

    // add neighbours
    for (unsigned int i = 0; i < MAP.size(); i++) {
        for (unsigned int j = 0; j < MAP[i].size(); j++) {

            for (int x = static_cast<int>(i) - 1; x <= static_cast<int>(i) + 1; x++) {
                for (int y = static_cast<int>(j) - 1; y <= static_cast<int>(j) + 1; y++) {
                    // out of bounds check
                    if (x >= 0 && x < static_cast<int>(map.size()) && y >= 0 && y < static_cast<int>(map[i].size())) {

                        if (MAP[x][y] != 1 && (static_cast<int>(i) != x || static_cast<int>(j) != y)) {
                            map[i][j].neighbours.push_back(&map[x][y]);
                        }
                    }
                }
            }
        }
    }
    std::cout << this->map.size() << " | " << this->map[0].size() << std::endl;
    DEBUG_MESSAGE("\t finished\n", 0);
}

struct AStar::PRIORITY {
    bool operator()(node *child, node *parent) const {
        return parent->f < child->f;
    }
};

bool AStar::isPassable(node *_n, bool traps) {
    return !(traps && _n->isTrap);
}

// TODO: Fix Weird Bug, where sometimes diagonals are chosen although a straight part is faster
bool AStar::findPath(node *start, node *end, bool watchForTraps) {

    int nodesChecked = 0;

    if (start == end) {
        DEBUG_MESSAGE("Start Node is End Node\n", 2.1);
        path.clear();
        path.push_back(*start);
        return true;
    }
    // init open- & closedList
    std::priority_queue<node *, std::vector<node *>, AStar::PRIORITY> openList;
    std::vector<node *> closedList;
    // add start to openList
    openList.push(start);
    start->isOpen = true;
    double temp_g;
    // update start.g & start.f
    start->g = 0;
    start->f = (start->g + heuristic(*start, *end));
    // loop until soultion is found or no solution possible
    DEBUG_MESSAGE("Searching Path... ", 2.1);
    while (!openList.empty()) {
        // choose node with lowest f
        node *cur = openList.top();
        DEBUG_MESSAGE("\tchecking Node: " + std::to_string(cur->x) + " | " + std::to_string(cur->y) + "\n", 2.1);

        // if cur and end are the same, the path is found
        if (cur == end) {
            DEBUG_MESSAGE("found Path " + std::to_string(nodesChecked) + "\n", -3);
            AStar::traversePath(end);
            for (node *element : closedList) {
                element->isClosed = false;
            }
            while (!openList.empty()) {
                openList.top()->isOpen = false;
                openList.pop();
            }
            return true;
        } else {
            // remove cur from openList & add cur to closedList
            openList.pop();
            cur->isOpen = false;
            // for every neighbour from cur:
            for (node *neighbour : cur->neighbours) {
                if (neighbour->isClosed || !isPassable(neighbour, watchForTraps)) {
                    continue;
                }
                // temp_g = g cost over cur
                temp_g = cur->g + heuristic(*cur, *neighbour);
                // if neighbour is in openList just update | otherwise add and update
                if (neighbour->isOpen) {
                    // only if path over cur is better
                    if (neighbour->g > temp_g) {
                        //if(!pqContainsNode(openList, neighbour) || temp_g < neighbour.g)
                        // update
                        neighbour->g = temp_g;
                        neighbour->f = temp_g + heuristic(*neighbour, *end);
                        neighbour->previous = cur;
                        //cout << " - updated " << neighbour << " - ";
                    }
                } else {
                    // add
                    neighbour->g = temp_g;
                    neighbour->f = temp_g + heuristic(*neighbour, *end);
                    neighbour->previous = cur;
                    openList.push(neighbour);
                    neighbour->isOpen = true;
                    //std::cout << " - added " << neighbour << " - " << std::endl;
                }
            }
            closedList.push_back(cur);
            cur->isClosed = true;
            nodesChecked++;
        }
    }
    // resetting everything
    for (node *element : closedList) {
        element->isClosed = false;
    }
    while (!openList.empty()) {
        openList.top()->isOpen = false;
        openList.pop();
    }

    path.clear();
    DEBUG_MESSAGE("no path found: " + std::to_string(nodesChecked) + "\n", 2);
    return false;
}

bool AStar::findPath(node *start, node *end) {
    return AStar::findPath(start, end, false);
}

bool AStar::findPath(std::pair<int, int> start, std::pair<int, int> end) {
    return AStar::findPath(&AStar::map[start.first][start.second], &AStar::map[start.first][start.second]);
}

bool AStar::findPath(std::pair<int, int> start, std::pair<int, int> end, bool watch_for_traps) {
    return AStar::findPath(&AStar::map[start.first][start.second], &AStar::map[start.first][start.second],
                           watch_for_traps);
}

void AStar::traversePath(node *end) {
    DEBUG_MESSAGE("traversing path...\n", 2);
    // clear
    path.clear();
    std::vector<node> temppath;
    node *temp;

    // old direction = no direction
    std::pair<int, int> oldDirection = {0, 0};
    while (end->previous != nullptr) {
        // if old Direction - new Direction != 0 : new Direction
        //DEBUG_MESSAGE("traversing through: " + to_string(end->previous->x) + " | " + to_string(end->previous->y), 0);
        if (oldDirection.first != end->x - end->previous->x || oldDirection.second != end->y - end->previous->y) {
            oldDirection.first = end->x - end->previous->x;
            oldDirection.second = end->y - end->previous->y;
            DEBUG_MESSAGE("\ttraversing trough: " + std::to_string(end->x) + " | " + std::to_string(end->y) + "\n", 2);
        }
        temppath.push_back(*end);
        temp = end->previous;
        end->previous = nullptr;
        end = temp;
        //cout << endl << end.x << " | " << end.y;
    }

    AStar::cleanUpPath(temppath);

    DEBUG_MESSAGE("finished traversing\n", 2.1);
}

void AStar::cleanUpPath(std::vector<node> temppath) {

    if (temppath.empty()) {
        return;
    }

    // begin = front node -> while all points from begin to curNode are on line, set next Node in path to cur;
    // repeat process until one point is not on line. Then add node before curNode to path and choose it as new beginn;
    // repeat process until checking reaches lastNode

    unsigned int end = temppath.size() - 1;
    unsigned int curStart = 0, curEnd = 2;

    node *curFirst;
    node *curLast;

    DEBUG_MESSAGE("cleaning up Path...\n", 2);
    // if start < end there are still nodes to check
    while (curEnd < end) {


        curFirst = &temppath[curStart];
        // new added Node (Node until to check to)
        curLast = &temppath[curEnd];

        // loop trough every point between cur and last
        for (unsigned int i = curStart + 1; i < curEnd - 1; ++i) {
            // point temppath[i] is on line. Continue to next
            double dist;
            if (curLast->x - curFirst->x == 0 && curLast->y - curFirst->y == 0) {
                dist = sqrt(pow(curLast->y - temppath[i].y, 2) + pow(curLast->x - temppath[i].x, 2));
            } else {
                const double den = abs(
                        (curLast->y - curFirst->y) * temppath[i].x - (curLast->x - curFirst->x) * temppath[i].y +
                        curLast->x * curFirst->y - curLast->y * curFirst->x);
                const double num = sqrt(pow(curLast->y - curFirst->y, 2) + pow(curLast->x - curFirst->x, 2));
                dist = den / num;
            }
            if (dist < sqrt(2))
                continue;
            // point is not on line. set new start, add to path and break the loop
            DEBUG_MESSAGE("\tAdding: " + std::to_string(temppath[curEnd - 1].x) + " | " +
                          std::to_string(temppath[curEnd - 1].y) +
                          " to Path.\n", 2);
            curStart = curEnd - 1;
            path.push_back(temppath[curEnd - 1]);
            break;
        }
        curEnd++;
    }
}

std::vector<std::pair<int, int>> AStar::pathToPair() {
    std::vector<std::pair<int, int>> pPath;
    for (const node &n: AStar::path) {
        pPath.emplace_back(n.x, n.y);
    }
    return pPath;
}


