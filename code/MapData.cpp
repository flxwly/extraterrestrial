#include "MapData.hpp"

//   ______            __
//  |      |.-----..--|  |.-----.
//  |   ---||  _  ||  _  ||  -__|
//  |______||_____||_____||_____|
//

/**     -----------     **/
/**                     **/
/**     Collectible     **/
/**                     **/
/**     -----------     **/

Collectible::Collectible(const PVector &p, const unsigned int &c, bool worthDouble) :
        pos(p), state(0), visited(0), isWorthDouble(worthDouble) {
    if (c <= 3) {
        color = c;
    } else {
        COLLECTIBLE_ERROR("Can't initialize Collectible with color " + std::to_string(c) + "!")
        return;
    }
}

bool Collectible::isCorrectCollectible(PVector robotPos, double angle, double uncertainty,
                                       std::vector<int> possibleStates) const {
    return (geometry::dist(robotPos + (geometry::angle2Vector((angle - COLOR_SENSOR_ANGLE_OFFSET) * M_PI / 180) *
                                       COLOR_SENSOR_DIST_TO_CORE), pos) < uncertainty
            || geometry::dist(robotPos + (geometry::angle2Vector((angle + COLOR_SENSOR_ANGLE_OFFSET) * M_PI / 180) *
                                          COLOR_SENSOR_DIST_TO_CORE), pos) < uncertainty)
           && (possibleStates.empty()
           || std::find(possibleStates.begin(), possibleStates.end(), state) != possibleStates.end());
}

bool Collectible::operator==(const Collectible &lhs) const {
    return pos == lhs.pos && color == lhs.color;
}


/**     -----------    **/
/**                    **/
/**        Field       **/
/**                    **/
/**     -----------    **/


Field::Field(const int &width, const int &height,
             const double &scaleX, const double &scaleY, const std::string &map,
             const std::vector<PVector> &deposits,
             const std::vector<Collectible> &collectibles) : width_(width), height_(height) {

    FIELD_LOG("Creating new Field-object")

    // field::deposits: Deposit Areas of the field
    Deposits_ = deposits;
    scale_ = {scaleX, scaleY};
    Map_ = map;

    // field::Collectibles: Collectible Points of the field
    for (auto collectible : collectibles) {
        if (collectible.color <= 3) {
            collectible.pos.x /= scaleX;
            collectible.pos.y /= scaleY;
            Collectibles_[collectible.color].push_back(collectible);
        } else {
            FIELD_ERROR("Can't add Collectible at: " << collectible.pos << " with color: " << collectible.color
                                                     << "\n\t -> Collectible will be skipped.")
        }
    }
}

/** Getter for field::collectibles **/
std::vector<PVector> Field::getDeposits() {
    return Deposits_;
}

/** Getter for field::collectibles **/
std::vector<Collectible *> Field::getCollectibles(const std::vector<unsigned int> &colors) {
    std::vector<Collectible *> returnVector = {};
    // return after all indices have been checked.
    for (unsigned int index : colors) {
        if (index <= 3) {
            for (Collectible &collectible : Collectibles_[index]) {
                returnVector.push_back(&collectible);
            }
        } else {
            FIELD_ERROR("index out of range");
        }
    }

    FIELD_LOG("Returning " << returnVector.size() << " collectibles")
    return returnVector;
}


void Field::spawnTempWall(PVector pos, int r) {

    tempWallTiles_.push_back({std::chrono::steady_clock::now(), {}});

    const int centerX = static_cast<int>(round(pos.x * scale_.x));
    const int centerY = static_cast<int>(round(pos.y * scale_.y));
    const int radius = static_cast<int>(round(pos.x * geometry::dist({0, 0}, scale_)));
    int index;

    for (int x = -radius + centerX; x < radius + centerX; ++x) {
        for (int y = -radius + centerY; y < radius + centerY; ++y) {

            if ((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) > r * r ||
                x < 0 || x >= width_ || y < 0 || y >= height_) {
                continue;
            }

            index = idx(x, y);

            if (Map_[index] == MAP_EMPTY_TILE) {
                tempWallTiles_.back().second.push_back(index);
                Map_.at(index) = MAP_TEMP_WALL_TILE;
            }
        }
    }

    FIELD_LOG("After spawning: " << Map_)
}


