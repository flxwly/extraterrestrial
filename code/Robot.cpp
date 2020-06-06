//
// Created by flxwl on 05.06.2020.
//

#include "Robot.hpp"

//====================================
//          Constructor
//====================================
Robot::Robot(int *_x, int *_y, int *_comp, int *_sobj_num, int *_sobj_x, int *_sobj_y,
             int *_rc_r, int *_rc_g, int *_rc_b, int *_lc_r, int *_lc_g, int *_lc_b,
             int *_rus, int *_fus, int *_lus,
             int *_whl_l, int *_whl_r, int *_led) {

    Robot::x = _x, Robot::y = _y;                                       // robots position
    Robot::comp = _comp;                                                // compass
    Robot::sobj_num = _sobj_num;                                        // super_object_num
    Robot::sobj_x = _sobj_x, Robot::sobj_y = _sobj_y;                   // last super_object_coords
    Robot::rc[0] = _rc_r, Robot::rc[1] = _rc_g, Robot::rc[2] = _rc_b;   // color sensors right
    Robot::lc[0] = _lc_r, Robot::lc[1] = _lc_g, Robot::lc[2] = _lc_b;   // color sensors left
    Robot::us[0] = _rus, Robot::us[1] = _fus, Robot::us[2] = _lus;      // ultrasonic sensors
    Robot::whl_l = _whl_l, Robot::whl_r = _whl_r;                       // wheels
    Robot::led = _led;                                                  // led for collect and deposit

    Robot::timer = time(NULL);

    Robot::collecting_since = -10;                                      // time var for collect function
    Robot::depoisiting_since = -10;                                     // time var for deposit function
}

//====================================
//          Private Functions
//====================================
void Robot::update_pos() {
    Robot::x += 0, Robot::y += 0; // TODO:
}

bool Robot::should_collect() {

    // if the difference is less or equal to 3 seconds the robot is still collecting;
    if (difftime(Robot::timer, Robot::collecting_since) <= 3)
        return true;

    // The robot is full; the robot cant collect items anyway
    if (Robot::loaded_objects_num >= 6)
        return false;

    // The objects color is Red
    if (isRed()) {
        // Since super objects count as red objects.
        // Only collect red objects if there's space including the chasing super objects
        return Robot::chasing_sobj_num + Robot::loaded_objects[0] < 2;
    } else if (isCyan()) {
        // nothin' special here
        return Robot::loaded_objects[1] < 2;
    } else if (isBlack()) {
        // nothin' special here
        return Robot::loaded_objects[2] < 2;
    }
    // if there's no object beneath the robot, don't try to collect anything
    return false;
}

void Robot::collect() {

    // the robot is already collecting
    if (difftime(Robot::timer, Robot::collecting_since) < 3) {
        // This is to prevent the robot from moving
        Robot::wheels(0, 0);
        *Robot::led = 1;

    }
        // the robot begins to collect
    else {
        // set collecting_since to now
        Robot::collecting_since = Robot::timer;

        // update the loaded_objects vars
        if (isRed() || isSuperObj()) {
            Robot::loaded_objects[0]++;
        } else if (isCyan()) {
            Robot::loaded_objects[1]++;
        } else if (isBlack()) {
            Robot::loaded_objects[2]++;
        }
        Robot::loaded_objects_num++;

        Robot::wheels(0, 0);
        *Robot::led = 1;
    }
}

bool Robot::should_deposit() {

    // while timer - depositing_since < 6the robot is still depositting
    if (difftime(Robot::timer, Robot::depoisiting_since) < 6)
        return true;

    // the robot uses a treshhold to determine if it has enough points so that it is worth it to deposit
    // This treshhold should actually not really matter since the robot is supposed to
    // only drive to the deposit area if it is fully loaded

    // basic points;
    int treshhold = Robot::loaded_objects[0] * 10 + Robot::loaded_objects[1] * 15 + Robot::loaded_objects[2] * 20;

    // one rgb-bonus
    if (Robot::loaded_objects[0] > 0 && Robot::loaded_objects[1] > 0 && Robot::loaded_objects[2] > 0) {
        treshhold += 90;
        // second rgb-bonus
        if (Robot::loaded_objects[0] > 1 && Robot::loaded_objects[1] > 1 && Robot::loaded_objects[2] > 1) {
            treshhold += 90;

        }
    }

    // 145 = 2 red + 1 cyan + 1 black | 20 + 15 + 20 + 90
    return treshhold >= 145;
}

void Robot::deposit() {
    // the robot is already depositing
    if (difftime(Robot::timer, Robot::depoisiting_since) < 6) {
        // This is to prevent the robot from moving
        Robot::wheels(0, 0);
        *Robot::led = 2;

    }
        // the robot begins to deposit
    else {
        // set deposit_since to now
        Robot::depoisiting_since = Robot::timer;

        // update the loaded_objects vars
        Robot::loaded_objects[0] = 0;
        Robot::loaded_objects[1] = 0;
        Robot::loaded_objects[2] = 0;
        Robot::loaded_objects_num = 0;

        Robot::wheels(0, 0);
        *Robot::led = 2;
    }
}

