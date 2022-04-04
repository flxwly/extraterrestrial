#ifndef PPSETTINGS_HPP
#define PPSETTINGS_HPP

#include <iostream>

/// ----------------------------- ///
///       program settings
/// ----------------------------- ///

/// define COLOR_LOGGING to enable color logging to console
//#define COLOR_LOGGING

/// define ROBOT_UPDATE_DUMB to output all in and out vars of the robot
// #define ROBOT_UPDATE_DUMB

#define DEBUG_CONSOLE_COLLECTIBLE 2

#define DEBUG_CONSOLE_FIELD 2
#define DEBUG_CONSOLE_NODE 2

#define DEBUG_CONSOLE_PATHFINDER 1
#define DEBUG_CONSOLE_ROBOT 2

#define DEBUG_CONSOLE_MISC 2

#define MINIMUM_TIME_BETWEEN_CYCLE 5


/// ----------------------------- ///
///       some debug logic
/// ----------------------------- ///

#define GENERAL_LOG(MESSAGE) {std::cout << "log " << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#define GENERAL_WARNING(MESSAGE) {std::cout << "warning " << __FUNCTION__ << "\t" <<  MESSAGE << std::endl;}
#define GENERAL_ERROR(MESSAGE) {std::cout << "error " << __FUNCTION__ << "\t" << MESSAGE << std::endl;}


// COLLECTIBLE
#pragma region COLLECTIBLE
#if DEBUG_CONSOLE_COLLECTIBLE <= 3
   #define COLLECTIBLE_ERROR(MESSAGE) GENERAL_ERROR(MESSAGE)
#else
   #define COLLECTIBLE_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_COLLECTIBLE <= 2
   #define COLLECTIBLE_WARNING(MESSAGE) GENERAL_WARNING(MESSAGE)
#else
   #define COLLECTIBLE_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_COLLECTIBLE <= 1
   #define COLLECTIBLE_LOG(MESSAGE) GENERAL_LOG(MESSAGE)
#else
   #define COLLECTIBLE_LOG(MESSAGE) {}
#endif
#pragma endregion

// FIELD
#pragma region FIELD
#if DEBUG_CONSOLE_FIELD <= 3
   #define FIELD_ERROR(MESSAGE) GENERAL_ERROR(MESSAGE)
#else
   #define FIELD_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_FIELD <= 2
   #define FIELD_WARNING(MESSAGE) GENERAL_WARNING(MESSAGE)
#else
   #define FIELD_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_FIELD <= 1
   #define FIELD_LOG(MESSAGE) GENERAL_LOG(MESSAGE)
#else
   #define FIELD_LOG(MESSAGE) {}
#endif
#pragma endregion

// NODE
#pragma region NODE
#if DEBUG_CONSOLE_NODE <= 3
   #define NODE_ERROR(MESSAGE) GENERAL_ERROR(MESSAGE)
#else
   #define NODE_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_NODE <= 2
   #define NODE_WARNING(MESSAGE) GENERAL_WARNING(MESSAGE)
#else
   #define NODE_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_NODE <= 1
   #define NODE_LOG(MESSAGE) GENERAL_LOG(MESSAGE)
#else
   #define NODE_LOG(MESSAGE) {}
#endif
#pragma endregion

// PATHFINDER
#pragma region PATHFINDER
#if DEBUG_CONSOLE_PATHFINDER <= 3
   #define PATHFINDER_ERROR(MESSAGE) GENERAL_ERROR(MESSAGE)
#else
   #define PATHFINDER_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_PATHFINDER <= 2
   #define PATHFINDER_WARNING(MESSAGE) GENERAL_WARNING(MESSAGE)
#else
   #define PATHFINDER_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_PATHFINDER <= 1
   #define PATHFINDER_LOG(MESSAGE) GENERAL_LOG(MESSAGE)
#else
   #define PATHFINDER_LOG(MESSAGE) {}
#endif
#pragma endregion

