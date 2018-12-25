#include "Base.h"
#include "Pivot.h"
#include "APIHook.h"
#include "Signatures.h"
#include "ModManager.h"

/*
 *	Description:
 *		Handles all the dirty work. Hooks Pivot's functions and calls all our events.
 *
 *		This file is not interesting to you unless you're good at C++ and know what
 *		you're doing.
 */

CBaseManager gBase;
CSignatures gSig;

// ==========  Hook all our stuff before Pivot starts ==========

#pragma region Hooking

typedef BOOL(WINAPI* InsertMenuItem_t)(_In_ HMENU hmenu, _In_ UINT item, _In_ BOOL fByPosition, _In_ LPCMENUITEMINFOA lpmi);
typedef ATOM(WINAPI* RegisterClass_t)(_In_ CONST WNDCLASSA *lpWndClass);
typedef BOOL(WINAPI* LineTo_t)(_In_ HDC hdc, _In_ int x, _In_ int y);
typedef HWND(WINAPI* CreateWindow_t)(
	_In_ DWORD dwExStyle, _In_opt_ LPCSTR lpClassName, _In_opt_ LPCSTR lpWindowName, _In_ DWORD dwStyle,
	_In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight,
	_In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID lpParam);


hook_t hk_InsertMenuItem, hk_RegisterClass, hk_CreateWindow, hk_LineTo;
InsertMenuItem_t pInsertMenuItem;
RegisterClass_t pRegisterClass;
CreateWindow_t pCreateWindow;
LineTo_t pLineTo;

#include "JumpHook.h"
CJumpHook jmp_MainFormCreate;
CJumpHook jmp_DrawAnimBorder;
CJumpHook jmp_PaintBoxMouseMove;
CJumpHook jmp_PaintBoxMouseDown;
CJumpHook jmp_PaintBoxMouseUp;

void CBaseManager::InitHooks()
{
	// Get info and original pointer to Win API functions
	// Then we can insert our hooks to customize Pivot's windows

	hook::InitializeHook(&hk_InsertMenuItem, "user32.dll", "InsertMenuItemA", gBase.Hooked_InsertMenuItem);
	//hook::InitializeHook(&hk_RegisterClass, "user32.dll", "RegisterClassA", gBase.OnRegisterClass);
	//hook::InitializeHook(&hk_CreateWindow, "user32.dll", "CreateWindowExA", gBase.Hooked_CreateWindow);
	hook::InitializeHook(&hk_LineTo, "gdi32.dll", "LineTo", gBase.Hooked_LineTo);

	pInsertMenuItem = (InsertMenuItem_t)hk_InsertMenuItem.APIFunction;
	//pRegisterClass = (RegisterClass_t)hk_RegisterClass.APIFunction;
	//pCreateWindow = (CreateWindow_t)hk_CreateWindow.APIFunction;
	pLineTo = (LineTo_t)hk_LineTo.APIFunction;

	hook::InsertHook(&hk_InsertMenuItem);
	//hook::InsertHook(&hk_RegisterClass);
	//hook::InsertHook(&hk_CreateWindow);
	hook::InsertHook(&hk_LineTo);

	DWORD pMainFormCreate = gSig.GetPivotSig("55 8B EC 6A 00 53 56 57 8B F0");
	DWORD pDrawAnimBorderLine = gSig.GetPivotSig("55 8B EC 53 56 57 8B D9 8B FA");
	DWORD pPaintBoxMouseMove = gSig.GetPivotSig("55 8B EC 83 C4 D4 53 56 57 8B D8 33 D2");
	DWORD pPaintBoxMouseDown = gSig.GetPivotSig("55 8B EC 83 C4 EC 53 56 57 8B D8 8B 7D 0C");
	DWORD pPaintBoxMouseUp = gSig.GetPivotSig("55 8B EC 83 C4 F0 53 56 57 8B F0 8B 45 10");
	CHECK_SIG(pMainFormCreate);
	CHECK_SIG(pDrawAnimBorderLine);
	CHECK_SIG(pPaintBoxMouseMove);
	CHECK_SIG(pPaintBoxMouseDown);
	CHECK_SIG(pPaintBoxMouseUp);

	jmp_MainFormCreate.Hook(pMainFormCreate, gBase.Hooked_MainFormCreate, 5);
	jmp_DrawAnimBorder.Hook(pDrawAnimBorderLine, gBase.Hooked_DrawAnimBorderLine, 5);
	jmp_PaintBoxMouseMove.Hook(pPaintBoxMouseMove, gBase.Hooked_EditPaintBoxMouseMove, 6);
	jmp_PaintBoxMouseDown.Hook(pPaintBoxMouseDown, gBase.Hooked_EditPaintBoxMouseDown, 6);
	jmp_PaintBoxMouseUp.Hook(pPaintBoxMouseUp, gBase.Hooked_EditPaintBoxMouseUp, 6);
}

