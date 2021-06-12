#include <algorithm>
#include "MapData.hpp"


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
    return (geometry::dist(robotPos + (geometry::angle2Vector(toRadians(angle - COLOR_SENSOR_ANGLE_OFFSET)) *
                                       COLOR_SENSOR_DIST_TO_CORE), pos) < uncertainty
            || geometry::dist(robotPos + (geometry::angle2Vector(toRadians(angle + COLOR_SENSOR_ANGLE_OFFSET)) *
                                          COLOR_SENSOR_DIST_TO_CORE), pos) < uncertainty)
           && possibleStates.empty()
           || std::find(possibleStates.begin(), possibleStates.end(), state) != possibleStates.end();
}

bool Collectible::operator==(const Collectible &lhs) const {
    return pos == lhs.pos && color == lhs.color;
}


/**     -----------     **/
/**                     **/
/**         Line        **/
/**                     **/
/**     -----------     **/

Line::Line(const PVector &_p1, const PVector &_p2) : p1{_p1}, p2{_p2} {}

/**     -----------     **/
/**                     **/
/**         Area        **/
/**                     **/
/**     -----------     **/

// Area::Area(): Constructor for Area class
Area::Area(const std::vector<PVector> &p_s) : min_{p_s.front()}, max_{0, 0}, Corners_{p_s} {

    PVector last_p = p_s.back();
    for (PVector p : p_s) {
        Edges_.emplace_back(last_p, p);
        last_p = p;

        // Set boundary box posX
        if (p.x < min_.x) {
            min_.x = p.x;
        }
        if (p.x > max_.x) {
            max_.x = p.x;
        }

        // Set boundary box posY
        if (p.y < min_.y) {
            min_.y = p.y;
        }
        if (p.y > max_.y) {
            max_.y = p.y;
        }
    }
}

const std::vector<PVector> &Area::getCorners() const {
    return Corners_;
}

const std::vector<Line> &Area::getEdges() const {
    return Edges_;
}

const PVector &Area::getMin() const {
    return min_;
}

const PVector &Area::getMax() const {
    return max_;
}


/**     -----------     **/
/**                     **/
/**       MapData       **/
/**                     **/
/**     -----------     **/


Field::Field(const int &width, const int &height,
             const std::vector<Area> &walls,
             const std::vector<Area> &traps,
             const std::vector<Area> &swamps,
             const std::vector<Area> &waters,
             const std::vector<PVector> &deposits,
             const std::vector<PVector> &wallNodes,
             const std::vector<PVector> &trapNodes,
             const std::vector<PVector> &swampNodes,
             const std::vector<Collectible> &collectibles) :
        size_{static_cast<double> (width), static_cast<double> (height)} {

    FIELD_LOG("Creating new Field-object")

    // field::(MapObjectName)_: Different MapObjects that can be displayed as areas.
    Walls_ = walls;
    FIELD_LOG("\t" << Walls_.size() << " walls")
    Traps_ = traps;
    FIELD_LOG("\t" << Traps_.size() << " traps")
    Swamps_ = swamps;
    FIELD_LOG("\t" << Swamps_.size() << " swamps")
    Waters_ = waters;
    FIELD_LOG("\t" << Waters_.size() << " bonus zones")

    // field::deposits: Deposit Areas of the field
    Deposits_ = deposits;
    FIELD_LOG("\t" << deposits.size() << " deposits")

    // Different nodes
    WallNodes_ = wallNodes;
    FIELD_LOG("\t" << WallNodes_.size() << " wall nodes")
    TrapNodes_ = trapNodes;
    FIELD_LOG("\t" << TrapNodes_.size() << " trap nodes")
    SwampNodes_ = swampNodes;
    FIELD_LOG("\t" << SwampNodes_.size() << " swamp nodes")



    // field::Collectibles: Collectible Points of the field
    for (auto collectible : collectibles) {
        if (0 <= collectible.color && collectible.color <= 3) {
            Collectibles_[collectible.color].push_back(collectible);
        } else {
            FIELD_ERROR("Can't add Collectible at: " << collectible.pos << " with color: " << collectible.color
                                                     << "\n\t -> Collectible will be skipped.")
        }
    }
}

