#pragma once

#include <Windows.h>
#include<iostream>
#include <psapi.h>
#include <dwmapi.h>
#include <d3d9.h>

#include "Memory.h"
#include "Offsets.h"
#include "Aimbot.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

#define M_PI 3.14159265358979323846
#define OFF_MEMB(type, name, offset)\
struct {\
    char zpad##name[offset];\
    type name;\
}\

struct XMFLOAT3 {
    float x, y, z;
};




struct CurrentProcess {
	DWORD ID;
	HANDLE Handle;
	HWND Hwnd;
	WNDPROC WndProc;
	int WindowWidth;
	int WindowHeight;
	int WindowLeft;
	int WindowRight;
	int WindowTop;
	int WindowBottom;
	LPCSTR Title;
	LPCSTR ClassName;
	LPCSTR Path;
}Process;

struct OverlayWindow {
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCSTR Name;
}Overlay;

struct DirectX9Interface {
	IDirect3D9Ex* IDirect3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParameters = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
}DirectX9;

std::string RandomString(int len) {
    srand(time(NULL));
    std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string newstr;
    int pos;
    while (newstr.size() != len) {
        pos = ((rand() % (str.size() - 1)));
        newstr += str.substr(pos, 1);
    }
    return newstr;
}

static const char* KeyNames[] = {
    "OFF",//0
    "VK_X1",//A
    "VK_X2",//B
    "VK_LBUTTON",//1
    "VK_RBUTTON",//2
    "VK_TAB",//3
    "VK_SHIFT",//4
    "VK_LSHIFT",//5
    "VK_RSHIFT",//6
    "VK_CONTROL",//7
    "VK_LCONTROL",//8
    "VK_RCONTROL",//9
    "VK_RMENU",//C
    "VK_CAPITAL",//D
};

int KeyCodes(int Key) {
    if (KeyNames[Key] == "VK_LBUTTON") {
        return 0x0001;
    } else if (KeyNames[Key] == "VK_RBUTTON") {
        return 0x0002;
    } else if (KeyNames[Key] == "VK_TAB") {
        return 0x0009;
    } else if (KeyNames[Key] == "VK_SHIFT") {
        return 0x10;
    } else if (KeyNames[Key] == "VK_LSHIFT") {
        return 0x00A0;
    } else if (KeyNames[Key] == "VK_RSHIFT") {
        return 0x00A1;
    } else if (KeyNames[Key] == "VK_CONTROL") {
        return 0x11;
    } else if (KeyNames[Key] == "VK_LCONTROL") {
        return 0x00A2;
    } else if (KeyNames[Key] == "VK_RCONTROL") {
        return 0x00A3;
    } else if (KeyNames[Key] == "VK_X1") {
        return 0x05;
    } else if (KeyNames[Key] == "VK_X2") {
        return 0x06;
    } else if (KeyNames[Key] == "VK_RMENU") {
        return 0x00A5;
    }  else if (KeyNames[Key] == "VK_CAPITAL") {
        return 0x0014;
    } else {
        return 0;
    }
}

struct Matrix {
    float M[4][4];
};

struct Quat {
    float x, y, z, w;
};

struct Vector3 {

    float x, y, z;

    Vector3() : x(0.f), y(0.f), z(0.f) {}

    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    ~Vector3() {}

    Vector3 operator+(Vector3 v) {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-(Vector3 v) {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator*(float ape) {
        return { x * ape, y * ape, z * ape };
    }

    Vector3 operator/(float ape) {
        return { x / ape, y / ape, z / ape };
    }

    Vector3 operator/=(float ape) {
        x /= ape;
        y /= ape;
        z /= ape;

        return *this;
    }

    Vector3 operator+=(Vector3 ape) {
        return { x += ape.x, y += ape.y, z += ape.z };
    }

    Vector3 operator-=(Vector3 ape) {
        return { x -= ape.x, y -= ape.y, z -= ape.z };
    }

    static float Dot(Vector3 lhs, Vector3 rhs) {
        return (((lhs.x * rhs.x) + (lhs.y * rhs.y)) + (lhs.z * rhs.z));
    }

    static float Distance(Vector3 a, Vector3 b) {
        Vector3 vector = Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        return sqrt(((vector.x * vector.x) + (vector.y * vector.y)) + (vector.z * vector.z));
    }
};

struct Vector2 {
    float x;
    float y;

    Vector2() {
        this->x = 0;
        this->y = 0;
    }

    Vector2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    static Vector2 Zero() {
        return Vector2(0.0f, 0.0f);
    }

    bool operator!=(const Vector2& src) const {
        return (src.x != x) || (src.y != y);
    }

    Vector2& operator+=(const Vector2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline float Distance(Vector2 v) {
        return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0)));
    }
};

