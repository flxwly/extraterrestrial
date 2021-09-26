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
    return (geometry::dist(robotPos + (geometry::angle2Vector((angle - COLOR_SENSOR_ANGLE_OFFSET) * M_PI / 180) *
                                       COLOR_SENSOR_DIST_TO_CORE), pos) < uncertainty
            || geometry::dist(robotPos + (geometry::angle2Vector((angle + COLOR_SENSOR_ANGLE_OFFSET) * M_PI / 180) *
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
            case 0: FIELD_LOG("Returnvector capacity (case 0): " << returnVector.capacity() << " reserving space for "
                                                                 << Walls_.size() << " elements")
                returnVector.insert(std::end(returnVector), std::begin(GAME1WALLS), std::end(GAME1WALLS));
                break;
            case 1: FIELD_LOG("Returnvector capacity (case 1): " << returnVector.capacity() << " reserving space for "
                                                                 << Traps_.size() << " elements")
                returnVector.insert(std::end(returnVector), std::begin(GAME1TRAPS), std::end(GAME1TRAPS));
                break;
            case 2: FIELD_LOG("Returnvector capacity (case 2): " << returnVector.capacity() << " reserving space for "
                                                                 << Swamps_.size() << " elements")
                returnVector.insert(std::end(returnVector), std::begin(GAME1SWAMPS), std::end(GAME1SWAMPS));
                break;
            case 3: FIELD_LOG("Returnvector capacity (case 3): " << returnVector.capacity() << " reserving space for "
                                                                 << Waters_.size() << " elements")
                returnVector.insert(std::end(returnVector), std::begin(GAME1WATERS), std::end(GAME1WATERS));
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
bool geometry::isInside(const PVector &p, const Area &area) {
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

bool geometry::isIntersecting(Line l1, const Area &Obstacle) {
    for (auto edge : Obstacle.getEdges()) {
        if (geometry::isIntersecting(l1, edge)) {
            return true;
        }
    }
    return false;
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



//------------- GAME0_Objects --------------//

		/*walls*/
const std::vector<Area>GAME0WALLS = {
	Area({{49,78},{73,78},{73,77},{49,77}}),
	Area({{28,60},{29,60},{29,36},{28,36}}),
	Area({{100,60},{100,36}}),
	Area({{51,11},{74,11}})};
		/*traps*/
const std::vector<Area>GAME0TRAPS = {
	Area({{58,52},{64,52},{64,47},{58,47}})};
		/*swamps*/
const std::vector<Area>GAME0SWAMPS = {};
		/*Water*/
const std::vector<Area>GAME0WATERS = {
	Area({{30,75},{51,75},{51,56},{30,56}})};
		/*deposit*/
const std::vector<PVector>GAME0DEPOSITS = {{98,13},{19,73}};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<PVector>GAME0WALLNODES = {{48,79},{74,79},{74,76},{48,76},{27,61},{30,61},{30,35},{27,35},{101,61},{101,35},{50,10},{75,10}};
		/*trap_nodes*/
const std::vector<PVector>GAME0TRAPNODES = {{57,53},{65,53},{65,46},{57,46}};
		/*swamp_nodes*/
const std::vector<PVector>GAME0SWAMPNODES = {};



	//------ Collectibles ------//
		/*collectibles*/
const std::vector<Collectible> GAME0COLLECTIBLES = {};

//------------- GAME1_Objects --------------//

		/*walls*/
const std::vector<Area>GAME1WALLS = {
	Area({{49,115},{126,115},{126,79},{122,79},{122,111},{53,111},{53,79},{49,79}}),
	Area({{122,63},{126,63},{126,25},{49,25},{49,62},{53,62},{53,29},{122,29}}),
	Area({{18,25},{25,25},{25,24},{28,21},{29,21},{29,15},{28,15},{26,13},{26,12},{23,12},{23,11},{20,11},{20,12},{17,12},{17,14},{16,14},{16,15},{15,15},{15,21},{16,21},{16,23},{17,23},{18,24}})};
		/*traps*/
const std::vector<Area>GAME1TRAPS = {
	Area({{86,75},{89,75},{89,74},{90,73},{91,73},{91,71},{92,71},{92,70},{91,70},{91,68},{89,68},{89,67},{86,67},{86,68},{85,69},{84,69},{84,73},{85,73},{86,74}})};
		/*swamps*/
const std::vector<Area>GAME1SWAMPS = {
	Area({{19,94},{19,93},{22,93},{22,92},{25,89},{26,89},{26,85},{26,82},{25,82},{25,80},{23,80},{23,79},{22,79},{22,78},{19,78},{19,77},{17,77},{17,78},{14,78},{14,79},{12,79},{12,80},{11,80},{11,82},{10,82},{10,84},{9,84},{9,87},{10,87},{10,89},{11,89},{11,91},{13,91},{13,92},{14,92},{14,93}}),
	Area({{54,94},{79,94},{79,90},{78,90},{78,89},{76,89},{76,88},{75,88},{75,87},{73,87},{73,86},{70,83},{69,83},{69,81},{68,81},{68,79},{54,79}}),
	Area({{95,94},{121,94},{121,79},{107,79},{107,80},{106,80},{106,82},{105,82},{99,88},{99,89},{97,89},{97,90},{95,90}}),
	Area({{164,63},{165,63},{165,62},{169,62},{169,61},{172,58},{173,58},{173,51},{172,51},{170,49},{170,48},{168,48},{168,47},{165,47},{165,46}}),
	Area({{107,62},{121,62},{121,46},{97,46},{97,51},{99,51},{99,52},{101,52},{101,53},{104,56},{105,56},{105,58},{106,58},{106,59},{107,59}}),
	Area({{54,61},{68,61},{68,59},{69,59},{69,57},{71,57},{71,55},{72,55},{75,52},{75,51},{77,51},{77,50},{79,50},{79,49},{80,49},{80,46},{54,46}})};
		/*Water*/
const std::vector<Area>GAME1WATERS = {
	Area({{52,135},{125,135},{125,122},{52,122}}),
	Area({{53,13},{125,13},{125,1},{53,1}})};
		/*deposit*/
const std::vector<PVector>GAME1DEPOSITS = {{87,32},{89,98}};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<PVector>GAME1WALLNODES = {{48,116},{127,116},{127,78},{121,78},{121,110},{54,110},{54,78},{48,78},{121,64},{127,64},{127,24},{48,24},{48,63},{54,63},{54,30},{121,30},{17,26},{26,26},{26,25},{29,22},{30,22},{30,14},{29,14},{27,12},{27,11},{24,11},{24,10},{19,10},{19,11},{16,11},{16,13},{15,13},{15,14},{14,14},{14,22},{15,22},{15,24},{16,24},{17,25}};
		/*trap_nodes*/
const std::vector<PVector>GAME1TRAPNODES = {{85,76},{90,76},{90,75},{91,74},{92,74},{92,72},{93,72},{93,69},{92,69},{92,67},{90,67},{90,66},{85,66},{85,67},{84,68},{83,68},{83,74},{84,74},{85,75}};
		/*swamp_nodes*/
const std::vector<PVector>GAME1SWAMPNODES = {{20,95},{20,94},{23,94},{23,93},{26,90},{27,90},{27,84},{27,81},{26,81},{26,79},{24,79},{24,78},{23,78},{23,77},{20,77},{20,76},{16,76},{16,77},{13,77},{13,78},{11,78},{11,79},{10,79},{10,81},{9,81},{9,83},{8,83},{8,88},{9,88},{9,90},{10,90},{10,92},{12,92},{12,93},{13,93},{13,94},{53,95},{80,95},{80,89},{79,89},{79,88},{77,88},{77,87},{76,87},{76,86},{74,86},{74,85},{71,82},{70,82},{70,80},{69,80},{69,78},{53,78},{94,95},{122,95},{122,78},{106,78},{106,79},{105,79},{105,81},{104,81},{98,87},{98,88},{96,88},{96,89},{94,89},{163,64},{166,64},{166,63},{170,63},{170,62},{173,59},{174,59},{174,50},{173,50},{171,48},{171,47},{169,47},{169,46},{166,46},{166,45},{106,63},{122,63},{122,45},{96,45},{96,52},{98,52},{98,53},{100,53},{100,54},{103,57},{104,57},{104,59},{105,59},{105,60},{106,60},{53,62},{69,62},{69,60},{70,60},{70,58},{72,58},{72,56},{73,56},{76,53},{76,52},{78,52},{78,51},{80,51},{80,50},{81,50},{81,45},{53,45}};



	//------ Collectibles ------//
		/*collectibles*/
const std::vector<Collectible> GAME1COLLECTIBLES = {};


 #pragma endregion
