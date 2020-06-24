#include "Point.hpp"

struct Point::dist_PRIORITY {
    bool operator()(std::pair<int, std::pair<int, int>> pobj, std::pair<int, std::pair<int, int>> cobj, Point *cur) {
        return pobj.first < cobj.first;
    }
};


double Point::get_dist(int rx, int ry) const {
    return dist(this->x, rx, this->y, ry);
}


Point::Point(int _x, int _y, std::vector<std::pair<int, int>> *_dp_areas, unsigned int _color) {

    this->x = _x;
    this->y = _y;
    this->color = static_cast<short int>(_color);

    // determine nearest deposit area
    this->nearest_deposit_area = _dp_areas->front();
    double t_dist = dist(this->nearest_deposit_area.first, this->x, this->nearest_deposit_area.second, this->y);
    for (auto dp_area : *_dp_areas) {
        if (t_dist > dist(dp_area.first, this->x, dp_area.second, this->y)) {
            t_dist = dist(dp_area.first, this->x, dp_area.second, this->y);
            this->nearest_deposit_area = dp_area;
        }
    }


    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, Point::dist_PRIORITY> nearest_r_points;
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, Point::dist_PRIORITY> nearest_c_points;
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, Point::dist_PRIORITY> nearest_b_points;


    int dist = 0;
    for (auto _point : r_points) {
        if (_point.first != this->x && _point.second != this->y) {
            dist = sqrt(pow(this->x - _point.first, 2) + pow(this->y - _point.second, 2)); 
            nearest_c_points.push(std::make_pair(dist, std::make_pair(_point.first, _point.second)));
        }
    }

    for(auto _point : c_points){
        if(_point.first != this->x && _point.second != this->y){
            dist = sqrt(pow(this->x - _point.first, 2) + pow(this->y - _point.second, 2)); 
            nearest_c_points.push(std::make_pair(dist, std::make_pair(_point.first, _point.second)));
        }
    }

    for(auto _point : b_points){
        if(_point.first != this->x && _point.second != this->y){
            dist = sqrt(pow(this->x - _point.first, 2) + pow(this->y - _point.second, 2)); 
            nearest_b_points.push(std::make_pair(dist, std::make_pair(_point.first, _point.second)));
        }
    }


}



