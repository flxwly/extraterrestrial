#define CsBot_AI_H //DO NOT delete this line
#define CsBot_AI_C  //DO NOT delete this line
#ifndef CSBOT_REAL

// Libs
#include "libs/PPSettings.hpp"
#include "libs/CoSpaceFunctions.hpp"
#include "libs/CommonFunctions.hpp"
#include "libs/ColorRecognition.hpp"

// Classes
#include "Pathfinder.hpp"
#include "MapData.hpp"
#include "Robot.hpp"

#include <iostream>
#include <fileapi.h>

#endif


#ifdef SFML

#include "SFMLWindow.hpp"

DebugWindow *debugWindow = nullptr;

#endif


void updateHSL() {

    hueR = rgb2h(CSRight_R, CSRight_G, CSRight_B);
    hueL = rgb2h(CSLeft_R, CSLeft_G, CSLeft_B);

    satR = rgb2s(CSRight_R, CSRight_G, CSRight_B);
    satL = rgb2s(CSLeft_R, CSLeft_G, CSLeft_B);

    lumR = rgb2l(CSRight_R, CSRight_G, CSRight_B);
    lumL = rgb2l(CSLeft_R, CSLeft_G, CSLeft_B);
}

Field *GAME0 = nullptr;

Field *GAME1 = nullptr;

Robot *Bot = nullptr;

void Setup() {
    system("cls");

    // ----------- Initialisation of static objects -------------------- //

    static Field Game0(270, 180, GAME0WALLS, GAME0TRAPS, GAME0SWAMPS, GAME0WATERS, GAME0DEPOSITS, GAME0WALLNODES,
                       GAME0TRAPNODES,
                       GAME0COLLECTIBLES);
    GAME0 = &Game0;

    static Field Game1(360, 270, GAME1WALLS, GAME1TRAPS, GAME1SWAMPS, GAME1WATERS, GAME1DEPOSITS, GAME1WALLNODES,
                       GAME1TRAPNODES,
                       GAME1COLLECTIBLES);
    GAME1 = &Game1;

    static Robot bot(&PositionX, &PositionY, &Compass, &SuperObj_Num, &SuperObj_X, &SuperObj_Y,
                     &CSRight_R, &CSRight_G, &CSRight_B, &CSLeft_R, &CSLeft_G, &CSLeft_B,
                     &US_Left, &US_Front, &US_Right,
                     &WheelLeft, &WheelRight, &LED_1, &Teleport, &Time, GAME0, GAME1);
    Bot = &bot;

#ifdef SFML
	static DebugWindow window(Bot);
	window.startDebugging(GAME1);
	debugWindow = &window;
#endif

    updateHSL();

}

/*
 * ///_________________________________GAME0________________________________________///
 *
 * ///__________________________________________________________________________________///
*/

void Game0() {
    updateHSL();
    Bot->game0Loop();
}

/*
 * ///_________________________________GAME1________________________________________///
 *
 * ///__________________________________________________________________________________///
*/




void Game1() {
    updateHSL();

	Bot->game1Loop();
}