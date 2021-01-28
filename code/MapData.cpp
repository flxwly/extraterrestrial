#include "MapData.hpp"


/**     -----------     **/
/**                     **/
/**     Collectible     **/
/**                     **/
/**     -----------     **/

Collectible::Collectible(const PVector &p, const unsigned int &c) : pos{p}, color{c}, state{0} {}

bool Collectible::isCorrectCollectible(PVector robotPos, double angle, double uncertainty) const {

    auto p = robotPos + (geometry::angle2Vector(toRadians(angle + COLOR_SENSOR_ANGLE_OFFSET)) * COLOR_SENSOR_DIST_TO_CORE);

    if (geometry::dist(p, pos) < uncertainty) {
        return true;
    }

    p = robotPos + (geometry::angle2Vector(toRadians(angle - COLOR_SENSOR_ANGLE_OFFSET)) * COLOR_SENSOR_DIST_TO_CORE);
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
             const std::vector<PVector> &swampNodes,
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
    SwampNodes_ = swampNodes;

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
            case 2:
                returnVector.insert(std::end(returnVector), std::begin(SwampNodes_),
                                    std::end(SwampNodes_));
            default: ERROR_MESSAGE("index out of range/invalid")
                break;
        }
    }

    ERROR_MESSAGE(returnVector.size())
    return returnVector;
}

