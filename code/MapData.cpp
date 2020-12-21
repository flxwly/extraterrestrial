#include "MapData.hpp"

/**     -----------     **/
/**                     **/
/**        PVector        **/
/**                     **/
/**     -----------     **/


PVector::PVector(double _x, double _y) {
	PVector::x = _x;
	PVector::y = _y;
}

PVector PVector::round() const {
	return PVector(std::round(x), std::round(y));
}

bool operator==(const PVector &p1, const PVector &p2) {
	return p1.x == p2.x && p1.y == p2.y;
}

bool operator!=(const PVector &p1, const PVector &p2) {
	return !(p1 == p2);
}

PVector::operator bool() const {
	return x != 0 || y != 0;
}

PVector operator*(const PVector &p, const double &m) {
	return PVector(p.x * m, p.y * m);
}

PVector operator/(const PVector &p, const double &m) {
	return PVector(p.x / m, p.y / m);
}

void PVector::set(double _x, double _y) {
	x = _x, y = _y;
}

void PVector::set(PVector p) {
	set(p.x, p.y);
}

double PVector::getMag() const {
	return sqrt(x * x + y * y);
}

void PVector::setMag(double mag) {
	double oldMag = getMag();
	set(x / oldMag * mag, y / oldMag * mag);
}

std::string PVector::str(PVector pVector) {
    return std::to_string(pVector.x) + " | " + std::to_string(pVector.y);
}

bool operator==(const PVector &p, const double &n) {
    return p.x == n && p.y == n;
}

bool operator!=(const PVector &p, const double &n) {
    return p.x != n && p.y != n;
}

/**     -----------     **/
/**                     **/
/**     Collectible     **/
/**                     **/
/**     -----------     **/

Collectible::Collectible(const PVector &p, const int &c) {
	Collectible::pos_ = p;
	Collectible::color_ = c;
	Collectible::state_ = 0;
}

unsigned int Collectible::getColor() const {
	return color_;
}

const PVector &Collectible::getPos() const {
	return pos_;
}

unsigned int Collectible::getState() const {
	return state_;
}

void Collectible::setState(unsigned int state) {
	state_ = state;
}

bool Collectible::isCorrectCollectible(PVector robot_pos, double angle, double uncertainty) {

	auto p = robot_pos + (geometry::angle2Vector(angle + COLOR_SENSOR_ANGLE_OFFSET) * COLOR_SENSOR_DIST_TO_CORE);

	if (geometry::dist(p, pos_) < uncertainty) {
		return true;
	}

	p = robot_pos + (geometry::angle2Vector(angle - COLOR_SENSOR_ANGLE_OFFSET) * COLOR_SENSOR_DIST_TO_CORE);
	if (geometry::dist(p, pos_) < uncertainty) {
		return true;
	}
	return false;
}


/**     -----------     **/
/**                     **/
/**         Line        **/
/**                     **/
/**     -----------     **/

// Line::Line(): Constructor for Line Class
Line::Line(const PVector &p1, const PVector &p2) {
	/** Set private vars **/
	Line::p1_ = p1;
	Line::p2_ = p2;
}

const PVector &Line::getP1() const {
	return p1_;
}

const PVector &Line::getP2() const {
	return p2_;
}

void Line::setP1(const PVector &p_1) {
	p1_ = p_1;
}

void Line::setP2(const PVector &p_2) {
	p2_ = p_2;
}

/**     -----------     **/
/**                     **/
/**         Area        **/
/**                     **/
/**     -----------     **/

