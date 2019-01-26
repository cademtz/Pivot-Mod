#pragma once
#include "Base.h"

/*
 *	Description:
 *		Manages the mod's user options / controls, has events, etc.
 *
 *		This class controls most of the functionality.
 */

class CModManager
{
	// - Indicates whether we should block the mouse
	bool bMouseEnabled = true;
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
	BOOL WINAPI OnLineTo(HDC hdc, int x, int y);
	// - Runs every time Pivot draws a cricle
	BOOL WINAPI OnEllipse(HDC hdc, int left, int top, int right, int bottom);
	
	// - Called when Pivot is adding controls to the toolbar
	// - This is where we can add our own options
	static void AddOptions(HMENU hmenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOA lpmi);
	// - Runs all of our windows on a separate thread
	// - Suspends MainThread when not currently needed
	static void RunWindows();

	// - Returns true if the mouse is over a region
	bool IsMouseOver(int x, int y, int w, int h) const;
	// - Returns true if the previous mouse position is over a region
	bool WasMouseOver(int x, int y, int w, int h) const;

	// - Use when you need mouse input without it registering under your controls
	inline void BlockMouse() { bMouseEnabled = false; }
	// - Use when you want to restore mouse input to Pivot
	inline void ReleaseMouse() { bMouseEnabled = true; }
	// - Returns whether the mouse is currently blocked or not
	inline bool IsMouseBlocked() { return !bMouseEnabled; }
};

extern CModManager gMod;