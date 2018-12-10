#pragma once
#include "Base.h"

/*
 *	Description:
 *		Manages the mod's user options / controls.
 *
 *		For now, this is where you add your controls and windows.
 */

class CModManager
{
public:
	// - Only runs when the process sends messages to its main window
	// Because of macros, IntelliSense keeps farding and pooping the bed, so it looks undefined
	static LRESULT WINAPI OnPivotWndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	// - Called when Pivot is adding controls to the toolbar
	// - This is where we can add our own options
	static void AddOptions(HMENU hmenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOA lpmi);
	// - Runs all of our windows on a separate thread
	// - Suspends MainThread when not currently needed
	static void RunWindows();
};

extern CModManager gMod;