void Field::clearTempWall(double lifetime) {

    int removed = 0;
    while (!tempWallTiles_.empty()) {
        if (std::chrono::duration<double, std::milli>(
                std::chrono::steady_clock::now() - tempWallTiles_.front().first).count() >= lifetime) {
            for (auto index : tempWallTiles_.front().second) {
                Map_.at(index) = MAP_EMPTY_TILE;
            }

            tempWallTiles_.pop_front();
            removed++;
        } else {
            return;
        }
    }

    if (removed != 0) {
        FIELD_LOG("After clearing " << removed << " objects: " << Map_)
    }
}


Collectible *
Field::getCollectible(PVector robotPos, double angle, double uncertainty, int color, std::vector<int> possibleStates) {

    std::vector<Collectible *> collectibles;

    if (color == -1) {
        collectibles = getCollectibles({0, 1, 2, 3});
    } else if (color >= 0 && color <= 3) {
        collectibles = getCollectibles({static_cast<unsigned int> (color)});
    } else {
        FIELD_ERROR(color << " is an invalid color")
    }

    for (auto collectible : collectibles) {
        if (collectible->isCorrectCollectible(robotPos, angle, uncertainty, possibleStates)) {
            return collectible;
        }
    }

    FIELD_WARNING("no valid collectible found")
    return nullptr;
}

Collectible *Field::addCollectible(Collectible collectible) {
    Collectibles_[collectible.color].push_back(collectible);
    return &Collectibles_[collectible.color].back();
}

bool Field::removeCollectible(Collectible collectible) {
    auto it = std::find(Collectibles_[collectible.color].begin(), Collectibles_[collectible.color].end(), collectible);

    if (it != Collectibles_[collectible.color].end()) {
        Collectibles_[collectible.color].erase(it);
        FIELD_LOG("Successfully removed Collectible")
        return true;
    }
    FIELD_WARNING("Couldn't remove Collectible")
    return false;
}

// TODO: The pathfinding function seems to fail or delete the found path entirely.
//  Fix it so that it doesn't fail anymore
std::vector<PVector> Field::AStarFindPath(PVector start, PVector end) {

    PATHFINDER_LOG("Setting up path finder... from " << round(start.x * scale_.x) << " | " << round(start.y * scale_.y)
                                                     << " to " << round(end.x * scale_.x) << " | "
                                                     << round(end.y * scale_.y))

    auto comp = [](const std::tuple<double, int> &n1, const std::tuple<double, int> &n2) {
        return std::get<0>(n1) > std::get<0>(n2);
    };

    const int startIdx = idx(static_cast<int>(floor(start.x * scale_.x)), static_cast<int>(round(start.y * scale_.y)));
    const int endIdx = idx(static_cast<int>(floor(end.x * scale_.x)), static_cast<int>(round(end.y * scale_.y)));

    if (startIdx == endIdx) {
        PATHFINDER_WARNING("Start is end")
        return {end};
    }

    const int size = width_ * height_;

    if (startIdx >= size || endIdx >= size) {
        PATHFINDER_ERROR("Start or end is outside of the map!")
        return {};
    }

    std::priority_queue<std::tuple<double, int>,
            std::vector<std::tuple<double, int>>, decltype(comp)> pq(comp);
    std::vector<int> p(size); // prev
    std::vector<double> d(size, INT_MAX); // dist (filled with max distance)

    PATHFINDER_LOG("Starting path finder...")

    d[startIdx] = 0;
    pq.push(std::make_tuple(0 + heuristic(startIdx, endIdx), startIdx));

    while (!pq.empty()) {

        int u = std::get<1>(pq.top());
        pq.pop(); // Get top element

        for (auto e : {-width_ - 1, -width_ + 1, +width_ - 1, +width_ + 1, +1, -1, +width_, -width_}) {
            int v = u + e; // neighbor node
            if (((e == 1 || e == -width_ + 1 || e == width_ + 1) && (v % width_ == 0)) // x-bounds
                || ((e == -1 || e == -width_ - 1 || e == width_ - 1) && (u % width_ == 0)))
                continue;

            if (0 <= v && v < size) { // Out of bounds check
                if (d[v] > d[u] + heuristic(u, v) && Map_[v] == MAP_EMPTY_TILE) {
                    p[v] = u;
                    d[v] = d[u] + heuristic(u, v);

                    if (v == endIdx) {
                        PATHFINDER_LOG("Path finder found a path")

                        std::vector<PVector> tPath{}, path{};

                        // Traverse whole path back
                        for (int i = endIdx; i != startIdx;) {
                            PVector cur = coord(i);
                            i = p[i];
                            tPath.emplace_back(cur.x, cur.y);
                        }
                        tPath.push_back(start);


                        // shorten path to only necessary nodes
                        int i = 0, j = 0;
                        while (i < tPath.size() - 1 && j < tPath.size()) {

                            path.push_back(tPath[i]);

                            for (j = i + 1; j < tPath.size(); ++j) {

                                for (int k = i; k < j; ++k) {
                                    // Check if inbetween nodes are one a line
                                    if (geometry::distToLine(tPath[i], tPath[j], tPath[k]) > 1) {
                                        // inbetween node is not on a line, go back one and break out of the checking loop
                                        i = j - 1;
                                        k = j;
                                    }
                                }
                            }
                        }
                        path.push_back(start);
                        PATHFINDER_LOG("Returning " << path.size() << " Nodes")
                        return path;

                    }

                    pq.push(std::make_tuple(d[v] + heuristic(v, endIdx), v));

                }
            }
        }
    }

    PATHFINDER_WARNING("Path finder couldn't find a path")
    return {};
}

