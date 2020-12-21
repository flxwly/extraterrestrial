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
		Area({{89,  238},
		      {261, 238},
		      {261, 149},
		      {236, 149},
		      {236, 213},
		      {114, 213},
		      {114, 149},
		      {89,  149}}),
		Area({{235, 134},
		      {260, 134},
		      {260, 42},
		      {89,  42},
		      {89,  133},
		      {114, 133},
		      {114, 67},
		      {235, 67}}),
		Area({{32, 59},
		      {55, 59},
		      {55, 58},
		      {59, 58},
		      {59, 57},
		      {64, 52},
		      {65, 52},
		      {65, 50},
		      {66, 50},
		      {66, 46},
		      {67, 46},
		      {67, 25},
		      {66, 25},
		      {66, 22},
		      {65, 22},
		      {59, 16},
		      {59, 15},
		      {55, 15},
		      {55, 14},
		      {32, 14},
		      {32, 15},
		      {28, 15},
		      {28, 16},
		      {26, 16},
		      {26, 18},
		      {25, 18},
		      {23, 21},
		      {22, 21},
		      {22, 23},
		      {21, 23},
		      {21, 46},
		      {22, 46},
		      {22, 50},
		      {23, 50},
		      {23, 53},
		      {24, 53},
		      {28, 57},
		      {28, 58},
		      {32, 58}})};
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
		      {234, 125},
		      {234, 90},
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
		Area({{115, 124},
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
		      {115, 90}})};
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
const std::vector<PVector> GAME1WALLNODES = {{88,  239},
                                             {262, 239},
                                             {262, 148},
                                             {235, 148},
                                             {235, 212},
                                             {115, 212},
                                             {115, 148},
                                             {88,  148},
                                             {234, 135},
                                             {261, 135},
                                             {261, 41},
                                             {88,  41},
                                             {88,  134},
                                             {115, 134},
                                             {115, 68},
                                             {234, 68},
                                             {31,  60},
                                             {56,  60},
                                             {56,  59},
                                             {60,  59},
                                             {60,  58},
                                             {65,  53},
                                             {66,  53},
                                             {66,  51},
                                             {67,  51},
                                             {67,  47},
                                             {68,  47},
                                             {68,  24},
                                             {67,  24},
                                             {67,  21},
                                             {66,  21},
                                             {60,  15},
                                             {60,  14},
                                             {56,  14},
                                             {56,  13},
                                             {31,  13},
                                             {31,  14},
                                             {27,  14},
                                             {27,  15},
                                             {25,  15},
                                             {25,  17},
                                             {24,  17},
                                             {22,  20},
                                             {21,  20},
                                             {21,  22},
                                             {20,  22},
                                             {20,  47},
                                             {21,  47},
                                             {21,  51},
                                             {22,  51},
                                             {22,  54},
                                             {23,  54},
                                             {27,  58},
                                             {27,  59},
                                             {31,  59}};
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

