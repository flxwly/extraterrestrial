#include "Pathfinder.hpp"

node::node(int _x, int _y, bool _is_w, bool _is_t, bool _is_s) {
    node::isClosed = false, node::isOpen = false,
            // for nodes that may influence the path
    node::isTrap = _is_t, node::isSwamp = _is_s;
    // for nodes that cant be passed
    node::isWall = _is_w;

    // position
    node::x = _x, node::y = _y;
    // cost and value
    node::g = 0, node::f = 0;
    // used to traverse the path
    node::previous = nullptr;
}
// Distance between two nodes
double Pathfinder::heuristic(const node &cur, const node &end) {
    int xDiff = abs(cur.x - end.x);
    int yDiff = abs(cur.y - end.y);

    return sqrt(pow(xDiff, 2) + pow(yDiff, 2));
}
Pathfinder::Pathfinder(const std::vector<std::vector<int>> &MAP) {
    // copy map to Pathfinder object
    for (unsigned int i = 0; i < MAP.size(); i++) {
        // insert node array
        const std::vector<node> _v;
        Pathfinder::map.push_back(_v);
        for (unsigned int j = 0; j < MAP[i].size(); j++) {
            // insert node

            Pathfinder::map[i].push_back(
                    node(static_cast<int>(i), static_cast<int>(j), MAP[i][j] == 1, MAP[i][j] == 2, MAP[i][j] == 3));

        }
    }

    // add neighbours
    for (unsigned int i = 0; i < Pathfinder::map.size(); i++) {
        for (unsigned int j = 0; j < Pathfinder::map[i].size(); j++) {

            for (int x = static_cast<int>(i) - 1; x <= static_cast<int>(i) + 1; x++) {
                for (int y = static_cast<int>(j) - 1; y <= static_cast<int>(j) + 1; y++) {
                    // out of bounds check
                    if (x >= 0 && x < static_cast<int>(map.size()) && y >= 0 && y < static_cast<int>(map[i].size())) {

                        if (!Pathfinder::map[x][y].isWall && (static_cast<int>(i) != x || static_cast<int>(j) != y)) {
                            Pathfinder::map[i][j].neighbours.push_back(&Pathfinder::map[x][y]);
                        }
                    }
                }
            }
        }
    }

    std::cout << "created Map: " << Pathfinder::map.size() << " | " << Pathfinder::map[0].size() << std::endl;
}
bool Pathfinder::isPassable(node *_n, bool traps) {
    return !(traps && _n->isTrap);
}
std::vector<std::pair<int, int>> Pathfinder::AStar(node *start, node *end, bool watchForTraps) {

    // start = end ==> no real path
    if (start == end) {
        return {{start->x, start->y}};
    }

    // init open- & closedList
    std::priority_queue<node *, std::vector<node *>, Pathfinder::PRIORITY> openList;
    std::vector<node *> closedList;

    // add start to openList
    openList.push(start);
    start->isOpen = true;
    double temp_g;

    // update start.g & start.f
    start->g = 0;
    start->f = (start->g + heuristic(*start, *end));

    // loop until soultion is found or no solution possible
    while (!openList.empty()) {
        // choose node with lowest f
        node *cur = openList.top();

        // if cur and end are the same, the path is found
        if (cur == end) {

            std::vector<std::pair<int, int>> p_path = Pathfinder::to_pair(Pathfinder::traverse(end));

            for (node *element : closedList) {
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
            for (node *neighbour : cur->neighbours) {
                if (neighbour->isClosed || !isPassable(neighbour, watchForTraps)) {
                    continue;
                }
                // temp_g = g cost over cur
                // cost is multiplied if neighbour isSwamp
                temp_g =
                        cur->g + ((neighbour->isSwamp) ? 2 * heuristic(*cur, *neighbour) : heuristic(*cur, *neighbour));
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

    return {};
}
std::vector<std::pair<int, int>> Pathfinder::AStar(std::pair<int, int> start, std::pair<int, int> end, bool watch_for_traps) {
    return Pathfinder::AStar(&Pathfinder::map[start.first][start.second], &Pathfinder::map[end.first][end.second],
                                watch_for_traps);
}
std::vector<node> Pathfinder::traverse(node *end) {
    // clear
    std::vector<node> t_path;
    node *t_ptr;

    // old direction = no direction
    std::pair<int, int> oldDirection = {0, 0};
    while (end->previous != nullptr) {
        // if old Direction - new Direction != 0 : new Direction
        if (oldDirection.first != end->x - end->previous->x || oldDirection.second != end->y - end->previous->y) {
            oldDirection.first = end->x - end->previous->x;
            oldDirection.second = end->y - end->previous->y;
        }
        t_path.push_back(*end);
        t_ptr = end->previous;
        end->previous = nullptr;
        end = t_ptr;
        //cout << endl << end.x << " | " << end.y;
    }

    return Pathfinder::shorten(t_path);
}
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
std::vector<std::pair<int, int>> Pathfinder::to_pair(const std::vector<node> &p) {
    std::vector<std::pair<int, int>> p_path;
    p_path.reserve(p.size());
    for (const node &n: p) {
        p_path.emplace_back(n.x, n.y);
    }
    return p_path;
}




