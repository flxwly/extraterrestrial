#include "Robot.hpp"

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
	compass = In[12];
	time = In[13];

	// Update out information
	Out[0] = wheels.l;
	Out[1] = wheels.r;
	Out[2] = led;
}

void Robot::Game0() {

	ROBOT_LOG("Robot is running game0")




	// Steer towards desired velocity
	PVector steer = desiredVelocity - getVelocity();


}

void Robot::Game1() {

}

bool Robot::collisionAvoidance(int minDistanceToWallLeft, int minDistanceToWallFront, int minDistanceToWallRight,
                               int minDistanceToMapEnd) {

	int binaryState = 0;
	if (ultraSonicSensors.l < minDistanceToWallLeft) {
		binaryState += 1;
	}
	if (ultraSonicSensors.f < minDistanceToWallFront) {
		binaryState += 2;
	}
	if (ultraSonicSensors.r < minDistanceToWallRight) {
		binaryState += 4;
	}

	switch (binaryState) {
		case 0:
			// No need to adjust
			break;
	}


	return false;
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

	ROBOT_LOG("Rotation: " + std::to_string(compass))
	ROBOT_LOG("WheelLeft: " + std::to_string(wheelLeft))
	ROBOT_LOG("WheelRight: " + std::to_string(wheelRight))

	ROBOT_LOG("WheelLeft: " + std::to_string(v1))
	ROBOT_LOG("WheelLeft: " + std::to_string(v2))


	ROBOT_LOG("1. Velocity: " + PVector::str(vel))
	vel.rotate(compass * M_PI / 180);
	ROBOT_LOG("2. Velocity: " + PVector::str(vel))

	return vel;
}
