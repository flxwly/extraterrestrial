#include "Robot.hpp"


//====================================
//          Constructor
//====================================
Robot::Robot(int *input[20],
             Field *_map0,
             Field *_map1) : x{input[0]}, y{input[1]}, compass{input[2]},
                             superObjectNum{input[3]}, superObject_x{input[4]}, superObject_y{input[5]},
                             rightColorSensors{input[6], input[7], input[8]},
                             leftColorSensors{input[9], input[10], input[11]},
                             ultraSonicSensors{input[12], input[13], input[14]},
                             wheelLeft{input[15]}, wheelRight{input[16]}, led{input[17]}, tp{input[18]},
                             gameTime{input[19]}, loadedObjects_{0, 0, 0}, loadedObjectsNum_{0},
                             collectingSince_{timer::now()}, depositingSince_{timer::now()}, aPos_{0, 0},
                             lPos_{-1, -1}, lastPositionUpdate_{timer::now()}, map0_{_map0}, map1_{_map1},
                             nTarget_{-1, -1}, nTargetIsLast_{false}, chasingSuperObjNum_{0},
                             pathfinder0_{*map0_, false}, pathfinder0T_{*map0_, true},
                             pathfinder1_{*map1_, false}, pathfinder1T_{*map1_, true} {

	std::cout << "constructed Bot-Object" <<
	          std::endl;
}

//====================================
//          Private Functions
//====================================

// TODO: updatePos_ function
Point Robot::updatePos() {
	// time difference between last known position and now
	double time_dif = std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - lastPositionUpdate_).count();

	// total distance
	Point change = getVelocity(time_dif);

	// add change on actual position
	aPos_ += change;

	// update robot x and y position
	*x = static_cast<int>(round(aPos_.x)), *y = static_cast<int>(round(aPos_.y));

	// return change
	return change;
}

//TODO: logic to should collect function
bool Robot::shouldCollect() {

	// if the difference is less or equal to 3.5 seconds the robot is still collecting;
	if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - collectingSince_).count() <= 3500)
		return true;

	// The robot is full; the robot cant collect items anyway
	if (loadedObjectsNum_ >= 6)
		return false;

	// The objects color is Red
	if (isRed()) {
		// Since super objects count as red objects.
		// Only collect red objects if there's space including the chasing super objects
		return chasingSuperObjNum_ + loadedObjects_[0] < 2;


	} else if (isCyan()) {
		// nothin' special here
		return loadedObjects_[1] < 2;
	} else if (isBlack()) {
		// nothin' special here
		return loadedObjects_[2] < 2;
	}
	// if there's no object beneath the robot, don't try to collect anything
	return false;
}

int Robot::collect() {

	// the robot is already collecting
	if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - collectingSince_).count() <= 4000) {
		// This is to prevent the robot from moving
		wheels(0, 0);
		*led = 1;
		return 0;
	}
		// the robot begins to collect
	else {
		// set collectingSince to now
		collectingSince_ = Robot::timer::now();
		wheels(0, 0);
		*led = 1;

		// update the loadedObjects vars
		Robot::loadedObjectsNum_++;

		if (isRed() || isSuperObj()) {
			loadedObjects_[0]++;
			return 1;
		} else if (isCyan()) {
			loadedObjects_[1]++;
			return 2;
		} else if (isBlack()) {
			loadedObjects_[2]++;
			return 3;
		}
	}
	return 0;
}

// TODO: logic to should deposit function
bool Robot::shouldDeposit() {

	// while timer - depositingSince < 6the robot is still depositting
	if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - depositingSince_).count() <= 4000)
		return true;

	// the robot uses a treshhold to determine if it has enough points so that it is worth it to deposit
	// This treshhold should actually not really matter since the robot is supposed to
	// only drive to the deposit area if it is fully loaded

	// basic points;
	int treshhold = loadedObjects_[0] * 10 + loadedObjects_[1] * 15 + loadedObjects_[2] * 20;

	// one rgb-bonus
	if (loadedObjects_[0] > 0 && loadedObjects_[1] > 0 && loadedObjects_[2] > 0) {
		treshhold += 90;
		// second rgb-bonus
		if (loadedObjects_[0] > 1 && loadedObjects_[1] > 1 && loadedObjects_[2] > 1) {
			treshhold += 90;

		}
	}

	// 145 = 2 red + 1 cyan + 1 black | 20 + 15 + 20 + 90
	return treshhold >= 145;
}

