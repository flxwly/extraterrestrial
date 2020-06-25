//only for second map
#include "MapData.hpp"
#include "CommonFunctions.hpp"

#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<queue>


//TODO: init Points as objects
//TODO: mayebe nearest points in relate of dp area

std::vector<Point> game_0_points;
std::vector<Point> game_1_points;

void init_points_game_1();
void init_points_game_0();


class Point {
public:
    Point(int _x, int _y, std::vector<std::pair<int, int>> *_dp_areas, int _color);

    void calculate_closest_points(std::vector<Point *> *_points);
    double calculate_dist_to_point(const int &x1, const int &y1);

private:
    int x, y;

    //undef: -1, r: 1, c: 2, b: 3;
    int color;

    std::pair<int, int> closest_deposit_area;

    struct dist_PRIORITY {
        bool operator()(std::pair<double, Point *> pobj, std::pair<double, Point *> cobj) {
            return pobj.first < cobj.first;
        }
    };

    std::priority_queue<std::pair<double, Point *>, std::vector<std::pair<double, Point *>>, Point::dist_PRIORITY> r_points;
    std::priority_queue<std::pair<double, Point *>, std::vector<std::pair<double, Point *>>, Point::dist_PRIORITY> c_points;
    std::priority_queue<std::pair<double, Point *>, std::vector<std::pair<double, Point *>>, Point::dist_PRIORITY> b_points;


};

