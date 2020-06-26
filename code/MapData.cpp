#include "MapData.hpp"

MapData::MapData(int mapX, int mapY,
                 std::vector<std::pair<int, int>> *redPoints,
                 std::vector<std::pair<int, int>> *greenPoints,
                 std::vector<std::pair<int, int>> *blackPoints,
                 std::vector<std::pair<int, int>> *deposits,
                 std::vector<std::pair<int, int>> *walls,
                 std::vector<std::pair<int, int>> *traps,
                 std::vector<std::pair<int, int>> *swamps) {
    // Map Init
    MapData::MapX = mapX;
    MapData::MapY = mapY;
    for (int i = 0; i < MapX; i++) {
        const std::vector<int> _v;
        MapData::Map.push_back(_v);
        for (int j = 0; j < MapY; j++) {
            MapData::Map[i].push_back(0);
        }
    }
    for (std::pair<int, int> wall : *walls) {
        MapData::Map[wall.first][wall.second] = 1;
    }
    for (std::pair<int, int> trap : *traps) {
        MapData::Map[trap.first][trap.second] = 2;
    }
    for (std::pair<int, int> swamp : *swamps) {
        MapData::Map[swamp.first][swamp.second] = 3;
    }

    // Deposit Areas
    for (std::pair<int, int> deposit : *deposits) {
        MapData::DepositAreas.push_back(deposit);
    }

    // Point Init
    for (auto p : *redPoints) {
        MapData::RedPoints.emplace_back(p.first, p.second, deposits, 0);
    }
    for (auto p : *greenPoints) {
        MapData::GreenPoints.emplace_back(p.first, p.second, deposits, 1);
    }
    for (auto p : *blackPoints) {
        MapData::BlackPoints.emplace_back(p.first, p.second, deposits, 2);
    }

    for (auto p : MapData::RedPoints, MapData::GreenPoints, MapData::BlackPoints) {
        MapData::AllPoints.push_back(&p);
    }

    // calculate distances
    for (auto p : MapData::AllPoints) {
        p->calculate_closest_points(&this->AllPoints);
    }
}

int MapData::getPointCount(int pColor) {
    return MapData::getPtrToArr(pColor)->size();
}

int MapData::findPoint(std::pair<int, int> p, const int pColor) {
    std::vector<Point> *ptr = MapData::getPtrToArr(pColor);

    for (unsigned int i = 0; i < ptr->size(); i++) {
        if (ptr->at(i).pos() == p) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int MapData::erasePoint(const std::pair<int, int> p, const int pColor) {
    std::vector<Point> *ptr = MapData::getPtrToArr(pColor);
    int i = findPoint(p, pColor);
    if (i >= 0) {
        ptr->erase(ptr->begin() + i);
        return 1;
    }
    return 0;
}

int MapData::erasePoint(const int index, const int pColor) {
    std::vector<Point> *ptr = MapData::getPtrToArr(pColor);
    if (index >= 0 && index < static_cast<int>(ptr->size())) {
        ptr->erase(ptr->begin() + index);
        return 1;
    }
    return 0;
}

std::pair<int, int> MapData::getPoint(const int index, const int pColor) {
    std::vector<Point> *ptr = MapData::getPtrToArr(pColor);

    if (static_cast<unsigned int>(index) < ptr->size() && index >= 0) {
        return ptr->at(index).pos();
    } else {
        // TODO: Throw exeption
        return ptr->at(0).pos();
    }
}

std::vector<Point> *MapData::getPtrToArr(const int pColor) {
    switch (pColor) {
        case 0:
            return &this->RedPoints;
        case 1:
            return &this->GreenPoints;
        case 2:
            return &this->BlackPoints;
        default:
            // TODO: Throw exeption;
            return &this->RedPoints;
    }
}

std::vector<std::pair<int, int>> MapData::getDepositAreas() {
    return MapData::DepositAreas;
}

int MapData::availableColors() {
    int sum = 0;
    if (MapData::getPointCount(0) > 0) {
        sum += 1;
    }
    if (MapData::getPointCount(1) > 0) {
        sum += 2;
    }
    if (MapData::getPointCount(2) > 0) {
        sum += 4;
    }
    return sum;
}

std::vector<Point *> MapData::getAllPoints() {

    return MapData::AllPoints;
}

std::vector<std::pair<int, int>> MapData::getNodes() {
    return MapData::Nodes;
}

std::pair<std::vector<Point *>, std::pair<int, int>> MapData::get_path(std::array<int, 3> loaded_objects, int loaded_objects_num, std::pair<int, int> pos) {
    // point array which which will later be returned
    std::vector<Point *> point_path;

    // point finding algorithm. (See Idea 4 on https://stackoverflow.com/questions/62179174)
    //      This isn't an optimal solution

    double b_overall_dist = -1;
    std::pair<int, int> t_deposit_area;

    for (auto deposit_area : MapData::getDepositAreas()) {
        t_deposit_area = deposit_area;
        // temporary array to be able to compare
        std::vector<Point *> cur_path;
        cur_path.emplace_back(MapData::getClosestPoint(pos));

        // p_dist is used to decide which route to which deposit_area to pick
        double cur_path_dist = 0;

        // add upto 6 points to cur_path;
        for (int i = loaded_objects_num; i < 6; ++i) {

            Point *b_point = nullptr;
            double b_f_cost = -1;

            // getAllPoints() returns a vector<array<int, 3>>:
            //      get the point with the lowest f
            for (const auto &point : MapData::getAllPoints()) {

                // check if the object_type is chosen less or equal to 2 times
                if (loaded_objects[point->color()] <= 2) {

                    // g_dist is the dist from the last chosen point to the current point
                    double g_dist = dist(point->pos().first, cur_path.back()->pos().first, point->pos().second,
                                         cur_path.back()->pos().second);
                    // h_dist is the distance to the deposit area
                    double h_dist = dist(point->pos().first, deposit_area.first, point->pos().second,
                                         deposit_area.second);

                    // if f_cost is lower set f_cost to cur_cost
                    if (g_dist + h_dist < b_f_cost || b_point == nullptr) {
                        // check if point is already in path
                        if (std::find(cur_path.begin(), cur_path.end(), point) != cur_path.end()) {
                            b_f_cost = g_dist + h_dist;
                            b_point = point;
                        }
                    }
                }
            }

            loaded_objects[b_point->color()]++;
            // The best point is added to the cur_path alongside with it's distance
            cur_path_dist += dist(b_point->pos().first, cur_path.back()->pos().first, b_point->pos().second, cur_path.back()->pos().first);
            cur_path.emplace_back(b_point);
        }
        // add the dist from the last point to the deposit_area
        cur_path_dist += dist(deposit_area.first, cur_path.back()->pos().first, deposit_area.second, cur_path.back()->pos().first);

        // if the overall path length is shorter then before -> set best to cur
        if (cur_path_dist < b_overall_dist || b_overall_dist == -1) {
            b_overall_dist = cur_path_dist;

            // this wont overfill because point_path gets set to cur_path and therefore forgets it's earlier content
            point_path = cur_path;

        }
    }

    return {point_path, t_deposit_area};

}

Point *MapData::getClosestPoint(std::pair<int, int> pos) {
    double b_dist = 0;
    Point *b_point = nullptr;
    for (const auto &p : MapData::getAllPoints()) {
        if (dist(p->pos().first, pos.first, p->pos().second, pos.second) < b_dist || b_point == nullptr) {
            b_dist = dist(p->pos().first, pos.first, p->pos().second, pos.second);
            b_point = p;
        }
    }
    return b_point;
}
