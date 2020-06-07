//
// Created by flxwl on 05.06.2020.
//

#ifndef EXTRATERRESTRIAL_ROBOT_HPP
#define EXTRATERRESTRIAL_ROBOT_HPP

#include <vector>
#include <ctime>
#include "CommonFunctions.hpp"
#include "ColorRecognition.hpp"

class Robot {
public:
    Robot(int *_x, int *_y, int *_comp, int *_sobj_num, int *_sobj_x, int *_sobj_y,
          int *_rc_r, int *_rc_g, int *_rc_b, int *_lc_r, int *_lc_g, int *_lc_b,
          int *_rus, int *_fus, int *_lus,
          int *_whl_l, int *_whl_r, int *_led); // constructor

    void wheels(int l, int r);
    int move_to(std::pair<int, int> p);
    int move_to(int x, int y);
    int check_us_sensors(int l, int f, int r);

    void loop0();

private:
    // === Variable pointers to vars updated by the sim ===
    int *x, *y;             // robots position
    int *comp;              // compass
    int *sobj_num;          // super_object_num
    int *sobj_x, *sobj_y;   // last super_object_coords
    int *rc[3], *lc[3];     // color sensors
    int *us[3];             // ultrasonic sensors
    int *whl_l, *whl_r;     // wheels
    int *led;               // led for collect and deposit

    time_t timer;               // global time var

    // Robot vars
    int loaded_objects_num; // number of objects loaded
    int loaded_objects[3];  // complete inventory of robot

    int chasing_sobj_num; // the super_objects_num that the robot chases in it's current path

    void update_pos();      // whenever the position is lost, use this method to update the position based on math

    time_t collecting_since;// collecting vars and functions
    bool should_collect();
    void collect();

    time_t depositing_since;// depositing vars and functions
    bool should_deposit();
    void deposit();
};


#endif //EXTRATERRESTRIAL_ROBOT_HPP
