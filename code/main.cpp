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

Robot *robot = nullptr;

void UpdateLoop() {

    HANDLE hStdout, hNewScreenBuffer, hNewScreenBuffer2;
    SMALL_RECT srctWriteRect;
    CHAR_INFO chiBuffer[robot->map1.Map_.size()];
    COORD coordBufSize;
    COORD coordBufCoord;
    BOOL fSuccess;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    coordBufSize.X = robot->map1.width_;
    coordBufSize.Y = robot->map1.height_;

    coordBufCoord.X = 0;
    coordBufCoord.Y = 0;

    srctWriteRect.Left = srctWriteRect.Top = 0;
    srctWriteRect.Right = robot->map1.width_ - 1;
    srctWriteRect.Bottom = robot->map1.height_ - 1;

    hNewScreenBuffer = CreateConsoleScreenBuffer(
            GENERIC_WRITE,
            0,
            NULL,                    // default security attributes
            CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE
            NULL);                   // reserved; must be NULL

    if (hNewScreenBuffer == INVALID_HANDLE_VALUE) {
        printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
        return;
    }

    SetConsoleTitle("Plz let me double buffering.");
    SetConsoleScreenBufferSize(hNewScreenBuffer, coordBufSize);
    SetConsoleWindowInfo(hNewScreenBuffer, TRUE, &srctWriteRect);

    MISC_LOG("Starting UpdateLoop")

    while (RunUpdateLoop) {

        for (int i = 0; i < robot->map1.Map_.size(); ++i)
        {
                chiBuffer[i].Char.UnicodeChar = robot->map1.Map_[i];
                chiBuffer[i].Attributes = FOREGROUND_GREEN;
        }

        // present

        fSuccess = WriteConsoleOutput(
                hNewScreenBuffer, // screen buffer to write to
                chiBuffer,        // buffer to copy from
                coordBufSize,     // col-row size of chiBuffer
                coordBufCoord,    // top left src cell in chiBuffer
                &srctWriteRect);

        if (! SetConsoleActiveScreenBuffer(hNewScreenBuffer)) {
            printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
            return;
        }








        std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();

        Update();

        auto waitingTime = std::chrono::duration<int, std::milli>(MINIMUM_TIME_BETWEEN_CYCLE) -
                           std::chrono::duration_cast<std::chrono::milliseconds>(
                                   begin - std::chrono::high_resolution_clock::now());

        MISC_LOG("Waiting " << waitingTime.count() << " milliseconds")
        std::this_thread::sleep_for(waitingTime);
    }

    MISC_LOG("Exit UpdateLoop")
}


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