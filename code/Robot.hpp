#ifndef ROBOT_HPP
#define ROBOT_HPP

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
	      int *_ultraSonicSensorLeft, int *_ultraSonicSensorFront, int *_ultraSonicSensorRight,
	      int *_wheelLeft, int *_wheelRight, int *_led, int *_tp, int *_gameTime,
	      Field *_map0, Field *_map1);

	//               ______
	//______________/ vars \_____________

	// === Variable pointers to vars updated by the sim ===
	int *posX, *posY;
	int *compass;
	int *superObjectNum;
	int *superObjectX, *superObjectY;
	std::vector<PVector> superObjects;
	HSLColor leftColor, rightColor;
	std::array<int *, 3> ultraSonicSensors;
	int *wheelLeft, *wheelRight;
	int *led;
	int *tp;
	int *gameTime;

	/// typedef for time (basically a macro)
	typedef std::chrono::steady_clock timer;

	/// the complete path (contains sub-paths from point to point)
	std::vector<Path> completePath;

	/// controls the robots wheels
	void wheels(int l, int r) const;

	void moveAlongPath(Path &path, bool trapSensitive);

    /// updates the position of the robot mathematically and returns the change
    PVector updatePos();

	int moveToPosition(PVector p, bool safety);

	/// checks if l, f or r is higher than the us-sensor vals. returns a binary-encoded value
	int checkUsSensors(int l, int f, int r);

	void updateLoop();

	/// game loop for first map
	void game0Loop();

	/// game loop for second map
	void game1Loop();

    PVector nTarget_;                                       ///< pathfinder waypoint chasing
    PVector aPos_;                                          ///< more accurate position of the robot

public:

	// === Robot vars ===

	Field *map0_, *map1_;                                   ///< field vars
	Pathfinder pathfinder0_, pathfinder1_;                  ///< Pathfinders that ignore traps
	Pathfinder pathfinder0T_, pathfinder1T_;                ///< Pathfinders that don't ignore traps
private:
	int loadedObjectsNum_;                                  ///< number of objects loaded
	std::array<int, 3> loadedObjects_;                      ///< complete inventory of robot; 0 - rot, 1 - cyan, 2 - black

	int chasingSuperObjNum_;                                ///< the super_objects_num that the robot chases in it's current path


	PVector lPos_;                                          ///< last coordinates of the robot (for signal loss)
	std::chrono::time_point<timer> lastCycle_;              ///< the time the last cycle was executed
	std::chrono::time_point<timer> depositingSince_;        ///< the time last depositing has started
	std::chrono::time_point<timer> collectingSince_;        ///< the time last collecting has started
	std::chrono::time_point<timer> lastPositionUpdate_;     ///< the last time the position was updated

	//               ___________
	//______________/ functions \_____________

	/// gets the current velocity for a certain change in time (in ms)
	[[nodiscard]] PVector getVelocity(long long int dt) const;


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
	[[nodiscard]] int avoidVoid() const;
};


#endif //ROBOT_HPP
