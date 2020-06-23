//only for second map
#include"MapData.hpp"

#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<queue>


//TODO: init Points as objects
//TODO: mayebe nearest points in relate of dp area


class Point
{
public:
    Point(int _x, int _y,
            std::pair<int, int> dp_area1, std::pair<int, int> dp_area2, 
            std::vector<std::pair<int, int>> r_points, std::vector<std::pair<int, int>> c_points, 
            std::vector<std::pair<int, int>> b_points);

    double dist_to_robot;
    void count_dist_to_robot(const int rx, const int ry);
            
private:
    int x, y;
    
    int color = -1;
    //undef: -1, r: 1, c: 2, b: 3;
    std::pair<int, int> nearest_deposit_area;

    struct dist_PRIORITY;




};

