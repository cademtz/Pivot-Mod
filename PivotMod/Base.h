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

#define CHECK_SIG(dwSig) if (!dwSig) gBase.FatalError("Used outdated signature in '%s'\nAsk the dev to update this mod!", __FUNCTION__)

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
	// - Call original pointer to Ellipse
	static BOOL WINAPI Ellipse(HDC hdc, int top, int left, int right, int bottom);

	// - Gives the user a formatted warning message
	static void Error(const char* szFormat, const char* szArgs = nullptr, ...);
	// - Gives the user an error message and closes the process
	static void FatalError(const char* szFormat, const char* szArgs = nullptr, ...);

	// - Only runs on DLL_ATTACH
	void OnAttach(HINSTANCE Instance);

private:
	// - Only runs when the process calls InsertMenuItemA
	static BOOL WINAPI Hooked_InsertMenuItem(HMENU hmenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOA lpmi);
	// - Only runs when Pivot calls TMainForm::Create
	// - Used for getting a pointer to the CMainForm class
	static int Hooked_MainFormCreate();
	// - Only runs when Pivot calls TMainForm::DrawAnimBorderLine
	// - Used for drawing our tools like camera without affecting export / thumbnails
	static int Hooked_DrawAnimBorderLine();
	// - Only runs when Pivot calls TMainForm::EditPaintBoxMouseMove
	// - Used to get the current mouse position
	static int Hooked_EditPaintBoxMouseMove(int y, int x);
	// - Only runs when Pivot calls TMainForm::EditPaintBoxMouseDown
	// - Used to update the mouse state
	static int Hooked_EditPaintBoxMouseDown(char arg4, int arg3, int arg2);
	// - Only runs when Pivot calls TMainForm::EditPaintBoxMouseUp
	// - Used to update the mouse state
	static int Hooked_EditPaintBoxMouseUp(char arg5, int arg4, int arg3);
	// - Only runs when Pivot calls LineTo
	// - Used to control Pivot's lines
	static BOOL WINAPI Hooked_LineTo(HDC hdc, int x, int y);
	// - Only runs when Pivot calls Ellipse
	// - Used to control Pivot's circles
	static BOOL WINAPI Hooked_Ellipse(HDC hdc, int left, int top, int right, int bottom);

	// Unused stuff

	// - Only runs when the process calls RegsterClassA
	static ATOM WINAPI OnRegisterClass(WNDCLASSA *wndc);
	// - Only runs when the process class CreateWindowExA
	static HWND WINAPI Hooked_CreateWindow(
		DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
		int X, int Y, int nWidth, int nHeight,
		HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
		LPVOID lpParam);

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