PVector Field::getSize() const {
    return size_;
}

std::vector<Area> Field::getMapObjects(const std::vector<unsigned int> &indices) {
    std::vector<Area> returnVector = {};
    // return after all indices have been checked.
    for (unsigned int index : indices) {
        switch (index) {
            case 0:
            FIELD_LOG("Returnvector capacity (case 0): " << returnVector.capacity() << " reserving space for " << Walls_.size() << " elements")
                returnVector.insert(std::end(returnVector), std::begin(Walls_), std::end(Walls_));
                break;
            case 1:
            FIELD_LOG("Returnvector capacity (case 1): " << returnVector.capacity() << " reserving space for " << Traps_.size() << " elements")
                returnVector.insert(std::end(returnVector), std::begin(Traps_), std::end(Traps_));
                break;
            case 2:
            FIELD_LOG("Returnvector capacity (case 2): " << returnVector.capacity() << " reserving space for " << Swamps_.size() << " elements")
                returnVector.insert(std::end(returnVector), std::begin(Swamps_), std::end(Swamps_));
                break;
            case 3:
            FIELD_LOG("Returnvector capacity (case 3): " << returnVector.capacity() << " reserving space for " << Waters_.size() << " elements")
                returnVector.insert(std::end(returnVector), std::begin(Waters_), std::end(Waters_));
                break;
            default: FIELD_ERROR("index out of range/invalid")
        }
    }

    FIELD_LOG("Returning " << returnVector.size() << " map objects")
    return returnVector;
}

std::vector<PVector> Field::getMapNodes(const std::vector<unsigned int> &indices) {
    std::vector<PVector> returnVector = {};

    // return after all indices have been checked.
    for (unsigned int index : indices) {
        switch (index) {
            case 0:
                returnVector.insert(returnVector.end(), WallNodes_.begin(),
                                    WallNodes_.end());
                break;
            case 1:
                returnVector.insert(std::end(returnVector), std::begin(TrapNodes_),
                                    std::end(TrapNodes_));
                break;
            case 2:
                returnVector.insert(std::end(returnVector), std::begin(SwampNodes_),
                                    std::end(SwampNodes_));
                break;
            default: FIELD_ERROR("index out of range/invalid")
                break;
        }
    }

    FIELD_LOG("Returning " << returnVector.size() << " map nodes")
    return returnVector;
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

// geometry::isInside():  Test if a point p2 is left/on/right a line through p0 and p1.
//      Input:  PVector p0, p1, p2
//      Return: >0 left; =0 on; <0 right
bool geometry::isInside(const PVector &p, Area &area) {
    // PVector in Polygon(PIP) using the winding number algorithm:
    // source: https://en.wikipedia.ord/wiki/Point_in_polygon

    int wn = 0;    // the  winding number counter
    unsigned int n = area.getCorners().size(); // The number of corners

    std::vector<PVector> poly = area.getCorners();      // vector with all corners + V[0] at V[n+1]
    poly.push_back(poly.front());

    // loop through all edges of the polygon
    for (unsigned int i = 0; i < n; i++) {

        // edge from poly[i] to  poly[i+1]
        if (poly[i].y <= p.y) {                                 // start posY <= P.posY
            if (poly[i + 1].y > p.y)                            // an upward crossing
                if (isLeft(poly[i], poly[i + 1], p))        // P left of  edge
                    ++wn;                                       // have  a valid up intersect
        } else {                                                // start posY > P.posY (no test needed)
            if (poly[i + 1].y <= p.y)                           // a downward crossing
                if (!isLeft(poly[i], poly[i + 1], p))        // P right of  edge
                    --wn;                                       // have  a valid down intersect
        }
    }
    return wn != 0;
}

PVector geometry::intersection(Line &l1, Line &l2) {
    // line - line intersection using determinants:
    // source: https://en.wikipedia.ord/wiki/Line%E2%%80%90line_intersection


    const double den = (l1.p1.x - l1.p2.x) * (l2.p1.y - l2.p2.y) - (l1.p1.y - l1.p2.y) * (l2.p1.x - l2.p2.x);
    // Lines are parallel and don't cross
    if (den == 0) {
        return {};
    }

    const double t = ((l1.p1.x - l2.p1.x) * (l2.p1.y - l2.p2.y) - (l1.p1.y - l2.p1.y) * (l2.p1.x - l2.p2.x)) / den;
    const double u = -((l1.p1.x - l1.p2.x) * (l1.p1.y - l2.p1.y) - (l1.p1.y - l1.p2.y) * (l1.p1.x - l2.p1.x)) / den;

    if (0 <= t && t <= 1 && 0 <= u && u <= 1) {
        return {l1.p1.x + t * (l1.p2.x - l1.p1.x),
                l1.p1.y + t * (l1.p2.y - l1.p1.y)};
    }

    // Lines don't cross
    return {};
}

bool geometry::isIntersecting(Line &l1, Line &l2) {
    return static_cast<bool>(intersection(l1, l2));
}

bool geometry::isIntersecting(Line l1, const std::vector<Area> &Obstacles) {
    for (const Area &Obstacle : Obstacles) {
        for (auto edge : Obstacle.getEdges()) {
            if (geometry::isIntersecting(l1, edge)) {
                return true;
            }
        }
    }
    return false;
}

double geometry::sqDist(const PVector &p1, const PVector &p2) {
    return pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2);
}

