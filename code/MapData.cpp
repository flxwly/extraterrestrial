#include "MapData.hpp"

/**     -----------     **/
/**                     **/
/**        Point        **/
/**                     **/
/**     -----------     **/


Point::Point(int x, int y) {
	Point::x = x;
	Point::y = y;
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
	int n = area.getCorners().size(); // The number of corners

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
	Area({{63,154},{176,154},{176,136},{63,136}}),
	Area({{77,95},{94,95},{94,94},{117,72},{117,71},{123,71},{123,72},{146,94},{146,95},{163,95},{163,94},{164,94},{164,78},{163,78},{137,51},{136,51},{136,43},{135,43},{135,41},{134,41},{134,40},{132,40},{132,39},{131,39},{131,38},{129,38},{129,1},{110,1},{110,38},{109,38},{109,39},{107,39},{107,40},{105,42},{104,42},{104,45},{103,45},{103,51},{102,51},{77,77},{76,77},{76,94},{77,94}}),
	Area({{178,52},{202,52},{202,28},{178,28}}),
	Area({{38,51},{61,51},{61,28},{38,28}})};
		/*traps*/
const std::vector<Area>GAME0TRAPS = {
	Area({{39,138},{62,138},{62,135},{67,135},{67,110},{39,110}}),
	Area({{177,138},{202,138},{202,110},{173,110},{173,135},{177,135}})};
		/*swamps*/
const std::vector<Area>GAME0SWAMPS = {};
		/*Water*/
const std::vector<Area>GAME0WATERS = {
	Area({{0,60},{44,60},{44,52},{37,52},{37,27},{62,27},{62,35},{70,35},{70,1},{0,1}}),
	Area({{195,60},{239,60},{239,1},{170,1},{170,35},{177,35},{177,27},{203,27},{203,53},{195,53}})};
		/*deposit*/
const std::vector<Point>GAME0DEPOSITS = {{60,129},{178,128}};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<Point>GAME0WALLNODES = {{62,155},{177,155},{177,135},{62,135},{76,96},{95,96},{95,95},{118,73},{118,72},{122,72},{122,73},{145,95},{145,96},{164,96},{164,95},{165,95},{165,77},{164,77},{138,50},{137,50},{137,42},{136,42},{136,40},{135,40},{135,39},{133,39},{133,38},{132,38},{132,37},{130,37},{109,37},{108,37},{108,38},{106,38},{106,39},{104,41},{103,41},{103,44},{102,44},{102,50},{101,50},{76,76},{75,76},{75,95},{76,95},{177,53},{203,53},{203,27},{177,27},{37,52},{62,52},{62,27},{37,27}};
		/*trap_nodes*/
const std::vector<Point>GAME0TRAPNODES = {{38,139},{63,139},{63,136},{68,136},{68,109},{38,109},{176,139},{203,139},{203,109},{172,109},{172,136},{176,136}};

//------------- Game1_Objects --------------//

		/*walls*/
const std::vector<Area>GAME1WALLS = {
	Area({{169,240},{190,240},{190,239},{195,239},{195,238},{197,238},{197,237},{199,237},{199,236},{201,236},{201,235},{207,229},{208,229},{208,226},{209,226},{209,224},{210,224},{210,221},{211,221},{211,196},{210,196},{210,192},{209,192},{209,190},{208,190},{208,188},{207,188},{207,187},{206,187},{206,185},{205,185},{203,183},{203,182},{201,182},{201,181},{199,180},{199,179},{196,179},{196,178},{193,178},{193,177},{188,177},{188,176},{171,176},{171,177},{166,177},{166,178},{163,178},{163,179},{160,179},{160,180},{158,181},{158,182},{156,182},{156,183},{153,186},{152,186},{152,188},{151,188},{151,190},{150,190},{150,192},{149,192},{149,196},{148,196},{148,221},{149,221},{149,224},{150,224},{150,226},{151,226},{151,228},{152,228},{152,230},{153,230},{158,235},{158,236},{160,236},{160,237},{162,237},{162,238},{164,238},{164,239},{169,239}}),
	Area({{103,174},{127,174},{127,120},{103,120}}),
	Area({{232,174},{256,174},{256,121},{232,121}}),
	Area({{153,94},{206,94},{206,71},{153,71}})};
		/*traps*/
