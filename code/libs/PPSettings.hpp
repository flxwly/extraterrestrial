#ifndef PPSETTINGS_HPP
#define PPSETTINGS_HPP

/// ----------------------------- ///
///       program settings
/// ----------------------------- ///

/// define DEBUG to enable debug messages
//#define DEBUG

/// define SFML to enable debugging via SFML
#define SFML

/// define COLOR_LOGGING to enable color logging to console
//#define COLOR_LOGGING

#ifdef SFML
#undef DEBUG
#endif

#ifdef DEBUG
/// DEBUG is defined so ERROR_MESSAGE prints out the function it is called in joined by a message
#define ERROR_MESSAGE(MESSAGE) {std::cerr << __FUNCTION__ << "\t" << MESSAGE << std::endl;}
#endif

#ifndef DEBUG
/// DEBUG is not defined so ERROR_MESSAGE does nothing
#define ERROR_MESSAGE(message) {}
#endif


/// ----------------------------- ///
///         robot settings
/// ----------------------------- ///

/// The speed of the robot in cm/ms for 20 % of maximum speed
#define ROBOT_SPEED 0.01

#define RPOS_ERROR_MARGIN 3

/// The distance between the two powered wheels in cm
#define ROBOT_AXLE_LENGTH 12.6

#define RCOLOR_X -3.6
#define RCOLOR_Y 6

#define LCOLOR_X 3.6
#define LCOLOR_Y 6

#define RWHEEL_X -6.3
#define RWHEEL_Y -4.2
#define RWHEEL_RADIUS 3


#define LWHEEL_X 6.3
#define LWHEEL_Y -4.2
#define LWHEEL_RADIUS 3


#define COLOR_SENSOR_ANGLE_OFFSET 49.13072561
#define COLOR_SENSOR_DIST_TO_CORE 6.99714227

#define GAME0_TIME 180
#define GAME1_TIME 300

/// The factor by what speed is reduced in swamps TODO
#define SWAMP_SPEED_PENALITY 100

/// Standard radius of a path in cm
#define PATH_RADIUS 10


#endif //PPSETTINGS_HPP
