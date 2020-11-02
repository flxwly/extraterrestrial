//
// Created by flxwl on 05.06.2020.
//

#ifndef EXTRATERRESTRIAL_ROBOT_HPP
#define EXTRATERRESTRIAL_ROBOT_HPP

#include <vector>
#include <ctime>
#include <chrono>
#include <array>
#include "libs/CommonFunctions.hpp"
#include "ColorRecognition.hpp"
#include "MapData.hpp"
#include "Pathfinder.hpp"

#define ROBOT_SPEED 0.03333333               // in milliseconds for wheel_speed = 1
#define ERROR_MESSAGE(MESSAGE) {std::cerr << __FUNCTION__ << "\t" << MESSAGE << std::endl;}

/** The robot of the CoSpace-simulator
 *
 *  @tparam *input[20] a pointer array to all vars of the simulator.
 *  @tparam _map0 a pointer to the first MapData object.
 *	@tparam _map0 a pointer to the second MapData object.
 *
 *
*/
class Robot {
public:
	Robot(int *input[20], Field *_map0, Field *_map1);

	/// typedef for time (basically a macro)
	typedef std::chrono::steady_clock timer;

	/// the complete path (contains sub-paths from point to point)
	std::vector<std::vector<PVector>> completePath;

	/// controls the robots wheels
	void wheels(int l, int r);
	void addWheels(int l, int r);

	/// points the robot towards a point. Safety regulates if the robot should approach the target slowly
	int moveTo(double x, double y, bool safety);
	int moveTo(PVector p, bool safety);

	void moveAlongPath(Path& path);

	/// checks if l, f or r is higher than the us-sensor vals. returns a binary-encoded value
	int checkUsSensors(int l, int f, int r);

	void game0Loop();
	void game1Loop();

	/// returns loaded objects
	std::array<int, 3> getLoadedObjects();

	/// returns the number of the loaded objects
	[[nodiscard]] int getLoadedObjectsNum() const;

private:
	//               ______
	//______________/ vars \_____________

	// === Variable pointers to vars updated by the sim ===
	int *x, *y;                                             ///< robots position
	int *compass;                                           ///< compass
	int *superObjectNum;                                    ///< super_object_num
	int *superObject_x, *superObject_y;                     ///< last super_object_coords
	std::array<int *, 3> rightColorSensors, leftColorSensors;  	///< color sensors
	std::array<int *, 3> ultraSonicSensors;                   	///< ultrasonic sensors
	int *wheelLeft, *wheelRight;                            ///< wheels
	int *led;                                               ///< led for collect and deposit
	int *tp;                                                ///< where to teleport
	int *gameTime;                                          ///< the in-game time

	// === Robot vars ===

	Field* map0_, * map1_;                                   ///< Field vars
	Pathfinder pathfinder0_, pathfinder1_;                  ///< Pathfinders that ignore traps
	Pathfinder pathfinder0T_, pathfinder1T_;                ///< Pathfinders that don't ignore traps

	int loadedObjectsNum_;                                  ///< number of objects loaded
	std::array<int, 3>
		loadedObjects_;                      ///< complete inventory of robot; 0 - rot, 1 - cyan, 2 - black

	PVector nTarget_;                                         ///< pathfinder waypoint chasing
	bool nTargetIsLast_;                                    ///< is nTarget the last element of a path
	int
		chasingSuperObjNum_;                                ///< the super_objects_num that the robot chases in it's current path

	PVector aPos_;                                            ///< more accurate position of the robot
	PVector lPos_;                                            ///< last coordinates of the robot (for signal loss)
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

	/// gets the current velocity for a certain change in time
	PVector getVelocity(double dt);


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


#endif //EXTRATERRESTRIAL_ROBOT_HPP