// Area::Area(): Constructor for Area class
Area::Area(const std::vector<PVector> &p_s) : min_{p_s.front()}, max_{0, 0} {

	Area::Corners_ = p_s;
	PVector last_p = p_s.back();
	for (PVector p : p_s) {
		Area::Edges_.emplace_back(last_p, p);
		last_p = p;

		// Set boundary box x
		if (p.x < Area::min_.x) {
			Area::min_.x = p.x;
		}
		if (p.x > Area::max_.x) {
			Area::max_.x = p.x;
		}

		// Set boundary box y
		if (p.y < Area::min_.y) {
			Area::min_.y = p.y;
		}
		if (p.y > Area::max_.y) {
			Area::max_.y = p.y;
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

	// Field::(MapObjectName)_: Different MapObjects that can be displayed as areas.
	Walls_ = walls;
	Traps_ = traps;
	Swamps_ = swamps;
	Waters_ = waters;

	// Field::Deposits_: Deposit Areas of the Field
	Deposits_ = deposits;
	WallNodes_ = wallNodes;
	TrapNodes_ = trapNodes;

	// Field::Collectibles: Collectible Points of the Field
	for (auto collectible : collectibles) {
		Collectibles_[collectible.getColor()].push_back(collectible);
	}
}

PVector Field::getSize() {
	return size_;
}

std::vector<Area> Field::getMapObjects(const std::vector<unsigned int>& indices) {
	std::vector<Area> return_vector = {};
	// return after all indices have been checked.
	for (unsigned int index : indices) {
		switch (index) {
			case 0:
				return_vector.insert(std::end(return_vector), std::begin(Field::Walls_), std::end(Field::Walls_));
				break;
			case 1:
				return_vector.insert(std::end(return_vector), std::begin(Field::Traps_), std::end(Field::Traps_));
				break;
			case 2:
				return_vector.insert(std::end(return_vector), std::begin(Field::Swamps_), std::end(Field::Swamps_));
				break;
			case 3:
				return_vector.insert(std::end(return_vector), std::begin(Field::Waters_), std::end(Field::Waters_));
				break;
			default: ERROR_MESSAGE("index out of range/invalid")
		}
	}

	return return_vector;
}

std::vector<PVector> Field::getMapNodes(const std::vector<unsigned int>& indices) {
	std::vector<PVector> return_vector;

	// return after all indices have been checked.
	for (unsigned int index : indices){
		switch (index) {
			case 0:
				return_vector.insert(return_vector.end(), Field::WallNodes_.begin(),
				                     Field::WallNodes_.end());
				break;
			case 1:
				return_vector.insert(std::end(return_vector), std::begin(Field::TrapNodes_),
				                     std::end(Field::TrapNodes_));
				break;
			default: ERROR_MESSAGE("index out of range/invalid")
				break;
		}
	}

	ERROR_MESSAGE(return_vector.size())
	return return_vector;
}

/** Getter for Field::Collectibles_ **/
std::vector<PVector> Field::getDeposits() {
	return Deposits_;
}

/** Getter for Field::Collectibles_ **/
std::vector<Collectible> Field::getCollectibles(const std::vector<unsigned int>& colors) {
	std::vector<Collectible> return_vector = {};
	// return after all indices have been checked.
	for(unsigned int index : colors){
		if (index <= 2) {
			return_vector.insert(std::end(return_vector), std::begin(Field::Collectibles_[index]),
			                     std::end(Field::Collectibles_[index]));
		} else {
			ERROR_MESSAGE("index out of range");
		}
	}
	return return_vector;
}

Collectible *Field::getCollectible(PVector robot_pos, double angle, double uncertainty, int color) {

	if (color < 0 || color > 3) {
		ERROR_MESSAGE("color " << color << " is not existing!")
		return nullptr;
	}

	auto Collectibles = Collectibles_[color];

	for (auto &collectible : Collectibles) {
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
		c.push_back(Collectibles_[0][i].getPos());
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
		if (poly[i].y <= p.y) {                                 // start y <= P.y
			if (poly[i + 1].y > p.y)                            // an upward crossing
				if (isLeft(poly[i], poly[i + 1], p))        // P left of  edge
					++wn;                                       // have  a valid up intersect
		} else {                                                // start y > P.y (no test needed)
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
	double L1 = l1.getP1().x * l1.getP2().y - l1.getP2().x * l1.getP1().y;
	double L2 = l2.getP1().x * l2.getP2().y - l2.getP2().x * l2.getP1().y;

	// difs
	double L1xdif = l1.getP1().x - l1.getP2().x;
	double L1ydif = l1.getP1().y - l1.getP2().y;
	double L2xdif = l2.getP1().x - l2.getP2().x;
	double L2ydif = l2.getP1().y - l2.getP2().y;

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

	if (x == 0)
		return (y >= 0) ? 0 : 180;
	else if (y == 0)
		return (x > 0) ? 90 : 270;

	double angle = atan(y / x);

	// bottom left (90 - 180)
	if (x < 0 && y < 0)
		// angle is positive (180 location)
		angle = M_PI / 2;
		// top left (0 - 90)
	else if (x < 0)
		// angle is negative (90 positive) + (0 location)
		angle += M_PI / 2;
		// bottom right (180 - 270)
	else if (y < 0)
		// angle is negative (90 positive) + (180 location)
		angle += 1.5 * M_PI;
		// top right (270 - 360)
	else {
		angle += 1.5 * M_PI;
		// angle is positive
	}
	return angle;
}

double geometry::vector2Angle(PVector v) {
	return geometry::vector2Angle(v.x, v.y);
}

double geometry::dot(PVector p1, PVector p2) {
	return p1.x * p2.x + p1.y * p2.y;
}

PVector geometry::getNormalPoint(Line line, PVector point) {
	PVector ap = point - line.getP1();
	PVector ab = line.getP2() - line.getP1();

	// normalize ab (set mag to 1)
	ab.setMag(1);
	ab = ab * geometry::dot(ap, ab);

	// normal point
	return point + ab;
}




///   _______                _____          __
///  |   |   |.---.-..-----.|     \ .---.-.|  |_ .---.-.
///  |       ||  _  ||  _  ||  --  ||  _  ||   _||  _  |
///  |__|_|__||___._||   __||_____/ |___._||____||___._|
///                  |__|



//------------- Game0_Objects --------------//

		/*walls*/
const std::vector<Area>GAME0WALLS = {};
		/*traps*/
const std::vector<Area>GAME0TRAPS = {};
		/*swamps*/
const std::vector<Area>GAME0SWAMPS = {};
		/*Water*/
const std::vector<Area>GAME0WATERS = {};
		/*deposit*/
const std::vector<PVector>GAME0DEPOSITS = {};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<PVector>GAME0WALLNODES = {};
		/*trap_nodes*/
const std::vector<PVector>GAME0TRAPNODES = {};



	//------ Collectibles ------//
		/*collectibles*/
const std::vector<Collectible> GAME0COLLECTIBLES = {};

//------------- Game1_Objects --------------//

		/*walls*/
const std::vector<Area>GAME1WALLS = {};
		/*traps*/
const std::vector<Area>GAME1TRAPS = {};
		/*swamps*/
const std::vector<Area>GAME1SWAMPS = {};
		/*Water*/
const std::vector<Area>GAME1WATERS = {};
		/*deposit*/
const std::vector<PVector>GAME1DEPOSITS = {};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<PVector>GAME1WALLNODES = {};
		/*trap_nodes*/
const std::vector<PVector>GAME1TRAPNODES = {};



	//------ Collectibles ------//
		/*collectibles*/
const std::vector<Collectible> GAME1COLLECTIBLES = {};