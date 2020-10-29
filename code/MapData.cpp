#include "MapData.hpp"

/**     -----------     **/
/**                     **/
/**        Point        **/
/**                     **/
/**     -----------     **/


Point::Point(int _x, int _y) {
	Point::x = _x;
	Point::y = _y;
}

bool operator==(const Point &p1, const Point &p2) {
	return p1.x == p2.x && p1.y == p2.y;
}

bool operator!=(const Point &p1, const Point &p2) {
	return !(p1 == p2);
}

Point::operator bool() const {
	return Point::x == -1 && Point::y == -1;
}

Point operator+(const Point &a, const Point &b) {
	return Point(a.x + b.x, a.y + b.y);
}

Point operator-(const Point &a, const Point &b) {
	return Point(a.x - b.x, a.y - b.y);
}

/**     -----------     **/
/**                     **/
/**     Collectible     **/
/**                     **/
/**     -----------     **/

Collectible::Collectible(const Point &p, const int &c) {
	Collectible::pos_ = p;
	Collectible::color_ = c;
	Collectible::state_ = 0;
}

unsigned int Collectible::getColor() const {
	return color_;
}

const Point &Collectible::getPos() const {
	return pos_;
}

unsigned int Collectible::getState() const {
	return state_;
}

void Collectible::setState(unsigned int state) {
	state_ = state;
}

bool Collectible::isCorrectCollectible(Point robot_pos, double angle, double uncertainty) {

	return true;
}


/**     -----------     **/
/**                     **/
/**         Line        **/
/**                     **/
/**     -----------     **/

// Line::Line(): Constructor for Line Class
Line::Line(const Point &p1, const Point &p2) {
	/** Set private vars **/
	Line::p1_ = p1;
	Line::p2_ = p2;
}

const Point &Line::getP1() const {
	return p1_;
}

const Point &Line::getP2() const {
	return p2_;
}

void Line::setP1(const Point &p_1) {
	p1_ = p_1;
}

void Line::setP2(const Point &p_2) {
	p2_ = p_2;
}

/**     -----------     **/
/**                     **/
/**         Area        **/
/**                     **/
/**     -----------     **/

