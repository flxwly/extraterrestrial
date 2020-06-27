#include "Point.hpp"
#define ERROR_MESSAGE(MESSAGE) {std::cerr << __FUNCTION__ << "\t" << MESSAGE << std::endl;}


Point::Point(std::pair<int, int> *_pos, int _c) {
    Point::pos = *_pos;
    Point::color = _c;                              // -1 = undefined; 0 = r; 1 = c; 2 = b;
    Point::state = -1;                                  // -1 = undefined; 0 = nicht da; 1 = gesehen; TODO: Points can reapear after some time
}

/*void Point::init(const std::vector<Point *>& points) {
    ERROR_MESSAGE("\tcurrent Point: " << str(this->pos) << " #"
                              << std::to_string(Point::color))
    // for each point
    for (auto point : points) {
        ERROR_MESSAGE( "\tchecking Point: " << str(point->pos) << " #"
                  << std::to_string(point->color) << " ...")
        // if the point is not the same as this
        if (point != this) {
            switch (point->color) {
                case 0:
                    Point::r_points.push(
                            {Point::dist(point->pos), point});
                    break;
                case 1:
                    Point::c_points.push(
                            {Point::dist(point->pos), point});
                    break;
                case 2:
                    Point::b_points.push(
                            {Point::dist(point->pos), point});
                    break;
                default:
                    ERROR_MESSAGE("\tinvalid Point: " << str(point->pos) << " #"
                                              << std::to_string(point->color))
                    break;
            }
            Point::all_points.push(
                    {Point::dist(point->pos), point});
        }
    }
}*/

double Point::dist(std::pair<int, int> _pos) {
    //ERROR_MESSAGE("Dist from " + str(this->pos) + " to " + str(_pos) + " = " + std::to_string(::dist(Point::pos.first, _pos.first, Point::pos.second, _pos.second)))
    return ::dist(Point::pos.first, _pos.first, Point::pos.second, _pos.second);
}

/*
Point *Point::closest_point(int _color) {
    switch (color) {
        case 0:
            return Point::r_points.top().second;
        case 1:
            return Point::c_points.top().second;
        case 2:
            return Point::b_points.top().second;
        default:
            std::cerr << __FUNCTION__ << "\tColor: " << std::to_string(_color) << "is not valid" << std::endl;
            return nullptr;
    }
}*/