int Field::idx(int x, int y) const {
    return x + y * width_;
}

double Field::heuristic(int idx1, int idx2) const {
    return sqrt(pow(idx1 % width_ - idx2 % width_, 2) + pow(idx1 / width_ - idx2 / width_, 2));
}

PVector Field::coord(int idx) const {
    return PVector(idx % width_, idx / width_);
}

PVector Field::getScale() const {
    return scale_;
}

std::string Field::getMap() {
    return Map_;
}

std::string Field::getFlippedMap() {
    std::string newMap;
    newMap.reserve(Map_.size());
    for (int j = 0; j < height_; ++j) {
        for (int i = 0; i < width_; ++i) {
            newMap.at(i + (height_ - j - 1) * width_) = Map_.at(i + j * width_);
        }
    }
    return newMap;
}

char Field::getMapAt(int x, int y) {
    return Map_.at(x + y * width_);
}

char Field::getMapAtRealPos(PVector pos) {
    return Map_.at(idx(static_cast<int>(round(pos.x * scale_.x)), static_cast<int>(round(pos.y * scale_.y))));
}


int Field::getHeight() const {
    return height_;
}

[[maybe_unused]] int Field::getWidth() {
    return width_;
}


/**     -----------     **/
/**                     **/
/**      geometry       **/
/**                     **/
/**     -----------     **/


// geometry::onSide():  Test if a point p2 is left/on/right a line through p0 and p1.
//      Input:  PVector p0, p1, p2
//      Return: >0 left; =0 on; <0 right
bool geometry::isLeft(PVector p0, PVector p1, PVector p2) {
    return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y) > 0;
}

double geometry::sqDist(double x1, double y1, double x2, double y2) {
    return pow(x1 - x2, 2) + pow(y1 - y2, 2);
}

double geometry::sqDist(const PVector &p1, const PVector &p2) {
    return pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2);
}

double geometry::dist(const PVector &p1, const PVector &p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

double geometry::dist(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

PVector geometry::angle2Vector(double a) {
    return {cos(a), sin(a)};
}

double geometry::vector2Angle(double x, double y) {

    double angle = atan2(y, x);

    return (angle >= 0) ? angle : 2 * M_PI + angle;
}

double geometry::vector2Angle(PVector v) {
    return geometry::vector2Angle(v.x, v.y);
}

double geometry::dot(PVector p1, PVector p2) {
    return p1.x * p2.x + p1.y * p2.y;
}

double geometry::distToLine(double p1_x, double p1_y, double p2_x, double p2_y, double p0_x, double p0_y) {
    return fabs((p2_x - p1_x) * (p1_y - p0_y) - (p1_x - p0_x) * (p2_y - p1_y)) /
           sqrt((p2_x - p1_x) * (p2_x - p1_x) + (p2_y - p1_y) * (p2_y - p1_y));
}

double geometry::distToLine(PVector p1, PVector p2, PVector p0) {
    return distToLine(p1.x, p1.y, p2.x, p2.y, p0.x, p0.y);
}

bool geometry::isInside(PVector point, PVector min, PVector max) {
    return min.x <= point.x && point.x < max.x && min.y <= point.y && point.y < max.y;
}
