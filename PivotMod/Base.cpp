#include "Base.h"
#include "APIHook.h"

/*
 *	Description:
 *		Handles all the dirty work. Hooks Pivot's functions and calls all our events.
 *
 *		This file is not interesting to you unless you're good at C++ and know what
 *		you're doing.
 */

CBaseManager gBase;

// ==========  Hook all our stuff before Pivot starts ==========

#pragma region Hooking

typedef BOOL(WINAPI* InsertMenuItem_t)(_In_ HMENU hmenu, _In_ UINT item, _In_ BOOL fByPosition, _In_ LPCMENUITEMINFOA lpmi);
typedef ATOM(WINAPI* RegisterClass_t)(_In_ CONST WNDCLASSA *lpWndClass);
typedef HWND(WINAPI* CreateWindow_t)(
	_In_ DWORD dwExStyle, _In_opt_ LPCSTR lpClassName, _In_opt_ LPCSTR lpWindowName, _In_ DWORD dwStyle,
	_In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight,
	_In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID lpParam);

hook_t hk_InsertMenuItem, hk_RegisterClass, hk_CreateWindow;
InsertMenuItem_t pInsertMenuItem;
RegisterClass_t pRegisterClass;
CreateWindow_t pCreateWindow;

void CBaseManager::InitHooks()
{
	// Get info and original pointer to Win API functions
	// Then we can insert our hooks to customize Pivot's windows

	hook::InitializeHook(&hk_InsertMenuItem, "user32.dll", "InsertMenuItemA", gBase.OnInsertMenuItem);
	//hook::InitializeHook(&hk_RegisterClass, "user32.dll", "RegisterClassA", gBase.OnRegisterClass);
	//hook::InitializeHook(&hk_CreateWindow, "user32.dll", "CreateWindowExA", gBase.OnCreateWindow);

	pInsertMenuItem = (InsertMenuItem_t)hk_InsertMenuItem.APIFunction;
	//pRegisterClass = (RegisterClass_t)hk_RegisterClass.APIFunction;
	//pCreateWindow = (CreateWindow_t)hk_CreateWindow.APIFunction;

	hook::InsertHook(&hk_InsertMenuItem);
	//hook::InsertHook(&hk_RegisterClass);
	//hook::InsertHook(&hk_CreateWindow);
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

	// Change the Pivot window title (for fun) and hook it
	SetWindowText(mainWnd, "Pivot Animator - Base mod by \"Hold on!\"");
	gBase.piv_wndproc = (WNDPROC)SetWindowLongPtr(mainWnd, GWLP_WNDPROC, (LONG)gMod.OnPivotWndproc);

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
BOOL WINAPI CBaseManager::OnInsertMenuItem(_In_ HMENU hmenu, _In_ UINT item, _In_ BOOL fByPosition, _In_ LPCMENUITEMINFOA lpmi)
{
	static bool bOnce = true;
	if (!strcmp(lpmi->dwTypeData, "&Help") && bOnce)
	{
		bOnce = false; // Run once, before the 'Help' tab is added so we can keep it at the end
		gMod.AddOptions(hmenu, item, fByPosition, lpmi);
		hook::Unhook(&hk_InsertMenuItem); // We've done our job. Time to unhook
	}

	return pInsertMenuItem(hmenu, item, fByPosition, lpmi);
}

#pragma endregion

// ==========  Error handling and basic utilities  ==========

#pragma region Utilities

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

HWND WINAPI CBaseManager::OnCreateWindow(
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