#include "Point.hpp"

struct Point::dist_PRIORITY{
    bool operator() (std::pair<int, std::pair<int,int>> fobj, std::pair<int, std::pair<int,int>> sobj, Point *cur){
        return fobj.first < sobj.first;
    }
};


void Point::count_dist_to_robot(const int rx, const int ry){
    this->dist_to_robot = sqrt(pow(this->x - rx, 2) + pow(this->y - ry, 2));
}


Point::Point(int _x, int _y,
                std::pair<int, int> dp_area1, std::pair<int, int> dp_area2, 
                std::vector<std::pair<int, int>> r_points, std::vector<std::pair<int, int>> c_points, 
                std::vector<std::pair<int, int>> b_points){

    this->x = _x;
    this->y = _y;

    //determine nearest deposit area
    double dist1, dist2;
    dist1 = sqrt(pow(this->x - dp_area1.first, 2) + pow(this->y - dp_area1.second, 2));
    dist2 = sqrt(pow(this->x - dp_area2.first, 2) + pow(this->y - dp_area2.second, 2));
    nearest_deposit_area = dist1 <= dist2 ? dp_area1 : dp_area2;
    

    std::priority_queue<std::pair<int, std::pair<int,int>>, std::vector<std::pair<int, std::pair<int,int>>>, Point::dist_PRIORITY> nearest_r_points;
    std::priority_queue<std::pair<int, std::pair<int,int>>, std::vector<std::pair<int, std::pair<int,int>>>, Point::dist_PRIORITY> nearest_c_points;
    std::priority_queue<std::pair<int, std::pair<int,int>>, std::vector<std::pair<int, std::pair<int,int>>>, Point::dist_PRIORITY> nearest_b_points;
 

    int dist = 0;
    for(auto _point : r_points){
        if(_point.first != this->x && _point.second != this->y){
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



