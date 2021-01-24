#ifndef CSBOT_PPSETTINGS_HPP
#define CSBOT_PPSETTINGS_HPP

/// ----------------------------- ///
///       program settings
/// ----------------------------- ///

/// define DEBUG to enable debug messages
//#define DEBUG

/// define SFML to enable debugging via SFML
#define SFML

#ifdef SFML
#undef DEBUG
#endif

#ifdef DEBUG
#define ERROR_MESSAGE(MESSAGE) {std::cerr << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#endif

#ifndef DEBUG
#define ERROR_MESSAGE(message) {}
#endif


/// ----------------------------- ///
///         robot settings
/// ----------------------------- ///

// TODO measure predefined constants

/// The speed of the robot in cm/ms for 20 % of maximum speed
#define ROBOT_SPEED 0.01
/// The distance between the two powered wheels in cm
#define ROBOT_AXLE_LENGTH 12.6


#define COLOR_SENSOR_ANGLE_OFFSET 3.5
#define COLOR_SENSOR_DIST_TO_CORE 5

/// The factor by what speed is reduced in swamps
#define SWAMP_SPEED_PENALITY 10

/// Standard radius of a path in cm
#define PATH_RADIUS 10


#endif //CSBOT_PPSETTINGS_HPP
