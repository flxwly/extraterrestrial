#include "MapData.hpp"


/**     -----------     **/
/**                     **/
/**     Collectible     **/
/**                     **/
/**     -----------     **/

Collectible::Collectible(const PVector &p, const unsigned int &c) : pos{p}, color{c}, state{0} {}

bool Collectible::isCorrectCollectible(PVector robotPos, double angle, double uncertainty) const {

    auto p = robotPos + (geometry::angle2Vector(angle + COLOR_SENSOR_ANGLE_OFFSET) * COLOR_SENSOR_DIST_TO_CORE);

    if (geometry::dist(p, pos) < uncertainty) {
        return true;
    }

    p = robotPos + (geometry::angle2Vector(angle - COLOR_SENSOR_ANGLE_OFFSET) * COLOR_SENSOR_DIST_TO_CORE);
    if (geometry::dist(p, pos) < uncertainty) {
        return true;
    }
    return false;
}


/**     -----------     **/
/**                     **/
/**         Line        **/
/**                     **/
/**     -----------     **/

Line::Line(const PVector &p1, const PVector &p2) : p1{p1}, p2{p2} {}

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
             const std::vector<Collectible> &collectibles) :
        size_{static_cast<double>(width), static_cast<double>(height)} {

    // field::(MapObjectName)_: Different MapObjects that can be displayed as areas.
    Walls_ = walls;
    Traps_ = traps;
    Swamps_ = swamps;
    Waters_ = waters;

    // field::deposits: Deposit Areas of the field
    Deposits_ = deposits;
    WallNodes_ = wallNodes;
    TrapNodes_ = trapNodes;

    // field::Collectibles: Collectible Points of the field
    for (auto collectible : collectibles) {
        Collectibles_[collectible.color].push_back(collectible);
    }
}

PVector Field::getSize() {
    return size_;
}

std::vector<Area> Field::getMapObjects(const std::vector<unsigned int> &indices) {
    std::vector<Area> returnVector = {};
    // return after all indices have been checked.
    for (unsigned int index : indices) {
        switch (index) {
            case 0:
                returnVector.insert(std::end(returnVector), std::begin(Walls_), std::end(Walls_));
                break;
            case 1:
                returnVector.insert(std::end(returnVector), std::begin(Traps_), std::end(Traps_));
                break;
            case 2:
                returnVector.insert(std::end(returnVector), std::begin(Swamps_), std::end(Swamps_));
                break;
            case 3:
                returnVector.insert(std::end(returnVector), std::begin(Waters_), std::end(Waters_));
                break;
            default: ERROR_MESSAGE("index out of range/invalid")
        }
    }

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
            default: ERROR_MESSAGE("index out of range/invalid")
                break;
        }
    }

    ERROR_MESSAGE(returnVector.size())
    return returnVector;
}

/** Getter for field::collectibles **/
std::vector<PVector> Field::getDeposits() {
    return Deposits_;
}

/** Getter for field::collectibles **/
std::vector<Collectible> Field::getCollectibles(const std::vector<unsigned int> &colors) {
    std::vector<Collectible> returnVector = {};
    // return after all indices have been checked.
    for (unsigned int index : colors) {
        if (index <= 2) {
            returnVector.insert(std::end(returnVector), std::begin(Collectibles_[index]),
                                std::end(Collectibles_[index]));
        } else {
            ERROR_MESSAGE("index out of range");
        }
    }
    return returnVector;
}

Collectible *Field::getCollectible(PVector robot_pos, double angle, double uncertainty, int color) {

    if (color < 0 || color > 3) {
        ERROR_MESSAGE("color " << color << " is not existing!")
        return nullptr;
    }

    auto collectibles = Collectibles_[color];

    for (auto &collectible : collectibles) {
        if (collectible.isCorrectCollectible(robot_pos, angle, uncertainty)) {
            return &collectible;
        }
    }

    ERROR_MESSAGE("no valid collectible found")
    return nullptr;
}

