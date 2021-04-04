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

class ObjectLoad {
private:
	std::array<std::vector<Collectible *>, 4> m_loadedCollectibles;
	unsigned int m_count;

public:
	ObjectLoad();

	explicit ObjectLoad(const std::vector<Collectible *> &collectibles);

	void addObject(Collectible *collectible);

	bool removeObject(Collectible *collectible);

	void setLoad(const std::vector<Collectible *> &collectible);

	void clearLoad();

	unsigned int getValue();

	unsigned int rgb();

	unsigned int red();

	unsigned int cyan();

	unsigned int black();

	unsigned int super();

	std::array<unsigned int, 4> all();

	const std::array<std::vector<Collectible *>, 4> &loadedObjects();

	[[nodiscard]] unsigned int num() const;
};


/** The robot of the CoSpace-simulator
 *
*/
class Robot {

#pragma region Pointers to Simulator Variables
private:

	// Input (read)
	const int *m_posX, *m_posY;
	int *m_compass;
	int *m_superObjectX, *m_superObjectY;
	std::array<int *, 3> m_leftColorSensor, m_rightColorSensor;
	std::array<int *, 3> m_ultraSonicSensors;
	int *m_gameTime;

	//TODO: 
	// Output (write)
	int *m_wheelLeft, *m_wheelRight;
	int *m_led;
	int *m_tp;

#pragma endregion

#pragma region Public Robot Variables

public:
	unsigned short level;

	PVector pos;
	int compass;
	PVector lastPos;

	std::vector<Path> completePath;
	Field *map0, *map1;
	Pathfinder pathfinder0, pathfinder1;
	Pathfinder pathfinder0T, pathfinder1T;

	std::array<int, 3> ultraSonic;
	HSLColor leftColor, rightColor;
	ObjectLoad loadedObjects;

	unsigned int lastRGBBonus;
	bool huntingSuperObj;
	std::vector<Collectible *> superObjects;

	int remainingMapTime;
	typedef std::chrono::steady_clock Timer;
	std::chrono::time_point<Timer> lastProgramCycle;
	std::chrono::time_point<Timer> depositingSince;
	std::chrono::time_point<Timer> collectingSince;
	std::chrono::time_point<Timer> lastPositionUpdate;

#pragma endregion

#pragma region Constructor

public:
	Robot(const int *x, const int *y, int *compass, int *superObjectX, int *superObjectY,
	      int *leftColorSensorR, int *leftColorSensorG, int *leftColorSensorB,
	      int *rightColorSensorR, int *rightColorSensorG, int *rightColorSensorB,
	      int *ultraSonicSensorLeft, int *ultraSonicSensorFront, int *ultraSonicSensorRight,
	      int *wheelLeft, int *wheelRight, int *led, int *tp, int *gameTime,

	      Field *map0, Field *map1) :

	      m_posX(x), m_posY(y), m_compass(compass),
	      m_superObjectX(superObjectX), m_superObjectY(superObjectY),
	      m_leftColorSensor{leftColorSensorR, leftColorSensorG, leftColorSensorB},
	      m_rightColorSensor{rightColorSensorR, rightColorSensorG, rightColorSensorB},
	      m_ultraSonicSensors{ultraSonicSensorLeft, ultraSonicSensorFront, ultraSonicSensorRight},
	      m_wheelLeft(wheelLeft), m_wheelRight(wheelRight),
	      m_led(led), m_tp(tp), m_gameTime(gameTime)





	      {};

#pragma endregion

	/// controls the robots wheels
	void wheels(int l, int r) const;

	void moveAlongPath(Path &path);

	/// updates the position of the robot mathematically and returns the change
	PVector updatePos();

	int moveToPosition(PVector p);

	/// checks if l, f or r is higher than the us-sensor values. returns a binary-encoded value
	int checkUsSensors(int l, int f, int r);

	void updateLoop();

	/// game loop for first map
	void game0Loop();

	/// game loop for second map
	void game1Loop();


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

	[[nodiscard]] std::array<int, 4> getDesiredLoad() const;

	std::vector<PVector> getPointPath(std::array<int, 4> desiredLoad);
};


#endif //ROBOT_HPP
