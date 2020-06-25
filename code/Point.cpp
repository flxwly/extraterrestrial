#include "Point.hpp"



Point::Point(int _x, int _y, std::vector<std::pair<int, int>> *_dp_areas, int _color) {

    Point::x = _x;
    Point::y = _y;
    Point::_color = static_cast<short int>(_color);

    // determine nearest deposit area
    Point::closest_deposit_area = _dp_areas->front();
    double t_dist = dist(Point::closest_deposit_area.first, Point::x, Point::closest_deposit_area.second, Point::y);
    for (auto dp_area : *_dp_areas) {
        if (t_dist > dist(dp_area.first, Point::x, dp_area.second, Point::y)) {
            t_dist = dist(dp_area.first, Point::x, dp_area.second, Point::y);
            Point::closest_deposit_area = dp_area;
        }
    }
}

void Point::calculate_closest_points(std::vector<Point *> *_points) {

    for (auto _point : *_points) {
        if (_point->x != Point::x && _point->y != Point::y) {
            double t_dist = dist(Point::x, _point->x, Point::y, _point->y);
            switch (_point->_color) {
                case 1:
                    r_points.push({t_dist, _point});
                    break;
                case 2:
                    c_points.push({t_dist, _point});
                    break;
                case 3:
                    b_points.push({t_dist, _point});
                    break;
                default:
                    break;
            }
        }
    }
}

double Point::calculate_dist_to_point(const int &x1, const int &y1){
    return dist(this->x, x1, this->y, y1);
}

std::pair<int, int> Point::pos() {
    return {Point::x, Point::y};
}

int Point::color() const {
    return Point::_color;
}

