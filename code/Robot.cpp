#include "Robot.hpp"

#include <utility>



/* ==============================================    Univers    =======================================================
 *
 *
 *   ,ad8888ba,    88              88                                   88                                             88
 *  d8"'    `"8b   88              ""                            ,d     88                                             88
 * d8'        `8b  88                                            88     88                                             88
 * 88          88  88,dPPYba,      88   ,adPPYba,   ,adPPYba,  MM88MMM  88            ,adPPYba,   ,adPPYYba,   ,adPPYb,88
 * 88          88  88P'    "8a     88  a8P_____88  a8"     ""    88     88           a8"     "8a  ""     `Y8  a8"    `Y88
 * Y8,        ,8P  88       d8     88  8PP"""""""  8b            88     88           8b       d8  ,adPPPPP88  8b       88
 *  Y8a.    .a8P   88b,   ,a8"     88  "8b,   ,aa  "8a,   ,aa    88,    88           "8a,   ,a8"  88,    ,88  "8a,   ,d88
 *   `"Y8888Y"'    8Y"Ybbd8"'      88   `"Ybbd8"'   `"Ybbd8"'    "Y888  88888888888   `"YbbdP"'   `"8bbdP"Y8   `"8bbdP"Y8
 *                                ,88
 *                              888P"
 *
 * ==================================================================================================================== */

#pragma region Code

// Parameterless constructor
ObjectLoad::ObjectLoad() : m_count(0), m_loadedCollectibles() {}

ObjectLoad::ObjectLoad(const std::vector<Collectible *> &objects) : m_count(objects.size()) {
	for (auto object : objects) {
		m_loadedCollectibles[object->color].push_back(object);
	}
}

void ObjectLoad::addObject(Collectible *object) {
	m_loadedCollectibles[object->color].push_back(object);
	m_count++;
}

bool ObjectLoad::removeObject(Collectible *object) {
	auto it = find(m_loadedCollectibles[object->color].begin(), m_loadedCollectibles[object->color].end(), object);
	if (it != m_loadedCollectibles[object->color].end()) {
		m_loadedCollectibles[object->color].erase(it);
		m_count--;
		return true;
	}

	return false;
}

void ObjectLoad::setLoad(const std::vector<Collectible *> &objects) {
	m_loadedCollectibles = {};
	for (auto object : objects) {
		addObject(object);
	}
}

void ObjectLoad::clearLoad() {
	m_loadedCollectibles = {};
	m_count = 0;
}

unsigned int ObjectLoad::rgb() {
	return std::min(red(), std::min(cyan(), black()));
}

unsigned int ObjectLoad::getValue() {
	unsigned int value = 0;

	for (auto red : m_loadedCollectibles[0]) {
		value += (red->isWorthDouble) ? 20 : 10;
	}

	for (auto cyan : m_loadedCollectibles[1]) {
		value += (cyan->isWorthDouble) ? 30 : 15;
	}

	for (auto black : m_loadedCollectibles[2]) {
		value += (black->isWorthDouble) ? 40 : 20;
	}

	for (auto super : m_loadedCollectibles[3]) {
		value += (super->isWorthDouble) ? 180 : 90;
	}

	value += rgb() * 90;

	return value;
}

unsigned int ObjectLoad::red() {
	return m_loadedCollectibles[0].size();
}

unsigned int ObjectLoad::cyan() {
	return m_loadedCollectibles[1].size();
}

unsigned int ObjectLoad::black() {
	return m_loadedCollectibles[2].size();
}

unsigned int ObjectLoad::super() {
	return m_loadedCollectibles[3].size();
}

std::array<unsigned int, 4> ObjectLoad::all() {
	return {red(), cyan(), black(), super()};
}

const std::array<std::vector<Collectible *>, 4> &ObjectLoad::loadedObjects() {
	return m_loadedCollectibles;
}

unsigned int ObjectLoad::num() const {
	return m_count;
}

#pragma endregion

/* =============================================================
 *
 * 88888888ba               88
 * 88      "8b              88                         ,d
 * 88      ,8P              88                         88
 * 88aaaaaa8P'  ,adPPYba,   88,dPPYba,    ,adPPYba,  MM88MMM
 * 88""""88'   a8"     "8a  88P'    "8a  a8"     "8a   88
 * 88    `8b   8b       d8  88       d8  8b       d8   88
 * 88     `8b  "8a,   ,a8"  88b,   ,a8"  "8a,   ,a8"   88,
 * 88      `8b  `"YbbdP"'   8Y"Ybbd8"'    `"YbbdP"'    "Y888
 *
 * ============================================================= */

