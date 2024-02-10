#pragma once

#include <Windows.h>
#include <string>

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include "Structures.h"

class Color {
public:
    DWORD R;
    DWORD G;
    DWORD B;
    DWORD A;

    Color() {
        this->R = 0;
        this->G = 0;
        this->B = 0;
        this->A = 0;
    }

    Color(DWORD r, DWORD g, DWORD b, DWORD a) {
        this->R = r;
        this->G = g;
        this->B = b;
        this->A = a;
    }

    Color(DWORD r, DWORD g, DWORD b) {
        this->R = r;
        this->G = g;
        this->B = b;
        this->A = 255;
    }

};

std::string string_To_UTF8(const std::string& str) {
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	std::string retStr(pBuf);
	delete[]pwBuf;
	delete[]pBuf;
	pwBuf = NULL;
	pBuf = NULL;
	return retStr;
}

void DrawString(Vector2 Position, Color color, const char* str) {
    int x = Position.x;
    int y = Position.y;
    ImFont a;
    std::string utf_8_1 = std::string(str);
    std::string utf_8_2 = string_To_UTF8(utf_8_1);
    ImGui::GetForegroundDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), utf_8_2.c_str());
}

void DrawLine(Vector2 From, Vector2 To, Color color, int thickness) {
    int x1 = From.x;
    int y1 = From.y;
    int x2 = To.x;
    int y2 = To.y;

	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), thickness);
}

void DrawCircle(Vector2 Position, int radius, Color color) {
    int x = Position.x;
    int y = Position.y;
	ImGui::GetForegroundDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)), 200);
}

void DrawCircleFilled(Vector2 Position, int radius, Color color) {
    int x = Position.x;
    int y = Position.y;
    ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color.R / 255.0, color.G / 255.0, color.B / 255.0, color.A / 255.0)));
}

void DrawBox(Rect rect, Color color, float thickness) {
    Vector2 v1 = Vector2(rect.x, rect.y);
    Vector2 v2 = Vector2(rect.x + rect.width, rect.y);
    Vector2 v3 = Vector2(rect.x + rect.width, rect.y + rect.height);
    Vector2 v4 = Vector2(rect.x, rect.y + rect.height);

    DrawLine(v1, v2, color, thickness);
    DrawLine(v2, v3, color, thickness);
    DrawLine(v3, v4, color, thickness);
    DrawLine(v4, v1, color, thickness);

}

void DrawCornerBox(Rect rect, Color Outline, Color Original, float thickness) {
    int X = rect.x;
    int Y = rect.y;
    int W = rect.width;
    int H = rect.height;

    float lineW = (W / 5);
    float lineH = (H / 6);
    float lineT = 1;

    //Outline Bounding Box
    DrawLine(Vector2(X - lineT, Y - lineT), Vector2(X + lineW, Y - lineT), Outline, thickness);
    DrawLine(Vector2(X - lineT, Y - lineT), Vector2(X - lineT, Y + lineH), Outline, thickness);
    DrawLine(Vector2(X - lineT, Y + H - lineH), Vector2(X - lineT, Y + H + lineT), Outline, thickness);
    DrawLine(Vector2(X - lineT, Y + H + lineT), Vector2(X + lineW, Y + H + lineT), Outline, thickness);
    DrawLine(Vector2(X + W - lineW, Y - lineT), Vector2(X + W + lineT, Y - lineT), Outline, thickness);
    DrawLine(Vector2(X + W + lineT, Y - lineT), Vector2(X + W + lineT, Y + lineH), Outline, thickness);
    DrawLine(Vector2(X + W + lineT, Y + H - lineH), Vector2(X + W + lineT, Y + H + lineT), Outline, thickness);
    DrawLine(Vector2(X + W - lineW, Y + H + lineT), Vector2(X + W + lineT, Y + H + lineT), Outline, thickness);

    //Inline Bounding Box
    DrawLine(Vector2(X, Y), Vector2(X, Y + lineH), Original, thickness);
    DrawLine(Vector2(X, Y), Vector2(X + lineW, Y), Original, thickness);
    DrawLine(Vector2(X + W - lineW, Y), Vector2(X + W, Y), Original, thickness);
    DrawLine(Vector2(X + W, Y), Vector2(X + W, Y + lineH), Original, thickness);
    DrawLine(Vector2(X, Y + H - lineH), Vector2(X, Y + H), Original, thickness);
    DrawLine(Vector2(X, Y + H), Vector2(X + lineW, Y + H), Original, thickness);
    DrawLine(Vector2(X + W - lineW, Y + H), Vector2(X + W, Y + H), Original, thickness);
    DrawLine(Vector2(X + W, Y + H - lineH), Vector2(X + W, Y + H), Original, thickness);
}

void DrawHealthBar(Vector2 screenPos, float height, float currentHealth, float maxHealth) {
    screenPos += Vector2(8.0f, 0.0f);
    DrawBox(Rect(screenPos.x, screenPos.y, 5.0f, height + 2), Color(0,0,0), 3);
    screenPos += Vector2(1.0f, 1.0f);
    float barHeight = (currentHealth * height) / maxHealth;

    Color HealthColor = Color(10, 255, 10);
    if (currentHealth <= (maxHealth * 0.8)) {
        HealthColor = { 255,255,10 };
    }
    if (currentHealth <= (maxHealth * 0.6)) {
        HealthColor = { 255,150,10 };
    }
    if (currentHealth < (maxHealth * 0.4)) {
        HealthColor = { 255,50,10 };
    }
    if (currentHealth < (maxHealth * 0.2)) {
        HealthColor = { 255,10,10 };
    }
    if (currentHealth < (maxHealth * 0)) {
        HealthColor = { 0,0,0 };
    }
    DrawBox(Rect(screenPos.x, screenPos.y, 3.0f, barHeight), HealthColor, 3);
}

void DrawCrosshair(Vector2 Screen, float Size, Color Color, float thickness) {
    Vector2 Center = Vector2(Screen.x/2, Screen.y/2);
    float x = Center.x - (Size / 2.0f);
    float y = Center.y - (Size / 2.0f);
    DrawLine(Vector2(x, Center.y), Vector2(x + Size, Center.y), Color, thickness);
    DrawLine(Vector2(Center.x, y), Vector2(Center.x, y + Size), Color, thickness);
}