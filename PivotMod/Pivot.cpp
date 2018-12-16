#include "Pivot.h"
#include "Base.h"
#include "Signatures.h"
/*
 *	Description:
 *		Contains organized namespaces with variables and functions from Pivot's code
 *		Useful for making Pivot redraw, load, and save figures as well as other things
 */

void Pivot::MainForm::DrawFigures()
{
	static DWORD* arg1, * pDrawFigures; // Pointer to the function and the arg to pass

	if (!pDrawFigures) // Get the pointer for the first time
	{
		DWORD callToFunc = gSig.GetPivotSig("A1 ? ? ? ? 8B 00 E8 ? ? ? ? C6 45 CF 00");
		if (!callToFunc)
			gBase.FatalError("Failed to find a pointer to '%s'!\nTypically caused by being outdated", __FUNCTION__);
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