#pragma endregion

// ==========  The events and threads that run the mod  ==========

#pragma region Events

#include "ModManager.h"

char wndBuff[256];
DWORD WINAPI CBaseManager::MainThread(LPVOID pArgs)
{
	// There's probably a much better method for this.
	// Keep nagging Pivot at startup until it makes the foreground window
	// Then we hook the window to allow control over the toolbar

	HWND mainWnd = GetForegroundWindow();
	while (!mainWnd || !GetWindowText(mainWnd, wndBuff, sizeof(wndBuff)) || strcmp(wndBuff, "Pivot Modded"))
	{
		Sleep(100);
		mainWnd = GetForegroundWindow();
	}

	// Change the Pivot window title (for fun) and hook it for user input on the menu items
	SetWindowText(mainWnd, "Pivot Animator - Base mod by \"Hold on!\"");
	gBase.piv_wndproc = (WNDPROC)SetWindowLongPtr(mainWnd, GWLP_WNDPROC, (LONG)gMod.OnPivotWndproc);
	gBase.hPiv_wnd = mainWnd;

	while (true) // Let the window manager take over our thread
		gMod.RunWindows();

	return 0;
}

void CBaseManager::OnAttach(HINSTANCE Instance)
{
#ifdef _DEBUG
	// Visual Studio can't just start Pivot then attach debugger,
	// so we have to freeze Pivot if we want to attach before running
	MessageBox(NULL, "Attach debugger", "Info", MB_OK);
#endif

	hInst = Instance;
	InitHooks();

	hMainThread = CreateThread(0, 0, MainThread, 0, 0, 0);
	if (!hMainThread)
		return gBase.FatalError("Failed to create MainThread in '%s'!", __FUNCTION__);
}

#include "CMenuItem.h"
BOOL WINAPI CBaseManager::Hooked_InsertMenuItem(_In_ HMENU hmenu, _In_ UINT item, _In_ BOOL fByPosition, _In_ LPCMENUITEMINFOA lpmi)
{
	if (!strcmp(lpmi->dwTypeData, "&Help"))
	{
		// Run once, before the 'Help' tab is added so we can keep it at the end
		gMod.AddOptions(hmenu, item, fByPosition, lpmi);
		hook::Unhook(&hk_InsertMenuItem); // We've done our job. Time to unhook
	}

	return pInsertMenuItem(hmenu, item, fByPosition, lpmi);
}

DWORD pJump;
int __declspec(naked) CBaseManager::Hooked_MainFormCreate()
{
	__asm
	{
		push eax
		mov Pivot::pMainForm, eax
	}

	pJump = jmp_MainFormCreate.GetOldCode();

	__asm
	{
		pop eax
		jmp pJump // When jumping, the original function returns for us
	}
}

int __declspec(naked) CBaseManager::Hooked_DrawAnimBorderLine()
{
	// Save these registers for calling TMainForm::DrawAnimBorderLine
	__asm
	{
		push eax
		push edx
		push ecx
	}

	// Get a pointer to the original ASM, and skip over the length where our hook is
	pJump = jmp_DrawAnimBorder.GetOldCode();
	
	// Restore the registers' original values to call the original function
	__asm
	{
		pop ecx
		pop edx
		pop eax
		call pJump
	}

	gMod.OnDrawOverlay(Pivot::pMainForm->GetCanvasHDC());
	
	__asm retn; // Force a return in ASM because C++ will try to stop us
}

