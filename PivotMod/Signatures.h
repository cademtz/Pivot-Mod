#pragma once
#include "Base.h"

/*
 *	Description:
 *		Signature scanning that supports IDA style signatures.
 *		Used for getting pointers without easily going outdated (unless code changes too much).
 */

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

class CSignatures
{
public:
	template<class T = DWORD>
	// - Finds a pointer in Pivot by using an IDA style signature
	// - Returns NULL / nullptr on failure
	T GetPivotSig(const char* IDASig)
	{
		static HMODULE hmModule = GetModuleHandle(NULL);
		static PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hmModule;
		static PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hmModule) + pDOSHeader->e_lfanew);

		return (T)dwFindPattern(((DWORD)hmModule) + pNTHeaders->OptionalHeader.BaseOfCode, ((DWORD)hmModule) + pNTHeaders->OptionalHeader.SizeOfCode, IDASig);
	}

	// - Converts an IDA style signature to a basic byte-mask style
	void ToBytemask(const char* IDASig, std::string& bytesOut, std::string& maskOut)
	{
		std::string pattern = "", mask = "";
		for (size_t i = 0; i < strlen(IDASig); i++)
		{
			if (IDASig[i] == ' ')
				continue;
			if (IDASig[i] == '?')
			{
				mask += '?', pattern += '?';
				continue;
			}
			unsigned char newByte = 0;
			for (size_t c = i; c < i + 2; c++)
				newByte <<= 4, newByte += (IDASig[c] >= 'A' && IDASig[c] <= 'F') ? IDASig[c] - 'A' + 0xA : IDASig[c] - '0';
			pattern += newByte, mask += 'x', i++;
		}

		bytesOut = pattern, maskOut = mask;
	}

	// - Finds a pointer by using an IDA style signature
	// - Returns NULL on failure
	DWORD dwFindPattern(DWORD dwAddress, DWORD dwLength, const char* szPattern)
	{
		const char* pat = szPattern;
		DWORD firstMatch = NULL;
		for (DWORD pCur = dwAddress; pCur < dwLength; pCur++)
		{
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;
			}
			else {
				pat = szPattern;
				firstMatch = 0;
			}
		}
		return NULL;
	}
};

extern CSignatures gSig;