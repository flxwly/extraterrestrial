//
// Created by flxwl on 05.06.2020.
//

#ifndef EXTRATERRESTRIAL_ROBOT_HPP
#define EXTRATERRESTRIAL_ROBOT_HPP

#include <vector>
#include <ctime>
#include <chrono>
#include <array>
#include "CommonFunctions.hpp"
#include "ColorRecognition.hpp"
#include "MapData.hpp"
#include "Pathfinding.hpp"

class Robot {
public:
    // constructor
    Robot(int *_x, int *_y, int *_comp, int *_sobj_num, int *_sobj_x, int *_sobj_y,
          int *_rc_r, int *_rc_g, int *_rc_b, int *_lc_r, int *_lc_g, int *_lc_b,
          int *_rus, int *_fus, int *_lus,
          int *_whl_l, int *_whl_r, int *_led, int *_tp, int *_g_time,
          MapData *_map0, MapData *_map1, AStar *_pathfinder0, AStar *_pathfinder1);


    std::vector<std::vector<std::pair<int, int>>> complete_path;

    // public functions
    void wheels(int l, int r);

    int move_to(int x, int y, bool safety);

    int move_to(std::pair<int, int> p, bool safety);

    int check_us_sensors(int l, int f, int r);

    void game_0_loop();

    void game_1_loop();

private:
    //               ______
    //______________/ Vars \_____________

    // === Variable pointers to vars updated by the sim ===
    int *x, *y;                                 // robots position
    int *comp;                                  // compass
    int *sobj_num;                              // super_object_num
    int *sobj_x, *sobj_y;                       // last super_object_coords
    std::array<int *, 3> rc{}, lc{};             // color sensors
    std::array<int *, 3> us{};                   // ultrasonic sensors
    int *whl_l, *whl_r;                         // wheels
    int *led;                                   // led for collect and deposit
    int *tp;                                    // where to teleport
    int *g_time;                                // game_time var

    // === Robot vars ===

    int l_x, l_y;                               // last coordinates of the robot (for signal loss)

    MapData *map0, *map1;
    AStar *pathfinder0, *pathfinder1;

    int loaded_objects_num;                     // number of objects loaded
    std::array<int, 3> loaded_objects;        // complete inventory of robot


    std::pair<int, int> n_target;               // pathfinder waypoint chasing
    bool n_target_is_last;                      // is n_target the last element of a path

    int chasing_sobj_num;                       // the super_objects_num that the robot chases in it's current path

    //               ___________
    //______________/ functions \_____________

    // function to update the position based on speed and time
    void update_pos();

    // typedef for time (basically a macro)
    typedef std::chrono::steady_clock timer;

    std::chrono::time_point<std::chrono::steady_clock> collecting_since;

    bool should_collect();

    void collect();

    std::chrono::time_point<std::chrono::steady_clock> depositing_since;

    bool should_deposit();

    void deposit();

    bool should_teleport();

    void teleport();

    int avoid_void();

    std::vector<std::pair<int, int>> get_points(MapData &mapData);
};


#endif //EXTRATERRESTRIAL_ROBOT_HPP
