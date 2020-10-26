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



/**     -----------     **/
/**                     **/
/**     Collectible     **/
/**                     **/
/**     -----------     **/

// Collectible::Collectible(): Constructor for Collectible Class
Collectible::Collectible(const Point &p, const int &c) {
    Collectible::pos_ = p;
    Collectible::color_ = c;
    Collectible::state_ = 0;
}

// Getter for Collectible::pos_
Point Collectible::pos() {
    return Collectible::pos_;
}

// Getter for Collectible::color_
int Collectible::color() {
    return Collectible::color_;
}

// Getter for Collectible::state_
int Collectible::state() {
    return static_cast<int>(Collectible::state_);
}

// Setter for Collectible::state_
void Collectible::setState(int s) {
    Collectible::state_ = s;
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

// Getter for Line::p1_
Point Line::p1() {
    return Line::p1_;
}

// Getter for Line::p2_
Point Line::p2() {
    return Line::p2_;
}


// Line::intersects():  Test if a line l intersects Line::
//      Input:  Line l
//      Return: intersection Point if existing. Otherwise {-1,-1}
Point Line::intersects(Line &l) {
    // line - line intersection using determinants:
    // source: https://en.wikipedia.ord/wiki/Line%E2%%80%90line_intersection

    // determiant = x1 * y2- x2 * y1
    int L1 = Line::p1().x * Line::p2().y - Line::p2().x * Line::p1().y;
    int L2 = l.p1().x * l.p2().y - l.p2().x * l.p1().y;

    // difs
    int L1xdif = Line::p1().x - Line::p2().x;
    int L1ydif = Line::p1().y - Line::p2().y;
    int L2xdif = l.p1().x - l.p2().x;
    int L2ydif = l.p1().y - l.p2().y;

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

bool Area::isInside(const Point &p) {
    return geometry::isInside(p, *this);
}

std::vector<Point> Area::Corners() {
    return Area::Corners_;
}

std::vector<Line> Area::Edges() {
    return Area::Edges_;
}


/**     -----------     **/
/**                     **/
/**        Field        **/
/**                     **/
/**     -----------     **/



/** Field::Field(): Constructor for Field class **/
Field::Field(const int &width, const int &height,
             const std::vector<Area> &walls,
             const std::vector<Area> &traps,
             const std::vector<Area> &swamps,
             const std::vector<Area> &waters,
             const std::vector<Point> &wnodes,
             const std::vector<Point> &tnodes,
             const std::vector<Point> &deposits,
             const std::array<std::vector<Collectible>, 3> &collectibles) {
    // Field::size_: Size of the Field
    Field::size_ = {width, height};

    // Field::(MapObjectName)_: Different MapObjects that can be displayed as areas.
    Field::Walls_ = walls;
    Field::Traps_ = traps;
    Field::Swamps_ = swamps;
    Field::Waters_ = waters;

    // Field::(type)Nodes_: Nodes for different MapObjects.
    Field::tNodes_ = tnodes;
    Field::wNodes_ = wnodes;

    // Field::Deposits_: Deposit Areas of the Field
    Field::Deposits_ = deposits;

    // Field::Collectibles: Collectible Points of the Field
    Field::Collectibles_ = collectibles;
}

/** Getter for Field::size **/
Point Field::size() {
    return Field::size_;
}

/** Getter for Field::(MapObjectName)_ **/
std::vector<Area> Field::MapObjects(std::vector<unsigned int> indices) {
    std::vector<Area> return_vector;
    // return after all indices have been checked.
    while (!indices.empty()) {
        unsigned int index = indices.back();
        indices.pop_back();
        switch (index) {
            case 1:
                return_vector.insert(std::end(return_vector), std::begin(Field::Walls_), std::end(Field::Walls_));
            case 2:
                return_vector.insert(std::end(return_vector), std::begin(Field::Traps_), std::end(Field::Traps_));
            case 3:
                return_vector.insert(std::end(return_vector), std::begin(Field::Swamps_), std::end(Field::Swamps_));
            case 5:
                return_vector.insert(std::end(return_vector), std::begin(Field::Waters_), std::end(Field::Waters_));
            default: ERROR_MESSAGE("index out of range/invalid")
        }
    }

    return return_vector;
}

/** Getter for Field::(type)Nodes_ **/
std::vector<Point> Field::Nodes(std::vector<unsigned int> indices) {
    std::vector<Point> return_vector;
    // return after all indices have been checked.
    while (!indices.empty()) {
        unsigned int index = indices.back();
        indices.pop_back();
        switch (index) {
            case 1:
                return_vector.insert(std::end(return_vector), std::begin(Field::wNodes_), std::end(Field::wNodes_));
            case 2:
                return_vector.insert(std::end(return_vector), std::begin(Field::tNodes_), std::end(Field::tNodes_));
            default: ERROR_MESSAGE("index out of range/invalid")
        }
    }

    return return_vector;
}

/** Getter for Field::Collectibles_ **/
std::vector<Point> Field::Deposits() {
    return Field::Deposits_;
}

/** Getter for Field::Collectibles_ **/
std::vector<Collectible> Field::Collectibles(std::vector<unsigned int> indices) {
    std::vector<Collectible> return_vector;
    // return after all indices have been checked.
    while (!indices.empty()) {
        unsigned int index = indices.back();
        indices.pop_back();
        switch (index) {
            case 1:
                return_vector.insert(std::end(return_vector), std::begin(Field::Collectibles_[0]),
                                     std::end(Field::Collectibles_[0]));
            case 2:
                return_vector.insert(std::end(return_vector), std::begin(Field::Collectibles_[1]),
                                     std::end(Field::Collectibles_[1]));
            case 3:
                return_vector.insert(std::end(return_vector), std::begin(Field::Collectibles_[2]),
                                     std::end(Field::Collectibles_[2]));
            default: ERROR_MESSAGE("index out of range")
        }
    }
    return return_vector;
}




/*
 *
 * namespace geometry
 *
 * */


// geometry::onSide():  Test if a point p2 is left/on/right a line through p0 and p1.
//      Input:  Point p0, p1, p2
//      Return: >0 left; =0 on; <0 right
int onSide(Point p0, Point p1, Point p2) {
    return ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));
}