#pragma region Code

//====================================
//          Constructor
//====================================


Robot::Robot(volatile int **IN, volatile int **OUT, std::array<int *, 3> superObject, Field *map0, Field *map1) :
		pathfinder0(*map0, false), pathfinder1(*map1, false),
		pathfinder0T(*map0, true), pathfinder1T(*map1, true),
		map0(map0), map1(map1), SUPER_OBJECT(superObject) {

	setIN(IN);
	setOUT(OUT);
}


bool Robot::setIN(volatile int **IN) {
	if (IN) {
		AI_GLOBAL_IN = IN;
		return true;
	} else {
		ERROR_MESSAGE("Input pointer is nullptr")
		return false;
	}
}

bool Robot::setOUT(volatile int **OUT) {
	if (OUT) {
		AI_GLOBAL_OUT = OUT;
		return true;
	} else {
		ERROR_MESSAGE("Output pointer is nullptr")
		return false;
	}
}

void Robot::updateSimVars() {

    AI_IN = *AI_GLOBAL_IN;
    AI_OUT = *AI_GLOBAL_OUT;

    std::cout << "In: "<< AI_IN[3] << std::endl;
    std::cout << "Out: "<< AI_OUT[1] << std::endl;

    // Input vars
	simPos.set(
			(AI_IN[9] != 0) ? static_cast<float>(AI_IN[9]) : NAN,
			(AI_IN[10] != 0) ? static_cast<float>(AI_IN[10]) : NAN);

	compass = AI_IN[12];
	superObject.set(*SUPER_OBJECT[0], *SUPER_OBJECT[1]);
	superObjectNum = *SUPER_OBJECT[2];

	leftColor = rgb2hsl({
		static_cast<float>(AI_IN[3]),
		static_cast<float>(AI_IN[4]),
		static_cast<float>(AI_IN[5])});

	rightColor = rgb2hsl({
		static_cast<float>(AI_IN[6]),
		static_cast<float>(AI_IN[7]),
		static_cast<float>(AI_IN[8])});

	ultraSonic[1] = AI_IN[0];
	ultraSonic[0] = AI_IN[1];
	ultraSonic[2] = AI_IN[2];
	gameTime = AI_IN[12];

	// Output vars
	AI_OUT[0] = wheelLeft;
	AI_OUT[1] = wheelRight;
	AI_OUT[2] = led;
	AI_OUT[3] = tp;
}

//====================================
//     position/update Methods
//====================================

PVector Robot::getVelocity(long long int dt) const {

	// For clarification on how this works see
	// https://math.stackexchange.com/questions/3962859/calculate-path-of-vehicle-with-two-wheels-parallel-to-each-other

	double penalty = (isSwamp(leftColor) || isSwamp(rightColor)) ? SWAMP_SPEED_PENALITY : 1;

	if (wheelLeft == wheelRight) {
		return geometry::angle2Vector(toRadians(compass + 90)) *
		       (static_cast<double>(wheelLeft) * ROBOT_SPEED / penalty) * static_cast<double>(dt);
	}

	double v1 = static_cast<double>(wheelLeft) * ROBOT_SPEED / penalty;
	double v2 = static_cast<double>(wheelRight) * ROBOT_SPEED / penalty;

	double s = (ROBOT_AXLE_LENGTH * (v1 + v2)) / (2 * (v1 - v2));

	PVector vel = (v1 != 0) ?
	              PVector(s * cos(v1 * static_cast<double> (dt) / (ROBOT_AXLE_LENGTH / 2 + s)) - s,
	                      s * sin(v1 * static_cast<double> (dt) / (ROBOT_AXLE_LENGTH / 2 + s))) :

	              PVector(s * cos(v2 * static_cast<double> (dt) / (ROBOT_AXLE_LENGTH / 2 - s)) - s,
	                      s * sin(v2 * static_cast<double> (dt) / (ROBOT_AXLE_LENGTH / 2 - s)));

	vel.rotate(toRadians(compass + 90));

	return vel;
}