class Rect {
public:
    float x;
    float y;
    float width;
    float height;

    Rect() {
        this->x = 0;
        this->y = 0;
        this->width = 0;
        this->height = 0;
    }

    Rect(float x, float y, float width, float height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    bool operator==(const Rect& src) const {
        return (src.x == this->x && src.y == this->y && src.height == this->height && src.width == this->width);
    }

    bool operator!=(const Rect& src) const {
        return (src.x != this->x && src.y != this->y && src.height != this->height && src.width != this->width);
    }
};

void Help(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()){
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

int OutsideBorder(Vector2 Position, Vector2 Screen) {
    Vector2 TopLeft(Screen.x * 0.04f, Screen.y * 0.04f);
    Vector2 BottomRight(Screen.x * 0.99f, Screen.y * 0.99f);

    int result = 0;
    if (Position.y < TopLeft.y) {
        result |= 1;
    }
    if (Position.x > BottomRight.x) {
        result |= 2;
    }
    if (Position.y > BottomRight.y) {
        result |= 4;
    }
    if (Position.x < TopLeft.x) {
        result |= 8;
    }
    return result;
}

Vector2 GetBorderPos(Vector2 Position, Vector2 Screen, int SizeOffset) {
    int X = (int)Position.x;
    int Y = (int)Position.y;

    int Borders = OutsideBorder(Position, Screen);

    if ((Borders & 1) == 1) {
        Y = 0 - SizeOffset;
    }
    if ((Borders & 2) == 2) {
        X = (int)Screen.x + SizeOffset;
    }
    if ((Borders & 4) == 4) {
        Y = (int)Screen.y + SizeOffset;
    }
    if ((Borders & 8) == 8) {
        X = 0 - SizeOffset;
    }
    return Vector2(X, Y);
}

Vector2 WorldToScreen(Vector3 worldPos, Matrix viewMatrix, int Width, int Height) {
    float screenX = (viewMatrix.M[0][0] * worldPos.x) + (viewMatrix.M[1][0] * worldPos.y) + (viewMatrix.M[2][0] * worldPos.z) + viewMatrix.M[3][0];
    float screenY = (viewMatrix.M[0][1] * worldPos.x) + (viewMatrix.M[1][1] * worldPos.y) + (viewMatrix.M[2][1] * worldPos.z) + viewMatrix.M[3][1];
    float screenW = (viewMatrix.M[0][3] * worldPos.x) + (viewMatrix.M[1][3] * worldPos.y) + (viewMatrix.M[2][3] * worldPos.z) + viewMatrix.M[3][3];
   
    if (screenW <= 0.1f)
        screenW = 0.1f;

    float camX = Width / 2.0f;
    float camY = Height / 2.0f;

    float x = camX + (camX * screenX / screenW) + screenX / screenW;
    float y = camY - (camY * screenY / screenW) + screenY / screenW;

    if (x < 0 || y < 0 || x >= Width || y >= Height) { return Vector2{}; }

    return { x ,y };
}


struct health_compo_t {
    union {
        OFF_MEMB(std::float_t, health, 0xE0);
        OFF_MEMB(std::float_t, health_max, 0xDC);
        OFF_MEMB(std::float_t, armor, 0x220);
        OFF_MEMB(std::float_t, armor_max, 0x21C);
        OFF_MEMB(std::float_t, barrier, 0x360);
        OFF_MEMB(std::float_t, barrier_max, 0x35C);
    };
};