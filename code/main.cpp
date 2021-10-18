#define CsBot_AI_H //DO NOT delete this line
#define CsBot_AI_C  //DO NOT delete this line

#include <windows.h>
#include <cstdio>
#include <thread>
// Libs
#include "libs/PPSettings.hpp"
#include "libs/CoSpaceFunctions.hpp"
#include "libs/ColorRecognition.hpp"
#include "Robot.hpp"
#include "libs/ConsolePainter.hpp"

Robot *robot = nullptr;


void Setup() {
    // Clear console
    system("cls");

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
	robot->Update();
	robot->Game1();

}