//====================================
//          Public Functions
//====================================
void Robot::wheels(int l, int r) {
    *Robot::whl_l = l, *Robot::whl_r = r;
}

int Robot::move_to(int _x, int _y) {
    // an angle should be created that represent the difference between the point to 0;
    // It should range from -180 to 180 instead of 0 tp 360;
    int angle = vector2Angle(*Robot::x - _x, *Robot::y - _y);

    // Difference between compass
    angle -= *Robot::comp;

    // If the angle is higher then 180 the point is on the other side
    if (abs(angle) > 180) {
        //          -> get the same angle but with another prefix
        angle = (angle + ((angle > 0) ? -360 : 360)) % 360;
    }

    switch (Robot::check_us_sensors(10, 8, 10)) {
        // case 0 means check_us_sensors has detected no near obstacles
        //      -> the robot can move freely
        case 0:
            // the angle to x, y is small so there's no correction of it needed
            //      -> drive straight
            if (abs(angle) < 10) {
                wheels(5, 5);
            }
                // the angle is a bit bigger so the robot needs to make a small correction
            else if (abs(angle) < 30) {
                if (angle < 0) {
                    if (dist(*Robot::x, _x, *Robot::y, _y) < 15) {
                        wheels(2, 1);
                    } else {
                        wheels(5, 4);
                    }
                } else {
                    if (dist(*Robot::x, _x, *Robot::y, _y) < 15) {
                        wheels(1, 2);
                    } else {
                        wheels(4, 5);
                    }
                }
            }
                // the angle is quite big; now a bigger correction is needed
            else if (abs(angle) < 60) {
                if (angle < 0) {
                    if (dist(*Robot::x, _x, *Robot::y, _y) < 15) {
                        wheels(2, -1);
                    } else {
                        wheels(5, 2);
                    }
                } else {
                    if (dist(*Robot::x, _x, *Robot::y, _y) < 15) {
                        wheels(-1, 2);
                    } else {
                        wheels(2, 5);
                    }
                }
            }
                // the angle is very big; a huge correction is needed
                //      -> no forward momentum; spinning
            else {
                if (angle < 0) {
                    wheels(3, -3);
                } else {
                    wheels(-3, 3);
                }
            }
            return 0;

        case 1: // obstacle left
            wheels(-2, -5);
            return 1;
        case 2: // obstacle front
            wheels(-5, -5);
            return 2;
        case 3: // obstacles left & front
            wheels(-2, -5);
            return 3;
        case 4: // obstacle right
            wheels(-5, -2);
            return 4;
        case 5: // obstacles left & right; turning would be fatal; just drive forward
            wheels(3, 3);
            return 5;
        case 6: // obstacles front & right
            wheels(-5, -2);
            return 6;
        case 7: // all sensors see an obstacle -> dead end; Just spinn
            wheels(-5, 5);
            return 7;
        default:
            return -1;
    }
}

int Robot::move_to(std::pair<int, int> p) {
    return Robot::move_to(p.first, p.second);
}

int Robot::check_us_sensors(int l, int f, int r) {
    int sum = 0;
    if (*Robot::us[0] < l)
        sum += 1;
    if (*Robot::us[1] < f)
        sum += 2;
    if (*Robot::us[2] < r)
        sum += 4;
    return sum;
}

void Robot::loop() {
    timer = time(NULL);
    if (Robot::should_deposit() && (isOrangeLeft() || isOrangeRight())) {
        if (isOrange()) {
            Robot::deposit();
        } else if (isOrangeRight()) {
            Robot::wheels(3, 0);
        } else {
            Robot::wheels(0, 3);
        }

    } else if (Robot::should_collect()) {
        Robot::collect();
    } else {
        // avoid trap on the right if objects are loaded
        if (isYellowRight() && Robot::loaded_objects_num > 0) {
            wheels(5, 0);
        }
        // avoid trap on the left
        else if (isYellowLeft() && Robot::loaded_objects_num > 0) {
            wheels(0, 5);
        }
        else {
            switch (Robot::check_us_sensors(8, 12, 8)) {
                // no obstacle
                case 0:
                    // 3 | 3 is standard movement speed in w1
                    wheels(3, 3);
                    break;
                case 1: // obstacle left
                    wheels(-2, -5);
                    break;
                case 2: // obstacle front
                    wheels(-5, -5);
                    break;
                case 3: // obstacles left & front
                    wheels(-2, -5);
                    break;
                case 4: // obstacle right
                    wheels(-5, -2);
                    break;
                case 5: // obstacles left & right; turning would be fatal; just drive forward
                    wheels(3, 3);
                    break;
                case 6: // obstacles front & right
                    wheels(-5, -2);
                    break;
                case 7: // all sensors see an obstacle -> dead end; Just spinn
                    wheels(-5, 5);
                    break;
                default:
                    break;
            }
            *Robot::led = 0;
        }
    }
}
