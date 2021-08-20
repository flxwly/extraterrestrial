#include "CoSpaceFunctions.hpp"

char AI_MyID[6] = {'1', '2', '3', '4', '5', '6'};

int Duration = 0;
int SuperDuration = 0;
int bGameEnd = 0;
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
int AI_SensorNum = 13;
int AI_TeamID = 1;   //Robot Team ID. 1:Blue Ream; 2:Red Team.
int MySMS; //A integer value which you want to send to the other robot.
//In Super Team mode, you can use this value to inform your another robot of your status.
//In Indiviual Mode, you should keep this value to 0.

//The following three variables save the information of the other robot.
int OtherRob_SMS; //Keep the recieved value of the other robot MySMS value.
//In Super Team mode, this value is sent by your another robot.
//In Indiviual Mode, this value has no meaning.
int OtherRob_PositionX;//The X coordinate of the other robot.
int OtherRob_PositionY;//The Y coordinate of the other robot.

//The following four variables keep the information of the last state-changed object.
int ObjState;    //The state (0: Disappear; 1:Appear.) of the last state changed object.
int ObjPositionX;//The X coordinate of the last state-changed object.
int ObjPositionY;//The Y coordinate of the last state-changed object.
int ObjDuration; //The duration(seconds) of the object maintains the current state;


#define CsBot_AI_C

// Is called periodically, after the user pressed the "go" button
DLL_EXPORT void SetGameID(int GameID) {

	CurGame = GameID;
	bGameEnd = 0;

	if (GameID == -1) {
		EndUpdateLoop();
	}
}

// Is called once, after the user loads the ai.dll into the sym
DLL_EXPORT void SetTeamID(int TeamID) {
	AI_TeamID = TeamID;
}

// Is called periodically, after the user loads the ai.dll into the sym.
// Gets called after SetTeamID was called once
DLL_EXPORT int GetGameID() {
	return CurGame;
}

//Only Used by CsBot Dance Platform
DLL_EXPORT int IsGameEnd() {
	return bGameEnd;
}

#ifndef CSBOT_REAL

// Is only called if the user opens the debug panel in the simulator
DLL_EXPORT char *GetDebugInfo() {
	char info[3000];
	sprintf(info,
	        "Duration=%d;SuperDuration=%d;bGameEnd=%d;CurAction=%d;CurGame=%d;SuperObj_Num=%d;SuperObj_X=%d;SuperObj_Y=%d;Teleport=%d;LoadedObjects=%d;US_Front=%d;US_Left=%d;US_Right=%d;CSLeft_R=%d;CSLeft_G=%d;CSLeft_B=%d;CSRight_R=%d;CSRight_G=%d;CSRight_B=%d;PositionX=%d;PositionY=%d;TM_State=%d;Compass=%d;Time=%d;WheelLeft=%d;WheelRight=%d;LED_1=%d;MyState=%d;",
	        Duration, SuperDuration, bGameEnd, CurAction, CurGame, SuperObj_Num, SuperObj_X, SuperObj_Y, Teleport,
	        LoadedObjects, US_Front, US_Left, US_Right, CSLeft_R, CSLeft_G, CSLeft_B, CSRight_R, CSRight_G, CSRight_B,
	        PositionX, PositionY, TM_State, Compass, Time, WheelLeft, WheelRight, LED_1, MyState);
	return info;
}


DLL_EXPORT char *GetTeamName() {
	return "Extraterrestrial";
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
//Used by CoSpace Rescue Simulation.
///Called each time frame by simulator to update the other robot information.
DLL_EXPORT void UpdateRobInfo(int sms, int X, int Y) {
	OtherRob_SMS = sms;
	OtherRob_PositionX = X;
	OtherRob_PositionY = Y;
}

//Used by CsBot Rescue Platform
DLL_EXPORT void UpdateObjectInfo(int X, int Y, int state, int duration) {
	ObjState = state;
	ObjPositionX = X;
	ObjPositionY = Y;
	ObjDuration = duration;
}

DLL_EXPORT int GetMySMS() {
	return MySMS;
}

#endif ////CSBOT_REAL

DLL_EXPORT void SetDataAI(volatile int *packet, volatile int *AI_IN) {

	int sum = 0;

	US_Front = AI_IN[0];
	packet[0] = US_Front;
	sum += US_Front;
	US_Left = AI_IN[1];
	packet[1] = US_Left;
	sum += US_Left;
	US_Right = AI_IN[2];
	packet[2] = US_Right;
	sum += US_Right;
	CSLeft_R = AI_IN[3];
	packet[3] = CSLeft_R;
	sum += CSLeft_R;
	CSLeft_G = AI_IN[4];
	packet[4] = CSLeft_G;
	sum += CSLeft_G;
	CSLeft_B = AI_IN[5];
	packet[5] = CSLeft_B;
	sum += CSLeft_B;
	CSRight_R = AI_IN[6];
	packet[6] = CSRight_R;
	sum += CSRight_R;
	CSRight_G = AI_IN[7];
	packet[7] = CSRight_G;
	sum += CSRight_G;
	CSRight_B = AI_IN[8];
	packet[8] = CSRight_B;
	sum += CSRight_B;
	PositionX = AI_IN[9];
	packet[9] = PositionX;
	sum += PositionX;
	PositionY = AI_IN[10];
	packet[10] = PositionY;
	sum += PositionY;
	TM_State = AI_IN[11];
	packet[11] = TM_State;
	sum += TM_State;
	Compass = AI_IN[12];
	packet[12] = Compass;
	sum += Compass;
	Time = AI_IN[13];
	packet[13] = Time;
	sum += Time;
	packet[14] = sum;

	In = AI_IN;

}

DLL_EXPORT void GetCommand(int *AI_OUT) {
	AI_OUT[0] = WheelLeft;
	AI_OUT[1] = WheelRight;
	AI_OUT[2] = LED_1;
	AI_OUT[3] = MyState;

	std::cout << "test" << std::endl;

	Out = AI_OUT;
}

volatile int *Out = nullptr;
volatile int *In = nullptr;
std::thread *Thread = nullptr;
int InitialisingState = 0;
bool RunUpdateLoop = true;

void Update() {
	switch (CurGame) {
		case 0:
			Game0();
			break;
		case 1:
			Game1();
			break;
		default: MISC_WARNING("CurGame has a non logical value: " << CurGame)
	}
}

void UpdateLoop() {
	RunUpdateLoop = true;

	MISC_LOG("Starting UpdateLoop")

	while (RunUpdateLoop) {

		std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();

		Update();

		auto timeDif = std::chrono::duration_cast<std::chrono::milliseconds>(begin - std::chrono::high_resolution_clock::now());
		auto maxTime = std::chrono::duration<int, std::milli>(MINIMUM_TIME_BETWEEN_CYCLE);
		auto waitingTime = maxTime - timeDif;

		MISC_LOG("Waiting " << waitingTime.count() << " milliseconds")
		std::this_thread::sleep_for(maxTime - timeDif);
	}

	MISC_LOG("Exit UpdateLoop")
}

void EndUpdateLoop() {
	RunUpdateLoop = false;
	if (Thread) {
		Thread->join();
	}
}

DLL_EXPORT void OnTimer() {
	switch (InitialisingState) {
		case 0:
			if (In) {
				InitialisingState = 1;
			}
			break;
		case 1:
			if (Out) {
				InitialisingState = 2;
			}
		case 2:
			Setup();
			InitialisingState = 3;
		case 3:
			// start loop

			static std::thread t(&UpdateLoop);
			Thread = &t;
			InitialisingState = -1;
			break;
		default:
			break;
	}
}