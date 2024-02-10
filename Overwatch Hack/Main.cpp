#include "Main.h"
#include <io.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <winhttp.h>
#include <fstream>
#include <string>
#include <direct.h>
#include <TlHelp32.h>
#include <vector>
#include <process.h>
#include <thread>
#include <bitset>
#include <mutex>
#include <array>
#include <dwmapi.h>
#include <atlstr.h>
#include <stdlib.h>
#include<string>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include "SignatureScanner.h"
using namespace std;
DWORD GetDwordFromBytes(BYTE* B, bool LittleEndian)
{
	if (!LittleEndian) { return (B[3]) | (B[2] << 8) | (B[1] << 16) | (B[0] << 24); }
	else { return (B[0]) | (B[1] << 8) | (B[2] << 16) | (B[3] << 24); }
}
void InputHandler() {
	for (int i = 0; i < 5; i++) {
		ImGui::GetIO().MouseDown[i] = false;
	}

	int Button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) {
		Button = 0;
	}

	if (Button != -1) {
		ImGui::GetIO().MouseDown[Button] = true;
	}
}

void Render() {
	if (GetAsyncKeyState(VK_INSERT) & 1) ShowMenu = !ShowMenu;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	HackRender();
	ImGui::GetIO().MouseDrawCursor = ShowMenu;

	if (ShowMenu == true) {
		InputHandler();
		MenuWindow();
		SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
		UpdateWindow(Overlay.Hwnd);
		SetFocus(Overlay.Hwnd);
	}
	else {
		SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
		UpdateWindow(Overlay.Hwnd);
	}
	ImGui::EndFrame();

	DirectX9.pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9.pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		DirectX9.pDevice->EndScene();
	}

	HRESULT result = DirectX9.pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9.pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		DirectX9.pDevice->Reset(&DirectX9.pParameters);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

void MainLoop() {
	static RECT OldRect;
	ZeroMemory(&DirectX9.Message, sizeof(MSG));
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 17.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	io.ImeWindowHandle = Process.Hwnd;
	while (DirectX9.Message.message != WM_QUIT) {
		if (PeekMessage(&DirectX9.Message, Overlay.Hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&DirectX9.Message);
			DispatchMessage(&DirectX9.Message);
		}
		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == Process.Hwnd) {
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(Overlay.Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(Process.Hwnd, &TempRect);
		ClientToScreen(Process.Hwnd, &TempPoint);

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		

		if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			Process.WindowWidth = TempRect.right;
			Process.WindowHeight = TempRect.bottom;
			DirectX9.pParameters.BackBufferWidth = Process.WindowWidth;
			DirectX9.pParameters.BackBufferHeight = Process.WindowHeight;
			SetWindowPos(Overlay.Hwnd, (HWND)0, TempPoint.x, TempPoint.y, Process.WindowWidth, Process.WindowHeight, SWP_NOREDRAW);
			DirectX9.pDevice->Reset(&DirectX9.pParameters);
		}
		Render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9.pDevice != NULL) {
		DirectX9.pDevice->EndScene();
		DirectX9.pDevice->Release();
	}
	if (DirectX9.IDirect3D9 != NULL) {
		DirectX9.IDirect3D9->Release();
	}
	DestroyWindow(Overlay.Hwnd);
	UnregisterClass(Overlay.WindowClass.lpszClassName, Overlay.WindowClass.hInstance);
}

bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9.IDirect3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = Overlay.Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = Process.WindowWidth;
	Params.BackBufferHeight = Process.WindowHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	if (FAILED(DirectX9.IDirect3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Overlay.Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9.pDevice))) {
		DirectX9.IDirect3D9->Release();
		return false;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(Overlay.Hwnd);
	ImGui_ImplDX9_Init(DirectX9.pDevice);
	DirectX9.IDirect3D9->Release();
	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message) {
	case WM_DESTROY:
		if (DirectX9.pDevice != NULL) {
			DirectX9.pDevice->EndScene();
			DirectX9.pDevice->Release();
		}
		if (DirectX9.IDirect3D9 != NULL) {
			DirectX9.IDirect3D9->Release();
		}
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (DirectX9.pDevice != NULL && wParam != SIZE_MINIMIZED) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9.pParameters.BackBufferWidth = LOWORD(lParam);
			DirectX9.pParameters.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = DirectX9.pDevice->Reset(&DirectX9.pParameters);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void SetupWindow() {
	Overlay.WindowClass = {
		sizeof(WNDCLASSEX), 0, WinProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, Overlay.Name, LoadIcon(nullptr, IDI_APPLICATION)
	};

	RegisterClassEx(&Overlay.WindowClass);
	if (Process.Hwnd) {
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(Process.Hwnd, &TempRect);
		ClientToScreen(Process.Hwnd, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		Process.WindowWidth = TempRect.right;
		Process.WindowHeight = TempRect.bottom;
	}

	Overlay.Hwnd = CreateWindowEx(NULL, Overlay.Name, Overlay.Name, WS_POPUP | WS_VISIBLE, Process.WindowLeft, Process.WindowTop, Process.WindowWidth, Process.WindowHeight, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(Overlay.Hwnd, &DirectX9.Margin);
	SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	ShowWindow(Overlay.Hwnd, SW_SHOW);
	UpdateWindow(Overlay.Hwnd);
}

DWORD WINAPI ProcessCheck(LPVOID lpParameter) {
	Entity = FindPatternsExRegs(Offsets::AOB, Offsets::MASK, Offsets::EntitySize);
	while (true) {
		if (isKeepUpdated) {
			Entity = FindPatternsExRegs(Offsets::AOB, Offsets::MASK, Offsets::EntitySize);

		}
		if (Process.Hwnd != NULL) {
			if (GetProcessID(TargetProcess) == 0) {
				CloseHandle(ProcessHandle);
				exit(0);
			}
		}
		Sleep(10);
	}
}

int main() {
	ProcessId = GetProcessID(TargetProcess);
	ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, ProcessId);
	ModuleBase = GetBaseAddress(ProcessId, TargetProcess);
    
	std::cout << "Scanning for VM...";
	SignatureScanner scanner = SignatureScanner(/*process:*/ProcessHandle, /*start address:*/ModuleBase/*,end address: (optional)*/);
	int Size_of_in = 7;
	std::vector<BYTE> Data;
	Data.resize(Size_of_in);
	uintptr_t result = scanner.scanEx("48 8B 0D ? ? ? ? 45 33 C0 E8 ? ? ? ? 48 8B F0"); // accepts both ? and ?? wildcards, uppercase and lowercase
	ReadProcessMemory(ProcessHandle, (void*)(result), &Data.front(), Size_of_in, NULL);
	printf("\nUnEncrypted VM: 0x%llX", result + (uintptr_t)GetDwordFromBytes(&Data[3], 1) - ModuleBase + Size_of_in);
	Offsets::ViewMatrix = result + (uintptr_t)GetDwordFromBytes(&Data[3], 1) - ModuleBase + Size_of_in;
	int file1=access("C:\\Windows\\win.txt", 0);
	int file2 = access("C:\\xj.ini", 0);
	if (file1 != 0 || file2 != 0) {
		//return 0;
	}

	bool WindowFocus = false;
	while (WindowFocus == false) {
		//DWORD ForegroundWindowProcessID;
		//GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
		//if (GetProcessID(TargetProcess) == ForegroundWindowProcessID) {
		HWND hwnd = FindWindow("TankWindowClass", NULL);
		if (hwnd == NULL) return 0;
			//Process.ID = GetCurrentProcessId();
		GetWindowThreadProcessId(hwnd, &Process.ID);
			Process.Handle = GetCurrentProcess();
			Process.Hwnd = hwnd;

			RECT TempRect;
			GetWindowRect(Process.Hwnd, &TempRect);
			Process.WindowWidth = TempRect.right - TempRect.left;
			Process.WindowHeight = TempRect.bottom - TempRect.top;
			Process.WindowLeft = TempRect.left;
			Process.WindowRight = TempRect.right;
			Process.WindowTop = TempRect.top;
			Process.WindowBottom = TempRect.bottom;

			char TempTitle[MAX_PATH];
			GetWindowText(Process.Hwnd, TempTitle, sizeof(TempTitle));
			Process.Title = TempTitle;

			char TempClassName[MAX_PATH];
			GetClassName(Process.Hwnd, TempClassName, sizeof(TempClassName));
			Process.ClassName = TempClassName;

			char TempPath[MAX_PATH];
			GetModuleFileNameEx(Process.Handle, NULL, TempPath, sizeof(TempPath));
			Process.Path = TempPath;

			InitMoveMouse();

			WindowFocus = true;
		//}
	}

	if (CreateConsole == false) {
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	}
	else {
		//cout << "»¶Ó­Ê¹ÓÃImperial." << Process.Title << endl;
		cout << "Welcome." << endl;
		cout << "Loading..." << endl;
		cout << "Finish." << endl;
	}
	Overlay.Name = RandomString(10).c_str();
	SetupWindow();
	DirectXInit();
	CreateThread(0, 0, ProcessCheck, 0, 0, 0);
	while (TRUE) {
		MainLoop();
	}
}
