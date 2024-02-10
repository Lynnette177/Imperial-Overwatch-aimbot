#pragma once
#include <atlstr.h>
#include <Windows.h>

LPCSTR TargetProcess = "Overwatch.exe";

LPCSTR MenuName = "AFREEONE";

std::vector<uintptr_t> Entity{};

int index = -1;


//int Dec = 15;
//int InstantFlickSpeed = 45;
//int FovCircle = 150, MSpeed = 1, Smoothness = 2, Bone = 1;

int Dec = GetPrivateProfileInt(_T("Aim Section"), _T("ShootingRange"), 15, _T(".\\config.ini"));
int Dec1 = Dec;
int FovCircle = GetPrivateProfileInt(_T("Aim Section"), _T("Fov"), 150, _T(".\\config.ini"));
int MSpeed = GetPrivateProfileInt(_T("Aim Section"), _T("AimSpeed"), 1, _T(".\\config.ini"));
int Smoothness = GetPrivateProfileInt(_T("Aim Section"), _T("Smooth"), 1, _T(".\\config.ini"));
int Bone = GetPrivateProfileInt(_T("Aim Section"), _T("Bone"), 2, _T(".\\config.ini"));
int InstantFlickSpeed = GetPrivateProfileInt(_T("Aim Section"), _T("SilentSpeed"), 50, _T(".\\config.ini"));
int FovCircle1 = FovCircle;
int shootrangeint= GetPrivateProfileInt(_T("Aim Section"), _T("Trigger Coefficient"), 200, _T(".\\config.ini"));
int shoottime = GetPrivateProfileInt(_T("Aim Section"), _T("Shooting time"), 1000, _T(".\\config.ini"));


TCHAR buf[100];


float AimSpeed = 5;
float sHootingcoefficient = ((float)shootrangeint)/1000;
/********** Menu **********/
int Tab = 0, SelectedKey = 0;

int cap = 0;
int re = 0;
int reeee = 0;

bool ShowMenu = true, CreateConsole = false;
bool isInstantFLick = false;
bool isAutoShoot = true;

bool isRightflick = false;

int realDistance[100] = { 0 };
int ownhead = 0;
bool isdistance = false;

int pre = 0;

int travelspeed = 110;

bool isShowPostoPre = false;
bool isShowDIs = false;
bool isPrediction = false;
bool hanzouEpre = false;
bool enableHanzoE = false;
bool hanzoleft = false;
int hleftdec = 0;

bool ishanzonassist = false;
DWORD startTime;
DWORD totalTime;
/********** Cheats **********/
//In Game
bool isKeepUpdated = true, isTeam;
bool isAssistant = false;

bool Silent = false;

//Esp
bool isCrossHairShow = false, isChangeTeam = true, isEspLines = true, isEspBox = true;
bool isShowIQ = true;
bool isIQBOOST = false;

const char* BoxTypeName = "Box Type";
const char* LineTypeName = "Default";

float IQ = 300;

int CrossHairSize = 40, CrossHairThick = 3, LineType = 1, BoxType = 2;

//Aimbot
int RR = 40; int  GG = 255; int BB = 0, colourpn = 0;
int k = 0;
int distantx[100];
int distanty[100];
int distant[100];
int ifthefirst = 1;
bool isReleasey = false;
bool isLiner = false;

const char* BoneName = "Head";

bool isGenjiauto = false;
int genjid = 15;
int tempsmooth = 2;
int genjismooth = 2;

bool isAimbot = true, isRandomizer = false, isBoneid = false, isTracking = false, isFlickBot = false, isTriggerBot = false, isCorrection = false, ShineColour = true;
bool isFov = true;
bool isAutoscaleFOV = true;
bool isAutoshootrange = true;
bool RAGE = false;
bool gravityPre = false;
int colourorno = 0, colourfreq = 10, redpn = 0;


int lasttime = 0;
unsigned long thest[100] = { 0 };
unsigned long lasst[100] = { 0 };
double velox[100] = { 0 };
double veloy[100] = { 0 };
double veloz[100] = { 0 };
double xold[100] = { 0 };
double yold[100] = { 0 };
double zold[100] = { 0 };

uint64_t GlobalKey1, GlobalKey2;

namespace Offsets {
	BYTE AOB[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\x02\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00";
	std::string MASK = "??xx??xx?xxx?xxxxxxxxxxxxxxx???";
	uintptr_t ViewMatrix = 0x0;
	enum Offsets {
		EntitySize = 0x180000,

		BorderLine = 0xE9A9D4,//0xE9A518,//0xEAD060,

		//0x4172EA8,//0x4216A98, //viewmatrix_base
		ViewMatrix_Ptr = 0x7E0, //0x7E0 VM BASE OFFSET

		Origin = 0x9C,
		Head = 0x8C,

		Alive = 0x1,
		Alive_Ret = 0x14,

		Team = 0x4,
		EnemyTeam = 0x8,
		OwnTeam = 0x10,
	};
}