//TODO
std::vector<PVector> Field::getPointPath() {
    std::vector<PVector> c;

    c.reserve(6);
    for (int i = 0; i < 6; ++i) {
        c.push_back(Collectibles_[0][i].pos);
    }
    return c;
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

PVector geometry::isIntersecting(Line &l1, Line &l2) {
    // line - line intersection using determinants:
    // source: https://en.wikipedia.ord/wiki/Line%E2%%80%90line_intersection

    // determiant = x1 * y2- x2 * y1
    double L1 = l1.p1.x * l1.p2.y - l1.p2.x * l1.p1.y;
    double L2 = l2.p1.x * l2.p2.y - l2.p2.x * l2.p1.y;

    // difs
    double L1xdif = l1.p1.x - l1.p2.x;
    double L1ydif = l1.p1.y - l1.p2.y;
    double L2xdif = l2.p1.x - l2.p2.x;
    double L2ydif = l2.p1.y - l2.p2.y;

    // determiant a*d - b*c
    double xnom = L1 * L2xdif - L2 * L1xdif;
    double ynom = L1 * L2ydif - L2 * L2ydif;
    double denom = L1xdif * L2ydif - L2xdif * L1ydif;

    // Lines don't cross
    if (denom == 0) {
        return {-1, -1};
    }

    // return intersection
    return {xnom / denom, ynom / denom};
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

    return (angle > 0) ? angle : 2 * M_PI + angle;
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



//------------- Game0_Objects --------------//

/*walls*/
const std::vector<Area> GAME0WALLS = {
        Area({{90,  164},
              {154, 164},
              {154, 145},
              {90,  145}}),
        Area({{47, 128},
              {66, 128},
              {66, 64},
              {47, 64}}),
        Area({{191, 128},
              {209, 128},
              {209, 64},
              {191, 64}}),
        Area({{93,  31},
              {156, 31},
              {156, 13},
              {93,  13}})};
/*traps*/
const std::vector<Area> GAME0TRAPS = {
        Area({{107, 113},
              {136, 113},
              {136, 85},
              {107, 85}})};
/*swamps*/
const std::vector<Area> GAME0SWAMPS = {};
/*Water*/
const std::vector<Area> GAME0WATERS = {
        Area({{59,  151},
              {89,  151},
              {89,  144},
              {103, 144},
              {103, 110},
              {67,  110},
              {67,  129},
              {59,  129}})};
/*deposit*/
const std::vector<PVector> GAME0DEPOSITS = {{197, 27},
                                            {38,  147}};

//------ Nodes ------//
/*wall_nodes*/
const std::vector<PVector> GAME0WALLNODES = {{89,  165},
                                             {155, 165},
                                             {155, 144},
                                             {89,  144},
                                             {46,  129},
                                             {67,  129},
                                             {67,  63},
                                             {46,  63},
                                             {190, 129},
                                             {210, 129},
                                             {210, 63},
                                             {190, 63},
                                             {92,  32},
                                             {157, 32},
                                             {157, 12},
                                             {92,  12}};
/*trap_nodes*/
const std::vector<PVector> GAME0TRAPNODES = {{106, 114},
                                             {137, 114},
                                             {137, 84},
                                             {106, 84}};



//------ Collectibles ------//
/*collectibles*/
const std::vector<Collectible> GAME0COLLECTIBLES = {{{157, 60},  2},
                                                    {{175, 49},  2},
                                                    {{173, 72},  2},
                                                    {{141, 42},  2},
                                                    {{137, 76},  2},
                                                    {{125, 58},  2},
                                                    {{138, 60},  2},
                                                    {{119, 69},  2},
                                                    {{105, 44},  2},
                                                    {{109, 78},  2},
                                                    {{165, 84},  2},
                                                    {{184, 64},  2},
                                                    {{155, 71},  2},
                                                    {{152, 95},  2},
                                                    {{167, 104}, 2},
                                                    {{187, 86},  2},
                                                    {{186, 125}, 2},
                                                    {{152, 86},  2},
                                                    {{154, 118}, 2},
                                                    {{106, 60},  2},
                                                    {{119, 47},  2},
                                                    {{124, 74},  2},
                                                    {{99,  51},  2},
                                                    {{104, 70},  2},
                                                    {{35,  148}, 0},
                                                    {{49,  132}, 0},
                                                    {{48,  166}, 0},
                                                    {{21,  130}, 0},
                                                    {{21,  160}, 0},
                                                    {{49,  148}, 0},
                                                    {{63,  168}, 0},
                                                    {{77,  167}, 0},
                                                    {{88,  163}, 0},
                                                    {{36,  162}, 0},
                                                    {{20,  53},  1},
                                                    {{34,  118}, 0},
                                                    {{38,  105}, 0},
                                                    {{22,  116}, 0},
                                                    {{11,  106}, 0},
                                                    {{66,  138}, 0},
                                                    {{81,  144}, 0},
                                                    {{72,  124}, 0},
                                                    {{84,  131}, 0},
                                                    {{93,  141}, 0},
                                                    {{96,  125}, 0},
                                                    {{84,  117}, 0},
                                                    {{219, 133}, 1},
                                                    {{174, 165}, 1},
                                                    {{171, 153}, 1},
                                                    {{161, 162}, 1},
                                                    {{160, 169}, 1},
                                                    {{149, 165}, 1},
                                                    {{158, 150}, 1},
                                                    {{222, 159}, 1},
                                                    {{223, 149}, 1},
                                                    {{227, 140}, 1},
                                                    {{235, 126}, 1},
                                                    {{222, 120}, 1},
                                                    {{230, 167}, 1},
                                                    {{69,  24},  1},
                                                    {{72,  36},  1},
                                                    {{82,  24},  1},
                                                    {{62,  33},  1},
                                                    {{62,  21},  1},
                                                    {{62,  48},  1},
                                                    {{73,  46},  1},
                                                    {{83,  38},  1},
                                                    {{15,  39},  1},
                                                    {{13,  30},  1},
                                                    {{73,  56},  1},
                                                    {{212, 117}, 1},
                                                    {{207, 131}, 1},
                                                    {{230, 107}, 1},
                                                    {{214, 170}, 1},
                                                    {{8,   124}, 0},
                                                    {{12,  144}, 0},
                                                    {{102, 169}, 0},
                                                    {{173, 40},  2},
                                                    {{194, 29},  2},
                                                    {{185, 54},  2},
                                                    {{155, 27},  2},
                                                    {{160, 47},  2},
                                                    {{173, 137}, 0},
                                                    {{30,  60},  1},
                                                    {{80,  72},  1}};

//------------- Game1_Objects --------------//

/*walls*/
const std::vector<Area> GAME1WALLS = {
        Area({{90,  238},
              {114, 238},
              {114, 237},
              {236, 237},
              {236, 238},
              {260, 238},
              {260, 150},
              {236, 150},
              {236, 213},
              {114, 213},
              {114, 150},
              {90,  150}}),
        Area({{89,  133},
              {113, 133},
              {113, 67},
              {236, 67},
              {236, 133},
              {260, 133},
              {260, 43},
              {89,  43}}),
        Area({{35, 59},
              {53, 59},
              {53, 58},
              {57, 58},
              {57, 57},
              {59, 57},
              {59, 56},
              {64, 51},
              {65, 51},
              {65, 47},
              {66, 47},
              {66, 25},
              {65, 25},
              {65, 22},
              {64, 22},
              {59, 17},
              {59, 16},
              {57, 16},
              {57, 15},
              {53, 15},
              {53, 14},
              {35, 14},
              {35, 15},
              {31, 15},
              {31, 16},
              {28, 16},
              {28, 17},
              {24, 21},
              {23, 21},
              {23, 24},
              {21, 24},
              {21, 40},
              {22, 40},
              {22, 48},
              {23, 48},
              {23, 52},
              {24, 52},
              {28, 56},
              {28, 57},
              {31, 57},
              {31, 58},
              {35, 58}})};
/*traps*/
const std::vector<Area> GAME1TRAPS = {
        Area({{164, 158},
              {186, 158},
              {186, 157},
              {188, 157},
              {188, 156},
              {190, 154},
              {191, 154},
              {191, 151},
              {192, 151},
              {192, 132},
              {191, 132},
              {191, 129},
              {190, 129},
              {189, 128},
              {189, 127},
              {187, 127},
              {187, 126},
              {186, 126},
              {186, 125},
              {165, 125},
              {165, 126},
              {163, 126},
              {163, 127},
              {161, 129},
              {160, 129},
              {160, 131},
              {159, 131},
              {159, 153},
              {160, 153},
              {160, 155},
              {161, 155},
              {162, 156},
              {162, 157},
              {164, 157}})};
/*swamps*/
const std::vector<Area> GAME1SWAMPS = {
        Area({{32, 189},
              {34, 189},
              {34, 188},
              {37, 188},
              {37, 189},
              {39, 189},
              {39, 188},
              {42, 188},
              {42, 187},
              {45, 187},
              {45, 186},
              {47, 186},
              {47, 185},
              {51, 181},
              {52, 181},
              {52, 179},
              {53, 179},
              {53, 177},
              {54, 177},
              {54, 172},
              {55, 172},
              {55, 168},
              {54, 168},
              {54, 165},
              {53, 165},
              {53, 162},
              {52, 162},
              {52, 161},
              {51, 161},
              {51, 158},
              {48, 158},
              {48, 157},
              {46, 156},
              {46, 155},
              {43, 155},
              {43, 154},
              {40, 154},
              {40, 153},
              {32, 153},
              {32, 154},
              {28, 154},
              {28, 155},
              {26, 155},
              {26, 156},
              {25, 156},
              {25, 157},
              {23, 157},
              {23, 158},
              {21, 160},
              {20, 160},
              {20, 162},
              {19, 162},
              {19, 163},
              {18, 163},
              {18, 167},
              {17, 167},
              {17, 175},
              {18, 175},
              {18, 178},
              {19, 178},
              {19, 180},
              {20, 180},
              {20, 181},
              {21, 181},
              {21, 183},
              {22, 183},
              {22, 184},
              {23, 184},
              {23, 186},
              {26, 186},
              {26, 187},
              {30, 187},
              {30, 188},
              {32, 188}}),
        Area({{115, 189},
              {159, 189},
              {159, 178},
              {157, 178},
              {157, 177},
              {155, 177},
              {155, 176},
              {153, 176},
              {153, 175},
              {151, 174},
              {151, 173},
              {149, 173},
              {149, 172},
              {141, 164},
              {140, 164},
              {140, 162},
              {139, 162},
              {139, 161},
              {138, 161},
              {138, 159},
              {137, 159},
              {137, 157},
              {136, 157},
              {136, 156},
              {115, 156}}),
        Area({{189, 189},
              {235, 189},
              {235, 157},
              {212, 157},
              {212, 159},
              {211, 159},
              {211, 161},
              {210, 161},
              {209, 163},
              {208, 163},
              {208, 165},
              {207, 165},
              {200, 172},
              {200, 173},
              {198, 173},
              {198, 175},
              {196, 175},
              {196, 176},
              {194, 176},
              {194, 177},
              {192, 177},
              {192, 178},
              {189, 178}}),
        Area({{326, 127},
              {332, 127},
              {332, 126},
              {336, 126},
              {336, 125},
              {339, 125},
              {339, 124},
              {341, 124},
              {341, 123},
              {346, 118},
              {347, 118},
              {347, 115},
              {348, 115},
              {348, 110},
              {349, 110},
              {349, 108},
              {348, 108},
              {348, 103},
              {347, 103},
              {347, 101},
              {346, 101},
              {346, 99},
              {345, 99},
              {342, 96},
              {342, 95},
              {340, 95},
              {340, 94},
              {338, 94},
              {338, 93},
              {336, 93},
              {336, 92},
              {331, 92},
              {331, 91},
              {328, 91},
              {328, 92},
              {323, 92},
              {323, 93},
              {321, 93},
              {321, 94},
              {319, 94},
              {319, 95},
              {317, 95},
              {317, 96},
              {314, 99},
              {313, 99},
              {313, 101},
              {312, 101},
              {312, 104},
              {311, 104},
              {311, 113},
              {312, 113},
              {312, 117},
              {313, 117},
              {313, 119},
              {314, 119},
              {318, 123},
              {318, 124},
              {320, 124},
              {320, 125},
              {323, 125},
              {323, 126},
              {326, 126}}),
        Area({{213, 125},
              {235, 125},
              {235, 90},
              {193, 90},
              {193, 103},
              {195, 103},
              {195, 104},
              {196, 104},
              {196, 105},
              {198, 105},
              {198, 106},
              {200, 106},
              {200, 107},
              {205, 112},
              {206, 112},
              {206, 114},
              {208, 114},
              {208, 116},
              {209, 116},
              {209, 117},
              {210, 117},
              {210, 119},
              {211, 119},
              {211, 120},
              {212, 120},
              {212, 122},
              {213, 122}}),
        Area({{114, 124},
              {136, 124},
              {136, 123},
              {137, 123},
              {137, 121},
              {138, 121},
              {138, 119},
              {139, 119},
              {139, 117},
              {140, 117},
              {141, 116},
              {141, 115},
              {143, 115},
              {143, 112},
              {144, 112},
              {146, 110},
              {146, 109},
              {148, 109},
              {148, 108},
              {152, 105},
              {152, 104},
              {154, 104},
              {154, 103},
              {156, 103},
              {156, 102},
              {158, 102},
              {158, 101},
              {160, 101},
              {160, 100},
              {162, 100},
              {162, 90},
              {114, 90}})};
/*Water*/
const std::vector<Area> GAME1WATERS = {
        Area({{103, 270},
              {251, 270},
              {251, 243},
              {103, 243}}),
        Area({{104, 28},
              {252, 28},
              {252, 1},
              {104, 1}})};
/*deposit*/
const std::vector<PVector> GAME1DEPOSITS = {{174, 68},
                                            {178, 192}};

//------ Nodes ------//
/*wall_nodes*/
const std::vector<PVector> GAME1WALLNODES = {{89,  239},
                                             {115, 239},
                                             {115, 238},
                                             {235, 238},
                                             {235, 239},
                                             {261, 239},
                                             {261, 149},
                                             {235, 149},
                                             {235, 212},
                                             {115, 212},
                                             {115, 149},
                                             {89,  149},

                                             {88,  134},
                                             {114, 134},
                                             {114, 68},
                                             {235, 68},
                                             {235, 134},
                                             {261, 134},
                                             {261, 42},
                                             {88,  42},

                                             {34,  60},
                                             {54,  60},
                                             {54,  59},
                                             {58,  59},
                                             {58,  58},
                                             {60,  58},
                                             {60,  57},
                                             {65,  52},
                                             {66,  52},
                                             {66,  48},
                                             {67,  48},
                                             {67,  24},
                                             {66,  24},
                                             {66,  21},
                                             {65,  21},
                                             {60,  16},
                                             {60,  15},
                                             {58,  15},
                                             {58,  14},
                                             {54,  14},
                                             {54,  13},
                                             {34,  13},
                                             {34,  14},
                                             {30,  14},
                                             {30,  15},
                                             {27,  15},
                                             {27,  16},
                                             {23,  20},
                                             {22,  20},
                                             {22,  23},
                                             {20,  23},
                                             {20,  41},
                                             {21,  41},
                                             {21,  49},
                                             {22,  49},
                                             {22,  53},
                                             {23,  53},
                                             {27,  57},
                                             {27,  58},
                                             {30,  58},
                                             {30,  59},
                                             {34,  59}};
/*trap_nodes*/
const std::vector<PVector> GAME1TRAPNODES = {{163, 159},
                                             {187, 159},
                                             {187, 158},
                                             {189, 158},
                                             {189, 157},
                                             {191, 155},
                                             {192, 155},
                                             {192, 152},
                                             {193, 152},
                                             {193, 131},
                                             {192, 131},
                                             {192, 128},
                                             {191, 128},
                                             {190, 127},
                                             {190, 126},
                                             {188, 126},
                                             {188, 125},
                                             {187, 125},
                                             {187, 124},
                                             {164, 124},
                                             {164, 125},
                                             {162, 125},
                                             {162, 126},
                                             {160, 128},
                                             {159, 128},
                                             {159, 130},
                                             {158, 130},
                                             {158, 154},
                                             {159, 154},
                                             {159, 156},
                                             {160, 156},
                                             {161, 157},
                                             {161, 158},
                                             {163, 158}};



//------ Collectibles ------//
/*collectibles*/
const std::vector<Collectible> GAME1COLLECTIBLES = {{{46,  215}, 0},
                                                    {{64,  196}, 0},
                                                    {{64,  217}, 0},
                                                    {{35,  199}, 0},
                                                    {{37,  234}, 0},
                                                    {{100, 238}, 0},
                                                    {{36,  257}, 0},
                                                    {{113, 254}, 0},
                                                    {{81,  222}, 0},
                                                    {{80,  250}, 0},
                                                    {{134, 240}, 0},
                                                    {{168, 251}, 0},
                                                    {{154, 259}, 0},
                                                    {{207, 255}, 0},
                                                    {{132, 260}, 0},
                                                    {{264, 241}, 0},
                                                    {{273, 229}, 0},
                                                    {{282, 255}, 0},
                                                    {{231, 252}, 0},
                                                    {{238, 263}, 0},
                                                    {{329, 208}, 0},
                                                    {{341, 195}, 0},
                                                    {{346, 223}, 0},
                                                    {{316, 188}, 0},
                                                    {{309, 223}, 0},
                                                    {{318, 255}, 0},
                                                    {{332, 244}, 0},
                                                    {{272, 207}, 0},
                                                    {{296, 211}, 0},
                                                    {{67,  27},  1},
                                                    {{62,  13},  1},
                                                    {{73,  46},  1},
                                                    {{13,  21},  1},
                                                    {{15,  89},  1},
                                                    {{79,  30},  1},
                                                    {{132, 13},  1},
                                                    {{124, 39},  1},
                                                    {{97,  17},  1},
                                                    {{97,  42},  1},
                                                    {{181, 23},  1},
                                                    {{192, 9},   1},
                                                    {{200, 35},  1},
                                                    {{166, 10},  1},
                                                    {{157, 38},  1},
                                                    {{263, 22},  1},
                                                    {{278, 41},  1},
                                                    {{248, 38},  1},
                                                    {{308, 27},  1},
                                                    {{322, 24},  1},
                                                    {{222, 10},  1},
                                                    {{238, 20},  1},
                                                    {{297, 38},  1},
                                                    {{293, 24},  1},
                                                    {{265, 40},  1},
                                                    {{325, 60},  1},
                                                    {{341, 17},  1},
                                                    {{279, 41},  1},
                                                    {{9,   38},  1},
                                                    {{39,  113}, 2},
                                                    {{52,  98},  2},
                                                    {{60,  129}, 2},
                                                    {{27,  95},  2},
                                                    {{28,  126}, 2},
                                                    {{323, 148}, 2},
                                                    {{329, 112}, 2},
                                                    {{342, 163}, 2},
                                                    {{313, 130}, 2},
                                                    {{309, 159}, 2},
                                                    {{66,  153}, 2},
                                                    {{79,  138}, 2},
                                                    {{78,  172}, 2},
                                                    {{48,  141}, 2},
                                                    {{287, 136}, 2},
                                                    {{306, 90},  2},
                                                    {{316, 79},  2},
                                                    {{340, 137}, 2},
                                                    {{295, 76},  2},
                                                    {{289, 105}, 2},
                                                    {{292, 178}, 2},
                                                    {{290, 162}, 2},
                                                    {{303, 198}, 2},
                                                    {{273, 193}, 2},
                                                    {{276, 125}, 2},
                                                    {{271, 82},  2},
                                                    {{271, 160}, 2},
                                                    {{265, 110}, 2},
                                                    {{262, 136}, 2},
                                                    {{75,  88},  2},
                                                    {{82,  104}, 2},
                                                    {{72,  65},  2},
                                                    {{69,  114}, 2},
                                                    {{284, 66},  1},
                                                    {{16,  209}, 1},
                                                    {{58,  83},  1}};

