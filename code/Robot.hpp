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

    // Pointer to Global input and output variables
    volatile int **AI_GLOBAL_IN{nullptr}, **AI_GLOBAL_OUT{nullptr};

	// Direct Input/Output to the Sim
	volatile int *AI_IN{nullptr}, *AI_OUT{nullptr};
	std::array<int *, 3> SUPER_OBJECT{nullptr, nullptr, nullptr};
	int *TELEPORT = nullptr;

	bool setIN(volatile int **IN);
	bool setOUT(volatile int **OUT);
	void updateSimVars();

	// Input (read)
	PVector simPos;
	int compass{0};
	PVector superObject{0, 0};
	int superObjectNum{0};
	HSLColor leftColor{0, 0, 0}, rightColor{0, 0, 0};
	std::array<int, 3> ultraSonic{0, 0, 0};
	int gameTime{0};
    int level = 0;

	// Output (write)
	int wheelLeft{0}, wheelRight{0};
	int led{0};
	int tp{0};

#pragma endregion

#pragma region Public Robot Variables

public:

	PVector pos{0, 0};
	PVector lastPos{0, 0};

	std::vector<Path> completePath{};
	Field *map0, *map1;
	Pathfinder pathfinder0, pathfinder1;
	Pathfinder pathfinder0T, pathfinder1T;

	ObjectLoad loadedObjects;

	unsigned int lastRGBBonus{0};
	bool huntingSuperObj{false};
	std::vector<Collectible *> superObjects;

	int remainingMapTime{0};
	typedef std::chrono::steady_clock Timer;
	std::chrono::time_point<Timer> lastProgramCycle;
	std::chrono::time_point<Timer> depositingSince;
	std::chrono::time_point<Timer> collectingSince;
	std::chrono::time_point<Timer> lastPositionUpdate;

#pragma endregion

#pragma region Constructor

public:

	Robot(volatile int **IN, volatile int **OUT, std::array<int *, 3> superObject, int *teleport, Field *map0, Field *map1);

#pragma endregion

	/// controls the robots wheels
	void wheels(int l, int r);

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
