#include "Robot.hpp"

const int Wheels::minVel;
const int Wheels::maxVel;

Robot::Robot(volatile int *pIn, volatile int *pOut) {
	In = pIn;
	Out = pOut;
}

void Robot::Update() {

	ROBOT_LOG("Robot is updating the variables")

	// Update in information
	ultraSonicSensors.set(In[1], In[0], In[2]);
	colorSensors.set(
			rgb2hsl({static_cast<float>(In[3]), static_cast<float>(In[4]), static_cast<float>(In[5])}),
			rgb2hsl({static_cast<float>(In[6]), static_cast<float>(In[7]), static_cast<float>(In[8])}));
	simPos.set(In[9], In[10]);
	compass = In[11];
	time = In[13];

	// Update out information
	Out[0] = wheels.l;
	Out[1] = wheels.r;
	Out[2] = led;
}

void Robot::Game0() {

    Teleport();
	ROBOT_LOG("Robot is running game0")

}
std::list<PVector> path = {{80, 20}, {50,250}, {280, 250}, {280,20}};

void Robot::Game1() {

    ROBOT_LOG("Robot is running game1")

    if (geometry::sqDist(path.front(), simPos) < 4) {
        path.push_back(path.front());
        path.pop_front();
    }

    std::cout << "Target: " << path.front() << std::endl;

    moveTo(path.front());
}

PVector Robot::collisionAvoidance(double maxForce) {

	PVector force = {0, 0};

	//TODO: Tweak constants
	const double G = 1;
	const double C = 1;

	force += geometry::angle2Vector(compass - ULTRASONIC_SENSOR_ANGLE_OFFSET - 180).setMag(
			G * C / (ultraSonicSensors.l * ultraSonicSensors.l));
	force += geometry::angle2Vector(compass - 180).setMag(G * C / (ultraSonicSensors.f * ultraSonicSensors.f));
	force += geometry::angle2Vector(compass + ULTRASONIC_SENSOR_ANGLE_OFFSET - 180).setMag(
			G * C / (ultraSonicSensors.r * ultraSonicSensors.r));

	if (force.getMag() > maxForce) {
		force.setMag(maxForce);
	}

	return force;
}

PVector Robot::getVelocity() {

	// For clarification on how this works see
	// https://math.stackexchange.com/questions/3962859/calculate-path-of-vehicle-with-two-wheels-parallel-to-each-other

	double penalty = (isSwamp(colorSensors.l) || isSwamp(colorSensors.r)) ? SWAMP_SPEED_PENALTY : 1;

	if (wheels.l == wheels.r) {
		return geometry::angle2Vector(static_cast<double>(compass + 90) / 180 * M_PI) *
		       (static_cast<double>(wheels.l) * ROBOT_SPEED / penalty);
	}

	// v1 and v2 are the linear velocities at the wheels
	double v1 = LWHEEL_RADIUS * REVPERMS * static_cast<double>(wheels.l) / penalty;
	double v2 = RWHEEL_RADIUS * REVPERMS * static_cast<double>(wheels.r) / penalty;

	double s = (ROBOT_AXLE_LENGTH * (v1 + v2)) / (2 * (v1 - v2));

	PVector vel = (v1 != 0) ?
	              PVector(-s * cos(v1 / (ROBOT_AXLE_LENGTH / 2 + s)) + s,
	                      s * sin(v1 / (ROBOT_AXLE_LENGTH / 2 + s))) :

	              PVector(-s * cos(v2 / (ROBOT_AXLE_LENGTH / 2 - s)) + s,
	                      s * sin(v2 / (ROBOT_AXLE_LENGTH / 2 - s)));

	ROBOT_LOG("WheelLeft: " + std::to_string(v1))
	ROBOT_LOG("WheelLeft: " + std::to_string(v2))

	vel.rotate(compass * M_PI / 180);

	return vel;
}

void Robot::moveTo(PVector position) {

    double angleDif = std::fmod(geometry::vector2Angle(simPos - position) * 180 / M_PI - compass + 90, 360);

    std::cout << "Angle dif: "<< angleDif << std::endl;

    if (angleDif > 180) {
		angleDif -= 360;
	} else if (angleDif < -180) {
		angleDif += 360;
	}


	int action = -1;
	if (std::fabs(angleDif) < 5) {
		action = 0;
	} else if (std::fabs(angleDif) < 15) {
		action = 1;
	} else if (std::fabs(angleDif) < 30) {
		action = 2;
	} else if (std::fabs(angleDif) < 45) {
		action = 3;
	} else if (std::fabs(angleDif) < 60) {
		action = 4;
	} else if (std::fabs(angleDif) < 75) {
		action = 5;
	} else {
		action = 16;
	}

    double magnitude = std::pow(2, std::min((position - simPos).getMag() / 10, 4.0)) / 16;
	std::cout << "Magnitude: " << magnitude << std::endl;

    switch (action) {
		case 0:
			wheels.set(magnitude * 100, magnitude * 100);
			break;
		case 1:
			if (angleDif > 0) {
				wheels.set(magnitude * 90, magnitude * 100);
			} else {
				wheels.set(magnitude * 100, magnitude * 90);
			}
			break;
		case 2:
			if (angleDif > 0) {
				wheels.set(magnitude * 50, magnitude * 100);
			} else {
				wheels.set(magnitude * 100, magnitude * 50);
			}
			break;
		case 3:
			if (angleDif > 0) {
				wheels.set(magnitude * -20, magnitude * 100);
			} else {
				wheels.set(magnitude * 70, magnitude * -70);
			}
			break;
		case 4:
			if (angleDif > 0) {
				wheels.set(magnitude * -40, magnitude * 70);
			} else {
				wheels.set(magnitude * 70, magnitude * -40);
			}
			break;
		case 5:
			if (angleDif > 0) {
				wheels.set(magnitude * -80, magnitude * 80);
			} else {
				wheels.set(magnitude * 80, magnitude * -80);
			}
			break;
		default:
			if (angleDif > 0) {
				wheels.set(magnitude * -100, magnitude * 100);
			} else {
				wheels.set(magnitude * 100, magnitude * -100);
			}
			break;
	}


}

std::vector<Collectible> Robot::getCollectiblePath(CollectibleLoad desiredLoad, bool finishOnDeposit) {

	const double power = 5;
	const int agents = 0;


	// TODO: Add field pointers
	Field *field = (level == 0) ? nullptr : nullptr;







	return std::vector<Collectible>();
}

void Robot::Teleport() {
	level = 1;
	loadedCollectibles.clear();
}
