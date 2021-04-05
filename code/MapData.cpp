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
		ERROR_MESSAGE("Can't initialize Collectible with color " + c + "!")
	}
}

bool Collectible::isCorrectCollectible(PVector robotPos, double angle, double uncertainty) const {
	return geometry::dist(robotPos + (geometry::angle2Vector(toRadians(angle - COLOR_SENSOR_ANGLE_OFFSET)) *
	                                  COLOR_SENSOR_DIST_TO_CORE), pos) < uncertainty
	       || geometry::dist(robotPos + (geometry::angle2Vector(toRadians(angle + COLOR_SENSOR_ANGLE_OFFSET)) *
	                                     COLOR_SENSOR_DIST_TO_CORE), pos) < uncertainty;
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
		if (index <= 3) {
			returnVector.insert(std::end(returnVector), std::begin(Collectibles_[index]),
			                    std::end(Collectibles_[index]));
		} else {
			ERROR_MESSAGE("index out of range");
		}
	}
	return returnVector;
}

Collectible *Field::getCollectible(PVector robotPos, double angle, double uncertainty, int color) {

	std::vector<Collectible> collectibles;

	if (color == -1) {
		collectibles = getCollectibles({0, 1, 2, 3});
	} else if (color >= 0 && color <= 3) {
		collectibles = Collectibles_[color];
	}

	for (auto &collectible : collectibles) {
		if (collectible.isCorrectCollectible(robotPos, angle, uncertainty)) {
			return &collectible;
		}
	}


	ERROR_MESSAGE("no valid collectible found")
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
		return true;
	}
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
	Area({{59,151},{87,151},{87,142},{103,142},{103,110},{69,110},{69,131},{59,131},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151},{59,151}})};
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
const std::vector<Collectible> GAME0COLLECTIBLES = {{{157,60},2,false},{{175,49},2,false},{{173,72},2,false},{{141,42},2,false},{{137,76},2,false},{{125,58},2,false},{{138,60},2,false},{{119,69},2,false},{{105,44},2,false},{{109,78},2,false},{{165,84},2,false},{{184,64},2,false},{{155,71},2,false},{{152,95},2,false},{{167,104},2,false},{{187,86},2,false},{{186,125},2,false},{{152,86},2,false},{{154,118},2,false},{{106,60},2,false},{{119,47},2,false},{{124,74},2,false},{{99,51},2,true},{{104,70},2,false},{{35,148},0,false},{{49,132},0,false},{{48,166},0,false},{{21,130},0,false},{{21,160},0,false},{{49,148},0,false},{{63,168},0,false},{{77,167},0,false},{{88,163},0,false},{{36,162},0,false},{{20,53},1,false},{{34,118},0,false},{{38,105},0,false},{{22,116},0,false},{{11,106},0,false},{{66,138},0,false},{{81,144},0,false},{{72,124},0,false},{{84,131},0,false},{{93,141},0,false},{{96,125},0,false},{{84,117},0,false},{{219,133},1,false},{{174,165},1,false},{{171,153},1,false},{{161,162},1,false},{{160,169},1,false},{{149,165},1,false},{{158,150},1,false},{{222,159},1,false},{{223,149},1,false},{{227,140},1,false},{{235,126},1,false},{{222,120},1,false},{{230,167},1,false},{{69,24},1,false},{{72,36},1,true},{{82,24},1,false},{{62,33},1,true},{{62,21},1,false},{{62,48},1,true},{{73,46},1,true},{{83,38},1,true},{{15,39},1,false},{{13,30},1,false},{{73,56},1,true},{{212,117},1,false},{{207,131},1,false},{{230,107},1,false},{{214,170},1,false},{{8,124},0,false},{{12,144},0,false},{{102,169},0,false},{{173,40},2,false},{{194,29},2,false},{{185,54},2,false},{{155,27},2,false},{{160,47},2,false},{{173,137},0,false},{{30,60},1,false},{{80,72},1,false}};

//------------- Game1_Objects --------------//

		/*walls*/
const std::vector<Area>GAME1WALLS = {};
		/*traps*/
const std::vector<Area>GAME1TRAPS = {
	Area({{162,160},{188,160},{188,159},{190,159},{190,158},{192,156},{193,156},{193,153},{194,153},{194,130},{193,130},{193,127},{192,127},{191,126},{191,125},{189,125},{189,124},{188,124},{188,123},{163,123},{163,124},{161,124},{161,125},{159,127},{158,127},{158,129},{157,129},{157,155},{158,155},{158,157},{159,157},{160,158},{160,159},{162,159}})};
		/*swamps*/