bool isInside(const Point &p, Area &area) {
    // Point in Polygon(PIP) using the winding number algorithm:
    // source: https://en.wikipedia.ord/wiki/Point_in_polygon

    int wn = 0;    // the  winding number counter
    int n = area.Corners().size(); // The number of corners


    std::vector<Point> poly = area.Corners();      // vector with all corners + V[0] at V[n+1]
    poly.push_back(poly.front());

    // loop through all edges of the polygon
    for (unsigned int i = 0; i < n; i++) {     // edge from V[i] to  V[i+1]
        if (poly[i].y <= p.y) {                                 // start y <= P.y
            if (poly[i + 1].y > p.y)                            // an upward crossing
                if (onSide(poly[i], poly[i + 1], p) > 0)        // P left of  edge
                    ++wn;                                       // have  a valid up intersect
        } else {                                                // start y > P.y (no test needed)
            if (poly[i + 1].y <= p.y)                           // a downward crossing
                if (onSide(poly[i], poly[i + 1], p) < 0)        // P right of  edge
                    --wn;                                       // have  a valid down intersect
        }
    }
    return wn != 0;
}

Point intersects(Line &l1, Line &l2) {
    // line - line intersection using determinants:
    // source: https://en.wikipedia.ord/wiki/Line%E2%%80%90line_intersection

    // determiant = x1 * y2- x2 * y1
    int L1 = l1.p1().x * l1.p2().y - l1.p2().x * l1.p1().y;
    int L2 = l2.p1().x * l2.p2().y - l2.p2().x * l2.p1().y;

    // difs
    int L1xdif = l1.p1().x - l1.p2().x;
    int L1ydif = l1.p1().y - l1.p2().y;
    int L2xdif = l2.p1().x - l2.p2().x;
    int L2ydif = l2.p1().y - l2.p2().y;

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