#include "MainForm.h"
#include "Signatures.h"

POINT CMainForm::mouse = { UINT_MAX, UINT_MAX };
POINT CMainForm::pmouse = mouse;
e_mb CMainForm::mb = e_mb::null;

void CMainForm::DrawFigures()
{
	static DWORD* arg1, *pDrawFigures = nullptr; // Pointer to the function and the arg to pass
	if (!pDrawFigures) // Get the pointer for the first time
	{
		DWORD callToFunc = gSig.GetPivotSig("A1 ? ? ? ? 8B 00 E8 ? ? ? ? C6 45 CF 00");
		CHECK_SIG(callToFunc);
		arg1 = **(DWORD***)(callToFunc + 0x1); // A1 ? ? ? ? - Points to a variable we need
		pDrawFigures = (DWORD*)(*(DWORD*)(callToFunc + 0x8) + callToFunc + 0xC); // E8 ? ? ? ? - Relative 32-bit call to function
	}

	__asm
	{
		push eax
		mov eax, arg1		// The function uses EAX for the argument,
		mov eax, [eax]		// so we set it to our argument's value
		call pDrawFigures	//
		pop eax
	}
}

void CMainForm::DrawAnimBorderLine()
{
	static DWORD* pDrawBorder = nullptr;
	if (!pDrawBorder)
	{
		DWORD callToFunc = gSig.GetPivotSig("E8 ? ? ? ? 46 66 83 FE 04");
		CHECK_SIG(callToFunc);
		pDrawBorder = (DWORD*)(*(DWORD*)(callToFunc + 0x1) + callToFunc + 0x5);
	}

	HDC hdc = GetCanvasHDC();
	__asm
	{
		mov eax, [this]
		mov edx, [hdc]
		mov ecx, 0
		call pDrawBorder
	}
}

HDC CMainForm::GetCanvasHDC()
{
	static DWORD* arg1, *pGetWinInfo;
	if (!pGetWinInfo)
	{
		DWORD callToFunc = gSig.GetPivotSig("A1 ? ? ? ? E8 ? ? ? ? 8B D0 0F B7 CE");
		CHECK_SIG(callToFunc);
		arg1 = **(DWORD***)(callToFunc + 0x1);
		pGetWinInfo = (DWORD*)(*(DWORD*)(callToFunc + 0x6) + callToFunc + 0xA);
	}

	HDC* hdc;
	__asm
	{
		push eax
		mov eax, arg1
		call pGetWinInfo
		mov hdc, eax
		pop eax
	}

	return hdc[1];
}

const POINT* CMainForm::GetAnimationDimensions()
{
	static POINT* dimensions;
	if (!dimensions)
	{
		DWORD offToDimens = gSig.GetPivotSig("A1 ? ? ? ? 8B 55 FC 89 10");
		CHECK_SIG(offToDimens);
		dimensions = **(POINT***)(offToDimens + 1);
	}

	return dimensions;
}