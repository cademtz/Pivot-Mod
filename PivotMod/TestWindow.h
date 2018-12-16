#pragma once
#include "CBaseWindow.h"
#include "CMenuItem.h"

/*
 *	Description:
 *		Example of a custom window with some basic controls.
 *
 *		This window appears when you select Piv Mod > Effects.
 *		This is all template code you can copy to use for another window.
 *		Only thing you need to change is the name, which is called "CTestWindow" for this example
 *
 *		The actually important code appears in: TestWindow.cpp
 *		Style and other factors of the window happen when you create it in: ModManager.cpp
 */

class CTestWindow : public CBaseWindow
{
	// - Indicates if we have registered this class already
	static ATOM wnd_class;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// - Registers the window's class if not previously done
	// - Privated to allow one instance
	CTestWindow()
	{
		if (wnd_class)
			return; // Already registered!

		InitType("CTestWindow", (HBRUSH)COLOR_BACKGROUND + 1);
		Register(WndProc, wnd_class);
	}
	// - Privated to allow one instance
	~CTestWindow() { }
public:
	// - Creates one, single class for us to use
	static CTestWindow& Instance()
	{
		static CTestWindow test_window;
		return test_window;
	}

	virtual void OnCreate(HWND hwnd, WPARAM wparam, LPARAM lparam);
};