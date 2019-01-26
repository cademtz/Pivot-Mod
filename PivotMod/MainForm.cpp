#include "MainForm.h"
#include "Signatures.h"
#include "Pivot.h"

/*
 * Description:
 *		Functions and variables that are part of Pivot's main form
 *
 *		Lots of signatures and ASM because all the functions are optimized
 *		to use ASM registers instead of normal calling conventions
 */

POINT CMainForm::mouse = { UINT_MAX, UINT_MAX };
POINT CMainForm::pmouse = mouse;
e_mb CMainForm::mb = e_mb::null;

void CMainForm::DrawFigures()
{
	static DWORD* arg1, *pDrawFigures = nullptr; // Pointer to the function and the arg to pass
	if (!pDrawFigures) // Get the pointer for the first time
	{
		/*DWORD callToFunc = gSig.GetPivotSig("A1 ? ? ? ? 8B 00 E8 ? ? ? ? C6 45 CF 00");
		CHECK_SIG(callToFunc);
		arg1 = **(DWORD***)(callToFunc + 0x1); // A1 ? ? ? ? - Points to a variable we need
		pDrawFigures = (DWORD*)(*(DWORD*)(callToFunc + 0x8) + callToFunc + 0xC); // E8 ? ? ? ? - Relative 32-bit call to function*/

		DWORD funcCode = gSig.GetPivotSig("55 8B EC 83 C4 E8 53 56 57 8B D8 33 C9");
		CHECK_SIG(funcCode);
		pDrawFigures = (DWORD*)funcCode;
	}

	__asm
	{
		push eax
		//mov eax, Pivot::pMainForm	// The function uses EAX for the argument,
		mov eax, [this]				// so we set it to our argument's value
		call pDrawFigures			//
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

const AnimInfo_t& CMainForm::GetAnimInfo()
{
	static AnimInfo_t* animInfo = nullptr;
	if (!animInfo)
	{
		DWORD offToInfo = gSig.GetPivotSig("A1 ? ? ? ? 33 D2 89 10 C6 86");
		CHECK_SIG(offToInfo);
		animInfo = **(AnimInfo_t***)(offToInfo + 1);
	}
	
	return *animInfo;
}

const BITMAP& CMainForm::GetCanvasBitmap()
{
	static BITMAP bmp;
	GetObject(GetCurrentObject(GetCanvasHDC(), OBJ_BITMAP), sizeof(BITMAP), &bmp);
	return bmp;
}

const POINT& CMainForm::GetCanvasCorner()
{
	static POINT topleft;

	auto bmp = GetCanvasBitmap();
	auto size = GetAnimationDimensions();

	topleft.x = (bmp.bmWidth / 2) - (size.x / 2);
	topleft.y = (bmp.bmHeight / 2) - (size.y / 2);

	return topleft;
}