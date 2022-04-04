#define CsBot_AI_H //DO NOT delete this line
#define CsBot_AI_C  //DO NOT delete this line

// Libs
#include "libs/PPSettings.hpp"
#include "libs/CoSpaceFunctions.hpp"
#include "Robot.hpp"

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

#ifdef RUNNING_WITHOUT_COSPACE
int main() {

    CurGame = 1;

    static int AI_IN[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0};
    static int AI_OUT[] = {0, 0, 0, 0};

    In = reinterpret_cast<volatile int *>(&AI_IN);
    Out = reinterpret_cast<volatile int *>(&AI_OUT);

    Setup();
    StartUpdateLoop();
    while(RunUpdateLoop) {

    };
}
#endif