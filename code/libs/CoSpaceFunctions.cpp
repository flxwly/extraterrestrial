#include "CoSpaceFunctions.hpp"

volatile int *INPUT = nullptr;
volatile int *OUTPUT = nullptr;

char AI_MyID[2] = {'0', '2'};
int Duration = 0;
int SuperDuration = 0;
int bGameEnd = false;
int CurAction = -1;
int CurGame = 0;
int SuperObj_Num = 0;
int SuperObj_X = 0;
int SuperObj_Y = 0;
int Teleport = 0;
int LoadedObjects = 0;
int US_Front = 0;
int US_Left = 0;
int US_Right = 0;
int CSLeft_R = 0;
int CSLeft_G = 0;
int CSLeft_B = 0;
int CSRight_R = 0;
int CSRight_G = 0;
int CSRight_B = 0;
int PositionX = 0;
int PositionY = 0;
int TM_State = 0;
int Compass = 0;
int Time = 0;
int WheelLeft = 0;
int WheelRight = 0;
int LED_1 = 0;
int MyState = 0;
unsigned int cycle = 0;
int AI_SensorNum = 13;

// only for superTeam challenge
int OtherRob_PositionX;//The X coordinate of the other robot.
int OtherRob_PositionY;//The Y coordinate of the other robot.

// walls only. No points
//The following four variables keep the information of the last state-changed object.
int ObjState;    //The state (0: Disappear; 1:Appear.) of the last state changed object.
int ObjPositionX;//The X coordinate of the last state-changed object.
int ObjPositionY;//The Y coordinate of the last state-changed object.
int ObjDuration; //The duration(seconds) of the object maintains the current state;

DLL_EXPORT void SetGameID(int GameID) {
    if (GameID == -1) {
        Stop();
    }
    CurGame = GameID;
    bGameEnd = 0;
}
DLL_EXPORT int GetGameID() {
    return CurGame;
}
//Only Used by CsBot Dance Platform
DLL_EXPORT int IsGameEnd() {
    return bGameEnd;
}
DLL_EXPORT char *GetDebugInfo() {
    static char info[1024];
    sprintf(info,
            "Duration=%d;SuperDuration=%d;bGameEnd=%d;CurAction=%d;CurGame=%d;SuperObj_Num=%d;SuperObj_X=%d;SuperObj_Y=%d;Teleport=%d;LoadedObjects=%d;US_Front=%d;US_Left=%d;US_Right=%d;CSLeft_R=%d;CSLeft_G=%d;CSLeft_B=%d;CSRight_R=%d;CSRight_G=%d;CSRight_B=%d;PositionX=%d;PositionY=%d;TM_State=%d;Compass=%d;Time=%d;WheelLeft=%d;WheelRight=%d;LED_1=%d;MyState=%d;",
            Duration, SuperDuration, bGameEnd, CurAction, CurGame, SuperObj_Num, SuperObj_X, SuperObj_Y, Teleport,
            LoadedObjects, US_Front, US_Left, US_Right, CSLeft_R, CSLeft_G, CSLeft_B, CSRight_R, CSRight_G, CSRight_B,
            PositionX, PositionY, TM_State, Compass, Time, WheelLeft, WheelRight, LED_1, MyState);
    return info;
}
DLL_EXPORT char *GetTeamName() {
    static char team_name[30] = "Extraterrestrial";
    return team_name;
}
DLL_EXPORT int GetCurAction() {
    return CurAction;
}
//Only Used by CsBot Rescue Platform
DLL_EXPORT int GetTeleport() {
    return Teleport;
}
//Only Used by CsBot Rescue Platform
DLL_EXPORT void SetSuperObj(int X, int Y, int num) {
    SuperObj_X = X;
    SuperObj_Y = Y;
    SuperObj_Num = num;
}
//Only Used by CsBot Rescue Platform
DLL_EXPORT void GetSuperObj(int *X, int *Y, int *num) {
    *X = SuperObj_X;
    *Y = SuperObj_Y;
    *num = SuperObj_Num;
}
DLL_EXPORT void SetDataAI(volatile int *packet, volatile int *AI_IN) {

    for (int i = 0; i < 14; ++i) {
        packet[i] = AI_IN[i];
        packet[14] += packet[i];
    }

    INPUT = AI_IN;
}
DLL_EXPORT void GetCommand(int *AI_OUT) {
    OUTPUT = AI_OUT;
}

DLL_EXPORT void OnTimer() {
    if (cycle == 1) {
        Setup();
    } else if (INPUT && OUTPUT) {
        Game1();
    }
    cycle++;
}