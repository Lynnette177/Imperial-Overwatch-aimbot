#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <time.h>
#include <tchar.h>
#include <iostream>
#include <vector>

using namespace std;

DWORD ProcessId;
HANDLE ProcessHandle;
uintptr_t ModuleBase;

int GetProcessID(const char* module_name) {
	int pid = 0;
	int threadCount = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnap, &pe);
	while (Process32Next(hSnap, &pe)) {
		if (_tcsicmp(pe.szExeFile, _T(module_name)) == 0) {
			if ((int)pe.cntThreads > threadCount) {
				threadCount = pe.cntThreads;
				pid = pe.th32ProcessID;
			}
		}
	}

	return pid;
}

uintptr_t GetBaseAddress(DWORD Pid, const char* TargetProcess) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, Pid);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!strcmp(modEntry.szModule, TargetProcess)) {
					CloseHandle(hSnap);
					return (uintptr_t)modEntry.modBaseAddr;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
}

template<typename T> T Read(SIZE_T address) {
	T buffer;
	ReadProcessMemory(ProcessHandle, (LPCVOID)address, &buffer, sizeof(T), NULL);
	return buffer;
}

template <typename T>
constexpr void Write(const uintptr_t& address, const T& value) {
	::WriteProcessMemory(ProcessHandle, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
}

string ReadString(uintptr_t StrPtr, int StrLength) {
	char Names[1028];
	for (int x = 0; x < StrLength; ++x) {
		Names[x] = Read<char>(StrPtr + x);
	}
	Names[StrLength] = '\0';
	string ClsNames = string(Names);
	return ClsNames;
}

//Aob Pattern Scan
std::vector<MEMORY_BASIC_INFORMATION64> mbis;
bool CompressMbis = false;

bool UpdateMemoryQuery() {
	MEMORY_BASIC_INFORMATION64 mbi = { 0, };
	MEMORY_BASIC_INFORMATION64 old = { 0, };
	DWORD64 current_address = 0x7ffe0000;
	std::vector<MEMORY_BASIC_INFORMATION64> addresses;
	while (true) {
		if (!VirtualQueryEx(ProcessHandle, (LPVOID)current_address, (PMEMORY_BASIC_INFORMATION)&mbi, sizeof(MEMORY_BASIC_INFORMATION64)))
			break;
		if ((mbi.State & 0x1000) != 0 && (mbi.Protect & 0x100) == 0) {
			if (old.BaseAddress + old.RegionSize == mbi.BaseAddress && CompressMbis)
				old.RegionSize += mbi.RegionSize;
			else
				addresses.push_back(mbi);

			old = mbi;
		}
		current_address = mbi.BaseAddress + mbi.RegionSize;
	}

	mbis = addresses;

	return (mbis.size() > 0);
}

DWORD64 FindPattern(BYTE* buffer, BYTE* pattern, std::string mask, int bufSize) {
	int pattern_len = mask.length();
	for (int i = 0; i < bufSize - pattern_len; i++) {
		bool found = true;
		for (int j = 0; j < pattern_len; j++) {
			if (mask[j] != '?' && pattern[j] != buffer[(i + j)]) {
				found = false;
				break;
			}
		}
		if (found)
			return i;
	}
	return -1;
}

std::vector<DWORD64> FindPatternEx(DWORD64 start, DWORD64 end, BYTE* pattern, std::string mask, MEMORY_BASIC_INFORMATION64 mbi, DWORD64 RgSize) {
	DWORD64 current_chunk = start;
	std::vector<DWORD64> found;
	if ((end - current_chunk > RgSize && RgSize != 0) || (end - current_chunk < RgSize && RgSize != 0))
		return found;
	while (current_chunk < end) {
		int bufSize = (int)(end - start);
		BYTE* buffer = new BYTE[bufSize];
		if (!ReadProcessMemory(ProcessHandle, (LPVOID)current_chunk, buffer, bufSize, nullptr)) {
			current_chunk += bufSize;
			delete[] buffer;
			continue;
		}

		DWORD64 internal_address = FindPattern(buffer, pattern, mask, bufSize);

		if (internal_address != -1) {
			found.push_back(current_chunk + internal_address);
		}
		current_chunk += bufSize;
		delete[] buffer;
	}
	return found;
}

std::vector<DWORD64> FindPatternExRegs(BYTE* pattern, std::string mask, DWORD64 RgSize) {
	std::vector<DWORD64> Result;
	if (!UpdateMemoryQuery())
		return Result;

	for (int i = 0; i < mbis.size(); i++) {
		MEMORY_BASIC_INFORMATION64 info = mbis[i];

		std::vector<DWORD64> arr = FindPatternEx(info.BaseAddress, info.RegionSize + info.BaseAddress, pattern, mask, info, RgSize);

		if (arr.size() > 0)
			Result.push_back(arr[0]);
	}

	return Result;
}

std::vector<DWORD64> FindPatterns(BYTE* buffer, BYTE* pattern, std::string mask, int bufSize) {
	std::vector<DWORD64> ret;
	int pattern_len = mask.length();
	for (int i = 0; i < bufSize - pattern_len; i++) {
		bool found = true;
		for (int j = 0; j < pattern_len; j++) {
			if (mask[j] != '?' && pattern[j] != buffer[i + j]) {
				found = false;
				break;
			}
		}
		if (found)
			ret.push_back(i);
	}
	return ret;
}

std::vector<DWORD64> FindPatternsExRegs(BYTE* pattern, std::string mask, DWORD64 RgSize) {
	std::vector<DWORD64> Result;
	std::vector<DWORD64> StartPointers = FindPatternExRegs(pattern, mask, RgSize);

	for (int i = 0; i < StartPointers.size(); i++) {
		for (int j = 0; j < mbis.size(); j++) {
			if (mbis[j].BaseAddress < StartPointers[i] && StartPointers[i] - mbis[j].BaseAddress < mbis[j].RegionSize) {
				StartPointers[i] = mbis[j].BaseAddress;
			}
		}

		BYTE* buf = new BYTE[RgSize];
		ReadProcessMemory(ProcessHandle, LPVOID(StartPointers[i]), buf, RgSize, nullptr);

		std::vector<DWORD64> Pointers = FindPatterns(buf, pattern, mask, RgSize);
		
		delete[] buf;

		for (int j = 0; j < Pointers.size(); j++)
			Pointers[j] += StartPointers[i];

		for (int j = 0; j < Pointers.size(); j++) {
			Result.push_back(Pointers[j]);
		}
	}
	return Result;
}