PVector Robot::updatePos() {

	// check if robot is in signal lost zone
	if (simPos) {

		pos += getVelocity(std::chrono::duration_cast<std::chrono::milliseconds>(
				Timer::now() - lastPositionUpdate).count());

		simPos = pos;

	} else {

		if (geometry::dist(pos, simPos) > RPOS_ERROR_MARGIN) {
			pos = simPos;
		} else {
			pos = pos + getVelocity(std::chrono::duration_cast<std::chrono::milliseconds>(
					Timer::now() - lastPositionUpdate).count());
		}
	}

	lastPositionUpdate = Timer::now();

	// return change
	return pos - lastPos;
}

//====================================
//        collect Methods
//====================================
bool Robot::shouldCollect() {

	// if the difference is less or equal to 3.5 seconds the robot is still collecting;
	if (std::chrono::duration_cast<std::chrono::milliseconds>(Timer::now() - collectingSince).count() <= 3500)
		return true;

	// The robot is full; the robot cant collect items anyway
	if (loadedObjects.num() >= 6)
		return false;

	// The objects color is Red
	if (isRed(leftColor) || isRed(rightColor)) {
		return loadedObjects.red() < getDesiredLoad()[0];
	} else if (isCyan(leftColor) || isCyan(rightColor)) {
		// nothin' special here
		return loadedObjects.cyan() < getDesiredLoad()[1];
	} else if (isBlack(leftColor) || isBlack(rightColor)) {
		// nothin' special here
		return loadedObjects.black() < getDesiredLoad()[2];
	} else if (isSuperObj(leftColor) || isSuperObj(rightColor)) {
		return loadedObjects.super() < getDesiredLoad()[3];
	}
	// if there's no object beneath the robot, don't try to collect anything
	return false;
}

int Robot::collect() {

	// the robot is already collecting
	if (std::chrono::duration_cast<std::chrono::milliseconds>(Timer::now() - collectingSince).count() <= 4000) {
		// This is to prevent the robot from moving
		wheels(0, 0);
		led = 1;
		return -1;
	}
		// the robot begins to collect
	else {
		// set collectingSince to now
		collectingSince = Robot::Timer::now();

		if (isRed(leftColor) || isRed(rightColor)) {
			return 0;
		} else if (isCyan(leftColor) || isCyan(rightColor)) {
			return 1;
		} else if (isBlack(leftColor) || isBlack(rightColor)) {
			return 2;
		} else if (isSuperObj(leftColor) || isSuperObj(rightColor)) {
			return 3;
		}
	}
	return -1;
}

//====================================
//        deposit Methods
//====================================
bool Robot::shouldDeposit() {

	// while timer - depositingSince < 6the robot is still depositting
	if (std::chrono::duration_cast<std::chrono::milliseconds>(Timer::now() - depositingSince).count() <= 4000)
		return true;

	// 145 = 2 red + 1 cyan + 1 black | 20 + 15 + 20 + 90
	return loadedObjects.getValue() >= 145 || remainingMapTime < 60 || loadedObjects.num() >= 6;
}

void Robot::deposit() {
	// the robot is already depositing
	if (std::chrono::duration_cast<std::chrono::milliseconds>(Timer::now() - depositingSince).count() <= 5000) {
		// This is to prevent the robot from moving
		wheels(0, 0);
		led = 2;

	}
		// the robot begins to deposit
	else {
		// set deposit_since to now
		depositingSince = Timer::now();

		// update the loadedObjects vars
		lastRGBBonus = loadedObjects.rgb();
		loadedObjects.clearLoad();

		wheels(0, 0);
		led = 2;
	}
}

//====================================
//       teleport Methods
//====================================
bool Robot::shouldTeleport() {
	// for ICool Challange because there's only one world
	// while in deposit_area don't teleport
	if (isOrange(leftColor) && isOrange(rightColor)) {
		return false;
	}

	// earliest possible teleport after 3min (180sec)
	//      only teleport if it has nothing to lose -> no objects loaded
	if (remainingMapTime < 0) {
		if (loadedObjects.num() == 0) {
			return true;
		}
	}
	// next possible teleport after 3min and 20sec (200sec)
	//      only teleport if points are more valuable then 145 points
	if (remainingMapTime < -20) {
		if (!shouldDeposit()) {
			return true;
		}
	}

	// last possible teleport after 4min (240sec)
	//      teleport in any case
	return remainingMapTime < -60;
}

