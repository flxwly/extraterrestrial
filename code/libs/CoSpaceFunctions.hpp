#ifndef COSPACE_SETTINGS
#define COSPACE_SETTINGS

#define CsBot_AI_H
#define DLL_EXPORT extern __declspec(dllexport)



#ifdef _MSC_VER
	#define DISABLE_C4996 __pragma(warning(push)) __pragma(warning(disable : 4996))
	#define ENABLE_C4996 __pragma(warning(pop))
#else
	#define DISABLE_C4996
	#define ENABLE_C4996
#endif


#include <thread>
#include "PPSettings.hpp"


extern char AI_MyID[6];

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
extern int AI_TeamID;   //Robot Team ID. 1:Blue Ream; 2:Red Team.
extern int MySMS; //A integer value which you want to send to the other robot.
//In Super Team mode, you can use this value to inform your another robot of your status.
//In Indiviual Mode, you should keep this value to 0.

//The following three variables save the information of the other robot.
extern int OtherRob_SMS; //Keep the recieved value of the other robot MySMS value.
//In Super Team mode, this value is sent by your another robot.
//In Indiviual Mode, this value has no meaning.
extern int OtherRob_PositionX;//The X coordinate of the other robot.
extern int OtherRob_PositionY;//The Y coordinate of the other robot.

//The following four variables keep the information of the last state-changed object.
extern int ObjState;    //The state (0: Disappear; 1:Appear.) of the last state changed object.
extern int ObjPositionX;//The X coordinate of the last state-changed object.
extern int ObjPositionY;//The Y coordinate of the last state-changed object.
extern int ObjDuration; //The duration(seconds) of the object maintains the current state;


#define CsBot_AI_C //DO NOT delete this line

DLL_EXPORT void SetGameID(int GameID);

DLL_EXPORT void SetTeamID(int TeamID);

DLL_EXPORT int GetGameID();

#ifndef CSBOT_REAL

DLL_EXPORT char *GetDebugInfo();

DLL_EXPORT char *GetTeamName();

DLL_EXPORT int GetCurAction();

DLL_EXPORT int GetTeleport();

DLL_EXPORT void SetSuperObj(int X, int Y, int num);

DLL_EXPORT void GetSuperObj(int *X, int *Y, int *num);

///Called each time frame by simulator to update the other robot information.
DLL_EXPORT void UpdateRobInfo(int sms, int X, int Y);

DLL_EXPORT void UpdateObjectInfo(int X, int Y, int state, int duration);

DLL_EXPORT int GetMySMS();

#endif ////CSBOT_REAL

DLL_EXPORT void SetDataAI(volatile int *packet, volatile int *AI_IN);

DLL_EXPORT void GetCommand(int *AI_OUT);

DLL_EXPORT void OnTimer();


extern volatile int *In;
extern volatile int *Out;

extern std::thread *Thread;
extern bool RunUpdateLoop;

extern void UpdateLoop();

extern void EndUpdateLoop();

extern void Update();

extern int InitialisingState;

extern void Setup();

extern void Game0();

extern void Game1();

#endif // !COSPACE_SETTINGS