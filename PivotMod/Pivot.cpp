#include "Pivot.h"
#include "Signatures.h"

HDC Pivot::GetCanvasHDC()
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