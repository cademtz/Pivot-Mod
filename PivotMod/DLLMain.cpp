#include "Base.h"

/*
 *	Description:
 *		Entry point of our mod.
 *
 *		When loaded into any program, DLLMain is the only function that runs.
 *		Everything in this program starts from here.
 */

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		gBase.OnAttach(hModule);
    return TRUE;
}