#pragma once
#define __ROL4__(x, n) ((x << n) | (x >> (32 - n)))
#define HIDWORD(_qw)    ((DWORD)(((_qw) >> 32) & 0xffffffff))
#define __ROR4__(x, n) ((x >> n) | (x << (32 - n)))
#define ISVALID(x) ((x) && (x) != INVALID_HANDLE_VALUE)


#include <windows.h>
#include <Windows.h>
#include <thread>
#include <emmintrin.h>
#include <interception.h>
#include <complex>
#include <stdint.h>
#include<iostream>
#include <cstdint>
#include  <cstdio>
#include <windef.h>
#include<mmsystem.h>


InterceptionContext context;
InterceptionDevice device;
InterceptionStroke stroke;

void NormalMouse() {
	while (interception_receive(context, device = interception_wait(context), &stroke, 1) > 0) {
		if (interception_is_mouse(device))
		{
			InterceptionMouseStroke& mstroke = *(InterceptionMouseStroke*)&stroke;
			interception_send(context, device, &stroke, 1);
		}
	}
}

void InitMoveMouse() {
	context = interception_create_context();
	interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_MOVE);
	device = interception_wait(context);

	while (interception_receive(context, device = interception_wait(context), &stroke, 1) > 0) {
		if (interception_is_mouse(device)) {
			InterceptionMouseStroke& mstroke = *(InterceptionMouseStroke*)&stroke;
			interception_send(context, device, &stroke, 1);
			break;
		}
	}
	std::thread normal(NormalMouse);
	normal.detach();
}

void mouse_move(int x, int y) {
	InterceptionMouseStroke& mstroke = *(InterceptionMouseStroke*)&stroke;
	mstroke.flags = 0;
	mstroke.information = 0;
	mstroke.x = x;
	mstroke.y = y;
	interception_send(context, device, &stroke, 1);
}

void mouse_click() {
	InterceptionMouseStroke& mstroke = *(InterceptionMouseStroke*)&stroke;
	mstroke.state = INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN;
	interception_send(context, device, &stroke, 1);
	mstroke.state = INTERCEPTION_MOUSE_LEFT_BUTTON_UP;
	interception_send(context, device, &stroke, 1);
	Sleep(10);
}

void mouse_left_down() {
	InterceptionMouseStroke& mstroke = *(InterceptionMouseStroke*)&stroke;
	mstroke.state = INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN;
	interception_send(context, device, &stroke, 1);
	//Sleep(10);
}

void mouse_right_click() {
	InterceptionMouseStroke& mstroke = *(InterceptionMouseStroke*)&stroke;
	mstroke.state = INTERCEPTION_MOUSE_RIGHT_BUTTON_DOWN;
	interception_send(context, device, &stroke, 1);
	mstroke.state = INTERCEPTION_MOUSE_RIGHT_BUTTON_UP;
	interception_send(context, device, &stroke, 1);
	Sleep(10);
}

/*
void mouse_click() {
	mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, 0, 0); 
	Sleep(10);
}

bool mouse_move(int x, int y) {
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	input.mi.dx = x;
	input.mi.dy = y;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(input));
	return true;
}
*/

bool isInsideFov(int x, int y, int ScreenWidth, int ScreenHeight, int Fov) {
	int centerx = ScreenWidth / 2;
	int centery = ScreenHeight / 2;
	if ((x - centerx) * (x - centerx) + (y - centery) * (y - centery) <= Fov * Fov)
		return true;
	else
		return false;
}

float CenterDist(int x, int y, int ScreenWidth, int ScreenHeight) {
	int centerx = ScreenWidth / 2;
	int centery = ScreenHeight / 2;
	return (x - centerx) * (x - centerx) + (y - centery) * (y - centery);
}

