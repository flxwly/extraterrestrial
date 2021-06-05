#ifndef PPSETTINGS_HPP
#define PPSETTINGS_HPP

/// ----------------------------- ///
///       program settings
/// ----------------------------- ///


/// define COLOR_LOGGING to enable color logging to console
//#define COLOR_LOGGING

#define DEBUG_CONSOLE_COLLECTIBLE 1

#define DEBUG_CONSOLE_FIELD 1

#define DEBUG_CONSOLE_NODE 1
#define DEBUG_CONSOLE_PATHFINDER 1

#define DEBUG_CONSOLE_ROBOT 1

#define DEBUG_CONSOLE_MISC 1

/// ---------------------------------
///         SFML settings
/// ---------------------------------

/// define SFML to enable debugging via SFML
//#define SFML

/// define DEBUG_PATHFINDER_NODES
#define DEBUG_PATHFINDER_NODES

/// define DEBUG_PATH to show the robot's path in SFML
//#define DEBUG_PATH

/// define DEBUG_POINTS to show available collectibles in SFML
//#define DEBUG_POINTS




/// ----------------------------- ///
///       some debug logic
/// ----------------------------- ///


// COLLECTIBLE
#pragma region COLLECTIBLE
#if DEBUG_CONSOLE_COLLECTIBLE <= 3
#define COLLECTIBLE_ERROR(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define COLLECTIBLE_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_COLLECTIBLE <= 2
#define COLLECTIBLE_WARNING(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define COLLECTIBLE_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_COLLECTIBLE <= 1
#define COLLECTIBLE_LOG(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define COLLECTIBLE_LOG(MESSAGE) {}
#endif
#pragma endregion

// FIELD
#pragma region FIELD
#if DEBUG_CONSOLE_FIELD <= 3
#define FIELD_ERROR(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define FIELD_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_FIELD <= 2
#define FIELD_WARNING(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define FIELD_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_FIELD <= 1
#define FIELD_LOG(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define FIELD_LOG(MESSAGE) {}
#endif
#pragma endregion

// NODE
#pragma region NODE
#if DEBUG_CONSOLE_NODE <= 3
#define NODE_ERROR(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define NODE_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_NODE <= 2
#define NODE_WARNING(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define NODE_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_NODE <= 1
#define NODE_LOG(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define NODE_LOG(MESSAGE) {}
#endif
#pragma endregion

// PATHFINDER
#pragma region PATHFINDER
#if DEBUG_CONSOLE_PATHFINDER <= 3
#define PATHFINDER_ERROR(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define PATHFINDER_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_PATHFINDER <= 2
#define PATHFINDER_WARNING(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define PATHFINDER_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_PATHFINDER <= 1
#define PATHFINDER_LOG(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define PATHFINDER_LOG(MESSAGE) {}
#endif
#pragma endregion

// ROBOT
#pragma region ROBOT
#if DEBUG_CONSOLE_ROBOT <= 3
#define ROBOT_ERROR(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define ROBOT_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_ROBOT <= 2
#define ROBOT_WARNING(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define ROBOT_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_ROBOT <= 1
#define ROBOT_LOG(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define ROBOT_LOG(MESSAGE) {}
#endif
#pragma endregion

// MISC
#pragma region MISC
#if DEBUG_CONSOLE_MISC <= 3
#define MISC_ERROR(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define MISC_ERROR(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_MISC <= 2
#define MISC_WARNING(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define MISC_WARNING(MESSAGE) {}
#endif
#if DEBUG_CONSOLE_MISC <= 1
#define MISC_LOG(MESSAGE) {std::cout << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#else
#define MISC_LOG(MESSAGE) {}
#endif
#pragma endregion


/// ----------------------------- ///
///         robot settings
/// ----------------------------- ///

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
#define SWAMP_SPEED_PENALITY 10

/// Standard radius of a path in cm
#define PATH_RADIUS 10

/// ----------------------------- ///
///      CoSpace constants
/// ----------------------------- ///

/// Time to deposit in ms
#define DEPOSIT_TIME 6000

/// Time to collect in ms
#define COLLECT_TIME 4000

#endif //PPSETTINGS_HPP