// Area::Area(): Constructor for Area class
Area::Area(const std::vector<Point> &p_s) {

	// xmin/ymin = first elem of p_s because there are problems with Infinity
	Area::min_ = {p_s.front().x, p_s.front().y};
	// xmax/ymax = 0;
	Area::max_ = {0, 0};

	Area::Corners_ = p_s;
	Point last_p = p_s.back();
	for (Point p : p_s) {
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

const std::vector<Point> &Area::getCorners() const {
	return Corners_;
}

const std::vector<Line> &Area::getEdges() const {
	return Edges_;
}

const Point &Area::getMin() const {
	return min_;
}

const Point &Area::getMax() const {
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
             const std::vector<Point> &deposits,
             const std::vector<Point> &wallNodes,
             const std::vector<Point> &trapNodes,
             const std::array<std::vector<Collectible>, 3> &collectibles) {
	// Field::size_: Size of the Field
	size_ = {width, height};

	// Field::(MapObjectName)_: Different MapObjects that can be displayed as areas.
	Walls_ = walls;
	Traps_ = traps;
	Swamps_ = swamps;
	Waters_ = waters;

	// Field::Deposits_: Deposit Areas of the Field
	Deposits_ = deposits;

	// Field::Collectibles: Collectible Points of the Field
	Collectibles_ = collectibles;
}

Point Field::getSize() {
	return size_;
}

std::vector<Area> Field::getMapObjects(std::vector<unsigned int> indices) {
	std::vector<Area> return_vector;
	// return after all indices have been checked.
	while (!indices.empty()) {
		unsigned int index = indices.back();
		indices.pop_back();
		switch (index) {
			case 0:
				return_vector.insert(std::end(return_vector), std::begin(Field::Walls_), std::end(Field::Walls_));
			case 1:
				return_vector.insert(std::end(return_vector), std::begin(Field::Traps_), std::end(Field::Traps_));
			case 2:
				return_vector.insert(std::end(return_vector), std::begin(Field::Swamps_), std::end(Field::Swamps_));
			case 3:
				return_vector.insert(std::end(return_vector), std::begin(Field::Waters_), std::end(Field::Waters_));
			default: ERROR_MESSAGE("index out of range/invalid")
		}
	}

	return return_vector;
}

std::vector<Point> Field::getMapNodes(std::vector<unsigned int> indices) {
	std::vector<Point> return_vector;
	// return after all indices have been checked.
	while (!indices.empty()) {
		unsigned int index = indices.back();
		indices.pop_back();
		switch (index) {
			case 0:
				return_vector.insert(std::end(return_vector), std::begin(Field::WallNodes_), std::end(Field::WallNodes_));
			case 1:
				return_vector.insert(std::end(return_vector), std::begin(Field::TrapNodes_), std::end(Field::TrapNodes_));
			default: ERROR_MESSAGE("index out of range/invalid")
		}
	}

	return return_vector;
}

/** Getter for Field::Collectibles_ **/
std::vector<Point> Field::getDeposits() {
	return Deposits_;
}

/** Getter for Field::Collectibles_ **/
std::vector<Collectible> Field::getCollectibles(std::vector<unsigned int> colors) {
	std::vector<Collectible> return_vector;
	// return after all indices have been checked.
	while (!colors.empty()) {
		unsigned int index = colors.back();
		colors.pop_back();
		if (index <= 2) {
			return_vector.insert(std::end(return_vector), std::begin(Field::Collectibles_[index]),
			                     std::end(Field::Collectibles_[index]));
		} else {
			ERROR_MESSAGE("index out of range");
		}
	}
	return return_vector;
}


/**     -----------     **/
/**                     **/
/**      geometry       **/
/**                     **/
/**     -----------     **/


// geometry::onSide():  Test if a point p2 is left/on/right a line through p0 and p1.
//      Input:  Point p0, p1, p2
//      Return: >0 left; =0 on; <0 right
bool geometry::isLeft(Point p0, Point p1, Point p2) {
	return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y) > 0;
}

// geometry::isInside():  Test if a point p2 is left/on/right a line through p0 and p1.
//      Input:  Point p0, p1, p2
//      Return: >0 left; =0 on; <0 right
bool geometry::isInside(const Point &p, Area &area) {
	// Point in Polygon(PIP) using the winding number algorithm:
	// source: https://en.wikipedia.ord/wiki/Point_in_polygon

	int wn = 0;    // the  winding number counter
	unsigned int n = area.getCorners().size(); // The number of corners

	std::vector<Point> poly = area.getCorners();      // vector with all corners + V[0] at V[n+1]
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

Point geometry::isIntersecting(Line &l1, Line &l2) {
	// line - line intersection using determinants:
	// source: https://en.wikipedia.ord/wiki/Line%E2%%80%90line_intersection

	// determiant = x1 * y2- x2 * y1
	int L1 = l1.getP1().x * l1.getP2().y - l1.getP2().x * l1.getP1().y;
	int L2 = l2.getP1().x * l2.getP2().y - l2.getP2().x * l2.getP1().y;

	// difs
	int L1xdif = l1.getP1().x - l1.getP2().x;
	int L1ydif = l1.getP1().y - l1.getP2().y;
	int L2xdif = l2.getP1().x - l2.getP2().x;
	int L2ydif = l2.getP1().y - l2.getP2().y;

	// determiant a*d - b*c
	double xnom = L1 * L2xdif - L2 * L1xdif;
	double ynom = L1 * L2ydif - L2 * L2ydif;
	double denom = L1xdif * L2ydif - L2xdif * L1ydif;

	// Lines don't cross
	if (denom == 0) {
		return {-1, -1};
	}

	// return intersection
	return {static_cast<int>(round(xnom / denom)), static_cast<int>(round(ynom / denom))};
}

double geometry::sqDist(const Point &p1, const Point &p2) {
	return pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2);
}

double geometry::dist(const Point &p1, const Point &p2) {
	return sqrt(geometry::sqDist(p1, p2));
}




///   _______                _____          __
///  |   |   |.---.-..-----.|     \ .---.-.|  |_ .---.-.
///  |       ||  _  ||  _  ||  --  ||  _  ||   _||  _  |
///  |__|_|__||___._||   __||_____/ |___._||____||___._|
///                  |__|



//------------- Game0_Objects --------------//

		/*walls*/
const std::vector<Area>GAME0WALLS = {
	Area({{90,164},{154,164},{154,145},{90,145}}),
	Area({{47,128},{66,128},{66,64},{47,64}}),
	Area({{191,128},{209,128},{209,64},{191,64}}),
	Area({{93,31},{156,31},{156,13},{93,13}})};
		/*traps*/
const std::vector<Area>GAME0TRAPS = {
	Area({{107,113},{136,113},{136,85},{107,85}})};
		/*swamps*/
const std::vector<Area>GAME0SWAMPS = {};
		/*Water*/
const std::vector<Area>GAME0WATERS = {
	Area({{59,151},{89,151},{89,144},{103,144},{103,110},{67,110},{67,129},{59,129}})};
		/*deposit*/
const std::vector<Point>GAME0DEPOSITS = {{197,27},{38,147}};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<Point>GAME0WALLNODES = {{89,165},{155,165},{155,144},{89,144},{46,129},{67,129},{67,63},{46,63},{190,129},{210,129},{210,63},{190,63},{92,32},{157,32},{157,12},{92,12}};
		/*trap_nodes*/
const std::vector<Point>GAME0TRAPNODES = {{106,114},{137,114},{137,84},{106,84}};

//------------- Game1_Objects --------------//

		/*walls*/
const std::vector<Area>GAME1WALLS = {
	Area({{89,238},{261,238},{261,149},{236,149},{236,213},{114,213},{114,149},{89,149}}),
	Area({{235,134},{260,134},{260,42},{89,42},{89,133},{114,133},{114,67},{235,67}}),
	Area({{32,59},{55,59},{55,58},{59,58},{59,57},{64,52},{65,52},{65,50},{66,50},{66,46},{67,46},{67,25},{66,25},{66,22},{65,22},{59,16},{59,15},{55,15},{55,14},{32,14},{32,15},{28,15},{28,16},{26,16},{26,18},{25,18},{23,21},{22,21},{22,23},{21,23},{21,46},{22,46},{22,50},{23,50},{23,53},{24,53},{28,57},{28,58},{32,58}})};
		/*traps*/
const std::vector<Area>GAME1TRAPS = {
	Area({{164,158},{186,158},{186,157},{188,157},{188,156},{190,154},{191,154},{191,151},{192,151},{192,132},{191,132},{191,129},{190,129},{189,128},{189,127},{187,127},{187,126},{186,126},{186,125},{165,125},{165,126},{163,126},{163,127},{161,129},{160,129},{160,131},{159,131},{159,153},{160,153},{160,155},{161,155},{162,156},{162,157},{164,157}})};
		/*swamps*/
const std::vector<Area>GAME1SWAMPS = {
	Area({{32,189},{34,189},{34,188},{37,188},{37,189},{39,189},{39,188},{42,188},{42,187},{45,187},{45,186},{47,186},{47,185},{51,181},{52,181},{52,179},{53,179},{53,177},{54,177},{54,172},{55,172},{55,168},{54,168},{54,165},{53,165},{53,162},{52,162},{52,161},{51,161},{51,158},{48,158},{48,157},{46,156},{46,155},{43,155},{43,154},{40,154},{40,153},{32,153},{32,154},{28,154},{28,155},{26,155},{26,156},{25,156},{25,157},{23,157},{23,158},{21,160},{20,160},{20,162},{19,162},{19,163},{18,163},{18,167},{17,167},{17,175},{18,175},{18,178},{19,178},{19,180},{20,180},{20,181},{21,181},{21,183},{22,183},{22,184},{23,184},{23,186},{26,186},{26,187},{30,187},{30,188},{32,188}}),
	Area({{115,189},{159,189},{159,178},{157,178},{157,177},{155,177},{155,176},{153,176},{153,175},{151,174},{151,173},{149,173},{149,172},{141,164},{140,164},{140,162},{139,162},{139,161},{138,161},{138,159},{137,159},{137,157},{136,157},{136,156},{115,156}}),
	Area({{189,189},{235,189},{235,157},{212,157},{212,159},{211,159},{211,161},{210,161},{209,163},{208,163},{208,165},{207,165},{200,172},{200,173},{198,173},{198,175},{196,175},{196,176},{194,176},{194,177},{192,177},{192,178},{189,178}}),
	Area({{326,127},{332,127},{332,126},{336,126},{336,125},{339,125},{339,124},{341,124},{341,123},{346,118},{347,118},{347,115},{348,115},{348,110},{349,110},{349,108},{348,108},{348,103},{347,103},{347,101},{346,101},{346,99},{345,99},{342,96},{342,95},{340,95},{340,94},{338,94},{338,93},{336,93},{336,92},{331,92},{331,91},{328,91},{328,92},{323,92},{323,93},{321,93},{321,94},{319,94},{319,95},{317,95},{317,96},{314,99},{313,99},{313,101},{312,101},{312,104},{311,104},{311,113},{312,113},{312,117},{313,117},{313,119},{314,119},{318,123},{318,124},{320,124},{320,125},{323,125},{323,126},{326,126}}),
	Area({{213,125},{234,125},{234,90},{193,90},{193,103},{195,103},{195,104},{196,104},{196,105},{198,105},{198,106},{200,106},{200,107},{205,112},{206,112},{206,114},{208,114},{208,116},{209,116},{209,117},{210,117},{210,119},{211,119},{211,120},{212,120},{212,122},{213,122}}),
	Area({{115,124},{136,124},{136,123},{137,123},{137,121},{138,121},{138,119},{139,119},{139,117},{140,117},{141,116},{141,115},{143,115},{143,112},{144,112},{146,110},{146,109},{148,109},{148,108},{152,105},{152,104},{154,104},{154,103},{156,103},{156,102},{158,102},{158,101},{160,101},{160,100},{162,100},{162,90},{115,90}})};
		/*Water*/
const std::vector<Area>GAME1WATERS = {
	Area({{103,270},{251,270},{251,243},{103,243}}),
	Area({{104,28},{252,28},{252,1},{104,1}})};
		/*deposit*/
const std::vector<Point>GAME1DEPOSITS = {{174,68},{178,192}};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<Point>GAME1WALLNODES = {{88,239},{262,239},{262,148},{235,148},{235,212},{115,212},{115,148},{88,148},{234,135},{261,135},{261,41},{88,41},{88,134},{115,134},{115,68},{234,68},{31,60},{56,60},{56,59},{60,59},{60,58},{65,53},{66,53},{66,51},{67,51},{67,47},{68,47},{68,24},{67,24},{67,21},{66,21},{60,15},{60,14},{56,14},{56,13},{31,13},{31,14},{27,14},{27,15},{25,15},{25,17},{24,17},{22,20},{21,20},{21,22},{20,22},{20,47},{21,47},{21,51},{22,51},{22,54},{23,54},{27,58},{27,59},{31,59}};
		/*trap_nodes*/
const std::vector<Point>GAME1TRAPNODES = {{163,159},{187,159},{187,158},{189,158},{189,157},{191,155},{192,155},{192,152},{193,152},{193,131},{192,131},{192,128},{191,128},{190,127},{190,126},{188,126},{188,125},{187,125},{187,124},{164,124},{164,125},{162,125},{162,126},{160,128},{159,128},{159,130},{158,130},{158,154},{159,154},{159,156},{160,156},{161,157},{161,158},{163,158}};

