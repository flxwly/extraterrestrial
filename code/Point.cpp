#include "Point.hpp"

Point::Point(std::pair<int, int> _pos, int _color) {
    Point::pos = _pos;
    Point::color = _color;                              // -1 = undefined; 0 = r; 1 = c; 2 = b;
    Point::state = -1;                                  // -1 = undefined; 0 = nicht da; 1 = gesehen; TODO: Points can reapear after some time
}

void Point::init(const std::vector<Point *>& points) {
    // for each point
    for (auto point : points) {
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
                    std::cerr << __FUNCTION__ << "\tPoint: " << str(point->pos) << " has no valid color ("
                              << std::to_string(point->color) << ")" << std::endl;
                    break;
            }
            Point::all_points.push(
                    {Point::dist(point->pos), point});
        }
    }
}

double Point::dist(std::pair<int, int> _pos) {
    return ::dist(Point::pos.first, _pos.first, Point::pos.second, _pos.second);
}

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
}