const std::vector<Area>GAME1TRAPS = {
	Area({{85,201},{115,201},{115,175},{102,175},{102,171},{85,171}}),
	Area({{244,201},{275,201},{275,171},{257,171},{257,175},{244,175}}),
	Area({{164,34},{195,34},{195,4},{164,4}})};
		/*swamps*/
const std::vector<Area>GAME1SWAMPS = {
	Area({{120,208},{147,208},{147,195},{148,195},{148,191},{149,191},{149,189},{150,189},{150,187},{151,187},{151,185},{152,185},{155,182},{155,181},{157,181},{157,180},{159,179},{159,178},{162,178},{162,177},{165,177},{165,176},{170,176},{170,175},{189,175},{189,176},{194,176},{194,177},{197,177},{197,178},{200,178},{200,179},{202,180},{202,181},{204,181},{204,182},{206,184},{207,184},{207,186},{208,186},{208,187},{209,187},{209,189},{210,189},{210,191},{211,191},{211,195},{212,195},{212,208},{239,208},{239,175},{231,175},{231,168},{128,168},{128,175},{120,175}})};
		/*Water*/
const std::vector<Area>GAME1WATERS = {
	Area({{128,167},{231,167},{231,127},{199,127},{199,95},{160,95},{160,127},{128,127}}),
	Area({{0,63},{31,63},{31,62},{37,62},{37,61},{40,61},{40,60},{44,60},{44,59},{46,59},{46,58},{48,58},{48,57},{50,57},{50,56},{52,56},{52,55},{54,55},{54,54},{55,54},{55,53},{57,53},{57,52},{59,52},{59,51},{62,49},{62,48},{64,48},{64,47},{68,43},{69,43},{69,41},{70,41},{72,38},{73,38},{73,36},{74,36},{74,35},{75,35},{75,33},{76,33},{76,32},{77,32},{77,30},{78,30},{78,28},{79,28},{79,25},{80,25},{80,23},{81,23},{81,21},{82,21},{82,17},{83,17},{83,13},{84,13},{84,5},{85,5},{85,1},{0,1}}),
	Area({{329,63},{359,63},{359,1},{275,1},{275,9},{276,9},{276,15},{277,15},{277,19},{278,19},{278,22},{279,22},{279,24},{280,24},{280,26},{281,26},{281,29},{282,29},{282,31},{283,31},{283,32},{284,32},{284,34},{285,34},{285,35},{286,35},{286,37},{287,37},{287,38},{288,38},{288,40},{289,40},{297,48},{297,49},{299,49},{299,50},{301,51},{301,52},{303,52},{303,53},{304,53},{304,54},{306,54},{306,55},{307,55},{307,56},{309,56},{309,57},{312,57},{312,58},{314,58},{314,59},{316,59},{316,60},{319,60},{319,61},{323,61},{323,62},{329,62}})};
		/*deposit*/
const std::vector<Point>GAME1DEPOSITS = {{91,118},{267,118},{179,214}};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<Point>GAME1WALLNODES = {{168,241},{191,241},{191,240},{196,240},{196,239},{198,239},{198,238},{200,238},{200,237},{202,237},{202,236},{208,230},{209,230},{209,227},{210,227},{210,225},{211,225},{211,222},{212,222},{212,195},{211,195},{211,191},{210,191},{210,189},{209,189},{209,187},{208,187},{208,186},{207,186},{207,184},{206,184},{204,182},{204,181},{202,181},{202,180},{200,179},{200,178},{197,178},{197,177},{194,177},{194,176},{189,176},{189,175},{170,175},{170,176},{165,176},{165,177},{162,177},{162,178},{159,178},{159,179},{157,180},{157,181},{155,181},{155,182},{152,185},{151,185},{151,187},{150,187},{150,189},{149,189},{149,191},{148,191},{148,195},{147,195},{147,222},{148,222},{148,225},{149,225},{149,227},{150,227},{150,229},{151,229},{151,231},{152,231},{157,236},{157,237},{159,237},{159,238},{161,238},{161,239},{163,239},{163,240},{168,240},{102,175},{128,175},{128,119},{102,119},{231,175},{257,175},{257,120},{231,120},{152,95},{207,95},{207,70},{152,70}};
		/*trap_nodes*/
const std::vector<Point>GAME1TRAPNODES = {{84,202},{116,202},{116,174},{103,174},{103,170},{84,170},{243,202},{276,202},{276,170},{256,170},{256,174},{243,174},{163,35},{196,35},{196,3},{163,3}};

