#pragma once
#include "Base.h"
#include "CMenuItem.h"

/*
 *	Description:
 *		Simplifies the process of using Windows' ugly API. Use this class as a base to make
 *		windows very easily. I plan to improve this and make it less complicated.
 *		
 *		See an example of how to use this for a window: CTestWindow.h
 *
 *		I need to improve this, but I might also not :)
 *		Because Pivot is cross-platform and uses a custom style, I'll have to get these controls
 *		to draw in Pivot's style, otherwise they look out of place and ugly.
 */

class CBaseWindow
{
	HWND window = nullptr;
protected:
	WNDCLASSEX type{ 0 };

	// - Initializes some type defaults
	CBaseWindow()
	{
		// Just setting some defaults we might not want to specify later on
		type.hCursor = LoadCursor(NULL, IDC_ARROW);
	}
	// - Gets existing window class from handle
	CBaseWindow(HWND hwnd) { window = hwnd; }

	// - Allow default values and adds them to the 'type' variable
	// - Feel free to specify custom values in 'type' afterwards
	void InitType(
		const char* szClassName,
		HBRUSH hbrBackground = (HBRUSH)COLOR_BACKGROUND,
		DWORD dwClassStyles = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW)
	{
		type.cbSize = sizeof(WNDCLASSEX);

		type.lpszClassName = szClassName;
		type.hbrBackground = hbrBackground;
		type.style = dwClassStyles;
	}

	// - Registers the window class and automatically assigns hInstance. Only needed once before creation
	// - Requires a static wnd_class variable to indicate it is already registered once
	// - Returns true on success
	bool Register(WNDPROC WndProc, ATOM& wnd_class)
	{
		if (wnd_class) // Already registered!
			return true;

		type.lpfnWndProc = WndProc;
		type.hInstance = gBase.GetInstance();

		wnd_class = RegisterClassEx(&type);
		if (!wnd_class)
		{
			gBase.Error("Failed to register window type '%s'\nError code: %i", type.lpszClassName, GetLastError());
			return false;
		}

		return true;
	}
public:
	// - Returns a handle to the window
	// - Handle must be made first by calling 'CBaseWindow::Create'
	HWND Get() const { return window; }

	// - Creates a new window handle and allows default values
	// - Only use after registering the class once
	// - Returns true on success
	bool Create(const char* szName, int Width, int Height, int X = CW_USEDEFAULT, int Y = CW_USEDEFAULT, DWORD dwStyle = WS_SYSMENU, DWORD dwExStyle = WS_EX_DLGMODALFRAME)
	{
		if (window) // Window already exists!
			return true;

		window = CreateWindowEx(dwExStyle, type.lpszClassName, szName, dwStyle, X, Y, Width, Height, 0, 0, type.hInstance, 0);
		if (!window)
		{
			gBase.Error("Failed to create window '%s'\nError code: %i", szName, GetLastError());
			return false;
		}
		return true;
	}
	// - Is called when the window procedure passes WM_CREATE
	// - Use this to create all your controls
	virtual void OnCreate(HWND hwnd, WPARAM wparam, LPARAM lparam) { }

	// - Destroys the window handle. Must be created again for later use
	bool Destroy()
	{
		if (!window)
			return false;

		DestroyWindow(window);
		window = nullptr;
		return true;
	}

	// - Runs the window once
	// - Return value tells you if you can keep looping
	bool Run()
	{
		MSG msg;
		if (!GetMessage(&msg, 0, 0, 0))
			return false;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		return true;
	}

	// - Returns the center of the screen for your window
	static inline RECT Center(int winWidth, int winHeight)
	{
		RECT rect;
		GetClientRect(GetDesktopWindow(), &rect);
		rect.left = (rect.right / 2) - (winWidth / 2);
		rect.top = (rect.bottom / 2) - (winHeight / 2);
		return rect;
	}
};