const std::vector<Area>GAME1SWAMPS = {
	Area({{25,196},{46,196},{46,195},{49,195},{49,194},{52,194},{52,193},{54,193},{54,192},{58,188},{59,188},{59,186},{60,186},{60,184},{61,184},{61,179},{62,179},{62,161},{61,161},{61,158},{60,158},{60,155},{59,155},{59,154},{58,154},{58,151},{55,151},{55,150},{53,149},{53,148},{50,148},{50,147},{47,147},{47,146},{25,146},{25,147},{21,147},{21,148},{19,148},{19,149},{18,149},{18,150},{16,150},{16,151},{14,153},{13,153},{13,155},{12,155},{12,156},{11,156},{11,160},{10,160},{10,182},{11,182},{11,185},{12,185},{12,187},{13,187},{13,188},{14,188},{14,190},{15,190},{15,191},{16,191},{16,193},{19,193},{19,194},{23,194},{23,195},{25,195}}),
	Area({{100,196},{166,196},{166,171},{164,171},{164,170},{162,170},{162,169},{160,169},{160,168},{158,167},{158,166},{156,166},{156,165},{148,157},{147,157},{147,155},{146,155},{146,154},{145,154},{145,152},{144,152},{144,150},{143,150},{143,149},{100,149}}),
	Area({{182,196},{251,196},{251,150},{205,150},{205,152},{204,152},{204,154},{203,154},{202,156},{201,156},{201,158},{200,158},{193,165},{193,166},{191,166},{191,168},{189,168},{189,169},{187,169},{187,170},{185,170},{185,171},{182,171}}),
	Area({{319,134},{339,134},{339,133},{343,133},{343,132},{346,132},{346,131},{348,131},{348,130},{353,125},{354,125},{354,122},{355,122},{355,117},{356,117},{356,101},{355,101},{355,96},{354,96},{354,94},{353,94},{353,92},{352,92},{349,89},{349,88},{347,88},{347,87},{345,87},{345,86},{343,86},{343,85},{338,85},{338,84},{321,84},{321,85},{316,85},{316,86},{314,86},{314,87},{312,87},{312,88},{310,88},{310,89},{307,92},{306,92},{306,94},{305,94},{305,97},{304,97},{304,120},{305,120},{305,124},{306,124},{306,126},{307,126},{311,130},{311,131},{313,131},{313,132},{316,132},{316,133},{319,133}}),
	Area({{206,132},{251,132},{251,83},{186,83},{186,110},{188,110},{188,111},{189,111},{189,112},{191,112},{191,113},{193,113},{193,114},{198,119},{199,119},{199,121},{201,121},{201,123},{202,123},{202,124},{203,124},{203,126},{204,126},{204,127},{205,127},{205,129},{206,129}}),
	Area({{100,131},{143,131},{143,130},{144,130},{144,128},{145,128},{145,126},{146,126},{146,124},{147,124},{148,123},{148,122},{150,122},{150,119},{151,119},{153,117},{153,116},{155,116},{155,115},{159,112},{159,111},{161,111},{161,110},{163,110},{163,109},{165,109},{165,108},{167,108},{167,107},{169,107},{169,83},{100,83}})};
		/*Water*/
const std::vector<Area>GAME1WATERS = {
	Area({{103,270},{251,270},{251,243},{103,243},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270},{103,270}}),
	Area({{104,28},{252,28},{252,1},{104,1},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28},{104,28}})};
		/*deposit*/
const std::vector<PVector>GAME1DEPOSITS = {{174,64},{177,197}};

	//------ Nodes ------//
		/*wall_nodes*/
const std::vector<PVector>GAME1WALLNODES = {};
		/*trap_nodes*/
const std::vector<PVector>GAME1TRAPNODES = {{161,161},{189,161},{189,160},{191,160},{191,159},{193,157},{194,157},{194,154},{195,154},{195,129},{194,129},{194,126},{193,126},{192,125},{192,124},{190,124},{190,123},{189,123},{189,122},{162,122},{162,123},{160,123},{160,124},{158,126},{157,126},{157,128},{156,128},{156,156},{157,156},{157,158},{158,158},{159,159},{159,160},{161,160}};
		/*swamp_nodes*/