// ROBOT
#pragma region ROBOT
#if DEBUG_CONSOLE_ROBOT <= 3
   #define ROBOT_ERROR(MESSAGE) GENERAL_ERROR(MESSAGE)
#else
   #define ROBOT_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_ROBOT <= 2
   #define ROBOT_WARNING(MESSAGE) GENERAL_WARNING(MESSAGE)
#else
   #define ROBOT_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_ROBOT <= 1
   #define ROBOT_LOG(MESSAGE) GENERAL_LOG(MESSAGE)
#else
   #define ROBOT_LOG(MESSAGE) {}
#endif
#pragma endregion

// MISC
#pragma region MISC
#if DEBUG_CONSOLE_MISC <= 3
   #define MISC_ERROR(MESSAGE) GENERAL_ERROR(MESSAGE)
#else
   #define MISC_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_MISC <= 2
   #define MISC_WARNING(MESSAGE) GENERAL_WARNING(MESSAGE)
#else
   #define MISC_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_MISC <= 1
   #define MISC_LOG(MESSAGE) GENERAL_LOG(MESSAGE)
#else
   #define MISC_LOG(MESSAGE) {}
#endif
#pragma endregion


/// ----------------------------- ///
///         robot settings
/// ----------------------------- ///

/// Absolute minimum Distance of Robot to walls and map end
#define ABSOLUT_MIN_DISTANCE 5

/// The speed of the robot in cm/ms for 1 % of maximum speed (wheels(1, 1))
#define ROBOT_SPEED 0.0005
#define BREAKING_COEFFICIENT 0.01

#define RPOS_ERROR_MARGIN 3

/// The distance between the two powered wheels in cm
#define ROBOT_AXLE_LENGTH 12.6

#define RCOLOR_X -3.6
#define RCOLOR_Y 6

#define LCOLOR_X 3.6
#define LCOLOR_Y 6

/// Revolutions per millisecond at 1% of robot's max wheel speed (1/30 rev/ms)
#define REVPERMS 0.00003333

/// X Distance of the right wheel to the robot's center in cm
#define RWHEEL_X -6.3
/// Y Distance of the right wheel to the robot's center in cm
#define RWHEEL_Y -4.2
/// Radius of the right wheel in cm
#define RWHEEL_RADIUS 3


/// X Distance of the left wheel to the robot's center in cm
#define LWHEEL_X 6.3
/// Y Distance of the left wheel to the robot's center in cm
#define LWHEEL_Y -4.2
/// Radius of the left wheel in cm
#define LWHEEL_RADIUS 3


#define COLOR_SENSOR_ANGLE_OFFSET 49.13072561
#define COLOR_SENSOR_DIST_TO_CORE 6.99714227

#define GAME0_TIME 180
#define GAME1_TIME 300

/// The factor by what speed is reduced in swamps TODO
#define SWAMP_SPEED_PENALTY 10

/// The maximum force the robot is allowed to steer with
#define MAX_STEERING_FORCE 10

/// Standard radius of a path in cm
#define PATH_RADIUS 10

/// Maximum number of loaded objects at a time
#define MAX_LOADED_OBJECTS 6

#define ULTRASONIC_SENSOR_DIST_TO_CORE_F 6.5
#define ULTRASONIC_SENSOR_ANGLE_OFFSET 45
#define ULTRASONIC_SENSOR_DIST_TO_CORE 6.40312


/// ----------------------------- ///
///      CoSpace constants
/// ----------------------------- ///

/// Time to deposit in ms
#define DEPOSIT_TIME 6000

/// Time to collect in ms
#define COLLECT_TIME 4000

/// Map sizes
#define REAL_GAME0MAP_WIDTH 270
#define REAL_GAME0MAP_HEIGHT 180
#define REAL_GAME1MAP_WIDTH 360
#define REAL_GAME1MAP_HEIGHT 270

#endif //PPSETTINGS_HPP