void Robot::deposit() {
	// the robot is already depositing
	if (std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - depositingSince_).count() <= 5000) {
		// This is to prevent the robot from moving
		wheels(0, 0);
		*led = 2;

	}
		// the robot begins to deposit
	else {
		// set deposit_since to now
		depositingSince_ = timer::now();

		// update the loadedObjects vars
		loadedObjects_ = {0, 0, 0};
		loadedObjectsNum_ = 0;

		wheels(0, 0);
		*led = 2;
	}
}

// teleport functions
bool Robot::shouldTeleport() {
	// for ICool Challange because there's only one world
	// while in deposit_area don't teleport
	if (isOrange()) {
		return false;
	}

	// earliest possible teleport after 3min (180sec)
	//      only teleport if it has nothing to lose -> no objects loaded
	if (*gameTime > 180) {
		if (loadedObjectsNum_ == 0) {
			return true;
		}
	}
	// next possible teleport after 3min and 20sec (200sec)
	//      only teleport if points are more valuable then 145 points
	if (*gameTime > 200) {
		if (!shouldDeposit()) {
			return true;
		}
	}

	// last possible teleport after 4min (240sec)
	//      teleport in any case
	return *gameTime > 240;
}

void Robot::teleport() {
	loadedObjectsNum_ = 0;
	loadedObjects_ = {0, 0, 0};

	*tp = 1;
}


int Robot::avoidVoid() {
	if (*x >= 350 && *compass > 270 && *compass <= 360) {
		return -1;
	} else if (*x >= 350 && *compass > 180 && *compass <= 270) {
		return 1;
		//Right END
	} else if (*x <= 10 && *compass > 0 && *compass <= 90) {
		return 1;
	} else if (*x <= 10 && *compass > 90 && *compass <= 180) {
		return -1;
		//TOP END
	} else if (*y >= 260 && *compass > 270 && *compass <= 360) {
		return 1;
	} else if (*y >= 260 && *compass >= 0 && *compass <= 90) {
		return -1;
		//BOTTOM END
	} else if (*y <= 10 && *compass > 180 && *compass <= 270) {
		return -1;
	} else if (*y <= 10 && *compass < 180 && *compass >= 90) {
		return 1;
	}
	return 0;
}


//====================================
//          Public Functions
//====================================
void Robot::wheels(int l, int r) {
	*wheelRight = l, *wheelRight = r;
}

std::array<int, 3> Robot::getLoadedObjects() {
	return loadedObjects_;
}

int Robot::getLoadedObjectsNum() const {
	return loadedObjectsNum_;
}

