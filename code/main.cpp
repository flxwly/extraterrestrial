#define CsBot_AI_H //DO NOT delete this line
#define CsBot_AI_C  //DO NOT delete this line

// Libs
#include "libs/PPSettings.hpp"
#include "libs/CoSpaceFunctions.hpp"
#include "libs/ColorRecognition.hpp"
#include "Robot.hpp"

Robot *robot = nullptr;

void Setup() {
	MISC_LOG("Running Setup");

	static Robot robot1(In, Out);
	robot = &robot1;
}

void Game0() {
	MISC_LOG("Running Game0");
	robot->Update();
	robot->Game0();
}

void Game1() {
	MISC_LOG("Running Game1");
}