void AimBot(int ScreenWidth, int ScreenHeight, float x, float y, float AimSpeed, int smoothness) {
	int ScreenCenterX = ScreenWidth / 2, ScreenCenterY = ScreenHeight / 2;

	float TargetX = 0;
	float TargetY = 0;
	if (x != 0) {
		if (x > ScreenCenterX) {
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX) {
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0) {
		if (y > ScreenCenterY) {
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY) {
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}
	TargetX /= smoothness;
	TargetY /= smoothness;
	if (isRandomizer) {
		if (abs(TargetX) < 1) {
			if (TargetX > 0)
			{
				TargetX = 1;
			}
			if (TargetX < 0)
			{
				TargetX = -1;
			}
		}
		if (abs(TargetY) < 1) {
			if (TargetY > 0)
			{
				TargetY = 1;
			}
			if (TargetY < 0)
			{
				TargetY = -1;
			}
		}
	}
	mouse_move((int)TargetX, (int)(TargetY));
}
void FlickBot(int ScreenWidth, int ScreenHeight, float x, float y, float AimSpeed, int smoothness) {
	int ScreenCenterX = ScreenWidth / 2, ScreenCenterY = ScreenHeight / 2;

	float TargetX = 0;
	float TargetY = 0;
	if (x != 0) {
		if (x > ScreenCenterX) {
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX) {
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0) {
		if (y > ScreenCenterY) {
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY) {
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}
	TargetX /= smoothness;
	TargetY /= smoothness;

	if (isRandomizer) {
		if (abs(TargetX) < 1) {
			if (TargetX > 0)
			{
				TargetX = 1;
			}
			if (TargetX < 0)
			{
				TargetX = -1;
			}
		}
		if (abs(TargetY) < 1) {
			if (TargetY > 0)
			{
				TargetY = 1;
			}
			if (TargetY < 0)
			{
				TargetY = -1;
			}
		}
	}
	mouse_move((int)TargetX, (int)(TargetY));
}
void Correct1(int ScreenWidth, int ScreenHeight, float x, float y, float AimSpeed, int smoothness) {
	int ScreenCenterX = ScreenWidth / 2, ScreenCenterY = ScreenHeight / 2;

	float TargetX = 0;
	float TargetY = 0;

	if (x != 0) {
		if (x > ScreenCenterX) {
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX) {
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0) {
		if (y > ScreenCenterY) {
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY) {
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}
	TargetX /= smoothness;
	TargetY /= smoothness;

	if (isRandomizer) {
		if (abs(TargetX) < 1) {
			if (TargetX > 0)
			{
				TargetX = 1;
			}
			if (TargetX < 0)
			{
				TargetX = -1;
			}
		}
		if (abs(TargetY) < 1) {
			if (TargetY > 0)
			{
				TargetY = 1;
			}
			if (TargetY < 0)
			{
				TargetY = -1;
			}
		}
	}
	mouse_move((int)2*TargetX, (int)(2*TargetY));
	mouse_click();
}
void Correct(int ScreenWidth, int ScreenHeight, float x, float y, float AimSpeed, int smoothness) {
	int i;
		Correct1(ScreenWidth, ScreenHeight, x, y, AimSpeed, smoothness);

	int ScreenCenterX = ScreenWidth / 2, ScreenCenterY = ScreenHeight / 2;

	float TargetX = 0;
	float TargetY = 0;

	if (x != 0) {
		if (x > ScreenCenterX) {
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX) {
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0) {
		if (y > ScreenCenterY) {
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY) {
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}
	TargetX /= smoothness;
	TargetY /= smoothness;

	if (isRandomizer) {
		if (abs(TargetX) < 1) {
			if (TargetX > 0)
			{
				TargetX = 1;
			}
			if (TargetX < 0)
			{
				TargetX = -1;
			}
		}
		if (abs(TargetY) < 1) {
			if (TargetY > 0)
			{
				TargetY = 1;
			}
			if (TargetY < 0)
			{
				TargetY = -1;
			}
		}
	}
	for(i = 0; i < 5; i++) {
		mouse_move((int)( -2* TargetX), (int)(-2*TargetY));
	}
	
}

void instantflick(int ScreenWidth, int ScreenHeight, float x, float y, int InstantFlickSpeed,bool Silent) {
	float mulx = 4.36 / (ScreenWidth / 1920.0f);
	float muly = 3.25 / (ScreenHeight / 1080.0f);
	x = x - ScreenWidth / 2;
	y = y - ScreenHeight / 2;
	if (abs(x) > ScreenWidth / 3.5) {
		mulx = 3.28 / (ScreenWidth / 1920.0f);
	}
	x = x * mulx / (0.1 * InstantFlickSpeed / 2.5f);
	y = y * muly / (0.1 * InstantFlickSpeed / 2.5f);
	mouse_move(x, y );
	mouse_click();
	Sleep(10);
	if (Silent) {
		mouse_move(-(InstantFlickSpeed / (InstantFlickSpeed*15/55)) * x, -(InstantFlickSpeed / (InstantFlickSpeed * 15 / 55)) * y);
		Sleep(10);
	}
}

/*
bool GetGlobalKey() {
	static bool is_found = false;
	static auto key_sig = (BYTE*)"\x00\x00\x00\x00\x80\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x3f";
	static auto key_mask = "x???xx?xx?????xxxxxx";
	if (is_found == false) {
			GlobalKey1 = Read<uint64_t>(ModuleBase +0x2E56BC8 - 0x150);
			GlobalKey2 = Read<uint64_t>(ModuleBase + 0x2E56BC8 - 0x120);
			return true;
	}
}

uint64_t __fastcall DecryptVis(unsigned __int64 a1)
{
	unsigned __int64 v9; // rdi
	unsigned __int64 v10; // r8
	unsigned __int64 v11; // rax
	unsigned __int64 v12; // rbx
	unsigned __int64 v13; // rcx
	unsigned __int64 v14; // rcx
	__m128i v15; // xmm1
	__m128i v16; // xmm2
	__m128i v17; // xmm0
	__m128i v18; // xmm1

	v9 = (unsigned __int64)ModuleBase + 0x7BCC02;
	v10 = v9 + 8;
	v11 = 0i64;

	v12 = Read<uint64_t>(ModuleBase + 0x3B30E80 +
		8 * (((BYTE)a1 - 126) & 0x7F)
		+ (((a1 - 0x6CEB2EB08AE38E7Ei64) >> 7) & 7)) ^ v9 ^ (a1 - 0x6CEB2EB08AE38E7Ei64);

	v13 = ((unsigned __int64)v10 - v9 + 7) >> 3;
	if (v9 > (unsigned __int64)v10)
		v13 = 0i64;
	if (v13 && v13 >= 4)
	{
		v14 = v13 & 0xFFFFFFFFFFFFFFFCui64;
		v15 = {};
		v16 = {};
		do
		{
			v11 += 4i64;
			v15 = _mm_xor_si128(v15, _mm_loadu_si128((const __m128i*)v9));
			v17 = _mm_loadu_si128((const __m128i*)(v9 + 16));
			v9 += 32i64;
			v16 = _mm_xor_si128(v16, v17);
		} while (v11 < v14);
		v18 = _mm_xor_si128(v15, v16);
		v12 ^= _mm_xor_si128(v18, _mm_srli_si128(v18, 8)).m128i_u64[0];
	}
	for (; v9 < (unsigned __int64)v10; v9 += 8i64)
		v12 ^= Read<uint64_t>(v9);

	return v12 ^ ~v10 ^ 0x9314D14F751C7182i64;
}*/

void SolveQuartic(const std::complex<float> coefficients[5], std::complex<float> roots[4]) {
	const std::complex<float> a = coefficients[4];
	const std::complex<float> b = coefficients[3] / a;
	const std::complex<float> c = coefficients[2] / a;
	const std::complex<float> d = coefficients[1] / a;
	const std::complex<float> e = coefficients[0] / a;

	const std::complex<float> Q1 = c * c - 3.f * b * d + 12.f * e;
	const std::complex<float> Q2 = 2.f * c * c * c - 9.f * b * c * d + 27.f * d * d + 27.f * b * b * e - 72.f * c * e;
	const std::complex<float> Q3 = 8.f * b * c - 16.f * d - 2.f * b * b * b;
	const std::complex<float> Q4 = 3.f * b * b - 8.f * c;

	const std::complex<float> Q5 = std::pow(Q2 / 2.f + std::sqrt(Q2 * Q2 / 4.f - Q1 * Q1 * Q1), 1.f / 3.f);
	const std::complex<float> Q6 = (Q1 / Q5 + Q5) / 3.f;
	const std::complex<float> Q7 = 2.f * std::sqrt(Q4 / 12.f + Q6);

	roots[0] = (-b - Q7 - std::sqrt(4.f * Q4 / 6.f - 4.f * Q6 - Q3 / Q7)) / 4.f;
	roots[1] = (-b - Q7 + std::sqrt(4.f * Q4 / 6.f - 4.f * Q6 - Q3 / Q7)) / 4.f;
	roots[2] = (-b + Q7 - std::sqrt(4.f * Q4 / 6.f - 4.f * Q6 + Q3 / Q7)) / 4.f;
	roots[3] = (-b + Q7 + std::sqrt(4.f * Q4 / 6.f - 4.f * Q6 + Q3 / Q7)) / 4.f;
}


typedef unsigned long long QWORD;
typedef unsigned long DWORD;
typedef unsigned char _BYTE;
typedef unsigned int _DWORD;
typedef unsigned __int64 _QWORD;


unsigned __int64 __fastcall DecryptVis(unsigned __int64 a1)
{
	unsigned __int64 v9; // rdi
	unsigned __int64 v10; // r8
	unsigned __int64 v11; // rax
	unsigned __int64 v12; // rbx
	unsigned __int64 v13; // rcx
	unsigned __int64 v14; // rcx
	__m128i v15; // xmm1
	__m128i v16; // xmm2
	__m128i v17; // xmm0
	__m128i v18; // xmm1

	v9 = (unsigned __int64)ModuleBase + 0x7B5CC2;
	v10 = v9 + 8;
	v11 = 0i64;

	v12 = Read<uint64_t>(ModuleBase+ 0x3B2A7C0 +
		8 * (((_BYTE)a1 - 0x60) & 0x7F)
		+ (((a1 - 0x2833B934DB53AA60i64) >> 7) & 7)) ^ v9 ^ (a1 - 0x2833B934DB53AA60i64);

	v13 = ((unsigned __int64)v10 - v9 + 7) >> 3;
	if (v9 > (unsigned __int64)v10)
		v13 = 0i64;
	if (v13 && v13 >= 4)
	{
		v14 = v13 & 0xFFFFFFFFFFFFFFFCui64;
		v15 = {};
		v16 = {};
		do
		{
			v11 += 4i64;
			v15 = _mm_xor_si128(v15, _mm_loadu_si128((const __m128i*)v9));
			v17 = _mm_loadu_si128((const __m128i*)(v9 + 16));
			v9 += 32i64;
			v16 = _mm_xor_si128(v16, v17);
		} while (v11 < v14);
		v18 = _mm_xor_si128(v15, v16);
		v12 ^= _mm_xor_si128(v18, _mm_srli_si128(v18, 8)).m128i_u64[0];
	}
	for (; v9 < (unsigned __int64)v10; v9 += 8i64)
		v12 ^= Read<uint64_t>(v9);

	return v12 ^ ~v10 ^ 0xD7CC46CB24AC55A0ui64;
}


void sub_21964C0(_QWORD* a1, __int64* a2, __int64* a3, _DWORD* a4, uint8_t componentid)
{
	*a1 = (uint64_t)1 << (uint64_t)(componentid & 0x3F);
	*a2 = *a1 - 1;
	*a3 = componentid;
	*a4 = componentid / 0x3F;

}


uint64_t DecryptComponent(uint64_t parent, uint8_t componentid)
{

	unsigned __int64 v2; // rbx
	__int64 v3; // rbp
	unsigned __int64 v4; // rdx
	unsigned __int64 v5; // rbp
	unsigned __int64 v6; // rax
	__int64 v7; // rbx
	__int64 v8; // r8
	unsigned __int64 v9; // rdx
	unsigned __int64 v10; // rsi
	__int64 v11; // rax
	_QWORD* v12; // rbx
	__int64 v13; // [rsp+20h] [rbp-48h] BYREF
	unsigned __int64 v14; // [rsp+28h] [rbp-40h] BYREF
	__int64 v15; // [rsp+30h] [rbp-38h] BYREF
	unsigned int v16; // [rsp+70h] [rbp+8h] BYREF
	__int64 v17; // [rsp+80h] [rbp+18h] BYREF
	unsigned __int64 v18; // [rsp+88h] [rbp+20h] BYREF

	v2 = parent;
	sub_21964C0(&v14, &v15, &v17, &v16, componentid);
	v3 = Read<uint64_t>(v2 + 8i64 * v16 + 240);
	v4 = v15 & v3;
	v5 = (v14 & v3) >> v17;
	v6 = ((v4 - ((v4 >> 1) & 0x5555555555555555i64)) & 0x3333333333333333i64)
		+ (((v4 - ((v4 >> 1) & 0x5555555555555555i64)) >> 2) & 0x3333333333333333i64);
	v7 = Read<uint64_t>(v2 + 96)
		+ 8
		* (Read<unsigned __int8>(v16 + v2 + 272)
			+ ((0x101010101010101i64 * ((v6 + (v6 >> 4)) & 0xF0F0F0F0F0F0F0Fi64)) >> 56));
	HANDLE decv7 = (HANDLE)(uintptr_t)v7;
	if (!ISVALID(decv7)) return NULL;
	v7 = Read<uint64_t>(v7);

	v13 = uint64_t(0xfadd01d4cc0838ae);
	v18 = uint64_t(0xbdc1e89ca2952602);

	v8 = v7;
	v9 = (unsigned int)v8 | v8 & 0xFFFFFFFF00000000ui64 ^ (((unsigned int)v8 ^ (unsigned __int64)Read<unsigned int>(ModuleBase + 0x3B297A0 + (v18 & 0xFFF))) << 32);
	v10 = -(int)v5 & ((unsigned int)v13 ^ (unsigned int)v8 | ((unsigned int)v13 ^ (unsigned int)v9 |
		(v13 ^ ((unsigned int)v9 | v9 & 0xFFFFFFFF00000000ui64 ^ ((unsigned __int64)(unsigned int)(1570444788 - v9) << 32)))
		& 0xFFFFFFFF00000000ui64 ^ ((unsigned __int64)(__ROR4__(Read<uint64_t>(ModuleBase + 0x3B297A0 + (v18 >> 52)), 11)
			^ ~((unsigned int)v13 ^ (unsigned int)v9)) << 32)) & 0xFFFFFFFF00000000ui64 ^
		((unsigned __int64)(2 * ((unsigned int)v13 ^ (unsigned int)v8) - __ROL4__(HIDWORD(Read<uint64_t>(ModuleBase + 0x3B297A0 + (v18 & 0xFFF))), 9)) << 32));

	return v10;
}