void Robot::teleport() {
	loadedObjects.clearLoad();

	tp = 1;
	level = 1;
}

//====================================
//       movement Methods
//====================================
int Robot::avoidVoid() const {

	//Right END
	if (pos.x >= 350) {
		if (compass > 270 && compass <= 360)
			return -1;
		else if (compass > 180 && compass <= 270)
			return 1;
	}

	// LEFT END
	if (pos.x <= 10) {
		if (compass > 0 && compass <= 90)
			return 1;
		else if (compass > 90 && compass <= 180)
			return -1;
	}

	//TOP END
	if (pos.y >= 260) {
		if (compass > 270 && compass <= 360)
			return 1;
		else if (compass >= 0 && compass <= 90)
			return -1;
	}

	//BOTTOM END
	if (pos.y <= 10) {
		if (compass > 180 && compass <= 270)
			return -1;
		else if (compass < 180 && compass >= 90)
			return 1;
	}
	return 0;
}

void Robot::wheels(int l, int r) {
	wheelLeft = l * 20;
	wheelRight = r * 20;
}

int Robot::moveToPosition(PVector p) {

	double dist = geometry::dist(pos, p);

	// an angle should be created that represent the difference between the point to 0;
	// It should range from -180 to 180 instead of 0 tp 360;
	double angle = toDegrees(geometry::vector2Angle(p - pos)) - 90;

	// Difference between compass
	angle -= compass;

	// If the angle is higher then 180 the point is on the other side
	if (fabs(angle) > 180) {
		//          -> get the same angle but with another prefix
		angle = fmod(angle + ((angle > 0) ? -360 : 360), 360);
	}

	switch (checkUsSensors(10, 8, 10)) {
		// case 0 means checkUsSensors has detected no near obstacles
		//      -> the robot can move freely
		case 0:
			// the angle to posX, posY is small so there's no correction of it needed
			//      -> drive straight
			if (fabs(angle) < 8) {

				if (angle < 0) {
					if (dist < 8) {
						wheels(1, 0);
					} else {
						wheels(4, 4);
					}
				} else {
					if (dist < 8) {
						wheels(0, 1);
					} else {
						wheels(4, 4);
					}
				}

			}
				// the angle is a bit bigger so the robot needs to make a small correction
			else if (fabs(angle) < 20) {
				if (angle < 0) {
					if (dist < 8) {
						wheels(2, 1);
					} else {
						wheels(5, 4);
					}
				} else {
					if (dist < 8) {
						wheels(1, 2);
					} else {
						wheels(4, 5);
					}
				}
			}
				// the angle is quite big; now a bigger correction is needed
			else if (fabs(angle) < 60) {
				if (angle < 0) {
					if (dist < 8) {
						wheels(2, -2);
					} else {
						wheels(5, 2);
					}
				} else {
					if (dist < 8) {
						wheels(-2, 2);
					} else {
						wheels(2, 5);
					}
				}
			}
				// the angle is very big; a huge correction is needed
				//      -> no forward momentum; spinning
			else {

				if (angle < 0) {
					if (dist < 8) {
						wheels(-2, -2);
					} else {
						wheels(3, -3);
					}
				} else {
					if (dist < 8) {
						wheels(-2, -2);
					} else {
						wheels(-3, 3);
					}
				}
			}
			return 0;

		case 1: // obstacle left
			wheels(-2, -5);
			return 1;
		case 2: // obstacle front
			wheels(-5, -5);
			return 2;
		case 3: // obstacles left & front
			wheels(-2, -5);
			return 3;
		case 4: // obstacle right
			wheels(-5, -2);
			return 4;
		case 5: // obstacles left & right; turning would be fatal; just drive forward
			wheels(3, 3);
			return 5;
		case 6: // obstacles front & right
			wheels(-5, -2);
			return 6;
		case 7: // all sensors see an obstacle -> dead end; Just spinn
			wheels(-5, 4);
			return 7;
		default:
			return -1;
	}
}

void Robot::moveAlongPath(Path &path) {

	int movement = moveToPosition(path.getClosestNormalPoint(pos, 10));

	if (movement == -1) {
		std::cout << "No valid motion found" << std::endl;
	}

}

