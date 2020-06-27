
#ifndef POINT_HPP
#define POINT_HPP

#include "CommonFunctions.hpp"

#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<queue>


class Point {
public:
    Point(std::pair<int, int> *_pos, int _c);

    //void init(const std::vector<Point *>& points);
    double dist(std::pair<int, int> _pos);
    //Point* closest_point(int color);

    std::pair<int, int> pos;        // Position
    int color;                      // Color
    int state;                      // State

private:
    /*struct dist_PRIORITY {
        bool operator()(std::pair<double, Point *> pobj, std::pair<double, Point *> cobj) {
            return pobj.first < cobj.first;
        }
    };*/

    /*std::priority_queue<std::pair<double, Point *>, std::vector<std::pair<double, Point *>>, Point::dist_PRIORITY> r_points;
    std::priority_queue<std::pair<double, Point *>, std::vector<std::pair<double, Point *>>, Point::dist_PRIORITY> c_points;
    std::priority_queue<std::pair<double, Point *>, std::vector<std::pair<double, Point *>>, Point::dist_PRIORITY> b_points;
    std::priority_queue<std::pair<double, Point *>, std::vector<std::pair<double, Point *>>, Point::dist_PRIORITY> all_points;*/
};

#endif // !POINT_HPP