double geometry::dist(const PVector &p1, const PVector &p2) {
    return sqrt(geometry::sqDist(p1, p2));
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

PVector geometry::getNormalPoint(Line line, PVector point) {
    PVector ap = point - line.p1;
    PVector ab = line.p2 - line.p1;

    // normalize ab (set mag to 1)
    ab.setMag(1);
    ab = ab * geometry::dot(ap, ab);

    PVector normalPoint = PVector(line.p1.x + ab.x, line.p1.y + ab.y);

    // normal point
    return normalPoint;
}


///   _______                _____          __
///  |   |   |.---.-..-----.|     \ .---.-.|  |_ .---.-.
///  |       ||  _  ||  _  ||  --  ||  _  ||   _||  _  |
///  |__|_|__||___._||   __||_____/ |___._||____||___._|
///                  |__|

#pragma region MapData



//------------- Game0_Objects --------------//

/*walls*/
const std::vector<Area> GAME0WALLS = {Area({{10, 10},{10, 20},{20, 20}})};
/*traps*/
const std::vector<Area> GAME0TRAPS = {};
/*swamps*/
const std::vector<Area> GAME0SWAMPS = {};
/*Water*/
const std::vector<Area> GAME0WATERS = {};
/*deposit*/
const std::vector<PVector> GAME0DEPOSITS = {};

//------ Nodes ------//
/*wall_nodes*/
const std::vector<PVector> GAME0WALLNODES = {};
/*trap_nodes*/
const std::vector<PVector> GAME0TRAPNODES = {};
/*swamp_nodes*/
const std::vector<PVector> GAME0SWAMPNODES = {};



//------ Collectibles ------//
/*collectibles*/
const std::vector<Collectible> GAME0COLLECTIBLES = {};

//------------- Game1_Objects --------------//

/*walls*/
const std::vector<Area> GAME1WALLS = {};
/*traps*/
const std::vector<Area> GAME1TRAPS = {};
/*swamps*/
const std::vector<Area> GAME1SWAMPS = {};
/*Water*/
const std::vector<Area> GAME1WATERS = {};
/*deposit*/
const std::vector<PVector> GAME1DEPOSITS = {};

//------ Nodes ------//
/*wall_nodes*/
const std::vector<PVector> GAME1WALLNODES = {};
/*trap_nodes*/
const std::vector<PVector> GAME1TRAPNODES = {};
/*swamp_nodes*/
const std::vector<PVector> GAME1SWAMPNODES = {};



//------ Collectibles ------//
/*collectibles*/
const std::vector<Collectible> GAME1COLLECTIBLES = {};


#pragma endregion
