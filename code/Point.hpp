//only for second map
#include"MapData.hpp"
#include "CommonFunctions.hpp"

#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<queue>


//TODO: init Points as objects
//TODO: mayebe nearest points in relate of dp area


class Point {
public:
    Point(int _x, int _y, std::vector<std::pair<int, int>> *dp_areas, unsigned int color);

    double get_dist(int rx, int ry) const;

private:
    int x, y;
    short int color = -1;

    //undef: -1, r: 1, c: 2, b: 3;
    std::pair<int, int> nearest_deposit_area;

    struct dist_PRIORITY;


};

