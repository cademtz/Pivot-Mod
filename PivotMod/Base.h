#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

/*
 *	Description:
 *		Holds all common headers needed in most other files
 *		Handles all the dirty work. Hooks Pivot's functions and calls all our events.
 *
 *		Ordered by interesting stuff, so things on the bottom aren't interesting
 *		unless you're decent with C++ and the Win API.
 */

class CBaseManager
{
public:
	// - Returns our DLL instance
	inline HINSTANCE GetInstance() const { return hInst; }
	// - Returns a handle to our main thread
	inline HANDLE GetMainThread() const { return hMainThread; }
	// - Returns a handle to Pivot's main window
	inline HWND GetPivotWnd() const { return hPiv_wnd; }
	// - Returns a pointer to Pivot's original window procedure
	inline WNDPROC GetPivotWndProc() const { return piv_wndproc; }
	// - Returns our default font
	HFONT GetFont();

	// - Call original pointer to LineTo
	static BOOL WINAPI LineTo(HDC hdc, int x, int y);

	// - Gives the user a formatted warning message
	static void Error(const char* szFormat, const char* szArgs = nullptr, ...);
	// - Gives the user an error message and closes the process
	static void FatalError(const char* szFormat, const char* szArgs = nullptr, ...);

	// - Only runs on DLL_ATTACH
	void OnAttach(HINSTANCE Instance);
	// - Only runs when the process calls InsertMenuItemA
	static BOOL WINAPI OnInsertMenuItem(_In_ HMENU hmenu, _In_ UINT item, _In_ BOOL fByPosition, _In_ LPCMENUITEMINFOA lpmi);
	// - Only runs when the process calls RegsterClassA
	static ATOM WINAPI OnRegisterClass(WNDCLASSA *wndc);
	// - Only runs when the process class CreateWindowExA
	static HWND WINAPI OnCreateWindow(
		_In_ DWORD dwExStyle, _In_opt_ LPCSTR lpClassName, _In_opt_ LPCSTR lpWindowName, _In_ DWORD dwStyle,
		_In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight,
		_In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance,
		_In_opt_ LPVOID lpParam);
private:
	// - Stored instance of our DLL from DLLMain
	HINSTANCE hInst;
	// - Stored handle to our main thread
	HANDLE hMainThread;
	// - Stored handle to Pivot's main window
	HWND hPiv_wnd;
	// - Stored pointer of Pivot's original windows procedure function
	WNDPROC piv_wndproc;

	// - Hooks WinAPI functions for inserting its own controls
	void InitHooks();
	// - Our main thread. Used for all other tasks to prevent holding up Pivot while it loads.
	static DWORD WINAPI MainThread(LPVOID pArgs);
};

extern CBaseManager gBase;