int Robot::moveTo(Point p, bool safety) {

	double dist = geometry::dist(aPos_, p);

	// an angle should be created that represent the difference between the point to 0;
	// It should range from -180 to 180 instead of 0 tp 360;
	double angle = geometry::vector2Angle(p - aPos_);
	ERROR_MESSAGE("Angle: " + std::to_string(angle));

	// Difference between compass
	ERROR_MESSAGE("Compass: " + std::to_string(*compass));
	angle -= *compass;
	ERROR_MESSAGE("dif to Angle: " + std::to_string(angle));

	// If the angle is higher then 180 the point is on the other side
	if (fabs(angle) > 180) {
		//          -> get the same angle but with another prefix
		angle = fmod(angle + ((angle > 0) ? -360 : 360), 360);
	}
	ERROR_MESSAGE("turning angle Angle: " + std::to_string(angle));

	switch (Robot::checkUsSensors(10, 8, 10)) {
		// case 0 means checkUsSensors has detected no near obstacles
		//      -> the robot can move freely
		case 0:
			// the angle to x, y is small so there's no correction of it needed
			//      -> drive straight
			if (fabs(angle) < 10) {
				if (!safety || dist < 15) {
					wheels(1, 1);
				} else {
					wheels(4, 4);
				}
			}
				// the angle is a bit bigger so the robot needs to make a small correction
			else if (fabs(angle) < 20) {
				if (angle < 0) {
					if (!safety || dist < 15) {
						wheels(2, 1);
					} else {
						wheels(5, 4);
					}
				} else {
					if (!safety || dist < 15) {
						wheels(1, 2);
					} else {
						wheels(4, 5);
					}
				}
			}
				// the angle is quite big; now a bigger correction is needed
			else if (fabs(angle) < 60) {
				if (angle < 0) {
					if (!safety || dist < 15) {
						wheels(2, -1);
					} else {
						wheels(5, 2);
					}
				} else {
					if (!safety || dist < 15) {
						wheels(-1, 2);
					} else {
						wheels(2, 5);
					}
				}
			}
				// the angle is very big; a huge correction is needed
				//      -> no forward momentum; spinning
			else {
				if (angle < 0) {
					wheels(3, -3);
				} else {
					wheels(-3, 3);
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
			wheels(-5, 5);
			return 7;
		default:
			return -1;
	}
}

int Robot::moveTo(double x, double y, bool safety) {
	return Robot::moveTo(Point(x, y), safety);
}

int Robot::checkUsSensors(int l, int f, int r) {
	int sum = 0;

	if (*ultraSonicSensors[0] < l)
		sum += 1;
	if (*ultraSonicSensors[1] < f)
		sum += 2;
	if (*ultraSonicSensors[2] < r)
		sum += 4;
	return sum;
}


void Robot::game0Loop() {
	if (Robot::shouldDeposit() && (isOrangeLeft() || isOrangeRight())) {
		if (isOrange()) {
			Robot::deposit();
		} else if (isOrangeRight()) {
			Robot::wheels(3, 0);
		} else {
			Robot::wheels(0, 3);
		}

	} else if (Robot::shouldCollect()) {
		Robot::collect();
	} else {
		// avoid trap on the right if objects are loaded
		if (isYellowRight() && Robot::loadedObjectsNum_ > 0) {
			wheels(0, 5);
		}
			// avoid trap on the left
		else if (isYellowLeft() && Robot::loadedObjectsNum_ > 0) {
			wheels(5, 0);
		} else {
			/*switch (Robot::checkUsSensors(8, 12, 8)) {
				// no obstacle
				case 0:
					// 4 | 4 is standard movement speed in w1
					wheels(4, 4);
					break;
				case 1: // obstacle left
					wheels(4, 0);
					break;
				case 2: // obstacle front
					wheels(-3, -5);
					break;
				case 3: // obstacles left & front
					wheels(-1, -5);
					break;
				case 4: // obstacle right
					wheels(0, 4);
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
			}*/
			*Robot::led = 0;
		}
	}
	// Teleport
	if (Robot::shouldTeleport()) {
		Robot::teleport();
	}
}

void Robot::game1Loop() {

	ERROR_MESSAGE("Time for one cycle: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
			timer::now() - lastCycle_).count()));
	lastCycle_ = timer::now();

	// ====== Just for speed measure ====== //     (leave it in for later)
	if (false && *wheelLeft == *wheelRight && *wheelLeft != 0) {

		double time_dif = std::chrono::duration_cast<std::chrono::milliseconds>(
				timer::now() - lastPositionUpdate_).count();

		double distance = dist(*x, lX, *y, l_y);

		std::cout << distance / time_dif << std::endl;

	}


	// check if robot is in signal lost zone
	if (*x == 0 && *y == 0) {

		// set normal coords to last coords and update with function
		*x = l_x, *y = l_y;
		updatePos_();
		ERROR_MESSAGE("Position lost");
	}

	// set last coords to normal coords (last coords wont get overwritten by the sim)
	lX = *x, l_y = *y;
	latestPositionUpdate = timer::now();
	//#####################
	// -- PATHFINDING --
	//#####################

	// There's no path to follow
	if (completePath.empty()) {
		// get a path of points
		std::vector<Point> pathOfCollectibles = getPathOfCollectibles();
		Point start = {*x, *y};

		// calculate a path from one point to the next
		for (auto end : pathOfCollectibles) {
			if (start) {

				// depending on the current number of objects traps should be avoided or ignored
				std::vector<Point> p = (loadedObjectsNum_ > 0) ? pathfinder1T_.AStar(start, end)
				                                               : pathfinder1_.AStar(start, end);

				if (!p.empty()) {

					// add the path to the complete path
					// the first path is at the front of the vector
					completePath.push_back(p);
					//std::cout << "added Path\n";
				} else {
					// TODO: Throw exception
					ERROR_MESSAGE("No Path found");
				}
			}
			//std::cout << "Path from: " << str(start) << " to " << str(end) << std::endl;
			start = end;
		}
	}


		// get the next target
	else {

		if (nTarget_.x == -1 && nTarget_.y == -1) {

			// set nTarget_ to the last element of the first path in completePath
			//      the paths in completePath are reversed; The end of the path is the first element

			nTargetIsLast_ = completePath.front().size() == 1;
			nTarget_ = completePath.front().back();

			//std::cout << "new target: " << str(nTarget_) << std::endl;

			// if it's the last element remove the path entirely
			if (nTargetIsLast_) {
				completePath.erase(completePath.begin());
			}
				// Otherwise just remove it
			else {
				completePath.front().pop_back();//erase(completePath.front().begin());
			}
		}
	}

	/*--------------------
	 * Priority Structure
	 * -------------------
	 * Deposit
	 * Collect
	 * dodge traps and out of bounds <- only for safety.
	 * Pathfind using active collision avoidance
	 *
	 * */


	if (shouldDeposit_() && (isOrangeLeft() || isOrangeRight())) {
		if (isOrange()) {
			deposit();
		} else if (isOrangeRight()) {
			wheels(3, 0);
		} else {
			wheels(0, 3);
		}

	} else if (shouldCollect()) {
		Point *ptr = nullptr;
		switch (collect()) {
			case 1:
				ptr = map1->find_point({*x, *y}, 0);
				if (ptr != nullptr) ptr->state = 0;
				break;
			case 2:
				ptr = map1->find_point({*x, *y}, 1);
				if (ptr != nullptr) ptr->state = 0;
				break;
			case 3:
				ptr = map1->find_point({*x, *y}, 2);
				if (ptr != nullptr) ptr->state = 0;
				break;
		}
	} else {
		*led = 0;
		moveTo(nTarget_, nTargetIsLast_);
		ERROR_MESSAGE(str(nTarget_));
		//std::cout << "Is at: " << str(*x, *y) << "\tmoving to: " << str(nTarget_) << std::endl;

		// if the distance is very small the target has been reached
		if (geometry::dist(Point(*x, *y), nTarget_) < 5) {
			//std::cout << "reached Object" << std::endl;
			nTarget_ = {-1, -1};
		}

		// avoid the void by driving left || avoid trap on the right if objects are loaded
		if (avoidVoid() == -1 || (isYellowRight() && loadedObjectsNum_ > 0)) {
			wheels(0, 5);
		}
			// avoid the void by driving right || avoid trap on the left
		else if (avoidVoid() == 1 || (isYellowLeft() && loadedObjectsNum_ > 0)) {
			wheels(5, 0);
		}
	}
}

double Robot::getBrakingDistance(double friction) {
	return (*wheelLeft + *wheelRight) / 2;
}

Point Robot::getVelocity(double dt) {
	return (geometry::angle2Vector(*compass) * (*wheelLeft + *wheelRight) / 2) * dt;
}