//====================================
//        Sensor Methods
//====================================
int Robot::checkUsSensors(int l, int f, int r) {
	int sum = 0;

	if (ultraSonic[0] < l)
		sum += 1;
	if (ultraSonic[1] < f)
		sum += 2;
	if (ultraSonic[2] < r)
		sum += 4;
	return sum;
}


std::array<int, 4> Robot::getDesiredLoad() const {
	if (huntingSuperObj) {
		return {1, 1, 1, 3};
	} else if (remainingMapTime > 60) {
		return {2, 2, 2, 0};
	} else {
		return {0, 0,
		        static_cast<int>(6 - superObjects.size()), static_cast<int>(superObjects.size())};
	}
}

std::vector<PVector> Robot::getPointPath(std::array<int, 4> max) {

	// the field currently operating on
	Field *field = (level == 0) ? map0 : map1;

	// the point path
	std::vector<PVector> points = {};

	// start and end position of one point path segment
	PVector start, end = pos;

	// number of objects that need to be added to the point path
	int num = -static_cast<int> (loadedObjects.num());
	for (int i : max) {
		num += i;
	}

	// copy of loaded objects array that can be modified
	auto tLoadedObjects = loadedObjects.all();

	// add a certain number of objects and super objects to the point path
	for (unsigned int iter = 0; iter < num; iter++) {
		unsigned int color = 0;
		double dist = INFINITY;

		// ------------- normal objects --------------------
		for (unsigned int i = 0; i < 3; ++i) {
			if (tLoadedObjects[i] < max[i]) {
				for (auto collectible : field->getCollectibles({i})) {
					if ((geometry::dist(start, collectible.pos) < dist) && collectible.pos != start &&
					    collectible.state != 2) {

						color = collectible.color;
						end = collectible.pos;
						dist = geometry::dist(start, end);
					}
				}
			}
		}

		// -------------- super objects ----------------------
		if (tLoadedObjects[3] < max[3]) {
			for (auto superObj : superObjects) {
				if (geometry::dist(start, superObj->pos) < dist) {
					color = 3;
					end = superObj->pos;
					dist = geometry::dist(start, end);
				}
			}
		}

		// -------------- add object / super object to point path ----------------
		if (start != end) {
			start = end;
			points.push_back(end);
			tLoadedObjects[color]++;
		} else {
			return points;
		}

	}
	return points;
}

//====================================
//          loop Methods
//====================================
void Robot::updateLoop() {

    updateSimVars();

	// --------- Time ----------
	remainingMapTime = GAME0_TIME - Time + ((level == 1) ? GAME1_TIME : 0);

	ERROR_MESSAGE("Time for one cycle: " + std::to_string(
			std::chrono::duration_cast<std::chrono::milliseconds>(Timer::now() - lastProgramCycle).count()))

	// --------- Should start super obj hunting ----------
	huntingSuperObj = (remainingMapTime < 60 || superObjects.size() >= 3 || huntingSuperObj) && !superObjects.empty();

	// ---------- superObjects -----------
	if (superObject) {

		Collectible *newSuperObject = map1->addCollectible(
				Collectible(superObject,3, lastRGBBonus == 2));
		superObjects.emplace_back(newSuperObject);
		superObject.set(NAN, NAN);
	}

	// ---------- Position -------------
	// set last coords to normal coords (last coords wont get overwritten by the sim)
	lastPos = simPos;
	updatePos();
}

void Robot::game0Loop() {


	if (shouldDeposit() && (isOrange(leftColor) || isOrange(rightColor))) {
		if (isOrange(leftColor) && isOrange(rightColor)) {
			deposit();
		} else if (isOrange(leftColor)) {
			wheels(0, 3);
		} else {
			wheels(3, 0);
		}

	} else if (shouldCollect()) {
		//collect();
	} else {
		// avoid trap on the right if objects are loaded
		if (isYellow(rightColor) && loadedObjects.num() > 0) {
			wheels(0, 5);
		}
			// avoid trap on the left
		else if (isYellow(leftColor) && loadedObjects.num() > 0) {
			wheels(5, 0);
		} else {
			switch (checkUsSensors(12, 15, 12)) {
				// no obstacle
				case 0:
					// 4 | 4 is standard movement speed in w1
					wheels(3, 3);
					break;
				case 1: // obstacle left
					wheels(4, -1);
					break;
				case 2: // obstacle front
					wheels(-3, -5);
					break;
				case 3: // obstacles left & front
					wheels(-1, -5);
					break;
				case 4: // obstacle right
					wheels(-1, 4);
					break;
				case 5: // obstacles left & right; turning would be fatal; just drive forward
					wheels(3, 3);
					break;
				case 6: // obstacles front & right
					wheels(-5, -1);
					break;
				case 7: // all sensors see an obstacle -> dead end; Just spinn
					wheels(-5, 5);
					break;
				default:
					break;

			}
			led = 0;
		}
	}
	// Teleport
	if (shouldTeleport()) {
		teleport();
	}
}

