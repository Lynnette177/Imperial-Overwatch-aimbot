#pragma once
#include <Windows.h>
#include <cstdint>

typedef unsigned char byte;

class SignatureScanner
{
private:
	uintptr_t startAddress = 0;
	uintptr_t endAddress = 0;
	HANDLE hProcess = 0;
public:
	SignatureScanner(uintptr_t start, uintptr_t end = INTPTR_MAX);
	SignatureScanner(HANDLE hProcess, uintptr_t start, uintptr_t end = INTPTR_MAX);
	uintptr_t scan(std::string pattern, int skips = 0);
	uintptr_t scanEx(std::string pattern, int skips = 0);
};