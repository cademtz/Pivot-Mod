#pragma once
#include "Base.h"

/*
 *	Description:
 *		Manages the mod's user options / controls, has events, etc.
 *
 *		For now, this is where you add your controls and windows.
 */

class CModManager
{
public:
	// - Only runs when the process sends messages to its main window
	// Because of macros, IntelliSense keeps farding and pooping the bed, so it looks undefined
	static LRESULT WINAPI OnPivotWndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	// - Draws on top after Pivot draws the animation, overlaying everything
	static void OnDrawOverlay(HDC hdc);
	// - Only runs when Pivot updates the mouse
	static void OnMouseMove();
	// - Only runs when the mouse is clocked
	static void OnMouseClick();
	// - Only runs when the mouse button is released
	static void OnMouseRelease();
	// - Runs every time Pivot draws a line / segment
	// Using const magically makes this compile???
	BOOL WINAPI OnLineTo(_In_ HDC hdc, _In_ int x, _In_ int y);
	
	// - Called when Pivot is adding controls to the toolbar
	// - This is where we can add our own options
	static void AddOptions(HMENU hmenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOA lpmi);
	// - Runs all of our windows on a separate thread
	// - Suspends MainThread when not currently needed
	static void RunWindows();

	bool IsMouseOver(int x, int y, int w, int h) const;
	bool WasMouseOver(int x, int y, int w, int h) const;
};

extern CModManager gMod;