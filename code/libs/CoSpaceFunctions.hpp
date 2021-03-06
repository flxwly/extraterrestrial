#ifndef CSBOT_COSPACE_SETTINGS
#define CSBOT_COSPACE_SETTINGS

#define _USE_MATH_DEFINES
#include <bits/stdc++.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

#define USFYOffset 12.375
#define USRXOffset 3            //3.874
#define USRYOffset 11.045
#define USLXOffset -3           //-3.874
#define USLYOffset 11.045

#define CSXOffset 3.618
#define CSYOffset 10.125

#define wall 1
#define trap 2


#ifdef _MSC_VER
#define DISABLE_C4996 __pragma(warning(push)) __pragma(warning(disable : 4996))
#define ENABLE_C4996 __pragma(warning(pop))
#else
#define DISABLE_C4996
#define ENABLE_C4996
#endif

extern char AI_MyID[2];
extern int Duration;
extern int SuperDuration;
extern int bGameEnd;
extern int CurAction;
extern int CurGame;
extern int SuperObj_Num;
extern int SuperObj_X;
extern int SuperObj_Y;
extern int Teleport;
extern int LoadedObjects;
extern int US_Front;
extern int US_Left;
extern int US_Right;
extern int CSLeft_R;
extern int CSLeft_G;
extern int CSLeft_B;
extern int CSRight_R;
extern int CSRight_G;
extern int CSRight_B;
extern int hueR;
extern int hueL;
extern int satR;
extern int satL;
extern int lumR;
extern int lumL;
extern int PositionX;
extern int PositionY;
extern int TM_State;
extern int Compass;
extern int Time;
extern int WheelLeft;
extern int WheelRight;
extern int LED_1;
extern int MyState;
extern int AI_SensorNum;
extern int OtherRob_PositionX;//The X coordinate of the other robot.
extern int OtherRob_PositionY;//The Y coordinate of the other robot.

//The following four variables keep the information of the last state-changed object.
extern int ObjState;    //The state (0: Disappear; 1:Appear.) of the last state changed object.
extern int ObjPositionX;//The X coordinate of the last state-changed object.
extern int ObjPositionY;//The Y coordinate of the last state-changed object.
extern int ObjDuration; //The duration(seconds) of the object maintains the current state;


extern void Game0();
extern void Game1();
extern int cycles;
extern void Setup();

DLL_EXPORT void SetGameID(int GameID);
DLL_EXPORT int GetGameID();
//Only Used by CsBot Dance Platform
DLL_EXPORT int IsGameEnd();

DLL_EXPORT char *GetDebugInfo();
DLL_EXPORT char *GetTeamName();
DLL_EXPORT int GetCurAction();
//Only Used by CsBot Rescue Platform
DLL_EXPORT int GetTeleport();
//Only Used by CsBot Rescue Platform
DLL_EXPORT void SetSuperObj(int X, int Y, int num);
//Only Used by CsBot Rescue Platform
DLL_EXPORT void GetSuperObj(int *X, int *Y, int *num);

DLL_EXPORT void SetDataAI(volatile int *packet, volatile int *AI_IN);
DLL_EXPORT void GetCommand(int *AI_OUT);
DLL_EXPORT void OnTimer();

#endif // !COSPACE_SETTINGS