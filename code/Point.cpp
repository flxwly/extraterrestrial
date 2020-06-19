#include "Point.hpp"



Point::Point(std::pair<int, int> dp_area1, std::pair<int, int> dp_area2, 
                std::vector<std::pair<int, int>> r_points, std::vector<std::pair<int, int>> c_points, 
                std::vector<std::pair<int, int>> b_points){

    //determine nearest deposit area
    double dist1, dist2;
    dist1 = sqrt(pow(this->x - dp_area1.first, 2) + pow(this->y - dp_area1.second, 2));
    dist2 = sqrt(pow(this->x - dp_area2.first, 2) + pow(this->y - dp_area2.second, 2));


    nearest_deposit_area = dist1 <= dist2 ? dp_area1 : dp_area2;
    
    double dist, min_dist, sec_min_dist;
    std::pair<int, int> temp;
    //determine nearest red points
    int i = 1;
    for(std::pair<int, int> _point : r_points){
        dist = sqrt(pow(this->x - _point.first, 2) + pow(this->y - _point.second, 2));
        if(i == 1){
            this->nearest_r_points.push_back(_point);
            min_dist = dist;
        }       
        else if(i==2){
            this->nearest_r_points.push_back(_point);
            sec_min_dist = dist;
        } 
        else{
            if (dist < sec_min_dist)
            {
                if(dist < min_dist){
                    temp = this->nearest_r_points.front();
                    this->nearest_r_points.front() = _point;
                    this->nearest_r_points.back() = temp;
                }
                else{
                    nearest_r_points.back() = _point;
                }
            }
        }        
        i++;
    }

    //determine nearest cyan points
    i = 0;
    for(std::pair<int, int> _point : c_points){
        dist = sqrt(pow(this->x - _point.first, 2) + pow(this->y - _point.second, 2));
        if(i == 1){
            this->nearest_c_points.push_back(_point);
            min_dist = dist;
        }       
        else if(i==2){
            this->nearest_c_points.push_back(_point);
            sec_min_dist = dist;
        } 
        else{
            if (dist < sec_min_dist)
            {
                if(dist < min_dist){
                    temp = this->nearest_c_points.front();
                    this->nearest_c_points.front() = _point;
                    this->nearest_c_points.back() = temp;
                }
                else{
                    nearest_c_points.back() = _point;
                }
            }
        }        
        i++;
    }

    //determine nearest black points
    i = 0;
    for(std::pair<int, int> _point : b_points){
        dist = sqrt(pow(this->x - _point.first, 2) + pow(this->y - _point.second, 2));
        if(i == 1){
            this->nearest_b_points.push_back(_point);
            min_dist = dist;
        }       
        else if(i==2){
            this->nearest_b_points.push_back(_point);
            sec_min_dist = dist;
        } 
        else{
            if (dist < sec_min_dist)
            {
                if(dist < min_dist){
                    temp = this->nearest_b_points.front();
                    this->nearest_b_points.front() = _point;
                    this->nearest_b_points.back() = temp;
                }
                else{
                    nearest_b_points.back() = _point;
                }
            }
        }        
        i++;
    }

}