int tempx, tempy;
int __declspec(naked) CBaseManager::Hooked_EditPaintBoxMouseMove(int y, int x)
{
	__asm push eax;

	__asm
	{
		mov eax, [esp + 8]	// Get y
		mov [tempy], eax	// Move y to tempy
		mov eax, [esp + 12]	// Get x
		mov [tempx], eax	// Move x to tempx
	}

	Pivot::pMainForm->UpdateMousePos(tempx, tempy);
	gMod.OnMouseMove();
	pJump = jmp_PaintBoxMouseMove.GetOldCode();

	__asm
	{
		pop eax
		jmp pJump
	}
}

BYTE tempb;
int __declspec(naked) CBaseManager::Hooked_EditPaintBoxMouseDown(char arg4, int arg3, int arg2)
{
	__asm push eax;

	__asm
	{
		mov al, [esp + 10h]
		mov [tempb], al
	}

	Pivot::pMainForm->UpdateMouseDown(tempb);
	gMod.OnMouseClick();
	pJump = jmp_PaintBoxMouseDown.GetOldCode();

	__asm
	{
		pop eax
		jmp pJump
	}
}

void testdown(BYTE state)
{
	const char* szState = "Blank";
	if (state == 0)
		szState = "Left";
	else
		szState = "Right";

	MessageBox(0, szState, "Info", 0);
}

int __declspec(naked) CBaseManager::Hooked_EditPaintBoxMouseUp(char arg5, int arg4, int arg3)
{
	__asm push eax;

	__asm mov [tempb], cl

	Pivot::pMainForm->UpdateMouseUp(tempb);
	gMod.OnMouseRelease();
	pJump = jmp_PaintBoxMouseUp.GetOldCode();

	__asm
	{
		pop eax
		jmp pJump
	}
}

BOOL WINAPI CBaseManager::Hooked_LineTo(_In_ HDC hdc, _In_ int x, _In_ int y)
{
	return gMod.OnLineTo(hdc, x, y);
}

#pragma endregion

// ==========  Error handling and basic utilities  ==========

#pragma region Utilities

HFONT CBaseManager::GetFont()
{
	static HFONT hFont = CreateFontA(17, 0, 0, 0, FW_DONTCARE, false, false,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FIXED_PITCH | FF_MODERN, "Segoe UI");
	return hFont;
}

BOOL WINAPI CBaseManager::LineTo(HDC hdc, int x, int y)
{
	return pLineTo(hdc, x, y);
}

void CBaseManager::Error(const char* szFormat, const char* szArgs, ...)
{
	va_list va_alist;
	char szBuffer[1024] = { 0 };

	va_start(va_alist, szFormat);
	vsprintf_s(szBuffer, szFormat, va_alist);
	va_end(va_alist);

	MessageBox(NULL, szBuffer, "Warning", MB_OK);
}

void CBaseManager::FatalError(const char* szFormat, const char* szArgs, ...)
{
	va_list va_alist;
	char szBuffer[1024] = { 0 };

	va_start(va_alist, szFormat);
	vsprintf_s(szBuffer, szFormat, va_alist);
	va_end(va_alist);

	MessageBox(NULL, szBuffer, "Fatal error", MB_OK);
	MessageBox(NULL, "The program will now exit.", "Closing process...", MB_OK);
	exit(-1);
}

#pragma endregion

// ==========  Unused hooks and stuff. Might need them later.  ==========

#pragma region UnusedHooks

ATOM WINAPI CBaseManager::OnRegisterClass(WNDCLASSA * wndc)
{
	wndc->style |= CS_GLOBALCLASS; // Forces Pivot controls to public so we can use them
	return pRegisterClass(wndc);
}

HWND WINAPI CBaseManager::Hooked_CreateWindow(
	_In_ DWORD dwExStyle, _In_opt_ LPCSTR lpClassName, _In_opt_ LPCSTR lpWindowName, _In_ DWORD dwStyle,
	_In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight,
	_In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID lpParam)
{
	if (lpClassName)
		MessageBoxA(0, lpWindowName, lpClassName, 0);
	return pCreateWindow(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

#pragma endregion