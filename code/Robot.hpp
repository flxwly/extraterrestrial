#ifndef CSBOT_ROBOT_HPP
#define CSBOT_ROBOT_HPP

#include "libs/PPSettings.hpp"
#include "libs/CommonFunctions.hpp"
#include "libs/ColorRecognition.hpp"
#include "libs/CoSpaceFunctions.hpp"

#include "MapData.hpp"
#include "Pathfinder.hpp"

#include <vector>
#include <ctime>
#include <chrono>
#include <array>


/** The robot of the CoSpace-simulator
 *
*/
class Robot {
public:
	Robot(int *_x, int *_y, int *_compass, int *_superObjectNum, int *_superObjectX, int *_superObjectY,
	      int *_rightColorSensorRed, int *_rightColorSensorGreen, int *_rightColorSensorBlue,
	      int *_leftColorSensorRed, int *_leftColorSensorGreen, int *_leftColorSensorBlue,
	      int *_ultraSonicSensorLeft, int *_ultraSonicSensorFront, int *_ultraSonicSensorRight,
	      int *_wheelLeft, int *_wheelRight, int *_led, int *_tp, int *_gameTime,
	      Field *_map0, Field *_map1);

	//               ______
	//______________/ vars \_____________

	// === Variable pointers to vars updated by the sim ===
	int *posX, *posY;                                             ///< robots position
	int *compass;                                           ///< compass
	int *superObjectNum;                                    ///< super_object_num
	int *superObjectX, *superObjectY;                                ///< last super_object_coords
	std::array<int *, 3> rightColorSensors, leftColorSensors;        ///< color sensors
	std::array<int *, 3> ultraSonicSensors;                            ///< ultrasonic sensors
	int *wheelLeft, *wheelRight;                            ///< wheels
	int *led;                                               ///< led for collect and deposit
	int *tp;                                                ///< where to teleport
	int *gameTime;                                          ///< the in-game time



	std::vector<double> measures = {};



	/// typedef for time (basically a macro)
	typedef std::chrono::steady_clock timer;

	/// the complete path (contains sub-paths from point to point)
	std::vector<Path> completePath;

	/// controls the robots wheels
	void wheels(int l, int r);

	void moveAlongPath(Path &path);

	int moveToPosition(PVector p, bool safety);

	/// checks if l, f or r is higher than the us-sensor vals. returns a binary-encoded value
	int checkUsSensors(int l, int f, int r);

	/// game loop for first map
	void game0Loop();

	/// game loop for second map
	void game1Loop();

private:

	// === Robot vars ===

	Field *map0_, *map1_;                                   ///< Field vars
	Pathfinder pathfinder0_, pathfinder1_;                  ///< Pathfinders that ignore traps
	Pathfinder pathfinder0T_, pathfinder1T_;                ///< Pathfinders that don't ignore traps

	int loadedObjectsNum_;                                  ///< number of objects loaded
	std::array<int, 3> loadedObjects_;                      ///< complete inventory of robot; 0 - rot, 1 - cyan, 2 - black

	PVector nTarget_;                                       ///< pathfinder waypoint chasing
	bool nTargetIsLast_;                                    ///< is nTarget the last element of a path
	int chasingSuperObjNum_;                                ///< the super_objects_num that the robot chases in it's current path

	PVector aPos_;                                          ///< more accurate position of the robot
	PVector lPos_;                                          ///< last coordinates of the robot (for signal loss)
	std::chrono::time_point<timer> lastCycle_;              ///< the time the last cycle was executed
	std::chrono::time_point<timer> depositingSince_;        ///< the time last depositing has started
	std::chrono::time_point<timer> collectingSince_;        ///< the time last collecting has started
	std::chrono::time_point<timer> lastPositionUpdate_;     ///< the last time the position was updated

	//               ___________
	//______________/ functions \_____________

	/// updates the position of the robot mathematically and returns the change
	PVector updatePos();

	/// calculates how long the breaking distance is
	double getBrakingDistance(double friction);

	/// gets the current velocity for a certain change in time (in ms)
	PVector getVelocity(long long int dt);


	/// decides whether collecting a point is a good idea or not
	bool shouldCollect();

	/// collects a point
	int collect();

	/// decides whether depositing is a good idea or not
	bool shouldDeposit();

	/// deposits
	void deposit();

	/// decides whether teleporting is a good idea or not
	bool shouldTeleport();

	/// teleports
	void teleport();

	/// returns a turning direction if the robot is about to drive off map
	int avoidVoid();
};


#endif //CSBOT_ROBOT_HPP