const std::vector<PVector>GAME1SWAMPNODES = {{24,197},{47,197},{47,196},{50,196},{50,195},{53,195},{53,194},{55,194},{55,193},{59,189},{60,189},{60,187},{61,187},{61,185},{62,185},{62,180},{63,180},{63,160},{62,160},{62,157},{61,157},{61,154},{60,154},{60,153},{59,153},{59,150},{56,150},{56,149},{54,148},{54,147},{51,147},{51,146},{48,146},{48,145},{24,145},{24,146},{20,146},{20,147},{18,147},{18,148},{17,148},{17,149},{15,149},{15,150},{13,152},{12,152},{12,154},{11,154},{11,155},{10,155},{10,159},{9,159},{9,183},{10,183},{10,186},{11,186},{11,188},{12,188},{12,189},{13,189},{13,191},{14,191},{14,192},{15,192},{15,194},{18,194},{18,195},{22,195},{22,196},{24,196},{99,197},{167,197},{167,170},{165,170},{165,169},{163,169},{163,168},{161,168},{161,167},{159,166},{159,165},{157,165},{157,164},{149,156},{148,156},{148,154},{147,154},{147,153},{146,153},{146,151},{145,151},{145,149},{144,149},{144,148},{99,148},{181,197},{252,197},{252,149},{204,149},{204,151},{203,151},{203,153},{202,153},{201,155},{200,155},{200,157},{199,157},{192,164},{192,165},{190,165},{190,167},{188,167},{188,168},{186,168},{186,169},{184,169},{184,170},{181,170},{318,135},{340,135},{340,134},{344,134},{344,133},{347,133},{347,132},{349,132},{349,131},{354,126},{355,126},{355,123},{356,123},{356,118},{357,118},{357,100},{356,100},{356,95},{355,95},{355,93},{354,93},{354,91},{353,91},{350,88},{350,87},{348,87},{348,86},{346,86},{346,85},{344,85},{344,84},{339,84},{339,83},{320,83},{320,84},{315,84},{315,85},{313,85},{313,86},{311,86},{311,87},{309,87},{309,88},{306,91},{305,91},{305,93},{304,93},{304,96},{303,96},{303,121},{304,121},{304,125},{305,125},{305,127},{306,127},{310,131},{310,132},{312,132},{312,133},{315,133},{315,134},{318,134},{205,133},{252,133},{252,82},{185,82},{185,111},{187,111},{187,112},{188,112},{188,113},{190,113},{190,114},{192,114},{192,115},{197,120},{198,120},{198,122},{200,122},{200,124},{201,124},{201,125},{202,125},{202,127},{203,127},{203,128},{204,128},{204,130},{205,130},{99,132},{144,132},{144,131},{145,131},{145,129},{146,129},{146,127},{147,127},{147,125},{148,125},{149,124},{149,123},{151,123},{151,120},{152,120},{154,118},{154,117},{156,117},{156,116},{160,113},{160,112},{162,112},{162,111},{164,111},{164,110},{166,110},{166,109},{168,109},{168,108},{170,108},{170,82},{99,82}};



	//------ Collectibles ------//
		/*collectibles*/
const std::vector<Collectible> GAME1COLLECTIBLES = {{{46,215},0,false},{{64,196},0,false},{{64,217},0,false},{{35,199},0,false},{{37,234},0,false},{{100,238},0,false},{{36,257},0,false},{{113,254},0,true},{{81,222},0,false},{{80,250},0,false},{{134,240},0,false},{{168,251},0,true},{{154,259},0,true},{{207,255},0,true},{{132,260},0,true},{{264,241},0,false},{{273,229},0,false},{{282,255},0,false},{{231,252},0,true},{{238,263},0,true},{{329,208},0,false},{{341,195},0,false},{{346,223},0,false},{{316,188},0,false},{{309,223},0,false},{{318,255},0,false},{{332,244},0,false},{{272,207},0,false},{{296,211},0,false},{{67,27},1,false},{{62,13},1,false},{{73,46},1,false},{{13,21},1,false},{{15,89},1,false},{{79,30},1,false},{{132,13},1,true},{{124,39},1,false},{{97,17},1,false},{{97,42},1,false},{{181,23},1,true},{{192,9},1,true},{{200,35},1,false},{{166,10},1,true},{{157,38},1,false},{{263,22},1,false},{{278,41},1,false},{{248,38},1,false},{{308,27},1,false},{{322,24},1,false},{{222,10},1,true},{{238,20},1,true},{{297,38},1,false},{{293,24},1,false},{{265,40},1,false},{{325,60},1,false},{{341,17},1,false},{{279,41},1,false},{{9,38},1,false},{{39,113},2,false},{{52,98},2,false},{{60,129},2,false},{{27,95},2,false},{{28,126},2,false},{{323,148},2,false},{{329,112},2,false},{{342,163},2,false},{{313,130},2,false},{{309,159},2,false},{{66,153},2,false},{{79,138},2,false},{{78,172},2,false},{{48,141},2,false},{{287,136},2,false},{{306,90},2,false},{{316,79},2,false},{{340,137},2,false},{{295,76},2,false},{{289,105},2,false},{{292,178},2,false},{{290,162},2,false},{{303,198},2,false},{{273,193},2,false},{{276,125},2,false},{{271,82},2,false},{{271,160},2,false},{{265,110},2,false},{{262,136},2,false},{{75,88},2,false},{{82,104},2,false},{{72,65},2,false},{{69,114},2,false},{{284,66},1,false},{{16,209},1,false},{{58,83},1,false}};