void Robot::game1Loop() {

	// -------------------- //
	//    Get a new Path    //
	// -------------------- //
	if (completePath.empty()) {
		if (loadedObjects.num() < 6 && remainingMapTime > 30) {

			// get a path of points
			std::vector<PVector> pathOfCollectibles = getPointPath(getDesiredLoad());

			// the first start point should be the current position of the robot
			PVector start = pos;

			// calculate a path from one point to the next
			for (unsigned int i = 0; i < pathOfCollectibles.size(); i++) {

				auto end = pathOfCollectibles[i];

				// depending on the current number of objects traps should be avoided or ignored
				Path path = (loadedObjects.num() > 0 || i > 0) ? pathfinder1T.AStar(start, end)
				                                               : pathfinder1.AStar(start, end);

				if (!path.isEmpty()) {
					completePath.push_back(path);
				} else {
					ERROR_MESSAGE("No Path found")
				}

				//std::cout << "Path from: " << str(start) << " to " << str(end) << std::endl;
				start = end;

			}
		} else {

			std::vector<PVector> deposits = map1->getDeposits();

			if (deposits.empty()) {
				std::cout << "NO DEPOSITS EXISTING!!!!" << std::endl;
				return;
			}

			Path path = pathfinder1T.AStar(pos, deposits.front());

			for (unsigned int i = 1; i < deposits.size(); i++) {
				Path temp = pathfinder1T.AStar(pos, deposits[i]);
				if (path.length() < temp.length() && !temp.isEmpty()) {
					path = temp;
				}

			}

			completePath.push_back(path);

		}
	}

	// remove path if point reached
	if (geometry::dist(completePath.front().getLast(), pos) < 5) {
		completePath.erase(completePath.begin());
		Collectible *collectible = map1->getCollectible(pos, compass, 5, -1);
		if (collectible) {

			if (collectible->visited > 3) {

				// mark the collectible as collected since it seems to be missing
				collectible->state = 2;

				std::cout << "Mark Collectible: " << collectible << " as not existing." << std::endl;
			}
			collectible->visited++;
		}
	}


	/* --------------------
	 * Priority Structure
	 * --------------------
	 * Deposit (When inventory full)
	 * Collect (When matching collectible is found)
	 * Pathfinding
	 *
	 * -------------------- */


	if (shouldDeposit() && (isOrange(leftColor) || isOrange(rightColor))) {
		if (isOrange(leftColor) && isOrange(rightColor)) {
			deposit();
		} else if (isOrange(leftColor)) {
			wheels(0, 3);
		} else {
			wheels(3, 0);
		}

	} else if (shouldCollect()) {
		// 1. color will be != -1.
		// Afterwards, until collecting has finished, it will be -1
		int color = collect();

		if (color != -1) {

			Collectible *collectible = map1->getCollectible(pos, compass, 10, color);
			std::cout << "Mark Collectible: " << collectible << " as collected" << std::endl;
			if (collectible) {
				collectible->state = 2;
				loadedObjects.addObject(collectible);

				if (color == 3) {
					superObjects.erase(std::remove(superObjects.begin(), superObjects.end(), collectible),
					                   superObjects.end());
				}
			}

		}

	} else {

		led = 0;
		if (!completePath.empty()) {
			moveAlongPath(completePath.front());
		}

		// avoid the void by driving left || avoid trap on the right if objects are loaded
		if (avoidVoid() == -1 || (isYellow(rightColor) && loadedObjects.num() > 0)) {
			wheels(0, 3);
		}
			// avoid the void by driving right || avoid trap on the left
		else if (avoidVoid() == 1 || (isYellow(leftColor) && loadedObjects.num() > 0)) {
			wheels(3, 0);
		}
	}

	lastProgramCycle = Timer::now();
}


#pragma endregion