/** Getter for field::collectibles **/
std::vector<PVector> Field::getDeposits() {
    std::vector<PVector> returnVector(Deposits_);
    return returnVector;
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
std::vector<PVector> Field::getPointPath(PVector pos, std::array<int, 3> cur, int max) {
    std::vector<PVector> points = {};
    PVector start = pos;
    PVector end = pos;
    int num = std::max(max - cur[0], 0) + std::max(max - cur[1], 0) + std::max(max - cur[2], 0);
    int added = 0;

    while (num > added) {
        unsigned int color = 0;
        double dist = -1;
        for (int i = 0; i < 3; ++i) {
            for (auto collectible : Collectibles_[i]) {
                if ((geometry::dist(start, collectible.pos) < dist || dist == -1)
                    && collectible.pos != start && cur[i] < max) {

                    color = collectible.color;
                    end = collectible.pos;
                    dist = geometry::dist(start, end);
                }
            }
        }
        if (start != end) {
            start = end;
            points.push_back(end);
            cur[color]++;
        } else {
            return points;
        }
        added++;
    }
    return points;
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

bool geometry::isIntersecting(Line l1, const std::vector<Area>& Obstacles) {
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



//------------- Game0_Objects --------------//

		/*walls*/
const std::vector<Area>GAME0WALLS = {
	Area({{88,166},{156,166},{156,143},{88,143}}),
	Area({{45,130},{68,130},{68,62},{45,62}}),
	Area({{189,130},{211,130},{211,62},{189,62}}),
	Area({{91,33},{158,33},{158,11},{91,11}})};
		/*traps*/
const std::vector<Area>GAME0TRAPS = {
	Area({{105,115},{138,115},{138,83},{105,83}})};
		/*swamps*/
const std::vector<Area>GAME0SWAMPS = {};
		/*Water*/
const std::vector<Area>GAME0WATERS = {
	Area({{59,151},{87,151},{87,142},{103,142},{103,110},{69,110},{69,131},{59,131}})};
		/*deposit*/
const std::vector<PVector>GAME0DEPOSITS = {{197,27},{38,147}};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<PVector>GAME0WALLNODES = {{87,167},{157,167},{157,142},{87,142},{44,131},{69,131},{69,61},{44,61},{188,131},{212,131},{212,61},{188,61},{90,34},{159,34},{159,10},{90,10}};
		/*trap_nodes*/
const std::vector<PVector>GAME0TRAPNODES = {{104,116},{139,116},{139,82},{104,82}};
		/*swamp_nodes*/
const std::vector<PVector>GAME0SWAMPNODES = {};



	//------ Collectibles ------//
		/*collectibles*/
const std::vector<Collectible> GAME0COLLECTIBLES = {{{157,60},2},{{175,49},2},{{173,72},2},{{141,42},2},{{137,76},2},{{125,58},2},{{138,60},2},{{119,69},2},{{105,44},2},{{109,78},2},{{165,84},2},{{184,64},2},{{155,71},2},{{152,95},2},{{167,104},2},{{187,86},2},{{186,125},2},{{152,86},2},{{154,118},2},{{106,60},2},{{119,47},2},{{124,74},2},{{99,51},2},{{104,70},2},{{35,148},0},{{49,132},0},{{48,166},0},{{21,130},0},{{21,160},0},{{49,148},0},{{63,168},0},{{77,167},0},{{88,163},0},{{36,162},0},{{20,53},1},{{34,118},0},{{38,105},0},{{22,116},0},{{11,106},0},{{66,138},0},{{81,144},0},{{72,124},0},{{84,131},0},{{93,141},0},{{96,125},0},{{84,117},0},{{219,133},1},{{174,165},1},{{171,153},1},{{161,162},1},{{160,169},1},{{149,165},1},{{158,150},1},{{222,159},1},{{223,149},1},{{227,140},1},{{235,126},1},{{222,120},1},{{230,167},1},{{69,24},1},{{72,36},1},{{82,24},1},{{62,33},1},{{62,21},1},{{62,48},1},{{73,46},1},{{83,38},1},{{15,39},1},{{13,30},1},{{73,56},1},{{212,117},1},{{207,131},1},{{230,107},1},{{214,170},1},{{8,124},0},{{12,144},0},{{102,169},0},{{173,40},2},{{194,29},2},{{185,54},2},{{155,27},2},{{160,47},2},{{173,137},0},{{30,60},1},{{80,72},1}};

//------------- Game1_Objects --------------//

		/*walls*/
const std::vector<Area>GAME1WALLS = {
	Area({{88,240},{116,240},{116,239},{234,239},{234,240},{262,240},{262,148},{234,148},{234,211},{116,211},{116,148},{88,148}}),
	Area({{87,135},{115,135},{115,69},{234,69},{234,135},{262,135},{262,41},{87,41}}),
	Area({{33,61},{55,61},{55,60},{59,60},{59,59},{61,59},{61,58},{66,53},{67,53},{67,49},{68,49},{68,23},{67,23},{67,20},{66,20},{61,15},{61,14},{59,14},{59,13},{55,13},{55,12},{33,12},{33,13},{29,13},{29,14},{26,14},{26,15},{22,19},{21,19},{21,22},{19,22},{19,42},{20,42},{20,50},{21,50},{21,54},{22,54},{26,58},{26,59},{29,59},{29,60},{33,60}})};
		/*traps*/
const std::vector<Area>GAME1TRAPS = {
	Area({{162,160},{188,160},{188,159},{190,159},{190,158},{192,156},{193,156},{193,153},{194,153},{194,130},{193,130},{193,127},{192,127},{191,126},{191,125},{189,125},{189,124},{188,124},{188,123},{163,123},{163,124},{161,124},{161,125},{159,127},{158,127},{158,129},{157,129},{157,155},{158,155},{158,157},{159,157},{160,158},{160,159},{162,159}})};
		/*swamps*/
const std::vector<Area>GAME1SWAMPS = {
	Area({{31,190},{40,190},{40,189},{43,189},{43,188},{46,188},{46,187},{48,187},{48,186},{52,182},{53,182},{53,180},{54,180},{54,178},{55,178},{55,173},{56,173},{56,167},{55,167},{55,164},{54,164},{54,161},{53,161},{53,160},{52,160},{52,157},{49,157},{49,156},{47,155},{47,154},{44,154},{44,153},{41,153},{41,152},{31,152},{31,153},{27,153},{27,154},{25,154},{25,155},{24,155},{24,156},{22,156},{22,157},{20,159},{19,159},{19,161},{18,161},{18,162},{17,162},{17,166},{16,166},{16,176},{17,176},{17,179},{18,179},{18,181},{19,181},{19,182},{20,182},{20,184},{21,184},{21,185},{22,185},{22,187},{25,187},{25,188},{29,188},{29,189},{31,189}}),
	Area({{117,190},{160,190},{160,177},{158,177},{158,176},{156,176},{156,175},{154,175},{154,174},{152,173},{152,172},{150,172},{150,171},{142,163},{141,163},{141,161},{140,161},{140,160},{139,160},{139,158},{138,158},{138,156},{137,156},{137,155},{117,155}}),
	Area({{188,190},{233,190},{233,156},{211,156},{211,158},{210,158},{210,160},{209,160},{208,162},{207,162},{207,164},{206,164},{199,171},{199,172},{197,172},{197,174},{195,174},{195,175},{193,175},{193,176},{191,176},{191,177},{188,177}}),
	Area({{325,128},{333,128},{333,127},{337,127},{337,126},{340,126},{340,125},{342,125},{342,124},{347,119},{348,119},{348,116},{349,116},{349,111},{350,111},{350,107},{349,107},{349,102},{348,102},{348,100},{347,100},{347,98},{346,98},{343,95},{343,94},{341,94},{341,93},{339,93},{339,92},{337,92},{337,91},{332,91},{332,90},{327,90},{327,91},{322,91},{322,92},{320,92},{320,93},{318,93},{318,94},{316,94},{316,95},{313,98},{312,98},{312,100},{311,100},{311,103},{310,103},{310,114},{311,114},{311,118},{312,118},{312,120},{313,120},{317,124},{317,125},{319,125},{319,126},{322,126},{322,127},{325,127}}),
	Area({{212,126},{233,126},{233,89},{192,89},{192,104},{194,104},{194,105},{195,105},{195,106},{197,106},{197,107},{199,107},{199,108},{204,113},{205,113},{205,115},{207,115},{207,117},{208,117},{208,118},{209,118},{209,120},{210,120},{210,121},{211,121},{211,123},{212,123}}),
	Area({{116,125},{137,125},{137,124},{138,124},{138,122},{139,122},{139,120},{140,120},{140,118},{141,118},{142,117},{142,116},{144,116},{144,113},{145,113},{147,111},{147,110},{149,110},{149,109},{153,106},{153,105},{155,105},{155,104},{157,104},{157,103},{159,103},{159,102},{161,102},{161,101},{163,101},{163,89},{116,89}})};
		/*Water*/
const std::vector<Area>GAME1WATERS = {
	Area({{103,270},{251,270},{251,243},{103,243}}),
	Area({{104,28},{252,28},{252,1},{104,1}})};
		/*deposit*/
const std::vector<PVector>GAME1DEPOSITS = {{174,69},{178,191}};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<PVector>GAME1WALLNODES = {{87,241},{117,241},{117,240},{233,240},{233,241},{263,241},{263,147},{233,147},{233,210},{117,210},{117,147},{87,147},{86,136},{116,136},{116,70},{233,70},{233,136},{263,136},{263,40},{86,40},{32,62},{56,62},{56,61},{60,61},{60,60},{62,60},{62,59},{67,54},{68,54},{68,50},{69,50},{69,22},{68,22},{68,19},{67,19},{62,14},{62,13},{60,13},{60,12},{56,12},{56,11},{32,11},{32,12},{28,12},{28,13},{25,13},{25,14},{21,18},{20,18},{20,21},{18,21},{18,43},{19,43},{19,51},{20,51},{20,55},{21,55},{25,59},{25,60},{28,60},{28,61},{32,61}};
		/*trap_nodes*/
const std::vector<PVector>GAME1TRAPNODES = {{161,161},{189,161},{189,160},{191,160},{191,159},{193,157},{194,157},{194,154},{195,154},{195,129},{194,129},{194,126},{193,126},{192,125},{192,124},{190,124},{190,123},{189,123},{189,122},{162,122},{162,123},{160,123},{160,124},{158,126},{157,126},{157,128},{156,128},{156,156},{157,156},{157,158},{158,158},{159,159},{159,160},{161,160}};
		/*swamp_nodes*/
const std::vector<PVector>GAME1SWAMPNODES = {{30,191},{41,191},{41,190},{44,190},{44,189},{47,189},{47,188},{49,188},{49,187},{53,183},{54,183},{54,181},{55,181},{55,179},{56,179},{56,174},{57,174},{57,166},{56,166},{56,163},{55,163},{55,160},{54,160},{54,159},{53,159},{53,156},{50,156},{50,155},{48,154},{48,153},{45,153},{45,152},{42,152},{42,151},{30,151},{30,152},{26,152},{26,153},{24,153},{24,154},{23,154},{23,155},{21,155},{21,156},{19,158},{18,158},{18,160},{17,160},{17,161},{16,161},{16,165},{15,165},{15,177},{16,177},{16,180},{17,180},{17,182},{18,182},{18,183},{19,183},{19,185},{20,185},{20,186},{21,186},{21,188},{24,188},{24,189},{28,189},{28,190},{30,190},{116,191},{161,191},{161,176},{159,176},{159,175},{157,175},{157,174},{155,174},{155,173},{153,172},{153,171},{151,171},{151,170},{143,162},{142,162},{142,160},{141,160},{141,159},{140,159},{140,157},{139,157},{139,155},{138,155},{138,154},{116,154},{187,191},{234,191},{234,155},{210,155},{210,157},{209,157},{209,159},{208,159},{207,161},{206,161},{206,163},{205,163},{198,170},{198,171},{196,171},{196,173},{194,173},{194,174},{192,174},{192,175},{190,175},{190,176},{187,176},{324,129},{334,129},{334,128},{338,128},{338,127},{341,127},{341,126},{343,126},{343,125},{348,120},{349,120},{349,117},{350,117},{350,112},{351,112},{351,106},{350,106},{350,101},{349,101},{349,99},{348,99},{348,97},{347,97},{344,94},{344,93},{342,93},{342,92},{340,92},{340,91},{338,91},{338,90},{333,90},{333,89},{326,89},{326,90},{321,90},{321,91},{319,91},{319,92},{317,92},{317,93},{315,93},{315,94},{312,97},{311,97},{311,99},{310,99},{310,102},{309,102},{309,115},{310,115},{310,119},{311,119},{311,121},{312,121},{316,125},{316,126},{318,126},{318,127},{321,127},{321,128},{324,128},{211,127},{234,127},{234,88},{191,88},{191,105},{193,105},{193,106},{194,106},{194,107},{196,107},{196,108},{198,108},{198,109},{203,114},{204,114},{204,116},{206,116},{206,118},{207,118},{207,119},{208,119},{208,121},{209,121},{209,122},{210,122},{210,124},{211,124},{115,126},{138,126},{138,125},{139,125},{139,123},{140,123},{140,121},{141,121},{141,119},{142,119},{143,118},{143,117},{145,117},{145,114},{146,114},{148,112},{148,111},{150,111},{150,110},{154,107},{154,106},{156,106},{156,105},{158,105},{158,104},{160,104},{160,103},{162,103},{162,102},{164,102},{164,88},{115,88}};



	//------ Collectibles ------//
		/*collectibles*/
const std::vector<Collectible> GAME1COLLECTIBLES = {{{46,215},0},{{64,196},0},{{64,217},0},{{35,199},0},{{37,234},0},{{100,238},0},{{36,257},0},{{113,254},0},{{81,222},0},{{80,250},0},{{134,240},0},{{168,251},0},{{154,259},0},{{207,255},0},{{132,260},0},{{264,241},0},{{273,229},0},{{282,255},0},{{231,252},0},{{238,263},0},{{329,208},0},{{341,195},0},{{346,223},0},{{316,188},0},{{309,223},0},{{318,255},0},{{332,244},0},{{272,207},0},{{296,211},0},{{67,27},1},{{62,13},1},{{73,46},1},{{13,21},1},{{15,89},1},{{79,30},1},{{132,13},1},{{124,39},1},{{97,17},1},{{97,42},1},{{181,23},1},{{192,9},1},{{200,35},1},{{166,10},1},{{157,38},1},{{263,22},1},{{278,41},1},{{248,38},1},{{308,27},1},{{322,24},1},{{222,10},1},{{238,20},1},{{297,38},1},{{293,24},1},{{265,40},1},{{325,60},1},{{341,17},1},{{279,41},1},{{9,38},1},{{39,113},2},{{52,98},2},{{60,129},2},{{27,95},2},{{28,126},2},{{323,148},2},{{329,112},2},{{342,163},2},{{313,130},2},{{309,159},2},{{66,153},2},{{79,138},2},{{78,172},2},{{48,141},2},{{287,136},2},{{306,90},2},{{316,79},2},{{340,137},2},{{295,76},2},{{289,105},2},{{292,178},2},{{290,162},2},{{303,198},2},{{273,193},2},{{276,125},2},{{271,82},2},{{271,160},2},{{265,110},2},{{262,136},2},{{75,88},2},{{82,104},2},{{72,65},2},{{69,114},2},{{284,66},1},{{16,209},1},{{58,83},1}};

